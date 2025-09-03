#include "crafting.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "activity_actor_definitions.h"
#include "activity_handlers.h"
#include "activity_speed_adapters.h"
#include "avatar.h"
#include "avatar_functions.h"
#include "bionics.h"
#include "calendar.h"
#include "cata_utility.h"
#include "character.h"
#include "character_functions.h"
#include "color.h"
#include "color.h"
#include "craft_command.h"
#include "crafting_gui.h"
#include "debug.h"
#include "enums.h"
#include "faction.h"
#include "flag.h"
#include "flat_set.h"
#include "game.h"
#include "game_constants.h"
#include "game_inventory.h"
#include "handle_liquid.h"
#include "inventory.h"
#include "item.h"
#include "item_contents.h"
#include "item_stack.h"
#include "itype.h"
#include "iuse.h"
#include "line.h"
#include "map.h"
#include "map_selector.h"
#include "mapdata.h"
#include "messages.h"
#include "mutation.h"
#include "npc.h"
#include "options.h"
#include "output.h"
#include "pimpl.h"
#include "player.h"
#include "player_activity.h"
#include "point.h"
#include "recipe.h"
#include "recipe_dictionary.h"
#include "requirements.h"
#include "ret_val.h"
#include "rng.h"
#include "skill.h"
#include "string_formatter.h"
#include "string_id.h"
#include "string_input_popup.h"
#include "string_utils.h"
#include "translations.h"
#include "type_id.h"
#include "ui.h"
#include "units.h"
#include "value_ptr.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vehicle_part.h"
#include "vehicle_selector.h"
#include "vpart_position.h"

static const activity_id ACT_CRAFT( "ACT_CRAFT" );

static const efftype_id effect_contacts( "contacts" );

static const itype_id itype_plut_cell( "plut_cell" );

static const skill_id skill_electronics( "electronics" );
static const skill_id skill_tailor( "tailor" );

static const trait_id trait_BURROW( "BURROW" );
static const trait_id trait_DEBUG_HS( "DEBUG_HS" );
static const trait_id trait_HYPEROPIC( "HYPEROPIC" );

static const flag_id flag_BIONIC_TOGGLED( "BIONIC_TOGGLED" );
static const std::string flag_FULL_MAGAZINE( "FULL_MAGAZINE" );
static const std::string flag_NO_RESIZE( "NO_RESIZE" );
static const std::string flag_UNCRAFT_LIQUIDS_CONTAINED( "UNCRAFT_LIQUIDS_CONTAINED" );

static const std::string building_category( "CC_BUILDING" );

static auto yes_filter = []( bool ok, const Character & )
{
    return ok;
};

static bool crafting_allowed( const Character &who, const recipe &rec )
{

    //TODO: add other checks

    if( rec.category == building_category ) {
        add_msg( m_info, _( "Overmap terrain building recipes are not implemented yet!" ) );
        return false;
    }
    return true;
}

void Character::craft( const tripoint &loc )
{
    int batch_size = 0;
    const recipe *rec = select_crafting_recipe( batch_size );
    if( rec ) {
        if( crafting_allowed( *this, *rec ) ) {
            make_craft( rec->ident(), batch_size, loc );
        }
    }
}

void Character::recraft( const tripoint &loc )
{
    if( lastrecipe.str().empty() ) {
        popup( _( "Craft something first" ) );
    } else if( making_would_work( lastrecipe, last_batch ) ) {
        last_craft->execute( loc );
    }
}

void Character::long_craft( const tripoint &loc )
{
    int batch_size = 0;
    const recipe *rec = select_crafting_recipe( batch_size );
    if( rec ) {
        if( crafting_allowed( *this, *rec ) ) {
            make_all_craft( rec->ident(), batch_size, loc );
        }
    }
}

bool Character::making_would_work( const recipe_id &id_to_make, int batch_size )
{
    const auto &making = *id_to_make;
    if( !( making && crafting_allowed( *this, making ) ) ) {
        return false;
    }

    if( !can_make( &making, batch_size ) ) {
        std::string buffer = _( "You can no longer make that craft!" );
        buffer += "\n";
        buffer += making.simple_requirements().list_missing();
        popup( buffer, PF_NONE );
        return false;
    }

    return check_eligible_containers_for_crafting( making, batch_size );
}

bool Character::check_eligible_containers_for_crafting( const recipe &rec, int batch_size ) const
{
    std::vector<const item *> conts = get_eligible_containers_for_crafting();
    std::vector<detached_ptr<item>> all = rec.create_results( batch_size );
    std::vector<detached_ptr<item>> bps = rec.create_byproducts( batch_size );
    all.insert( all.end(), std::make_move_iterator( bps.begin() ),
                std::make_move_iterator( bps.end() ) );

    map &here = get_map();
    for( detached_ptr<item> &prod : all ) {
        if( !prod->made_of( LIQUID ) ) {
            continue;
        }

        // we go through half-filled containers first, then go through empty containers if we need
        std::ranges::sort( conts, item_ptr_compare_by_charges );

        int charges_to_store = prod->charges;
        for( const item *cont : conts ) {
            if( charges_to_store <= 0 ) {
                break;
            }

            if( !cont->is_container_empty() ) {
                if( cont->contents.front().typeId() == prod->typeId() ) {
                    charges_to_store -= cont->get_remaining_capacity_for_liquid( cont->contents.front(), true );
                }
            } else {
                charges_to_store -= cont->get_remaining_capacity_for_liquid( *prod, true );
            }
        }

        // also check if we're currently in a vehicle that has the necessary storage
        if( charges_to_store > 0 ) {
            if( optional_vpart_position vp = here.veh_at( pos() ) ) {
                const itype_id &ftype = prod->typeId();
                int fuel_cap = vp->vehicle().fuel_capacity( ftype );
                int fuel_amnt = vp->vehicle().fuel_left( ftype );

                if( fuel_cap >= 0 ) {
                    int fuel_space_left = fuel_cap - fuel_amnt;
                    charges_to_store -= fuel_space_left;
                }
            }
        }

        if( charges_to_store > 0 ) {
            if( !query_yn(
                    _( "You don't have anything in which to store %s and may have to pour it out or consume it as soon as it is prepared!  Proceed?" ),
                    prod->tname() ) ) {
                return false;
            }
        }
    }

    return true;
}

static bool is_container_eligible_for_crafting( const item &cont, bool allow_bucket )
{
    if( cont.is_watertight_container() || ( allow_bucket && cont.is_bucket() ) ) {
        return !cont.is_container_full( allow_bucket );
    }

    return false;
}

std::vector<const item *> Character::get_eligible_containers_for_crafting() const
{
    std::vector<const item *> conts;

    for( const item *it : wielded_items() ) {
        if( is_container_eligible_for_crafting( *it, true ) ) {
            conts.push_back( it );
        }
    }
    for( const auto &it : worn ) {
        if( is_container_eligible_for_crafting( *it, false ) ) {
            conts.push_back( it );
        }
    }
    for( size_t i = 0; i < inv.size(); i++ ) {
        for( const auto &it : inv.const_stack( i ) ) {
            if( is_container_eligible_for_crafting( *it, false ) ) {
                conts.push_back( it );
            }
        }
    }

    map &here = get_map();
    // get all potential containers within PICKUP_RANGE tiles including vehicles
    for( const tripoint &loc : closest_points_first( pos(), PICKUP_RANGE ) ) {
        // can not reach this -> can not access its contents
        if( pos() != loc && !here.clear_path( pos(), loc, PICKUP_RANGE, 1, 100 ) ) {
            continue;
        }
        if( here.accessible_items( loc ) ) {
            for( const auto &it : here.i_at( loc ) ) {
                if( is_container_eligible_for_crafting( *it, true ) ) {
                    conts.emplace_back( it );
                }
            }
        }

        if( const std::optional<vpart_reference> vp = here.veh_at( loc ).part_with_feature( "CARGO",
                true ) ) {
            for( const auto &it : vp->vehicle().get_items( vp->part_index() ) ) {
                if( is_container_eligible_for_crafting( *it, false ) ) {
                    conts.emplace_back( it );
                }
            }
        }
    }

    return conts;
}

bool Character::can_make( const recipe *r, int batch_size )
{
    const inventory &crafting_inv = crafting_inventory();

    if( has_recipe( r, crafting_inv, player_activity::get_assistants( *this, yes_filter, -1 ) ) < 0 ) {
        return false;
    }

    return r->deduped_requirements().can_make_with_inventory(
               crafting_inv, r->get_component_filter(), batch_size );
}

bool Character::can_start_craft( const recipe *rec, recipe_filter_flags flags, int batch_size )
{
    if( !rec ) {
        return false;
    }

    const inventory &inv = crafting_inventory();
    return rec->deduped_requirements().can_make_with_inventory(
               inv, rec->get_component_filter( flags ), batch_size, cost_adjustment::start_only );
}

const inventory &Character::crafting_inventory( bool clear_path )
{
    return crafting_inventory( tripoint_zero, PICKUP_RANGE, clear_path );
}

const inventory &Character::crafting_inventory( const tripoint &src_pos, int radius,
        bool clear_path )
{
    tripoint inv_pos = src_pos;
    if( src_pos == tripoint_zero ) {
        inv_pos = pos();
    }
    if( cached_moves == moves
        && cached_time == calendar::turn
        && cached_position == inv_pos ) {
        return cached_crafting_inventory;
    }
    cached_crafting_inventory.form_from_map( inv_pos, radius, this, false, clear_path );
    cached_crafting_inventory.add_items( inv, true );
    cached_crafting_inventory.add_item( primary_weapon(), true );
    cached_crafting_inventory.add_items( worn, true );
    for( const bionic &bio : get_bionic_collection() ) {
        const bionic_data &bio_data = bio.info();
        if( ( !bio_data.has_flag( flag_BIONIC_TOGGLED ) || bio.powered ) &&
            !bio_data.fake_item.is_empty() ) {
            cached_crafting_inventory.add_item( *item::spawn_temporary( bio.info().fake_item, calendar::turn,
                                                units::to_kilojoule( get_power_level() ) ), true );
        }
    }
    if( has_trait( trait_BURROW ) ) {
        cached_crafting_inventory.add_item( *item::spawn_temporary( "pickaxe", calendar::turn ), true );
        cached_crafting_inventory.add_item( *item::spawn_temporary( "shovel", calendar::turn ), true );
    }

    cached_moves = moves;
    cached_time = calendar::turn;
    cached_position = inv_pos;
    // cache the qualities of the items in cached_crafting_inventory
    cached_crafting_inventory.update_quality_cache();
    return cached_crafting_inventory;
}

