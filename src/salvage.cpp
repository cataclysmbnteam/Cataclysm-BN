#include "activity_actor_definitions.h"
#include "salvage.h"

#include <vector>
#include <set>
#include <unordered_map>

#include "activity_speed.h"
#include "character.h"
#include "flag.h"
#include "game.h"
#include "json.h"
#include "map.h"
#include "material.h"
#include "messages.h"
#include "output.h"
#include "player_activity.h"
#include "recipe_dictionary.h"
#include "itype.h"
#include "type_id.h"
#include "options.h"

const skill_id skill_fabrication( "fabrication" );

static std::unordered_map<material_id, std::set<quality_id>> salvage_material_quality_dictionary;
static std::set<material_id> all_salvagable_materials;

namespace salvage
{
// Helper to visit instances of all the sub-materials of an item.
void visit_salvage_products( const item &it,
                             const std::function<void( const item & )> &func )
{
    for( const material_id &material : it.made_of() ) {
        if( const std::optional<itype_id> id = material->salvaged_into() ) {
            item *tmp = item::spawn_temporary( *id );
            func( *tmp );
        }
    }
}

// Helper to find smallest sub-component of an item.
units::mass minimal_weight_to_cut( const item &it )
{
    units::mass min_weight = units::mass_max;
    visit_salvage_products( it, [&min_weight]( const item & exemplar ) {
        min_weight = std::min( min_weight, exemplar.weight() );
    } );
    return min_weight;
}

// Helper to find smallest sub-component of an item.
std::set<material_id> can_salvage_materials( const item &it )
{
    std::set<material_id> salvagable_materials;
    for( auto &quality : it.get_qualities() ) {
        std::copy( quality.first->salvagable_materials.begin(), quality.first->salvagable_materials.end(),
                   std::inserter( salvagable_materials, salvagable_materials.end() ) );
    }
    return salvagable_materials;
}

bool yn_ignore_query( const std::string &text, bool &ignore )
{
    const bool force_uc = get_option<bool>( "FORCE_CAPITAL_YN" );
    const auto &allow_key = force_uc
                            ? input_context::disallow_lower_case
                            : input_context::allow_all_keys;

    const auto &action = query_popup()
                         .context( "CANCEL_OR_IGNORE_QUERY" )
                         .message( force_uc
                                   ? pgettext( "cancel_activity_or_ignore_query",
                                           "<color_light_red>%s %s (Case Sensitive)</color>" )
                                   : pgettext( "cancel_activity_or_ignore_query",
                                           "<color_light_red>%s %s</color>" ),
                                   text, "SAMPLE_TEXT" )
                         .option( "YES", allow_key )
                         .option( "NO", allow_key )
                         .option( "IGNORE", allow_key )
                         .query()
                         .action;

    if( action == "YES" ) {
        return true;
    }
    if( action == "IGNORE" ) {
        ignore = true;
        return true;
    }
    //refresh_display();
    return false;
}

// It is used to check if an item can be salvaged or not.
bool try_salvage( Character &who, item &it, bool mute, bool mute_promts )
{
    if( it.is_null() ) {
        if( !mute ) {
            add_msg( m_info, _( "You do not have that item." ) );
        }
        return false;
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
        return false;
    }
    if( !it.contents.empty() ) {
        if( !mute ) {
            add_msg( m_info, _( "Please empty the %s before salvaged it up." ), it.tname() );
        }
        return false;
    }
    if( it.weight() < minimal_weight_to_cut( it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "The %s is too small to salvage any material from." ), it.tname() );
        }
        return false;
    }
    auto &inv = who.crafting_inventory();
    if( !has_salvage_tools( inv, it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "You lack proper tools to salvage any material from the %s." ), it.tname() );
        }
        return false;
    } else if( !( mute_promts && mute ) ) {
        for( auto &mat : it.made_of() ) {
            if( !has_salvage_tools( inv, mat ) )
                if( !query_yn( _( "You lack proper tools to salvage %s from the %s. Continue anyway?" ),
                               mat->name(), it.tname() ) ) {
                    return false;
                }
        }
    }

    // Softer warnings at the end so we don't ask permission and then tell them no.
    // mute_promts are doubled cuz previous query can override it
    if( !( mute_promts && mute ) ) {
        if( !mute_promts && who.is_wielding( it ) ) {
            if( !yn_ignore_query( _( "You are wielding that, are you sure?" ), mute_promts ) ) {
                return false;
            }
        } else if( !mute_promts && who.is_wearing( it ) ) {
            if( !yn_ignore_query( _( "You're wearing that, are you sure?" ), mute_promts ) ) {
                return false;
            }
        }
        if( !mute_promts && it.is_favorite ) {
            if( !yn_ignore_query( _( "This item is marked as favorite, are you sure?" ), mute_promts ) ) {
                return false;
            }
        }
    }
    return true;
}

