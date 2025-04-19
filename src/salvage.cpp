#include "activity_actor_definitions.h"
#include "salvage.h"

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
#include "generic_factory.h"

const skill_id skill_fabrication( "fabrication" );

static std::unordered_map<material_id, std::set<quality_id>> salvage_material_dictionary;
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

bool valid_to_salvage( const item &it )
{
    if( it.is_null() ) {
        return false;
    }
    // There must be some historical significance to these items.
    if( !it.is_salvageable() ) {
        return false;
    }
    if( !it.only_made_of( all_salvagable_materials ) ) {
        return false;
    }
    if( !it.contents.empty() ) {
        return false;
    }
    if( it.weight() < minimal_weight_to_cut( it ) ) {
        return false;
    }

    return true;
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

// It is used to check if an item can be salvaged or not.
bool try_salvage( Character &who, item &it, bool mute )
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
            }
        }
        return false;
    }

    if( !it.only_made_of( all_salvagable_materials ) ) {
        if( !mute ) {
            add_msg( m_info, _( "The %s is made of material that cannot be cut up." ), it.tname() );
        }
        return false;
    }
    if( !it.contents.empty() ) {
        if( !mute ) {
            add_msg( m_info, _( "Please empty the %s before cutting it up." ), it.tname() );
        }
        return false;
    }
    if( it.weight() < minimal_weight_to_cut( it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "The %s is too small to salvage material from." ), it.tname() );
        }
        return false;
    }
    if( !has_salvage_tools( who.crafting_inventory(), it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "You lack proper tools to salvage material from the %s." ), it.tname() );
        }
        return false;
    }


    // Softer warnings at the end so we don't ask permission and then tell them no.
    if( !mute ) {
        if( who.is_wielding( it ) ) {
            if( !query_yn( _( "You are wielding that, are you sure?" ) ) ) {
                return false;
            }
        } else if( who.is_wearing( it ) ) {
            if( !query_yn( _( "You're wearing that, are you sure?" ) ) ) {
                return false;
            }
        }
        if( it.is_favorite ) {
            if( !query_yn( _( "This item is marked as favorite, are you sure?" ) ) ) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::pair< material_id, float>> get_salvagable_materials( const item &target )
{
    auto materials = target.made_of();
    std::vector<std::pair< material_id, float>> salvagable_materials;
    //For now we assume that proportions for all materials are equal
    for( auto material : materials ) {
        salvagable_materials.emplace_back( material, 1.0f / materials.size() );
    }
    return salvagable_materials;
}

void complete_salvage( Character &who, item &cut, tripoint_abs_ms pos )
{
    const bool filthy = cut.is_filthy();
    float salvagable_percent = 1.0f;
    // Chance of us losing a material component to entropy.
    /** @EFFECT_FABRICATION reduces chance of losing components when cutting items up */
    int entropy_threshold = std::max( 5, 10 - who.get_skill_level( skill_fabrication ) );
    // Not much practice, and you won't get very far ripping things up.
    who.practice( skill_fabrication, rng( 0, 5 ), 1 );
    // Higher fabrication, less chance of entropy, but still a chance.
    if( rng( 1, 10 ) <= entropy_threshold ) {
        salvagable_percent *= 0.99;
    }
    // Fail dex roll, potentially lose more parts.
    /** @EFFECT_DEX randomly reduces component loss when cutting items up */
    if( dice( 3, 4 ) > who.dex_cur ) {
        salvagable_percent *= 0.95;
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
    for( const auto &salvaged : get_salvagable_materials( cut ) ) {
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

int moves_to_salvage( const item &target )
{
    int time = 0;
    for( auto &material : get_salvagable_materials( target ) ) {
        if( material.first ) {
            //based on density, weight and proportion of material
            time += 100.0f * material.first->density() * units::to_kilogram( target.weight() ) *
                    material.second;
        }
    }
    return time;
}

bool has_salvage_tools( const inventory &inv, item &item, bool check_charges )
{
    bool has_tools = false;
    for( auto &material : item.made_of_types() ) {
        auto it = salvage_material_dictionary.find( material->id );
        if( it != salvage_material_dictionary.end() ) {
            has_tools = false;
            for( auto &quality : it->second ) {
                //if(check_charges)
                if( inv.has_quality( quality ) ) {
                    has_tools = true;
                    break;
                }
            }
            if( !has_tools ) {
                return false;
            }
        } else {
            // If the material is not in the dictionary, we can't salvage item.
            return false;
        }
    }
    return true;
}

bool salvage::salvage_single( Character &who, item &target )
{
    map &here = get_map();

    if( !try_salvage( who, target, false ) ) {
        return false;
    }

    iuse_location loc;
    loc.loc = target;


    who.assign_activity( std::make_unique<player_activity>(
                             std::make_unique<salvage_activity_actor>(
                                 iuse_locations{ loc }, here.getglobal( who.pos() ) ) ) );

    return true;
}

bool salvage::salvage_all( Character &who )
{

    std::vector<iuse_location> targets;

    tripoint pos = who.pos();

    for( auto target : get_map().i_at( pos ) ) {
        if( try_salvage( who, *target, false ) ) {
            iuse_location loc;
            loc.loc = target;
            targets.push_back( std::move( loc ) );
        }
    }

    if( !targets.empty() ) {
        tripoint_abs_ms pos_abs( get_map().getabs( who.pos() ) );

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
    activity_reqs_adapter reqs;
    if( act.tools.empty() )
        reqs = activity_reqs_adapter( {}, get_type()->skills,
                                      std::make_pair( target.loc->weight(), target.loc->volume() ) );
    else
        reqs = activity_reqs_adapter( {}, get_type()->skills,
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
    if( progress.front().complete() ) {
        auto &target = targets.front();
        if( !target.loc ) {
            debugmsg( "Lost target of ", get_type() );
        } else {
            salvage::complete_salvage( who, *target.loc, pos );
        }
        targets.erase( targets.begin() );
        progress.pop();

        if( !progress.empty() ) {
            target = targets.front();
            if( !target.loc ) {
                debugmsg( "Lost target of ", get_type() );
                act.set_to_null();
            } else {
                if( salvage::try_salvage( who, *target.loc, false ) ) {
                    calc_all_moves( act, who );
                } else {
                    act.set_to_null();
                }
            }
        }
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

    jsout.end_object();
}

void populate_salvage_materials( quality &q )
{
    for( auto &material : q.salvagable_materials ) {
        salvage_material_dictionary[material].emplace( q.id );
    }
    std::copy( q.salvagable_materials.begin(), q.salvagable_materials.end(),
               std::inserter( all_salvagable_materials, all_salvagable_materials.end() ) );
}