void Character::invalidate_crafting_inventory()
{
    cached_time = calendar::before_time_starts;
    cached_position = tripoint_min;
}

void Character::make_craft( const recipe_id &id_to_make, int batch_size, const tripoint &loc )
{
    make_craft_with_command( id_to_make, batch_size, false, loc );
}

void Character::make_all_craft( const recipe_id &id_to_make, int batch_size, const tripoint &loc )
{
    make_craft_with_command( id_to_make, batch_size, true, loc );
}

void Character::make_craft_with_command( const recipe_id &id_to_make, int batch_size, bool is_long,
        const tripoint &loc )
{
    const auto &recipe_to_make = *id_to_make;

    if( !recipe_to_make ) {
        return;
    }

    *last_craft = craft_command( &recipe_to_make, batch_size, is_long, this, loc );
    last_craft->execute();
}

// @param offset is the index of the created item in the range [0, batch_size-1],
// it makes sure that the used items are distributed equally among the new items.
static void set_components( item &of, const std::vector<item *> &used,
                            const int batch_size, const size_t offset )
{
    location_vector<item> &components = of.get_components();
    if( batch_size <= 1 ) {
        for( item * const &it : used ) {
            components.push_back( item::spawn( *it ) );
        }
        return;
    }
    // This count does *not* include items counted by charges!
    size_t non_charges_counter = 0;
    for( auto &tmp : used ) {
        if( tmp->count_by_charges() ) {
            components.push_back( item::spawn( *tmp ) );
            // This assumes all (count-by-charges) items of the same type have been merged into one,
            // which has a charges value that can be evenly divided by batch_size.
            components.back()->charges = tmp->charges / batch_size;
        } else {
            if( ( non_charges_counter + offset ) % batch_size == 0 ) {
                components.push_back( item::spawn( *tmp ) );
            }
            non_charges_counter++;
        }
    }
}

/**
 * Helper for @ref set_item_map_or_vehicle
 * This is needed to still get a vaild item_location if overflow occurs
 */
static void set_item_map( const tripoint &loc, detached_ptr<item> &&newit )
{
    // Includes loc
    for( const tripoint &tile : closest_points_first( loc, 2 ) ) {
        // Pass false to disallow overflow, null_item_reference indicates failure.
        newit = get_map().add_item_or_charges( tile, std::move( newit ), false );
        if( !newit ) {
            return;
        }
    }
    debugmsg( "Could not place %s on map near (%d, %d, %d)", newit->tname(), loc.x, loc.y, loc.z );
    return;
}

/**
 * Set an item on the map or in a vehicle and return the new location
 */
static void set_item_map_or_vehicle( const Character &who, const tripoint &loc,
                                     detached_ptr<item> &&newit )
{
    if( !newit ) {
        return;
    }
    map &here = get_map();
    if( const std::optional<vpart_reference> vp = here.veh_at( loc ).part_with_feature( "CARGO",
            false ) ) {

        item &obj = *newit;
        newit = vp->vehicle().add_item( vp->part_index(), std::move( newit ) );
        if( !newit ) {
            who.add_msg_player_or_npc(
                pgettext( "item, furniture", "You put the %1$s on the %2$s." ),
                pgettext( "item, furniture", "<npcname> puts the %1$s on the %2$s." ),
                obj.tname(), vp->part().name() );
            return;
        }

        // Couldn't add the in progress craft to the target part, so drop it to the map.
        who.add_msg_player_or_npc(
            pgettext( "furniture, item", "Not enough space on the %s. You drop the %s on the ground." ),
            pgettext( "furniture, item", "Not enough space on the %s. <npcname> drops the %s on the ground." ),
            vp->part().name(), newit->tname() );

        return set_item_map( loc, std::move( newit ) );

    } else {
        if( here.has_furn( loc ) ) {
            const furn_t &workbench = *here.furn( loc );
            who.add_msg_player_or_npc(
                pgettext( "item, furniture", "You put the %1$s on the %2$s." ),
                pgettext( "item, furniture", "<npcname> puts the %1$s on the %2$s." ),
                newit->tname(), workbench.name() );
        } else {
            who.add_msg_player_or_npc(
                pgettext( "item", "You put the %s on the ground." ),
                pgettext( "item", "<npcname> puts the %s on the ground." ),
                newit->tname() );
        }
        return set_item_map( loc, std::move( newit ) );
    }
}

static void set_item_inventory( Character &who, detached_ptr<item> &&newit )
{
    who.inv_assign_empty_invlet( *newit );
    // We might not have space for the item
    if( who.can_pick_volume( *newit ) &&
        who.can_pick_weight( *newit, !get_option<bool>( "DANGEROUS_PICKUPS" ) ) ) {
        add_msg( m_info, "%c - %s", newit->invlet == 0 ? ' ' : newit->invlet,
                 newit->tname() );
        who.i_add( std::move( newit ) );
        return;
    }

    return set_item_map_or_vehicle( who, who.pos(), std::move( newit ) );
}

item *Character::start_craft( craft_command &command, const tripoint & )
{
    if( command.empty() ) {
        debugmsg( "Attempted to start craft with empty command" );
        return nullptr;
    }

    detached_ptr<item> craft = command.create_in_progress_craft();
    const recipe &making = craft->get_making();
    if( get_skill_level( command.get_skill_id() ) > making.difficulty * 1.25 ) {
        character_funcs::show_skill_capped_notice( *this, command.get_skill_id() );
    }

    // In case we were wearing something just consumed
    if( !craft->get_components().empty() ) {
        reset_encumbrance();
    }

    // Regardless of whether a workbench exists or not,
    // we still craft in inventory or under player, because QoL.
    item *craft_in_world = &*craft;
    set_item_inventory( *this, std::move( craft ) );

    assign_activity( std::make_unique<player_activity>(
                         std::make_unique<crafting_activity_actor>(
                             craft_in_world, command.is_long() ) ) );


    add_msg_player_or_npc(
        pgettext( "in progress craft", "You start working on the %s." ),
        pgettext( "in progress craft", "<npcname> starts working on the %s." ),
        craft_in_world->tname() );
    return craft_in_world;
}

void Character::craft_skill_gain( const item &craft, const int &multiplier )
{
    if( !craft.is_craft() ) {
        debugmsg( "craft_skill_check() called on non-craft '%s.' Aborting.", craft.tname() );
        return;
    }

    const recipe &making = craft.get_making();
    const int batch_size = craft.charges;

    std::vector<npc *> helpers = character_funcs::get_crafting_helpers( *this );

    if( making.skill_used ) {
        // Normalize experience gain to crafting time, giving a bonus for longer crafting
        const double batch_mult = batch_size + making.batch_time( batch_size ) / 30000.0  ;
        // This is called after every 5% crafting progress, so divide by 20
        // TODO: Don't multiply, instead divide the crafting time into more "learn bits"
        const int base_practice = roll_remainder( ( making.difficulty * 15 + 10 ) * batch_mult /
                                  20.0 ) * multiplier;
        const int skill_cap = static_cast<int>( making.difficulty * 1.25 );
        practice( making.skill_used, base_practice, skill_cap, true );
        // Subskills gain half the experience as primary skill
        for( const auto &pr : making.required_skills ) {
            if( pr.first != making.skill_used && !pr.first->is_combat_skill() ) {
                const int secondary_practice = roll_remainder( ( get_skill_level( pr.first ) * 15 + 10 ) *
                                               batch_mult /
                                               20.0 ) * multiplier / 2.0;
                const int skill_cap_secondary = static_cast<int>( pr.second * 1.25 );
                practice( pr.first, secondary_practice, skill_cap_secondary, true );
            }
        }

        // NPCs assisting or watching should gain experience...
        for( auto &helper : helpers ) {
            //If the NPC can understand what you are doing, they gain more exp
            if( helper->get_skill_level( making.skill_used ) >= making.difficulty ) {
                helper->practice( making.skill_used, roll_remainder( base_practice / 2.0 ),
                                  skill_cap );
                if( batch_size > 1 && one_in( 3 ) ) {
                    add_msg( m_info, _( "%s assists with crafting…" ), helper->name );
                }
                if( batch_size == 1 && one_in( 3 ) ) {
                    add_msg( m_info, _( "%s could assist you with a batch…" ), helper->name );
                }
                // NPCs around you understand the skill used better
            } else {
                helper->practice( making.skill_used, roll_remainder( base_practice / 10.0 ),
                                  skill_cap );
                if( one_in( 3 ) ) {
                    add_msg( m_info, _( "%s watches you craft…" ), helper->name );
                }
            }
        }
    }
}

static double crafting_success_roll( const recipe &making, const Character &who,
                                     std::vector<npc *> &assistants )
{
    int secondary_dice = 0;
    int secondary_difficulty = 0;
    for( const auto &pr : making.required_skills ) {
        secondary_dice += who.get_skill_level( pr.first );
        secondary_difficulty += pr.second;
    }

    // # of dice is 75% primary skill, 25% secondary (unless secondary is null)
    int skill_dice;
    if( secondary_difficulty > 0 ) {
        skill_dice = who.get_skill_level( making.skill_used ) * 3 + secondary_dice;
    } else {
        skill_dice = who.get_skill_level( making.skill_used ) * 4;
    }

    for( const npc *guy : assistants ) {
        if( guy->get_skill_level( making.skill_used ) >=
            who.get_skill_level( making.skill_used ) ) {
            // NPC assistance is worth half a skill level
            skill_dice += 2;
            who.add_msg_if_player( m_info, _( "%s helps with crafting…" ), guy->name );
            break;
        }
    }

    // farsightedness can impose a penalty on electronics and tailoring success
    // it's equivalent to a 2-rank electronics penalty, 1-rank tailoring
    if( who.has_trait( trait_HYPEROPIC ) && !who.worn_with_flag( flag_FIX_FARSIGHT ) &&
        !who.has_effect( effect_contacts ) ) {
        int main_rank_penalty = 0;
        if( making.skill_used == skill_electronics ) {
            main_rank_penalty = 2;
        } else if( making.skill_used == skill_tailor ) {
            main_rank_penalty = 1;
        }
        skill_dice -= main_rank_penalty * 4;
    }

    // It's tough to craft with paws.  Fortunately it's just a matter of grip and fine-motor,
    // not inability to see what you're doing
    for( const trait_id &mut : who.get_mutations() ) {
        for( const std::pair<const skill_id, int> &skib : mut->craft_skill_bonus ) {
            if( making.skill_used == skib.first ) {
                skill_dice += skib.second;
            }
        }
    }

    // Sides on dice is 16 plus your current intelligence
    ///\EFFECT_INT increases crafting success chance
    const int skill_sides = 16 + who.int_cur;

    int diff_dice;
    if( secondary_difficulty > 0 ) {
        diff_dice = making.difficulty * 3 + secondary_difficulty;
    } else {
        // Since skill level is * 4 also
        diff_dice = making.difficulty * 4;
    }

    const int diff_sides = 24; // 16 + 8 (default intelligence)

    const double skill_roll = dice( skill_dice, skill_sides );
    const double diff_roll = dice( diff_dice, diff_sides );

    if( diff_roll == 0 ) {
        // Automatic success
        return 2;
    }

    return skill_roll / diff_roll;
}

