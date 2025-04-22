#include "activity_actor_definitions.h"
#include "salvage.h"

#include <set>
#include <unordered_map>
#include <vector>

#include "activity_speed.h"
#include "character.h"
#include "flag.h"
#include "itype.h"
#include "json.h"
#include "map.h"
#include "material.h"
#include "messages.h"
#include "options.h"
#include "output.h"
#include "player_activity.h"
#include "popup.h"
#include "recipe_dictionary.h"
#include "type_id.h"
#include "ui_manager.h"
#include "game_inventory.h"
#include "player.h"

const skill_id skill_fabrication( "fabrication" );

static std::unordered_map<material_id, std::set<quality_id>> salvage_material_quality_dictionary;
static std::set<material_id> all_salvagable_materials;


namespace salvage
{

// Helper to visit instances of all the sub-materials of an item.
static void visit_salvage_products( const item &it,
                                    const std::function<void( const itype_id & )> &func )
{
    for( const material_id &material : it.made_of() ) {
        if( const std::optional<itype_id> id = material->salvaged_into() ) {
            func( *id );
        }
    }
}

// Helper to find smallest sub-component of an item.
units::mass minimal_weight_to_cut( const item &it )
{
    units::mass min_weight = units::mass_max;
    visit_salvage_products( it, [&min_weight]( const itype_id & exemplar ) {
        min_weight = std::min( min_weight, exemplar->weight );
    } );
    return min_weight;
}

static q_result yn_ignore_query( const std::string &text )
{
    const bool force_uc = get_option<bool>( "FORCE_CAPITAL_YN" );
    const auto &allow_key = force_uc
                            ? input_context::disallow_lower_case
                            : input_context::allow_all_keys;

    const auto &action = query_popup()
                         .context( "YN_IGNORE_QUERY" )
                         .message( force_uc
                                   ? pgettext( "YN_IGNORE_QUERY",
                                           "<color_light_red>%s (Case Sensitive)</color>" )
                                   : pgettext( "YN_IGNORE_QUERY",
                                           "<color_light_red>%s</color>" ),
                                   text )
                         .option( "YES", allow_key )
                         .option( "SKIP", allow_key )
                         .option( "ABORT", allow_key )
                         .option( "IGNORE", allow_key )
                         .query()
                         .action;

    ui_manager::redraw();
    refresh_display();

    if( action == "YES" ) {
        return q_result::yes;
    }
    if( action == "IGNORE" ) {
        return q_result::ignore;
    }
    if( action == "SKIP" ) {
        return q_result::skip;
    }
    return q_result::abort;
}

inline bool try_salvage_silent( const Character &who, const item &it, inventory inv )
{
    switch( try_salvage( who, it, inv, true, true ) ) {
        case q_result::yes:
        case q_result::ignore:
            return true;
        case q_result::skip:
        case q_result::abort:
        case q_result::fail:
        default:
            return false;
    }
}

// It is used to check if an item can be salvaged or not.
q_result try_salvage( const Character &who, const item &it, inventory inv, bool mute,
                      bool mute_promts )
{
    if( it.is_null() ) {
        if( !mute ) {
            add_msg( m_info, _( "You do not have that item." ) );
        }
        return q_result::fail;
    }
    // There must be some historical significance to these items.
    if( !it.is_salvageable() ) {
        if( !mute ) {
            add_msg( m_info, _( "Can't salvage anything from %s." ), it.tname() );
            if( recipe_dictionary::get_uncraft( it.typeId() ) ) {
                add_msg( m_info, _( "Try disassembling the %s instead." ), it.tname() );
            } else if( !it.only_made_of( all_salvagable_materials ) ) {
                add_msg( m_info, _( "The %s is made entirely of material that cannot be salvaged up." ),
                         it.tname() );
            }
        }
        return q_result::fail;
    }
    if( !it.contents.empty() ) {
        if( !mute ) {
            add_msg( m_info, _( "Please empty the %s before salvaged it up." ), it.tname() );
        }
        return q_result::fail;
    }
    if( it.weight() < minimal_weight_to_cut( it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "The %s is too small to salvage any material from." ), it.tname() );
        }
        return q_result::fail;
    }
    if( !has_salvage_tools( inv, it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "You lack proper tools to salvage any material from the %s." ), it.tname() );
        }
        return q_result::abort;


    } else
        // Softer warnings at the end so we don't ask permission and then tell them no.
        if( !( mute_promts && mute ) ) {
            for( auto &mat : it.made_of() ) {
                if( !has_salvage_tools( inv, mat ) ) {
                    auto result = yn_ignore_query(
                                      string_format(
                                          _( "You lack proper tools to salvage %s from the %s, meaning output for this material.  Salvage anyway?" ),
                                          mat->name(), it.tname() ) );
                    switch( result ) {
                        case q_result::yes:
                            break;
                        default:
                            return result;
                    }
                }
            }
        }

    if( !( mute_promts && mute ) ) {
        if( who.is_wielding( it ) ) {
            auto result = yn_ignore_query( _( "You are wielding that, salvage anyway?" ) ) ;
            switch( result ) {
                case q_result::yes:
                    break;
                default:
                    return result;
            }
        } else if( who.is_wearing( it ) ) {
            auto result = yn_ignore_query( _( "You're wearing that, salvage anyway?" ) ) ;
            switch( result ) {
                case q_result::yes:
                    break;
                default:
                    return result;
            }
        }
        if( it.is_favorite ) {
            auto result = yn_ignore_query( _( "This item is marked as favorite, salvage anyway?" ) );
            switch( result ) {
                case q_result::yes:
                    break;
                default:
                    return result;
            }
        }
    }

    return q_result::yes;
}