//Returns vector of pairs <material, fraction>, where fraction = [0.0f, 1.0f]
std::vector<std::pair< material_id, float>> salvage_result_proportions( const item &target )
{
    auto &materials = target.made_of();
    std::vector<std::pair< material_id, float>> salvagable_materials;
    //For now we assume that proportions for all materials are equal
    for( auto &material : materials ) {
        salvagable_materials.emplace_back( material, 1.0f / materials.size() );
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

    for( const auto &salvaged : salvage_result_proportions( cut ) ) {
        if( all_salvagable_materials.contains( salvaged.first ) ) {
            auto salvaged_into = salvaged.first->salvaged_into().value();
            int amount = std::floor( ( cut.weight() * salvaged.second * salvagable_percent ) /
                                     salvaged_into->weight );
            if( amount > 0 ) {
                item &result = *item::spawn_temporary( salvaged_into, calendar::turn );
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
                add_msg( m_bad, _( "Could not salvage a %s." ), salvaged_into->nname( 1 ) );
            }
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
            time += 100.0f * material.first->density() * units::to_kilogram( target.weight() ) *
                    material.second;
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
bool has_salvage_tools( const inventory &inv, item &item, bool strict )
{
    for( auto &material : item.made_of() ) {
        if( has_salvage_tools( inv, material ) ) {
            return true;
        } else if( strict ) {
            return false;
        }
    }
    return false;
}

bool salvage::salvage_single( Character &who, item &target )
{
    map &here = get_map();
    bool mute_promts = false;

    if( !try_salvage( who, target, false, mute_promts ) ) {
        return false;
    }

    iuse_location loc( target, 0 );

    who.assign_activity( std::make_unique<player_activity>(
                             std::make_unique<salvage_activity_actor>(
                                 iuse_locations{ loc }, here.getglobal( who.pos() ) ) ) );
    return true;
}

bool salvage::salvage_all( Character &who )
{
    map &here = get_map();
    tripoint pos = who.pos();
    std::vector<iuse_location> targets;

    for( auto target : here.i_at( pos ) ) {
        if( try_salvage_silent( who, *target ) ) {
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
    std::vector<activity_req<quality_id>> q_reqs = {};
    auto reqs = activity_reqs_adapter( q_reqs, get_type()->skills,
                                       std::make_pair( target.loc->weight(), target.loc->volume() ) );

    act.speed.calc_all_moves( who, reqs );
}

void salvage_activity_actor::start( player_activity &act, Character &who )
{
    for( auto &target : targets ) {
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
        } else {
            progress.emplace( target.loc->tname(), salvage::moves_to_salvage( *target.loc ) );
        }
    }
}

void salvage_activity_actor::do_turn( player_activity &act, Character &who )
{
    if( !progress.empty() && progress.front().not_started() ) {
        auto  &target = targets.front();
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
            act.set_to_null();
        } else {
            if( salvage::try_salvage( who, *target.loc, false, mute_promts ) ) {
                calc_all_moves( act, who );
            } else {
                targets.erase( targets.begin() );
                progress.pop();
            }
        }
    } else if( progress.front().complete() ) {
        auto &target = targets.front();
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
        } else {
            salvage::complete_salvage( who, *target.loc, pos );
        }
        targets.erase( targets.begin() );
        progress.pop();
    }
}

void salvage_activity_actor::finish( player_activity &act, Character &who )
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

//We check if atleast one material is salvagable
bool item::is_salvageable() const
{
    if( is_null() ) {
        return false;
    }
    for( auto &mat : made_of() ) {
        if( all_salvagable_materials.contains( mat ) ) {
            return !has_flag( flag_NO_SALVAGE );
        }
    }
    return false;
}