int item::get_next_failure_point() const
{
    if( !is_craft() ) {
        debugmsg( "get_next_failure_point() called on non-craft '%s.'  Aborting.", tname() );
        return INT_MAX;
    }
    return craft_data_->next_failure_point >= 0 ? craft_data_->next_failure_point : INT_MAX;
}

void item::set_next_failure_point( const Character &crafter, int failure_point_delta )
{
    if( !is_craft() ) {
        debugmsg( "set_next_failure_point() called on non-craft '%s.'  Aborting.", tname() );
        return;
    }

    craft_data_->next_failure_point = item_counter + failure_point_delta;
}

static void destroy_random_component( item &craft, const Character &crafter )
{
    if( craft.get_components().empty() ) {
        debugmsg( "destroy_random_component() called on craft with no components!  Aborting" );
        return;
    }

    detached_ptr<item> destroyed = random_entry_detached( craft.get_components() );

    crafter.add_msg_player_or_npc( _( "You mess up and destroy the %s." ),
                                   _( "<npcname> messes up and destroys the %s" ), destroyed->tname() );
}

/**
 * Handle failure during crafting.
 * Destroy components, lose progress, and set a new failure point.
 * @param crafter the crafting player.
 * @return whether the craft being worked on should be entirely destroyed
 */
bool item::handle_craft_failure( Character &crafter, const double success_roll )
{
    if( !is_craft() ) {
        debugmsg( "handle_craft_failure() called on non-craft '%s.'  Aborting.", tname() );
        return false;
    }

    const int starting_components = this->components.size();
    // Destroy at most 75% of the components, always a chance of losing 1 though
    const size_t max_destroyed = std::max<size_t>( 1, components.size() * 3 / 4 );
    for( size_t i = 0; i < max_destroyed; i++ ) {
        // This shouldn't happen
        if( components.empty() ) {
            break;
        }
        // If we roll success, skip destroying a component
        if( x_in_y( success_roll, 1.0 ) ) {
            continue;
        }
        destroy_random_component( *this, crafter );
    }
    if( starting_components > 0 && this->components.empty() ) {
        // The craft had components and all of them were destroyed.
        return true;
    }

    // Minimum 25% progress lost, average 35%.  Falls off exponentially
    // Loss is scaled by the success roll
    const double percent_progress_loss = rng_exponential( 0.25, 0.35 ) *
                                         ( 1.0 - std::min( success_roll, 1.0 ) );
    const int progess_loss = item_counter * percent_progress_loss;
    crafter.add_msg_player_or_npc( _( "You mess up and lose %d%% progress." ),
                                   _( "<npcname> messes up and loses %d%% progress." ), progess_loss / 100000 );
    item_counter = clamp( item_counter - progess_loss, 0, 10000000 );

    // Check if we can consume a new component and continue
    if( !crafter.can_continue_craft( *this ) ) {
        crafter.cancel_activity();
    }
    return false;
}

requirement_data item::get_continue_reqs() const
{
    if( !is_craft() ) {
        debugmsg( "get_continue_reqs() called on non-craft '%s.'  Aborting.", tname() );
        return requirement_data();
    }
    return requirement_data::continue_requirements( craft_data_->comps_used, components.as_vector() );
}

void item::inherit_flags( const item &parent, const recipe &making )
{
    // default behavior is to resize the clothing, which happens elsewhere
    if( making.has_flag( flag_NO_RESIZE ) ) {
        //If item is crafted from poor-fit components, the result is poorly fitted too
        if( parent.has_flag( flag_VARSIZE ) ) {
            unset_flag( flag_FIT );
        }
        //If item is crafted from perfect-fit components, the result is perfectly fitted too
        if( parent.has_flag( flag_FIT ) ) {
            set_flag( flag_FIT );
        }
    }
    for( const flag_id &f : parent.get_flags() ) {
        if( f->craft_inherit() ) {
            set_flag( f );
        }
    }
    for( const flag_id &f : parent.type->get_flags() ) {
        if( f->craft_inherit() ) {
            set_flag( f );
        }
    }
    if( parent.has_flag( flag_HIDDEN_POISON ) ) {
        poison = parent.poison;
    }
}

void item::inherit_flags( const std::vector<item *> &parents, const recipe &making )
{
    for( const item * const &parent : parents ) {
        inherit_flags( *parent, making );
    }
}

void complete_craft( Character &who, item &craft )
{
    if( !craft.is_craft() ) {
        debugmsg( "complete_craft() called on non-craft '%s.'  Aborting.", craft.tname() );
        return;
    }

    const recipe &making = craft.get_making();
    const int batch_size = craft.charges;
    std::vector<detached_ptr<item>> used = craft.remove_components();
    std::vector<item *> used_items;
    used_items.reserve( used.size() );
    for( detached_ptr<item> &it : used ) {
        used_items.push_back( &*it );
    }
    const double relative_rot = craft.get_relative_rot();
    const bool ignore_component = making.has_flag( "NUTRIENT_OVERRIDE" );

    // Set up the new item, and assign an inventory letter if available
    std::vector<detached_ptr<item>> newits = making.create_results( batch_size );

    const bool should_heat = making.hot_result();
    const bool is_dehydrated = making.dehydrate_result();

    bool first = true;
    size_t newit_counter = 0;
    for( detached_ptr<item> &newit : newits ) {

        // Points to newit unless newit is a non-empty container, then it points to newit's contents.
        // Necessary for things like canning soup; sometimes we want to operate on the soup, not the can.
        item &food_contained = ( newit->is_container() && !newit->contents.empty() ) ?
                               newit->contents.back() : *newit;

        // messages, learning of recipe, food spoilage calculation only once
        if( first ) {
            first = false;
            // TODO: reconsider recipe memorization
            if( who.knows_recipe( &making ) ) {
                add_msg( _( "You craft %s from memory." ), making.result_name() );
            } else {
                add_msg( _( "You craft %s using a book as a reference." ), making.result_name() );
                // If we made it, but we don't know it,
                // we're making it from a book and have a chance to learn it.
                // Base expected time to learn is 1000*(difficulty^4)/skill/int moves.
                // This means time to learn is greatly decreased with higher skill level,
                // but also keeps going up as difficulty goes up.
                // Worst case is lvl 10, which will typically take
                // 10^4/10 (1,000) minutes, or about 16 hours of crafting it to learn.
                int difficulty = who.has_recipe( &making, who.crafting_inventory(),
                                                 character_funcs::get_crafting_helpers( who ) );
                ///\EFFECT_INT increases chance to learn recipe when crafting from a book
                const double learning_speed =
                    std::max( who.get_skill_level( making.skill_used ), 1 ) *
                    std::max( who.get_int(), 1 );
                const double time_to_learn = 1000 * 8 * std::pow( difficulty, 4 ) / learning_speed;
                if( x_in_y( making.time, time_to_learn ) ) {
                    who.learn_recipe( &making );
                    add_msg( m_good, _( "You memorized the recipe for %s!" ),
                             making.result_name() );
                }
            }
        }


        food_contained.inherit_flags( used_items, making );

        for( const flag_id &flag : making.flags_to_delete ) {
            food_contained.unset_flag( flag );
        }

        // Don't store components for things that ignores components (e.g wow 'conjured bread')
        if( ignore_component ) {
            food_contained.set_flag( flag_NUTRIENT_OVERRIDE );
        } else if( recipe_dictionary::get_uncraft( making.result() ) &&
                   !food_contained.count_by_charges() &&
                   making.is_reversible() ) {
            // Don't store components for things made by charges,
            // Don't store components for things that can't be uncrafted.

            // Setting this for items counted by charges gives only problems:
            // those items are automatically merged everywhere (map/vehicle/inventory),
            // which would either lose this information or merge it somehow.
            set_components( food_contained, used_items, batch_size, newit_counter );
            newit_counter++;
        } else if( food_contained.is_food() && !food_contained.has_flag( flag_NUTRIENT_OVERRIDE ) ) {
            // if a component item has "cooks_like" it will be replaced by that item as a component
            for( detached_ptr<item> &comp : used ) {
                // only comestibles have cooks_like.  any other type of item will throw an exception, so filter those out
                if( comp->is_comestible() && !comp->get_comestible()->cooks_like.is_empty() ) {
                    comp = item::spawn( comp->get_comestible()->cooks_like, comp->birthday(), comp->charges );
                }
                // If this recipe is cooked or dehydrated, components are no longer raw.
                if( should_heat || is_dehydrated ) {
                    comp->set_flag_recursive( flag_COOKED );
                }
            }

            // byproducts get stored as a "component" but with a byproduct flag for consumption purposes
            if( making.has_byproducts() ) {
                for( detached_ptr<item> &byproduct : making.create_byproducts( batch_size ) ) {
                    byproduct->set_flag( flag_BYPRODUCT );
                    used.push_back( std::move( byproduct ) );
                }
            }
            // store components for food recipes that do not have the override flag
            set_components( food_contained, used_items, batch_size, newit_counter );

            // store the number of charges the recipe would create with batch size 1.
            //TODO!: check what ref level should be compared here
            if( newit != &food_contained ) {  // If a canned/contained item was crafted…
                // … the container holds exactly one completion of the recipe, no matter the batch size.
                food_contained.recipe_charges = food_contained.charges;
            } else { // Otherwise, the item is already stacked so we need to divide by batch size.
                newit->recipe_charges = newit->charges / batch_size;
            }
            newit_counter++;
        }

        if( food_contained.goes_bad() ) {
            food_contained.set_relative_rot( relative_rot );
        }

        newit->set_owner( who.get_faction()->id );
        // If these aren't equal, newit is a container, so finalize its contents too.
        //TODO!: same as above
        if( newit != &food_contained ) {
            food_contained.set_owner( who.get_faction()->id );
        }

        // If we created a tool that spawns empty, don't preset its ammotype.
        if( !newit->ammo_remaining() ) {
            newit->ammo_unset();
        }
        if( newit->made_of( LIQUID ) ) {
            liquid_handler::handle_all_liquid( std::move( newit ), PICKUP_RANGE );
        } else {
            set_item_inventory( who, std::move( newit ) );
        }
    }

    if( making.has_byproducts() ) {
        std::vector<detached_ptr<item>> bps = making.create_byproducts( batch_size );
        for( auto &bp : bps ) {
            if( bp->goes_bad() ) {
                bp->set_relative_rot( relative_rot );
            }
            bp->set_owner( who.get_faction()->id );
            bp->inherit_flags( used_items, making );
            if( bp->made_of( LIQUID ) ) {
                liquid_handler::handle_all_liquid( std::move( bp ), PICKUP_RANGE );
            } else {
                set_item_inventory( who, std::move( bp ) );
            }
        }
    }

    who.inv_restack( );
}