//Returns vector of pairs <material, fraction>, where fraction = [0.0f, 1.0f]
static std::vector<std::pair< material_id, float>> salvage_result_proportions( const item &target )
{
    auto &materials = target.made_of();
    std::vector<std::pair< material_id, float>> salvagable_materials;
    //For now we assume that proportions for all materials are equal
    for( auto &material : materials ) {
        salvagable_materials.emplace_back( material, 1.0f / materials.size() );
    }
    return salvagable_materials;
}

//Returns vector of pairs <item id, count>
std::vector<std::pair< itype_id, float>> salvage_results( const item &target )
{
    std::vector<std::pair< itype_id, float>> salvagable_materials;
    //For now we assume that proportions for all materials are equal
    for( auto &material : salvage_result_proportions( target ) ) {
        auto res = material.first->salvaged_into();
        if( all_salvagable_materials.contains( material.first ) && res ) {
            salvagable_materials.emplace_back( *res,
                                               //cuz we need actual float here
                                               target.weight().value() * material.second / ( **res ).weight.value() );
        }
    }
    return salvagable_materials;
}

void complete_salvage( Character &who, item &cut, tripoint_abs_ms pos )
{
    float salvagable_percent = 1.0f;
    // Chance of us losing a material component to entropy.
    /** @EFFECT_FABRICATION reduces chance of losing components when cutting items up */
    int entropy_threshold = std::max( 5, 10 - who.get_skill_level( skill_fabrication ) );
    // Not much practice, and you won't get very far ripping things up.
    who.practice( skill_fabrication, rng( 0, 5 ), 1 );
    // Higher fabrication, less chance of entropy, but still a chance.
    if( rng( 1, 10 ) <= entropy_threshold ) {
        salvagable_percent *= 0.95f;
    }
    // Fail dex roll, potentially lose more parts.
    /** @EFFECT_DEX randomly reduces component loss when cutting items up */
    if( dice( 3, 4 ) > who.dex_cur ) {
        salvagable_percent *= 0.95f;
    }
    // If more than 1 material component can still be salvaged,
    // chance of losing more components if the item is damaged.
    // If the item being cut is not damaged, no additional losses will be incurred.
    if( cut.damage() > 0 ) {
        float component_success_chance = std::min( std::pow( 0.8, cut.damage_level( 4 ) ),
                                         1.0 );
        salvagable_percent *= component_success_chance;
    }

    add_msg( m_info, _( "You try to salvage materials from the %s." ), cut.tname() );

    // Clean up before removing the item.
    remove_ammo( cut, who );
    // Original item has been consumed.
    cut.detach();
    // Force an encumbrance update in case they were wearing that item.
    who.reset_encumbrance();

    map &here = get_map();
    auto pos_here = here.getlocal( pos );
    const bool filthy = cut.is_filthy();

    for( const auto &salvaged : salvage_results( cut ) ) {
        int amount = std::floor( salvagable_percent * salvaged.second );
        if( amount > 0 ) {
            item &result = *item::spawn_temporary( salvaged.first, calendar::turn );
            // Time based on number of components.
            add_msg( m_good, vgettext( "Salvaged %1$i %2$s.", "Salvaged %1$i %2$s.", amount ),
                     amount, result.display_name( amount ) );
            if( filthy ) {
                result.set_flag( flag_FILTHY );
            }
            for( ; amount > 0; --amount ) {
                here.add_item_or_charges( pos_here, item::spawn( result ) );
            }
        } else {
            add_msg( m_bad, _( "Could not salvage a %s." ), salvaged.first->nname( 1 ) );
        }

    }
}