int expected_time_to_craft( Character &who, const recipe &rec,
                            const int batch_size )
{
    auto speed = crafting_activity_actor::speed_preset( who, rec );
    return rec.batch_time( batch_size ) * 100 / speed.total_moves();
}

bool Character::can_continue_craft( item &craft )
{
    if( !craft.is_craft() ) {
        debugmsg( "complete_craft() called on non-craft '%s.'  Aborting.", craft.tname() );
        return false;
    }
    if( has_trait( trait_DEBUG_HS ) ) {
        return true;
    }

    const recipe &rec = craft.get_making();

    const requirement_data continue_reqs = craft.get_continue_reqs();

    // Avoid building an inventory from the map if we don't have to, as it is expensive
    if( !continue_reqs.is_empty() ) {

        std::function<bool( const item & )> filter = rec.get_component_filter();
        const std::function<bool( const item & )> no_rotten_filter =
            rec.get_component_filter( recipe_filter_flags::no_rotten );
        // continue_reqs are for all batches at once
        const int batch_size = 1;

        if( !continue_reqs.can_make_with_inventory( crafting_inventory(), filter, batch_size ) ) {
            std::string buffer = _( "You don't have the required components to continue crafting!" );
            buffer += "\n";
            buffer += continue_reqs.list_missing();
            popup( buffer, PF_NONE );
            return false;
        }

        std::string buffer = _( "Consume the missing components and continue crafting?" );
        buffer += "\n";
        buffer += continue_reqs.list_all();
        if( !query_yn( buffer ) ) {
            return false;
        }

        if( continue_reqs.can_make_with_inventory( crafting_inventory(), no_rotten_filter,
                batch_size ) ) {
            filter = no_rotten_filter;
        } else {
            if( !query_yn( _( "Some components required to continue are rotten.\n"
                              "Continue crafting anyway?" ) ) ) {
                return false;
            }
        }

        inventory map_inv;
        map_inv.form_from_map( pos(), PICKUP_RANGE, this );

        std::vector<comp_selection<item_comp>> item_selections;
        for( const auto &it : continue_reqs.get_components() ) {
            comp_selection<item_comp> is = select_item_component( it, batch_size, map_inv, true, filter );
            if( is.use_from == usage_from::cancel ) {
                cancel_activity();
                add_msg( _( "You stop crafting." ) );
                return false;
            }
            item_selections.push_back( is );
        }
        for( const auto &it : item_selections ) {
            std::vector<detached_ptr<item>> items = consume_items( it, batch_size, filter );
            for( detached_ptr<item> &it : items ) {
                craft.add_component( std::move( it ) );
            }
        }
    }

    if( !craft.has_tools_to_continue() ) {

        const std::vector<std::vector<tool_comp>> &tool_reqs = rec.simple_requirements().get_tools();
        const int batch_size = craft.charges;

        std::vector<std::vector<tool_comp>> adjusted_tool_reqs;
        for( const std::vector<tool_comp> &alternatives : tool_reqs ) {
            std::vector<tool_comp> adjusted_alternatives;
            for( const tool_comp &alternative : alternatives ) {
                tool_comp adjusted_alternative = alternative;
                if( adjusted_alternative.count > 0 ) {
                    adjusted_alternative.count *= batch_size;
                    // Only for the next 5% progress
                    adjusted_alternative.count = std::max(
                                                     crafting::charges_for_continuing( adjusted_alternative.count ), 1 );
                }
                adjusted_alternatives.push_back( adjusted_alternative );
            }
            adjusted_tool_reqs.push_back( adjusted_alternatives );
        }

        const requirement_data tool_continue_reqs( adjusted_tool_reqs,
                std::vector<std::vector<quality_requirement>>(),
                std::vector<std::vector<item_comp>>() );

        if( !tool_continue_reqs.can_make_with_inventory( crafting_inventory(), return_true<item> ) ) {
            std::string buffer = _( "You don't have the necessary tools to continue crafting!" );
            buffer += "\n";
            buffer += tool_continue_reqs.list_missing();
            popup( buffer, PF_NONE );
            return false;
        }

        inventory map_inv;
        map_inv.form_from_map( pos(), PICKUP_RANGE, this );

        std::vector<comp_selection<tool_comp>> new_tool_selections;
        for( const std::vector<tool_comp> &alternatives : tool_reqs ) {
            comp_selection<tool_comp> selection =
                crafting::select_tool_component( alternatives, batch_size, map_inv, this, true, DEFAULT_HOTKEYS,
                                                 cost_adjustment::continue_only );
            if( selection.use_from == usage_from::cancel ) {
                return false;
            }
            new_tool_selections.push_back( selection );
        }

        craft.set_cached_tool_selections( new_tool_selections );
        craft.set_tools_to_continue( true );
    }

    return true;
}
const requirement_data *Character::select_requirements(
    const std::vector<const requirement_data *> &alternatives, int batch, const inventory &inv,
    const std::function<bool( const item & )> &filter ) const
{
    assert( !alternatives.empty() );
    if( alternatives.size() == 1 || !is_avatar() ) {
        return alternatives.front();
    }

    uilist menu;

    for( const requirement_data *req : alternatives ) {
        // Write with a large width and then just re-join the lines, because
        // uilist does its own wrapping and we want to rely on that.
        std::vector<std::string> component_lines =
            req->get_folded_components_list( TERMX - 4, c_light_gray, inv, filter, batch, "",
                                             requirement_display_flags::no_unavailable );
        menu.addentry_desc( "", join( component_lines, "\n" ) );
    }

    menu.allow_cancel = true;
    menu.desc_enabled = true;
    menu.title = _( "Use which selection of components?" );
    menu.query();

    if( menu.ret < 0 || static_cast<size_t>( menu.ret ) >= alternatives.size() ) {
        return nullptr;
    }

    return alternatives[menu.ret];
}

/* selection of component if a recipe requirement has multiple options (e.g. 'duct tap' or 'welder') */
comp_selection<item_comp> Character::select_item_component( const std::vector<item_comp>
        &components,
        int batch, inventory &map_inv, bool can_cancel,
        const std::function<bool( const item & )> &filter, bool player_inv )
{
    std::vector<item_comp> player_has;
    std::vector<item_comp> map_has;
    std::vector<item_comp> mixed;

    comp_selection<item_comp> selected;

    for( const auto &component : components ) {
        itype_id type = component.type;
        int count = ( component.count > 0 ) ? component.count * batch : std::abs( component.count );

        if( item::count_by_charges( type ) && count > 0 ) {
            int map_charges = map_inv.charges_of( type, INT_MAX, filter );

            // If map has infinite charges, just use them
            if( map_charges == item::INFINITE_CHARGES ) {
                selected.use_from = usage_from::map;
                selected.comp = component;
                return selected;
            }
            if( player_inv ) {
                int player_charges = charges_of( type, INT_MAX, filter );
                bool found = false;
                if( player_charges >= count ) {
                    player_has.push_back( component );
                    found = true;
                }
                if( map_charges >= count ) {
                    map_has.push_back( component );
                    found = true;
                }
                if( !found && player_charges + map_charges >= count ) {
                    mixed.push_back( component );
                }
            } else {
                if( map_charges >= count ) {
                    map_has.push_back( component );
                }
            }
        } else { // Counting by units, not charges

            // Can't use pseudo items as components
            if( player_inv ) {
                bool found = false;
                if( has_amount( type, count, false, filter ) ) {
                    player_has.push_back( component );
                    found = true;
                }
                if( map_inv.has_components( type, count, filter ) ) {
                    map_has.push_back( component );
                    found = true;
                }
                if( !found &&
                    amount_of( type, false, std::numeric_limits<int>::max(), filter ) +
                    map_inv.amount_of( type, false, std::numeric_limits<int>::max(), filter ) >= count ) {
                    mixed.push_back( component );
                }
            } else {
                if( map_inv.has_components( type, count, filter ) ) {
                    map_has.push_back( component );
                }
            }
        }
    }

    /* select 1 component to use */
    if( player_has.size() + map_has.size() + mixed.size() == 1 ) { // Only 1 choice
        if( player_has.size() == 1 ) {
            selected.use_from = usage_from::player;
            selected.comp = player_has[0];
        } else if( map_has.size() == 1 ) {
            selected.use_from = usage_from::map;
            selected.comp = map_has[0];
        } else {
            selected.use_from = usage_from::both;
            selected.comp = mixed[0];
        }
    } else if( is_npc() ) {
        if( !player_has.empty() ) {
            selected.use_from = usage_from::player;
            selected.comp = player_has[0];
        } else if( !map_has.empty() ) {
            selected.use_from = usage_from::map;
            selected.comp = map_has[0];
        } else {
            debugmsg( "Attempted a recipe with no available components!" );
            selected.use_from = usage_from::cancel;
            return selected;
        }
    } else { // Let the player pick which component they want to use
        uilist cmenu;
        // Populate options with the names of the items
        for( auto &map_ha : map_has ) { // Index 0-(map_has.size()-1)
            std::string tmpStr = string_format( _( "%s (%d/%d nearby)" ),
                                                item::nname( map_ha.type ),
                                                ( map_ha.count * batch ),
                                                item::count_by_charges( map_ha.type ) ?
                                                map_inv.charges_of( map_ha.type, INT_MAX, filter ) :
                                                map_inv.amount_of( map_ha.type, false, INT_MAX, filter ) );
            cmenu.addentry( tmpStr );
        }
        for( auto &player_ha : player_has ) { // Index map_has.size()-(map_has.size()+player_has.size()-1)
            std::string tmpStr = string_format( _( "%s (%d/%d on person)" ),
                                                item::nname( player_ha.type ),
                                                ( player_ha.count * batch ),
                                                item::count_by_charges( player_ha.type ) ?
                                                charges_of( player_ha.type, INT_MAX, filter ) :
                                                amount_of( player_ha.type, false, INT_MAX, filter ) );
            cmenu.addentry( tmpStr );
        }
        for( auto &component : mixed ) {
            // Index player_has.size()-(map_has.size()+player_has.size()+mixed.size()-1)
            int available = item::count_by_charges( component.type ) ?
                            map_inv.charges_of( component.type, INT_MAX, filter ) +
                            charges_of( component.type, INT_MAX, filter ) :
                            map_inv.amount_of( component.type, false, INT_MAX, filter ) +
                            amount_of( component.type, false, INT_MAX, filter );
            std::string tmpStr = string_format( _( "%s (%d/%d nearby & on person)" ),
                                                item::nname( component.type ),
                                                component.count * batch,
                                                available );
            cmenu.addentry( tmpStr );
        }

        // Unlike with tools, it's a bad thing if there aren't any components available
        if( cmenu.entries.empty() ) {
            if( player_inv ) {
                if( has_trait( trait_DEBUG_HS ) ) {
                    selected.use_from = usage_from::player;
                    return selected;
                }
            }
            debugmsg( "Attempted a recipe with no available components!" );
            selected.use_from = usage_from::cancel;
            return selected;
        }

        cmenu.allow_cancel = can_cancel;

        // Get the selection via a menu popup
        cmenu.title = _( "Use which component?" );
        cmenu.query();

        if( cmenu.ret < 0 ||
            static_cast<size_t>( cmenu.ret ) >= map_has.size() + player_has.size() + mixed.size() ) {
            selected.use_from = usage_from::cancel;
            return selected;
        }

        size_t uselection = static_cast<size_t>( cmenu.ret );
        if( uselection < map_has.size() ) {
            selected.use_from = usage_from::map;
            selected.comp = map_has[uselection];
        } else if( uselection < map_has.size() + player_has.size() ) {
            uselection -= map_has.size();
            selected.use_from = usage_from::player;
            selected.comp = player_has[uselection];
        } else {
            uselection -= map_has.size() + player_has.size();
            selected.use_from = usage_from::both;
            selected.comp = mixed[uselection];
        }
    }

    return selected;
}

static void drop_or_handle( detached_ptr<item> &&newit, Character &who )
{
    if( newit->made_of( LIQUID ) && who.is_avatar() ) {
        // TODO: what about NPCs?
        liquid_handler::handle_all_liquid( std::move( newit ), PICKUP_RANGE );
    } else {
        who.as_player()->i_add_or_drop( std::move( newit ) );
    }
}

// Prompts player to empty all newly-unsealed containers in inventory
// Called after something that might have opened containers (making them buckets) but not emptied them
static void empty_buckets( Character &p )
{
    // First grab (remove) all items that are non-empty buckets and not wielded
    std::vector<detached_ptr<item>> buckets;
    p.remove_items_with( [&p, &buckets]( detached_ptr<item> &&it ) {
        if( it->is_bucket_nonempty() && !p.is_wielding( *it ) ) {
            buckets.push_back( std::move( it ) );
        }
        return VisitResponse::SKIP;
    } );
    for( auto &it : buckets ) {
        for( detached_ptr<item> &in : it->contents.clear_items() ) {
            drop_or_handle( std::move( in ), p );
        }

        drop_or_handle( std::move( it ), p );
    }
}

std::vector<detached_ptr<item>> Character::consume_items( const comp_selection<item_comp> &is,
                             int batch,
                             const std::function<bool( const item & )> &filter )
{
    return consume_items( get_map(), is, batch, pos(), PICKUP_RANGE, filter );
}

std::vector<detached_ptr<item>> Character::consume_items( map &m,
                             const comp_selection<item_comp> &is,
                             int batch,
                             const tripoint &origin, int radius,
                             const std::function<bool( const item & )> &filter )
{
    std::vector<detached_ptr<item>> ret;

    if( has_trait( trait_DEBUG_HS ) ) {
        return ret;
    }

    item_comp selected_comp = is.comp;

    const tripoint &loc = origin;
    const bool by_charges = item::count_by_charges( selected_comp.type ) && selected_comp.count > 0;
    // Count given to use_amount/use_charges, changed by those functions!
    int real_count = ( selected_comp.count > 0 ) ? selected_comp.count * batch : std::abs(
                         selected_comp.count );
    // First try to get everything from the map, than (remaining amount) from player
    if( is.use_from & usage_from::map ) {
        if( by_charges ) {
            std::vector<detached_ptr<item>> tmp = m.use_charges( loc, radius, selected_comp.type, real_count,
                                                  filter );
            ret.insert( ret.end(), std::make_move_iterator( tmp.begin() ),
                        std::make_move_iterator( tmp.end() ) );
        } else {
            std::vector<detached_ptr<item>> tmp = g->m.use_amount( loc, radius, selected_comp.type, real_count,
                                                  filter );
            std::vector<item *> as_p;
            as_p.reserve( tmp.size() );
            for( detached_ptr<item> &i : tmp ) {
                as_p.push_back( &*i );
            }
            remove_ammo( as_p, *this );
            ret.insert( ret.end(), std::make_move_iterator( tmp.begin() ),
                        std::make_move_iterator( tmp.end() ) );
        }
    }
    if( is.use_from & usage_from::player ) {
        if( by_charges ) {
            std::vector<detached_ptr<item>> tmp = use_charges( selected_comp.type, real_count, filter );
            ret.insert( ret.end(), std::make_move_iterator( tmp.begin() ),
                        std::make_move_iterator( tmp.end() ) );
        } else {
            std::vector<detached_ptr<item>> tmp = use_amount( selected_comp.type, real_count, filter );
            std::vector<item *> as_p;
            as_p.reserve( tmp.size() );
            for( detached_ptr<item> &i : tmp ) {
                as_p.push_back( &*i );
            }
            remove_ammo( as_p, *this );
            ret.insert( ret.end(), std::make_move_iterator( tmp.begin() ),
                        std::make_move_iterator( tmp.end() ) );
        }
    }
    // condense those items into one
    if( by_charges && ret.size() > 1 ) {
        std::vector<detached_ptr<item>>::iterator b = ret.begin();
        b++;
        while( ret.size() > 1 ) {
            ret.front()->charges += ( *b )->charges;
            b = ret.erase( b );
        }
    }
    lastconsumed = selected_comp.type;
    empty_buckets( *this );
    return ret;
}

/* This call is in-efficient when doing it for multiple items with the same map inventory.
In that case, consider using select_item_component with 1 pre-created map inventory, and then passing the results
to consume_items */
std::vector<detached_ptr<item>> Character::consume_items( const std::vector<item_comp> &components,
                             int batch,
                             const std::function<bool( const item & )> &filter )
{
    inventory map_inv;
    map_inv.form_from_map( pos(), PICKUP_RANGE, this );
    return consume_items( select_item_component( components, batch, map_inv, false, filter ), batch,
                          filter );
}

struct avail_tool_comp {
    avail_tool_comp( comp_selection<tool_comp> comp, int charges, int ideal )
        : comp( comp ), charges( charges ), ideal( ideal )
    {}
    avail_tool_comp( const avail_tool_comp & ) = default;
    avail_tool_comp &operator =( const avail_tool_comp & ) = default;

    comp_selection<tool_comp> comp;
    int charges;
    int ideal;
};