//Returns number of moves needed to salvage item
int moves_to_salvage( const item &target )
{
    int time = 0;
    for( auto &material : salvage_result_proportions( target ) ) {
        if( material.first && all_salvagable_materials.contains( material.first ) ) {
            //based on density, weight and proportion of material
            auto w = units::to_milligram( target.weight() );
            time += material.first->density() * material.second * w / 10000.0f;
        }
    }
    return time;
}

//Checks if inventory has tools to salvage material
bool has_salvage_tools( const inventory &inv, const material_id &material )
{
    auto it = salvage_material_quality_dictionary.find( material->id );
    if( it != salvage_material_quality_dictionary.end() ) {
        for( auto &quality : it->second ) {
            if( inv.has_quality( quality ) ) {
                return true;
            }
        }
    }
    return false;
}

//Checks if inventory has tools to salvage an item
//strict = false - check if atleast one material is salvagable with current tools
//strict = true - check all materials
bool has_salvage_tools( inventory &inv, const item &item, bool strict )
{
    //we don't want to try and salvage item with itself
    inv.remove_item( &item );
    inv.update_quality_cache();
    for( auto &material : item.made_of() ) {
        if( has_salvage_tools( inv, material ) ) {
            return true;
        } else if( strict ) {
            return false;
        }
    }
    return false;
}

bool menu_salvage_single( player &you )
{
    item *target = game_menus::inv::salvage( you );
    if( target ) {
        return prompt_salvage_single( you, *target );
    } else {
        return false;
    }
}

bool prompt_salvage_single( Character &who, item &target )
{
    map &here = get_map();
    std::string msg;
    msg += string_format( _( "Salvaging the %s may yield:\n" ),
                          colorize( target.tname(), target.color_in_inventory() ) );
    const auto components = salvage_results( target );
    for( const auto &component : components ) {
        int c = std::floor( component.second );
        //%1$s: item name, % 2$d :  count
        msg += string_format( " - %1$d %2$s\n", c, component.first->nname( c ) );
    }
    msg += "\n";
    msg += _( "Really salvage?\n" );
    if( !query_yn( msg ) ) {
        add_msg( _( "Never mind." ) );
        return false;
    }

    if( !try_salvage_silent( who, target, who.crafting_inventory() ) ) {
        return false;
    }
    iuse_location loc( target, 0 );
    who.assign_activity( std::make_unique<player_activity>(
                             std::make_unique<salvage_activity_actor>(
                                 iuse_locations{ loc }, here.getglobal( who.pos() ) ) ) );
    return true;
}

bool salvage_single( Character &who, item &target )
{
    map &here = get_map();

    if( !try_salvage_silent( who, target, who.crafting_inventory() ) ) {
        return false;
    }

    iuse_location loc( target, 0 );

    who.assign_activity( std::make_unique<player_activity>(
                             std::make_unique<salvage_activity_actor>(
                                 iuse_locations{ loc }, here.getglobal( who.pos() ) ) ) );
    return true;
}