namespace crafting
{

static std::vector<avail_tool_comp>
find_tool_component( const Character *player_with_inv, const std::vector<tool_comp> &tools,
                     int batch,
                     const inventory &map_inv, cost_adjustment charge_mod )
{
    std::vector<avail_tool_comp> available_tools;

    auto calc_charges = [&]( const tool_comp & t ) {
        const int full_craft_charges = std::max( 1, t.count * batch );
        switch( charge_mod ) {
            case cost_adjustment::none:
                return std::make_pair( charges_for_complete( full_craft_charges ),
                                       charges_for_complete( full_craft_charges ) );
            case cost_adjustment::start_only:
                return std::make_pair( charges_for_starting( full_craft_charges ),
                                       charges_for_complete( full_craft_charges ) );
            case cost_adjustment::continue_only:
                return std::make_pair( charges_for_continuing( full_craft_charges ),
                                       charges_for_complete( full_craft_charges ) );
        }

        debugmsg( "Invalid tool_charge_mod" );
        return std::make_pair( INT_MAX, INT_MAX );
    };

    bool found_nocharge = false;
    // Use charges of any tools that require charges used
    for( auto it = tools.begin(); it != tools.end() && !found_nocharge; ++it ) {
        itype_id type = it->type;
        if( it->count > 0 ) {
            const std::pair<int, int> &expected_count = calc_charges( *it );
            const int count = expected_count.first;
            const int ideal = expected_count.second;
            if( player_with_inv ) {
                if( player_with_inv->has_charges( type, count ) ) {
                    int total_charges = player_with_inv->charges_of( type );
                    comp_selection<tool_comp> sel( usage_from::player, *it );
                    available_tools.emplace_back( sel, total_charges, ideal );
                }
            }
            if( map_inv.has_charges( type, count ) ) {
                int total_charges = map_inv.charges_of( type );
                comp_selection<tool_comp> sel( usage_from::map, *it );
                available_tools.emplace_back( sel, total_charges, ideal );
            }
        } else if( ( player_with_inv && player_with_inv->has_amount( type, 1 ) )
                   || map_inv.has_tools( type, 1 ) ) {
            comp_selection<tool_comp> sel( usage_from::none, *it );
            available_tools.emplace_back( sel, 0, 0 );
        }
    }

    std::ranges::sort( available_tools,
    []( const avail_tool_comp & lhs, const avail_tool_comp & rhs ) {
        if( lhs.comp.use_from == usage_from::none && rhs.comp.use_from != usage_from::none ) {
            return true;
        }
        if( rhs.comp.use_from == usage_from::none ) {
            return false;
        }
        if( lhs.charges >= lhs.ideal && rhs.charges < rhs.ideal ) {
            return true;
        }
        if( lhs.charges < lhs.ideal && rhs.charges >= rhs.ideal ) {
            return false;
        }
        // We want "bigger" tools first because those are likely to be vehicle mounted
        return static_cast<float>( lhs.ideal ) / lhs.charges > static_cast<float>
               ( rhs.ideal ) / rhs.charges;
    } );
    return available_tools;
}

static comp_selection<tool_comp>
query_tool_selection( const std::vector<avail_tool_comp> &available_tools,
                      const std::string &hotkeys, bool can_cancel, bool is_npc )
{
    if( available_tools.empty() ) {
        // This SHOULD only happen if cooking with a fire,
        // and the fire goes out.
        return comp_selection<tool_comp>( usage_from::none );
    }
    if( available_tools.front().comp.use_from == usage_from::none ) {
        // Default to using a tool that doesn't require charges
        return available_tools.front().comp;
    }
    if( available_tools.size() == 1 ) {
        return available_tools.front().comp;
    }
    if( is_npc ) {
        auto iter = std::ranges::find_if( available_tools,
        []( const avail_tool_comp & tool ) {
            return tool.comp.use_from == usage_from::player;
        } );
        if( iter != available_tools.end() ) {
            return iter->comp;
        }
        return available_tools.front().comp;
    }

    // Variety of options, list them and pick one
    // Populate the list
    uilist tmenu( hotkeys );
    for( const avail_tool_comp &tool : available_tools ) {
        const itype_id &comp_type = tool.comp.comp.type;
        if( tool.ideal > 1 ) {
            const char *format = tool.comp.use_from == usage_from::map
                                 ? _( "%s (%d/%d charges nearby)" )
                                 : _( "%s (%d/%d charges on person)" );
            std::string str = string_format( format,
                                             item::nname( comp_type ), tool.ideal,
                                             tool.charges );
            tmenu.addentry( str );
        } else {
            std::string str = tool.comp.use_from == usage_from::map
                              ? item::nname( comp_type ) + _( " (nearby)" )
                              : item::nname( comp_type );
            tmenu.addentry( str );
        }
    }

    tmenu.allow_cancel = can_cancel;

    // Get selection via a popup menu
    tmenu.title = _( "Use which tool?" );
    tmenu.query();

    if( tmenu.ret < 0 || static_cast<size_t>( tmenu.ret ) >= available_tools.size() ) {
        return comp_selection<tool_comp>( usage_from::cancel );
    }

    return available_tools.at( static_cast<size_t>( tmenu.ret ) ).comp;
}

comp_selection<tool_comp>
select_tool_component( const std::vector<tool_comp> &tools, int batch, const inventory &map_inv,
                       const Character *player_with_inv,
                       bool can_cancel,
                       const std::string &hotkeys,
                       cost_adjustment adjustment )
{
    std::vector<avail_tool_comp> options = find_tool_component( player_with_inv, tools, batch, map_inv,
                                           adjustment );
    bool is_npc = player_with_inv ? player_with_inv->is_npc() : false;
    return query_tool_selection( options, hotkeys, can_cancel, is_npc );
}

comp_selection<tool_comp>
select_tool_component( const std::vector<tool_comp> &tools, int batch, const inventory &map_inv,
                       const Character *player_with_inv,
                       bool can_cancel )
{
    return select_tool_component( tools, batch, map_inv, player_with_inv, can_cancel, DEFAULT_HOTKEYS,
                                  cost_adjustment::none );
}

} // namespace crafting

bool Character::craft_consume_tools( item &craft, int mulitplier, bool start_craft )
{
    if( !craft.is_craft() ) {
        debugmsg( "craft_consume_tools() called on non-craft '%s.' Aborting.", craft.tname() );
        return false;
    }
    if( has_trait( trait_DEBUG_HS ) ) {
        return true;
    }
    if( start_craft && mulitplier > 1 ) {
        debugmsg( "start_craft is true, but multiplier is %d > 1", mulitplier );
        return false;
    }

    const auto calc_charges = [&craft, &start_craft, &mulitplier]( int charges ) {
        int ret = charges;

        if( charges <= 0 ) {
            return ret;
        }

        // Account for batch size
        int full_cost = charges * craft.charges;

        if( start_craft ) {
            return crafting::charges_for_starting( full_cost );
        } else {
            // In case more than 5% progress was accomplished in one turn
            return crafting::charges_for_continuing( full_cost ) * mulitplier;
        }
    };

    // First check if we still have our cached selections
    const std::vector<comp_selection<tool_comp>> &cached_tool_selections =
                craft.get_cached_tool_selections();

    inventory map_inv;
    map_inv.form_from_map( pos(), PICKUP_RANGE, this );

    for( const comp_selection<tool_comp> &tool_sel : cached_tool_selections ) {
        itype_id type = tool_sel.comp.type;
        if( tool_sel.comp.count > 0 ) {
            const int count = calc_charges( tool_sel.comp.count );
            switch( tool_sel.use_from ) {
                case usage_from::player:
                    if( !has_charges( type, count ) ) {
                        add_msg_player_or_npc(
                            _( "You have insufficient %s charges and can't continue crafting" ),
                            _( "<npcname> has insufficient %s charges and can't continue crafting" ),
                            item::nname( type ) );
                        craft.set_tools_to_continue( false );
                        return false;
                    }
                    break;
                case usage_from::map:
                    if( !map_inv.has_charges( type, count ) ) {
                        add_msg_player_or_npc(
                            _( "You have insufficient %s charges and can't continue crafting" ),
                            _( "<npcname> has insufficient %s charges and can't continue crafting" ),
                            item::nname( type ) );
                        craft.set_tools_to_continue( false );
                        return false;
                    }
                    break;
                case usage_from::both:
                case usage_from::none:
                case usage_from::cancel:
                case usage_from::num_usages_from:
                    break;
            }
        } else if( !has_amount( type, 1 ) && !map_inv.has_tools( type, 1 ) ) {
            add_msg_player_or_npc(
                _( "You no longer have a %s and can't continue crafting" ),
                _( "<npcname> no longer has a %s and can't continue crafting" ),
                item::nname( type ) );
            craft.set_tools_to_continue( false );
            return false;
        }
    }

    // We have the selections, so consume them
    for( const comp_selection<tool_comp> &tool : cached_tool_selections ) {
        comp_selection<tool_comp> to_consume = tool;
        to_consume.comp.count = calc_charges( to_consume.comp.count );
        consume_tools( to_consume, 1 );
    }
    return true;
}

void Character::consume_tools( const comp_selection<tool_comp> &tool, int batch )
{
    consume_tools( get_map(), tool, batch, pos(), PICKUP_RANGE );
}

/* we use this if we selected the tool earlier */
void Character::consume_tools( map &m, const comp_selection<tool_comp> &tool, int batch,
                               const tripoint &origin, int radius )
{
    if( has_trait( trait_DEBUG_HS ) ) {
        return;
    }

    int quantity = tool.comp.count * batch;
    if( tool.use_from & usage_from::player ) {
        use_charges( tool.comp.type, quantity );
    }
    if( tool.use_from & usage_from::map ) {
        m.use_charges( origin, radius, tool.comp.type, quantity, return_true<item> );
    }

    // else, usage_from::none (or usage_from::cancel), so we don't use up any tools;
}

/* This call is in-efficient when doing it for multiple items with the same map inventory.
In that case, consider using select_tool_component with 1 pre-created map inventory, and then passing the results
to consume_tools */
void Character::consume_tools( const std::vector<tool_comp> &tools, int batch,
                               const std::string &hotkeys )
{
    inventory map_inv;
    map_inv.form_from_map( pos(), PICKUP_RANGE, this );
    consume_tools( crafting::select_tool_component( tools, batch, map_inv, this, false, hotkeys,
                   cost_adjustment::none ), batch );
}

ret_val<bool> crafting::can_disassemble( const Character &who, const item &obj,
        const inventory &inv )
{
    const auto &r = recipe_dictionary::get_uncraft( obj.typeId() );

    if( !r || obj.has_flag( flag_ETHEREAL_ITEM ) ) {
        return ret_val<bool>::make_failure( _( "You cannot disassemble this." ) );
    }

    // refuse to disassemble rotten items
    const item *food = obj.get_food();
    if( ( obj.goes_bad() && obj.rotten() ) || ( food && food->goes_bad() && food->rotten() ) ) {
        return ret_val<bool>::make_failure( _( "It's rotten, I'm not taking that apart." ) );
    }

    // refuse to disassemble items containing monsters/pets
    std::string monster = obj.get_var( "contained_name" );
    if( !monster.empty() ) {
        return ret_val<bool>::make_failure( _( "You must remove the %s before you can disassemble this." ),
                                            monster );
    }

    if( obj.count_by_charges() ) {
        int batch_size = r.disassembly_batch_size();
        if( obj.charges < batch_size ) {
            auto msg = vgettext( "You need at least %d charge of %s.",
                                 "You need at least %d charges of %s.", batch_size );
            return ret_val<bool>::make_failure( msg, batch_size, obj.tname() );
        }
    }
    const auto &dis = r.disassembly_requirements();

    for( const auto &opts : dis.get_qualities() ) {
        for( const auto &qual : opts ) {
            if( !qual.has( inv, return_true<item> ) ) {
                // Here should be no dot at the end of the string as 'to_string()' provides it.
                return ret_val<bool>::make_failure( _( "You need %s" ), qual.to_string() );
            }
        }
    }

    for( const auto &opts : dis.get_tools() ) {
        const bool found = std::ranges::any_of( opts,
        [&]( const tool_comp & tool ) {
            return ( tool.count <= 0 && inv.has_tools( tool.type, 1 ) ) ||
                   ( tool.count  > 0 && inv.has_charges( tool.type, tool.count ) );
        } );

        if( !found ) {
            const tool_comp &tool_required = opts.front();
            if( tool_required.count <= 0 ) {
                return ret_val<bool>::make_failure( _( "You need %s." ),
                                                    item::nname( tool_required.type ) );
            } else {
                //~ %1$s: tool name, %2$d: needed charges
                return ret_val<bool>::make_failure( vgettext( "You need a %1$s with %2$d charge.",
                                                    "You need a %1$s with %2$d charges.", tool_required.count ),
                                                    item::nname( tool_required.type ),
                                                    tool_required.count );
            }
        }
    }

    return ret_val<bool>::make_success();
}

struct disass_prompt_result {
    bool success = false;
    std::optional<int> batches;
    const recipe *r = nullptr;
};

static disass_prompt_result prompt_disassemble_in_seq( avatar &you, const item &obj,
        bool interactive, bool autoselect_max )
{
    disass_prompt_result res;

    const ret_val<bool> can_do = crafting::can_disassemble( you, obj, you.crafting_inventory() );

    if( !can_do.success() ) {
        if( interactive ) {
            you.add_msg_if_player( m_info, "%s", can_do.c_str() );
        }
        return res;
    }

    const recipe &r = recipe_dictionary::get_uncraft( obj.typeId() );
    res.r = &r;
    // Only worry about stealing from factions not already hostile
    if( !obj.is_owned_by( you, true ) && obj.get_owner()->likes_u >= -10 ) {
        if( !query_yn( _( "Disassembling the %s may anger the people who own it, continue?" ),
                       obj.tname() ) ) {
            return res;
        } else {
            if( obj.get_owner() ) {
                avatar_funcs::handle_theft_witnesses( you, obj.get_owner() );
            }
        }
    }

    int batch_size = r.disassembly_batch_size();

    if( interactive && get_option<bool>( "QUERY_DISASSEMBLE" ) ) {
        std::string msg;
        msg += string_format( _( "Disassembling the %s may yield:\n" ),
                              colorize( obj.tname(), obj.color_in_inventory() ) );
        const std::vector<item_comp> components = obj.get_uncraft_components();
        for( const item_comp &component : components ) {
            msg += "- " + component.to_string() + "\n";
        }
        if( batch_size != 1 ) {
            msg += string_format( _( "(per batch of %d)\n" ), batch_size );
        }
        msg += "\n";
        msg += _( "Really disassemble?\n" );
        if( !query_yn( msg ) ) {
            add_msg( _( "Never mind." ) );
            return res;
        }
    }

    if( obj.count_by_charges() ) {
        int num_batches = obj.charges / batch_size;
        if( num_batches == 0 ) {
            // Not enough charges for even one disassembly
            return res;
        } else if( num_batches == 1 || autoselect_max ) {
            // Skip prompt if can only do 1, or if we're doing all
            res.batches = num_batches;
        } else {
            string_input_popup popup_input;
            std::string title;
            std::string descr;
            if( batch_size != 1 ) {
                descr = string_format( _( "Disassembly will be done in batches of %d." ), batch_size );
                title = string_format(
                            _( "Disassemble how many batches of %s [MAX: %d]: " ),
                            obj.type_name( 1 ), num_batches );
            } else {
                title = string_format( _( "Disassemble how many %s [MAX: %d]: " ),
                                       obj.type_name( 1 ), num_batches );
            }
            int result = num_batches;
            popup_input.title( title ).description( descr ).edit( result );
            if( popup_input.canceled() || result <= 0 ) {
                add_msg( _( "Never mind." ) );
                return res;
            }
            res.batches = std::min( result, num_batches );
        }
    }

    res.success = true;
    return res;
}

static bool prompt_disassemble_single( avatar &you, item *target, bool interactive )
{
    if( !target ) {
        add_msg( _( "Never mind." ) );
        return false;
    }

    disass_prompt_result res = prompt_disassemble_in_seq( you, *target, interactive, false );

    if( !res.success ) {
        return false;
    }

    iuse_location loc;
    loc.loc = target;
    loc.count = res.batches ? *res.batches : 1;

    tripoint_abs_ms pos_abs( get_map().getabs( you.pos() ) );

    you.assign_activity( std::make_unique<player_activity>
    ( std::make_unique<disassemble_activity_actor>( std::vector<iuse_location> {{ loc }}, pos_abs,
    false ) ) );

    return true;
}

bool crafting::disassemble( avatar &you )
{
    item *target = game_menus::inv::disassemble( you );
    return prompt_disassemble_single( you, target, false );
}

bool crafting::disassemble( avatar &you, item &target )
{
    return prompt_disassemble_single( you, &target, true );
}

bool crafting::disassemble_all( avatar &you, bool recursively )
{
    std::vector<iuse_location> targets;

    tripoint pos = you.pos();

    for( item * const &itm : get_map().i_at( pos ) ) {
        disass_prompt_result res = prompt_disassemble_in_seq( you, *itm, false, true );
        if( res.success ) {
            iuse_location loc;
            loc.loc = itm;
            loc.count = res.batches ? *res.batches : 1;
            targets.push_back( std::move( loc ) );
        }
    }

    if( !targets.empty() ) {
        tripoint_abs_ms pos_abs( get_map().getabs( you.pos() ) );

        you.assign_activity( std::make_unique<player_activity>
                             ( std::make_unique<disassemble_activity_actor>( std::move(
                                         targets ), pos_abs, recursively ) ) );
        return true;
    } else {
        return false;
    }
}

void crafting::complete_disassemble( Character &who, const iuse_location &target,
                                     const tripoint &/*pos*/ )
{

    item &org_item = *target.loc;
    const recipe &dis = recipe_dictionary::get_uncraft( org_item.typeId() );

    // Get the proper recipe - the one for disassembly, not assembly
    const auto dis_requirements = dis.disassembly_requirements();

    // Make a copy to keep its data (damage/components) even after it
    // has been removed.
    item &dis_item = org_item;
    float component_success_chance = std::min( std::pow( 0.8, dis_item.damage_level( 4 ) ), 1.0 );

    add_msg( _( "You disassemble the %s into its components." ), dis_item.tname() );
    // Remove any batteries, ammo and mods first
    remove_ammo( dis_item, who );
    remove_radio_mod( dis_item, *who.as_player() );

    if( org_item.count_by_charges() ) {
        int batch_size = dis.disassembly_batch_size();
        org_item.charges -= batch_size * target.count;
    }

    // Consume tool charges
    for( const auto &it : dis_requirements.get_tools() ) {
        who.as_player()->consume_tools( it );
    }

    // add the components to the map
    // Player skills should determine how many components are returned

    int skill_dice = 2 + who.get_skill_level( dis.skill_used ) * 3;
    skill_dice += who.get_skill_level( dis.skill_used );

    // Sides on dice is 16 plus your current intelligence
    ///\EFFECT_INT increases success rate for disassembling items
    int skill_sides = 16 + who.int_cur;

    int diff_dice = dis.difficulty;
    int diff_sides = 24; // 16 + 8 (default intelligence)

    // disassembly only nets a bit of practice
    if( dis.skill_used ) {
        who.as_player()->practice( dis.skill_used, ( dis.difficulty ) * 2, dis.difficulty );
    }

    // If the components aren't empty, we want items exactly identical to them
    // Even if the best-fit recipe does not involve those items
    location_vector<item> &components = dis_item.get_components();

    // If the components are empty, item is the default kind and made of default components
    if( components.empty() ) {
        const bool uncraft_liquids_contained = dis.has_flag( flag_UNCRAFT_LIQUIDS_CONTAINED );
        for( const auto &altercomps : dis_requirements.get_components() ) {
            const item_comp &comp = altercomps.front();
            int compcount = comp.count * target.count;
            detached_ptr<item> newit = item::spawn( comp.type, calendar::turn );
            const bool is_liquid = newit->made_of( LIQUID );
            if( uncraft_liquids_contained && is_liquid && newit->charges != 0 ) {
                // Spawn liquid item in its default container
                compcount = compcount / newit->charges;
                if( compcount != 0 ) {
                    newit = item::in_its_container( std::move( newit ) );
                }
            } else {
                // Compress liquids and counted-by-charges items into one item,
                // they are added together on the map anyway and handle_liquid
                // should only be called once to put it all into a container at once.
                if( newit->count_by_charges() || is_liquid ) {
                    newit->charges = compcount;
                    compcount = 1;
                } else if( !newit->craft_has_charges() && newit->charges > 0 ) {
                    // tools that can be unloaded should be created unloaded,
                    // tools that can't be unloaded will keep their default charges.
                    newit->charges = 0;
                }
            }

            // If the recipe has a `FULL_MAGAZINE` flag, spawn any magazines full of ammo
            if( newit->is_magazine() && dis.has_flag( flag_FULL_MAGAZINE ) ) {
                newit->ammo_set( newit->ammo_default(), newit->ammo_capacity() );
            }

            for( ; compcount > 0; compcount-- ) {
                components.push_back( item::spawn( *newit ) );
            }
        }
    }

    std::vector<detached_ptr<item>> drop_items;

    for( detached_ptr<item> &newit : components.clear() ) {
        const bool comp_success = ( dice( skill_dice, skill_sides ) > dice( diff_dice,  diff_sides ) );
        if( dis.difficulty != 0 && !comp_success ) {
            add_msg( m_bad, _( "You fail to recover %s." ), newit->tname() );
            continue;
        }
        const bool dmg_success = component_success_chance > rng_float( 0, 1 );
        if( !dmg_success ) {
            // Show reason for failure (damaged item, tname contains the damage adjective)
            //~ %1s - material, %2$s - disassembled item
            add_msg( m_bad, _( "You fail to recover %1$s from the %2$s." ), newit->tname(),
                     dis_item.tname() );
            continue;
        }

        // Refitted clothing disassembles into refitted components (when applicable)
        if( dis_item.has_flag( flag_FIT ) && newit->has_flag( flag_VARSIZE ) ) {
            newit->set_flag( flag_FIT );
        }

        if( newit->made_of( LIQUID ) ) {
            liquid_handler::handle_all_liquid( std::move( newit ), PICKUP_RANGE );
        } else {
            drop_items.push_back( std::move( newit ) );
        }
    }

    // remove the item, except when it's counted by charges and still has some
    // It's important to remove/delete it after its contents are removed, lest they be deleted too
    if( !org_item.count_by_charges() || org_item.charges <= 0 ) {
        org_item.detach();
    }

    put_into_vehicle_or_drop( who, item_drop_reason::deliberate, drop_items );

    if( !dis.learn_by_disassembly.empty() && !who.knows_recipe( &dis ) ) {
        if( who.can_learn_by_disassembly( dis ) ) {
            const SkillLevelMap &char_skills = who.get_all_skills();
            float skill_bonus = ( 1.0f + char_skills.exceeds_recipe_requirements( dis ) ) * std::max( 1.0f,
                                0.9f + ( who.int_cur * 0.025f ) );
            if( x_in_y( skill_bonus, 4.0 ) ) {
                // TODO: change to forward an id or a reference
                who.learn_recipe( &*dis.ident() );
                add_msg( m_good, _( "You learned a recipe for %s from disassembling it!" ),
                         dis_item.tname() );
            } else {
                add_msg( m_info, _( "You might be able to learn a recipe for %s if you disassemble another." ),
                         dis_item.tname() );
            }
        } else {
            add_msg( m_info, _( "If you had better skills, you might learn a recipe next time." ) );
        }
    }
}