bool salvage_all( Character &who )
{
    map &here = get_map();
    tripoint pos = who.pos();
    std::vector<iuse_location> targets;
    //yes this should NOT be a reference
    inventory inv = who.crafting_inventory();

    for( auto target : here.i_at( pos ) ) {
        if( try_salvage_silent( who, *target, inv ) ) {
            iuse_location loc;
            loc.loc = target;
            targets.push_back( std::move( loc ) );
        }
    }

    if( !targets.empty() ) {
        tripoint_abs_ms pos_abs( here.getabs( pos ) );

        who.assign_activity( std::make_unique<player_activity>
                             ( std::make_unique<salvage_activity_actor>( std::move(
                                         targets ), pos_abs ) ) );
        return true;
    } else {
        return false;
    }
}

} // namespace salvage


void salvage_activity_actor::calc_all_moves( player_activity &act, Character &who )
{
    const auto &target = targets.front();
    const std::vector<activity_req<quality_id>> q_reqs = {};
    auto reqs = activity_reqs_adapter( q_reqs, get_type()->skills,
                                       std::make_pair( target.loc->weight(), target.loc->volume() ) );

    act.speed.calc_all_moves( who, reqs );
}

void salvage_activity_actor::start( player_activity &act, Character &who )
{
    //yes this should NOT be a reference
    inventory inv = who.crafting_inventory();
    for( auto &target : targets ) {
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
        } else {
            if( progress.empty() ) {
                switch( salvage::try_salvage( who, *target.loc, inv, false, mute_promts ) ) {
                    case salvage::q_result::ignore:
                        mute_promts = true;
                        [[fallthrough]];
                    case salvage::q_result::yes:
                        progress.emplace( target.loc->tname(), salvage::moves_to_salvage( *target.loc ) );
                        break;
                    case salvage::q_result::fail:
                    case salvage::q_result::skip:
                        targets.erase( targets.begin() );
                        break;
                    case salvage::q_result::abort:
                        act.set_to_null();
                        break;
                    default:
                        break;
                }
            } else {
                progress.emplace( target.loc->tname(), salvage::moves_to_salvage( *target.loc ) );
            }
        }
    }
}

void salvage_activity_actor::do_turn( player_activity &act, Character &who )
{
    if( progress.front().complete() ) {
        auto &target = targets.front();
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
        } else {
            salvage::complete_salvage( who, *target.loc, pos );
        }
        targets.erase( targets.begin() );
        progress.pop();
    }
    if( !progress.empty() && progress.front().not_started() ) {
        auto &target = targets.front();
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
            act.set_to_null();
        } else {
            inventory inv = who.crafting_inventory();
            switch( salvage::try_salvage( who, *target.loc, inv, false, mute_promts ) ) {
                case salvage::q_result::ignore:
                    mute_promts = true;
                    [[fallthrough]];
                case salvage::q_result::yes:
                    calc_all_moves( act, who );
                    break;
                case salvage::q_result::fail:
                case salvage::q_result::skip:
                    targets.erase( targets.begin() );
                    progress.pop();
                    break;
                case salvage::q_result::abort:
                    act.set_to_null();
                    break;
                default:
                    break;
            }
        }
    }
}

void salvage_activity_actor::finish( player_activity &act, Character & )
{
    if( !progress.complete() ) {
        debugmsg( "salvage_activity_actor call finish function while able to start new salvage" );
    }
    add_msg( _( "You finish salvaging." ) );
    act.set_to_null();
}

void salvage_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "progress", progress );
    jsout.member( "targets", targets );
    jsout.member( "pos", pos );
    jsout.member( "mute_promts", mute_promts );

    jsout.end_object();
}

void populate_salvage_materials( quality &q )
{
    for( auto &material : q.salvagable_materials ) {
        salvage_material_quality_dictionary[material].emplace( q.id );
    }
    std::copy( q.salvagable_materials.begin(), q.salvagable_materials.end(),
               std::inserter( all_salvagable_materials, all_salvagable_materials.end() ) );
}

//If strict == false we check if atleast one material is salvagable
//Else we check all materials
bool item::is_salvageable( bool strict ) const
{
    if( is_null() ) {
        return false;
    }
    for( auto &mat : made_of() ) {
        if( all_salvagable_materials.contains( mat ) ) {
            if( !strict ) {
                return !has_flag( flag_NO_SALVAGE );
            }
        } else {
            if( strict ) {
                return false;
            }
        }
    }
    return strict;
}