void remove_ammo( std::vector<item *> &dis_items, Character &who )
{
    for( auto &dis_item : dis_items ) {
        remove_ammo( *dis_item, who );
    }
}

void remove_ammo( item &dis_item, Character &who )
{
    std::vector<detached_ptr<item>> removed;
    dis_item.remove_items_with( [&removed]( detached_ptr<item> &&it ) {
        if( !it->is_irremovable() ) {
            removed.push_back( std::move( it ) );
        }
        return VisitResponse::NEXT;
    } );

    for( detached_ptr<item> &it : removed ) {
        drop_or_handle( std::move( it ), who );
    }

    if( dis_item.has_flag( flag_NO_UNLOAD ) ) {
        return;
    }
    if( dis_item.is_gun() && !dis_item.ammo_current().is_null() ) {
        detached_ptr<item> ammodrop = item::spawn( dis_item.ammo_current(), calendar::turn );
        ammodrop->charges = dis_item.charges;
        drop_or_handle( std::move( ammodrop ), who );
        dis_item.charges = 0;
    }
    if( dis_item.is_tool() && dis_item.charges > 0 && !dis_item.ammo_current().is_null() ) {
        detached_ptr<item> ammodrop = item::spawn( dis_item.ammo_current(), calendar::turn );
        ammodrop->charges = dis_item.charges;
        if( dis_item.ammo_current() == itype_plut_cell ) {
            ammodrop->charges /= PLUTONIUM_CHARGES;
        }
        drop_or_handle( std::move( ammodrop ), who );
        dis_item.charges = 0;
    }
}

namespace crafting
{

std::set<itype_id> get_books_for_recipe( const Character &c, const inventory &crafting_inv,
        const recipe *r )
{
    std::set<itype_id> book_ids;
    const int skill_level = c.get_skill_level( r->skill_used );
    for( auto &book_lvl : r->booksets ) {
        itype_id book_id = book_lvl.first;
        int required_skill_level = book_lvl.second;
        if( skill_level >= required_skill_level && crafting_inv.amount_of( book_id ) > 0 ) {
            book_ids.insert( book_id );
        }
    }
    return book_ids;
}

std::set<itype_id> get_books_for_recipe( const recipe *r )
{
    std::set<itype_id> book_ids;
    std::ranges::transform( r->booksets, std::inserter( book_ids, book_ids.end() ),
    []( const std::pair<itype_id, int> &pr ) {
        return pr.first;
    } );
    return book_ids;
}

int charges_for_complete( int full_charges )
{
    return full_charges;
}
int charges_for_starting( int full_charges )
{
    return full_charges / 20 + full_charges % 20;
}
int charges_for_continuing( int full_charges )
{
    return full_charges / 20;
}

} // namespace crafting

static float morale_factor_static( const Character &who, const recipe &rec )
{
    const int morale = who.get_morale_level();
    if( morale >= 0 ) {
        // No bonus for being happy yet
        return 1.0f;
    }

    // Harder jobs are more frustrating, even when skilled
    // For each skill where skill=difficulty, multiply effective morale by 200%
    float morale_mult = std::max( 1.0f, 2.0f * rec.difficulty / std::max( 1,
                                  who.get_skill_level( rec.skill_used ) ) );
    for( const std::pair<const skill_id, int> &pr : rec.required_skills ) {
        morale_mult *= std::max( 1.0f, 2.0f * pr.second / std::max( 1, who.get_skill_level( pr.first ) ) );
    }

    // Halve speed at -50 effective morale, quarter at -150
    float morale_effect = 1.0f + ( morale_mult * morale ) / -50.0f;

    return 1.0f / morale_effect;
}

float crafting_activity_actor::calc_morale_factor( const Character &who ) const
{
    return morale_factor_static( who, rec );
}

bool crafting_activity_actor::assistant_capable( const Character &who ) const
{
    return assistant_capable( who, rec );
}

bool crafting_activity_actor::assistant_capable( const Character &who, const recipe &recipe )
{
    return who.get_skill_level( recipe.skill_used ) >= recipe.difficulty;
}

void crafting_activity_actor::serialize( JsonOut &jsout ) const
{
}

inline void crafting_activity_actor::calc_all_moves( player_activity &act, Character &who )
{
    auto reqs = activity_reqs_adapter( rec, std::make_pair( target->weight(), target->volume() ) );
    act.speed.calc_all_moves( who, reqs );
}

activity_speed crafting_activity_actor::speed_preset( Character &who, const recipe &rec )
{
    auto reqs = activity_reqs_adapter( rec, std::make_pair( 0_gram, 0_liter ) );
    auto speed = activity_speed();
    speed.type = activity_id( "ACT_CRAFT" );
    if( speed.type->assistable() ) {
        speed.assistant_count = player_activity::get_assistants( who, [&rec]( bool ok,
        const Character & guy ) {
            return ok && assistant_capable( guy, rec );
        }, 8 ).size();
    }
    speed.morale_factor_custom_formula = [&]( const Character & who ) {
        return morale_factor_static( who, rec );
    };

    speed.calc_all_moves( who, reqs );
    return speed;
}

void crafting_activity_actor::start( player_activity &act, Character &who )
{
    if( !target ) {
        who.add_msg_player_or_npc(
            _( "You no longer have the in progress craft in your possession.  "
               "You stop crafting.  "
               "Reactivate the in progress craft to continue crafting." ),
            _( "<npcname> no longer has the in progress craft in their possession.  "
               "<npcname> stops crafting." ) );
        who.cancel_activity();
        return;
    }
    if( !target->is_craft() ) {
        debugmsg( "ACT_CRAFT target '%s' is not a craft.  Aborting ACT_CRAFT.", target->tname() );
        who.cancel_activity();
        return;
    }

    rec = target->get_making();
    int total_time = std::max( 1, rec.batch_time( 1 ) );
    int left = target->item_counter == 0
               ? total_time
               : total_time - target->item_counter / 10'000'000.0 * total_time;

    progress.emplace( target->tname( target->count() ), total_time, left );
    target->set_next_failure_point( who, crafting_success_roll( rec, who,
                                    act.assistants() ) * 10000000 );
}

void crafting_activity_actor::do_turn( player_activity &act, Character &who )
{
    // item_location::get_item() will return nullptr if the item is lost
    if( !target ) {
        who.add_msg_player_or_npc(
            _( "You no longer have the in progress craft in your possession.  "
               "You stop crafting.  "
               "Reactivate the in progress craft to continue crafting." ),
            _( "<npcname> no longer has the in progress craft in their possession.  "
               "<npcname> stops crafting." ) );
        who.cancel_activity();
        return;
    }

    if( !target->is_craft() ) {
        debugmsg( "ACT_CRAFT target '%s' is not a craft.  Aborting ACT_CRAFT.", target->tname() );
        who.cancel_activity();
        return;
    }

    if( !who.can_continue_craft( *target ) ) {
        who.cancel_activity();
        return;
    }

    if( act.speed.light <= 0.0f ) {
        who.add_msg_if_player( m_bad, _( "You can no longer see well enough to keep crafting." ) );
        who.cancel_activity();
        return;
    }

    // Current progress as a percent of base_total_moves to 2 decimal places
    target->item_counter = progress.front().to_counter();

    // Skill and tools are gained/consumed after every 5% progress
    int five_percent_steps = target->item_counter / 500'000 - old_counter / 500'000;
    if( five_percent_steps > 0 ) {
        who.craft_skill_gain( *target, five_percent_steps );
    }

    // Unlike skill, tools are consumed once at the start and should not be consumed at the end
    if( target->item_counter >= 10'000'000 ) {
        --five_percent_steps;
    }

    if( five_percent_steps > 0 ) {
        if( !who.craft_consume_tools( *target, five_percent_steps, false ) ) {
            // So we don't skip over any tool comsuption
            target->item_counter -= target->item_counter % 500000 + 1;
            who.cancel_activity();
            return;
        }
    }

    old_counter = target->item_counter;

    if( target->item_counter >= target->get_next_failure_point() ) {
        const int percent_left = 10000000 - target->item_counter;
        bool destroy = target->handle_craft_failure( who, crafting_success_roll( rec, who,
                       act.assistants() ) * percent_left );
        target->set_next_failure_point( who, crafting_success_roll( rec, who,
                                        act.assistants() ) * percent_left );
        // If the craft needs to be destroyed, do it and stop crafting.
        if( destroy ) {
            who.add_msg_player_or_npc( _( "There is nothing left of the %s to craft from." ),
                                       _( "There is nothing left of the %s <npcname> was crafting." ), target->tname() );
            act.targets.front()->detach();
            who.cancel_activity();
        }
    }
}

void crafting_activity_actor::finish( player_activity &, Character &who )
{
    //TODO!: CHEEKY check
    item *craft_copy = &*target;
    complete_craft( who, *craft_copy );
    target->detach();
    who.cancel_activity();
    if( auto p = who.as_player(); p && is_long ) {
        if( p->making_would_work( rec.ident(), craft_copy->charges ) ) {
            p->last_craft->execute();
        }
    }
}
