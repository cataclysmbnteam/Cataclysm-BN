#include "activity_actor.h"
#include "activity_actor_definitions.h"

#include <algorithm>
#include <cmath>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include "avatar_action.h"
#include "activity_handlers.h" // put_into_vehicle_or_drop and drop_on_map
#include "advanced_inv.h"
#include "avatar.h"
#include "avatar_functions.h"
#include "calendar.h"
#include "character.h"
#include "character_functions.h"
#include "crafting.h"
#include "debug.h"
#include "enums.h"
#include "event.h"
#include "event_bus.h"
#include "flag.h"
#include "game.h"
#include "gates.h"
#include "iexamine.h"
#include "int_id.h"
#include "item.h"
#include "item_group.h"
#include "item_hauling.h"
#include "json.h"
#include "line.h"
#include "locations.h"
#include "map.h"
#include "map_iterator.h"
#include "mapdata.h"
#include "npc.h"
#include "output.h"
#include "options.h"
#include "pickup.h"
#include "player.h"
#include "player_activity.h"
#include "point.h"
#include "ranged.h"
#include "recipe_dictionary.h"
#include "rng.h"
#include "sounds.h"
#include "timed_event.h"
#include "translations.h"
#include "uistate.h"
#include "units.h"
#include "vehicle.h"
#include "vehicle_part.h"
#include "vpart_position.h"

static const itype_id itype_bone_human( "bone_human" );
static const itype_id itype_electrohack( "electrohack" );

static const skill_id skill_computer( "computer" );
static const skill_id skill_mechanics( "mechanics" );

static const mtype_id mon_zombie( "mon_zombie" );
static const mtype_id mon_zombie_fat( "mon_zombie_fat" );
static const mtype_id mon_zombie_rot( "mon_zombie_rot" );
static const mtype_id mon_skeleton( "mon_skeleton" );
static const mtype_id mon_zombie_crawler( "mon_zombie_crawler" );

static const quality_id qual_LOCKPICK( "LOCKPICK" );

static const std::string has_thievery_witness( "has_thievery_witness" );

aim_activity_actor::aim_activity_actor() : fake_weapon( new fake_item_location() )
{
    initial_view_offset = get_avatar().view_offset;
}

std::unique_ptr<aim_activity_actor> aim_activity_actor::use_wielded()
{
    return std::make_unique<aim_activity_actor>();
}

std::unique_ptr<aim_activity_actor> aim_activity_actor::use_bionic( detached_ptr<item> &&fake_gun,
        const units::energy &cost_per_shot )
{
    std::unique_ptr<aim_activity_actor> act( new aim_activity_actor() );
    act->bp_cost_per_shot = cost_per_shot;
    act->fake_weapon = std::move( fake_gun );
    return act;
}

std::unique_ptr<aim_activity_actor> aim_activity_actor::use_mutation( detached_ptr<item>
        &&fake_gun )
{
    std::unique_ptr<aim_activity_actor> act( new aim_activity_actor() );
    act->fake_weapon = std::move( fake_gun );
    return act;
}

void aim_activity_actor::start( player_activity &act, Character &/*who*/ )
{
    // Time spent on aiming is determined on the go by the player
    act.moves_total = 1;
    act.moves_left = 1;
}

void aim_activity_actor::do_turn( player_activity &act, Character &who )
{
    if( !who.is_avatar() ) {
        debugmsg( "ACT_AIM not implemented for NPCs" );
        aborted = true;
        act.moves_left = 0;
        return;
    }
    avatar &you = get_avatar();

    item *weapon = get_weapon();
    if( !weapon || !avatar_action::can_fire_weapon( you, get_map(), *weapon ) ) {
        aborted = true;
        act.moves_left = 0;
        return;
    }

    gun_mode gun = weapon->gun_current_mode();
    if( !gun->ammo_remaining() && !reload_loc && gun->has_flag( flag_RELOAD_AND_SHOOT ) ) {
        if( !load_RAS_weapon() ) {
            aborted = true;
            act.moves_left = 0;
            return;
        }
    }
    std::optional<shape_factory> shape_gen;
    if( weapon->ammo_current() && weapon->ammo_current()->ammo &&
        weapon->ammo_current()->ammo->shape ) {
        shape_gen = weapon->ammo_current()->ammo->shape;
    }

    g->temp_exit_fullscreen();
    target_handler::trajectory trajectory;
    if( !shape_gen ) {
        trajectory = target_handler::mode_fire( you, *this );
    } else {
        trajectory = target_handler::mode_shaped( you, *shape_gen, *this );
    }
    g->reenter_fullscreen();

    if( aborted ) {
        act.moves_left = 0;
    } else {
        if( !trajectory.empty() ) {
            fin_trajectory = trajectory;
            act.moves_left = 0;
        }

        // Allow interrupting activity only during 'aim and fire'.
        // Prevents '.' key for 'aim for 10 turns' from conflicting with '.' key for 'interrupt activity'
        // in case of high input lag (curses, sdl sometimes...), but allows to interrupt aiming
        // if a bug happens / stars align to cause an endless aiming loop.
        act.interruptable_with_kb = action != "AIM";
    }
}

void aim_activity_actor::finish( player_activity &act, Character &who )
{
    act.set_to_null();
    item *weapon = get_weapon();
    if( !weapon ) {
        restore_view();
        return;
    }
    if( aborted ) {
        if( reload_requested ) {
            // Reload the gun / select different arrows
            // May assign ACT_RELOAD
            avatar_action::reload_wielded( true );
        }
        restore_view();
        return;
    }

    // Fire!
    gun_mode gun = weapon->gun_current_mode();

    item *ammo_loc = reload_loc ? &*reload_loc : nullptr;

    int shots_fired = ranged::fire_gun( who, fin_trajectory.back(), gun.qty, *gun, ammo_loc );

    if( shots_fired > 0 ) {
        // TODO: bionic power cost of firing should be derived from a value of the relevant weapon.
        if( bp_cost_per_shot > 0_J ) {
            who.mod_power_level( -bp_cost_per_shot * shots_fired );
        }
        if( stamina_cost_per_shot > 0 ) {
            who.mod_stamina( -stamina_cost_per_shot * shots_fired );
        }
    }

    if( !get_option<bool>( "AIM_AFTER_FIRING" ) ) {
        restore_view();
        return;
    }

    // re-enter aiming UI with same parameters
    std::unique_ptr<aim_activity_actor> aim_actor = std::make_unique<aim_activity_actor>();
    aim_actor->abort_if_no_targets = true;
    aim_actor->fake_weapon = std::move( this->fake_weapon );
    aim_actor->bp_cost_per_shot = this->bp_cost_per_shot;
    aim_actor->initial_view_offset = this->initial_view_offset;

    // if invalid target or it's dead - reset it so a new one is acquired
    shared_ptr_fast<Creature> last_target = who.as_player()->last_target.lock();
    if( last_target && last_target->is_dead_state() ) {
        who.as_player()->last_target.reset();
    }
    who.assign_activity( std::make_unique<player_activity>( std::move( aim_actor ) ), false );
}

void aim_activity_actor::canceled( player_activity &/*act*/, Character &/*who*/ )
{
    restore_view();
}

void aim_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "fake_weapon", fake_weapon ? *fake_weapon : null_item_reference() );
    jsout.member( "bp_cost_per_shot", bp_cost_per_shot );
    jsout.member( "stamina_cost_per_shot", stamina_cost_per_shot );
    jsout.member( "action", action );
    jsout.member( "aif_duration", aif_duration );
    jsout.member( "aiming_at_critter", aiming_at_critter );
    jsout.member( "snap_to_target", snap_to_target );
    jsout.member( "shifting_view", shifting_view );
    jsout.member( "initial_view_offset", initial_view_offset );
    jsout.member( "loaded_RAS_weapon", loaded_RAS_weapon );
    jsout.member( "reload_loc", reload_loc );
    jsout.member( "aborted", aborted );
    jsout.member( "reload_requested", reload_requested );
    jsout.member( "abort_if_no_targets", abort_if_no_targets );

    jsout.end_object();
}

std::unique_ptr<activity_actor> aim_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<aim_activity_actor> actor( new aim_activity_actor() );

    JsonObject data = jsin.get_object();

    data.read( "fake_weapon", actor->fake_weapon );
    data.read( "bp_cost_per_shot", actor->bp_cost_per_shot );
    data.read( "stamina_cost_per_shot", actor->stamina_cost_per_shot );
    data.read( "action", actor->action );
    data.read( "aif_duration", actor->aif_duration );
    data.read( "aiming_at_critter", actor->aiming_at_critter );
    data.read( "snap_to_target", actor->snap_to_target );
    data.read( "shifting_view", actor->shifting_view );
    data.read( "initial_view_offset", actor->initial_view_offset );
    data.read( "loaded_RAS_weapon", actor->loaded_RAS_weapon );
    data.read( "reload_loc", actor->reload_loc );
    data.read( "aborted", actor->aborted );
    data.read( "reload_requested", actor->reload_requested );
    data.read( "abort_if_no_targets", actor->abort_if_no_targets );

    return actor;
}

item *aim_activity_actor::get_weapon()
{
    if( fake_weapon ) {
        // TODO: check if the player lost relevant bionic/mutation
        return &*fake_weapon;
    } else {
        // Check for lost gun (e.g. yanked by zombie technician)
        // TODO: check that this is the same gun that was used to start aiming
        item *weapon = &get_player_character().primary_weapon();
        return weapon->is_null() ? nullptr : weapon;
    }
}

void aim_activity_actor::restore_view()
{
    avatar &player_character = get_avatar();
    bool changed_z = player_character.view_offset.z != initial_view_offset.z;
    player_character.view_offset = initial_view_offset;
    if( changed_z ) {
        get_map().invalidate_map_cache( player_character.view_offset.z );
        g->invalidate_main_ui_adaptor();
    }
}

bool aim_activity_actor::load_RAS_weapon()
{
    // TODO: use activity for fetching ammo and loading weapon
    player &you = get_avatar();
    item *weapon = get_weapon();
    gun_mode gun = weapon->gun_current_mode();

    // Will burn (0.2% max base stamina * the strength required to fire)
    stamina_cost_per_shot = gun->get_min_str() * static_cast<int>
                            ( 0.002f * get_option<int>( "PLAYER_MAX_STAMINA" ) );
    if( you.get_stamina() < stamina_cost_per_shot ) {
        you.add_msg_if_player( m_bad, _( "You're too tired to draw your %s." ), weapon->tname() );
        return false;
    }

    const auto ammo_location_is_valid = [&]() -> bool {
        if( !you.ammo_location )
        {
            return false;
        }
        if( !gun->can_reload_with( you.ammo_location->typeId() ) )
        {
            return false;
        }
        if( square_dist( you.pos(), you.ammo_location->position() ) > 1 )
        {
            return false;
        }
        return true;
    };
    item_reload_option opt = ammo_location_is_valid() ? item_reload_option( &you, weapon,
                             weapon, *you.ammo_location ) : character_funcs::select_ammo( you, *gun );
    if( !opt ) {
        // Menu canceled
        return false;
    }

    reload_loc = opt.ammo;
    loaded_RAS_weapon = true;
    return true;
}

void autodrive_activity_actor::start( player_activity &act, Character &who )
{
    const bool in_vehicle = who.in_vehicle && who.controlling_vehicle;
    const optional_vpart_position vp = get_map().veh_at( who.pos() );
    if( !( vp && in_vehicle ) ) {
        who.cancel_activity();
        return;
    }

    player_vehicle = &vp->vehicle();
    player_vehicle->is_autodriving = true;
    act.moves_left = calendar::INDEFINITELY_LONG;
}

void autodrive_activity_actor::do_turn( player_activity &act, Character &who )
{
    if( who.in_vehicle && who.controlling_vehicle && player_vehicle ) {
        if( who.moves <= 0 ) {
            // out of moves? the driver's not doing anything this turn
            // (but the vehicle will continue moving)
            return;
        }
        switch( player_vehicle->do_autodrive( who ) ) {
            case autodrive_result::ok:
                if( who.moves > 0 ) {
                    // if do_autodrive() didn't eat up all our moves, end the turn
                    // equivalent to player pressing the "pause" button
                    who.moves = 0;
                }
                sounds::reset_markers();
                break;
            case autodrive_result::abort:
                who.cancel_activity();
                break;
            case autodrive_result::finished:
                act.moves_left = 0;
                break;
        }
    } else {
        who.cancel_activity();
    }
}

void autodrive_activity_actor::canceled( player_activity &act, Character &who )
{
    who.add_msg_if_player( m_info, _( "Auto-drive canceled." ) );
    who.omt_path.clear();
    if( player_vehicle ) {
        player_vehicle->stop_autodriving( false );
    }
    act.set_to_null();
}

void autodrive_activity_actor::finish( player_activity &act, Character &who )
{
    who.add_msg_if_player( m_info, _( "You have reached your destination." ) );
    player_vehicle->stop_autodriving( false );
    act.set_to_null();
}

void autodrive_activity_actor::serialize( JsonOut &jsout ) const
{
    // Activity is not being saved but still provide some valid json if called.
    jsout.write_null();
}

std::unique_ptr<activity_actor> autodrive_activity_actor::deserialize( JsonIn & )
{
    return std::make_unique<autodrive_activity_actor>();
}

void dig_activity_actor::start( player_activity &act, Character & )
{
    act.moves_total = moves_total;
    act.moves_left = moves_total;
}

void dig_activity_actor::do_turn( player_activity &, Character & )
{
    sfx::play_activity_sound( "tool", "shovel", sfx::get_heard_volume( location ) );
    if( calendar::once_every( 1_minutes ) ) {
        //~ Sound of a shovel digging a pit at work!
        sounds::sound( location, 10, sounds::sound_t::activity, _( "hsh!" ) );
    }
}

void dig_activity_actor::finish( player_activity &act, Character &who )
{
    map &here = get_map();
    const bool grave = here.ter( location ) == t_grave;

    if( grave ) {
        if( one_in( 10 ) ) {
            static const std::array<mtype_id, 5> monids = { {
                    mon_zombie, mon_zombie_fat,
                    mon_zombie_rot, mon_skeleton,
                    mon_zombie_crawler
                }
            };

            g->place_critter_at( random_entry( monids ), byproducts_location );
            here.furn_set( location, f_coffin_o );
            who.add_msg_if_player( m_warning, _( "Something crawls out of the coffin!" ) );
        } else {
            here.spawn_item( location, itype_bone_human, rng( 5, 15 ) );
            here.furn_set( location, f_coffin_c );
        }
        std::vector<item *> dropped = g->m.place_items( item_group_id( "allclothes" ), 50, location,
                                      location, false,
                                      calendar::turn );
        g->m.place_items( item_group_id( "grave" ), 25, location, location, false, calendar::turn );
        g->m.place_items( item_group_id( "jewelry_front" ), 20, location, location, false, calendar::turn );
        for( item * const &it : dropped ) {
            if( it->is_armor() ) {
                it->set_flag( flag_FILTHY );
                it->set_damage( rng( 1, it->max_damage() - 1 ) );
            }
        }
        g->events().send<event_type::exhumes_grave>( who.getID() );
    }

    here.ter_set( location, ter_id( result_terrain ) );

    for( int i = 0; i < byproducts_count; i++ ) {
        here.spawn_items( byproducts_location,
                          item_group::items_from( item_group_id( byproducts_item_group ),
                                  calendar::turn ) );
    }

    const int act_exertion = act.moves_total;

    who.mod_stored_kcal( std::min( -1, -act_exertion / to_moves<int>( 80_seconds ) ) );
    who.mod_thirst( std::max( 1, act_exertion / to_moves<int>( 12_minutes ) ) );
    who.mod_fatigue( std::max( 1, act_exertion / to_moves<int>( 6_minutes ) ) );
    if( grave ) {
        who.add_msg_if_player( m_good, _( "You finish exhuming a grave." ) );
    } else {
        who.add_msg_if_player( m_good, _( "You finish digging the %s." ),
                               here.ter( location ).obj().name() );
    }

    act.set_to_null();
}

void dig_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "moves", moves_total );
    jsout.member( "location", location );
    jsout.member( "result_terrain", result_terrain );
    jsout.member( "byproducts_location", byproducts_location );
    jsout.member( "byproducts_count", byproducts_count );
    jsout.member( "byproducts_item_group", byproducts_item_group );

    jsout.end_object();
}

std::unique_ptr<activity_actor> dig_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<dig_activity_actor> actor( new dig_activity_actor( 0, tripoint_zero,
            {}, tripoint_zero, 0, {} ) );

    JsonObject data = jsin.get_object();

    data.read( "moves", actor->moves_total );
    data.read( "location", actor->location );
    data.read( "result_terrain", actor->result_terrain );
    data.read( "byproducts_location", actor->byproducts_location );
    data.read( "byproducts_count", actor->byproducts_count );
    data.read( "byproducts_item_group", actor->byproducts_item_group );

    return actor;
}

void dig_channel_activity_actor::start( player_activity &act, Character & )
{
    act.moves_total = moves_total;
    act.moves_left = moves_total;
}

void dig_channel_activity_actor::do_turn( player_activity &, Character & )
{
    sfx::play_activity_sound( "tool", "shovel", sfx::get_heard_volume( location ) );
    if( calendar::once_every( 1_minutes ) ) {
        //~ Sound of a shovel digging a pit at work!
        sounds::sound( location, 10, sounds::sound_t::activity, _( "hsh!" ) );
    }
}

void dig_channel_activity_actor::finish( player_activity &act, Character &who )
{
    map &here = get_map();
    here.ter_set( location, ter_id( result_terrain ) );

    for( int i = 0; i < byproducts_count; i++ ) {
        here.spawn_items( byproducts_location,
                          item_group::items_from( item_group_id( byproducts_item_group ),
                                  calendar::turn ) );
    }

    const int act_exertion = act.moves_total;

    who.mod_stored_kcal( std::min( -1, -act_exertion / to_moves<int>( 80_seconds ) ) );
    who.mod_thirst( std::max( 1, act_exertion / to_moves<int>( 12_minutes ) ) );
    who.mod_fatigue( std::max( 1, act_exertion / to_moves<int>( 6_minutes ) ) );
    who.add_msg_if_player( m_good, _( "You finish digging up %s." ),
                           here.ter( location ).obj().name() );

    act.set_to_null();
}

void dig_channel_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "moves", moves_total );
    jsout.member( "location", location );
    jsout.member( "result_terrain", result_terrain );
    jsout.member( "byproducts_location", byproducts_location );
    jsout.member( "byproducts_count", byproducts_count );
    jsout.member( "byproducts_item_group", byproducts_item_group );

    jsout.end_object();
}

std::unique_ptr<activity_actor> dig_channel_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<dig_channel_activity_actor> actor( new dig_channel_activity_actor( 0, tripoint_zero,
            {}, tripoint_zero, 0, {} ) );

    JsonObject data = jsin.get_object();

    data.read( "moves", actor->moves_total );
    data.read( "location", actor->location );
    data.read( "result_terrain", actor->result_terrain );
    data.read( "byproducts_location", actor->byproducts_location );
    data.read( "byproducts_count", actor->byproducts_count );
    data.read( "byproducts_item_group", actor->byproducts_item_group );

    return actor;
}

bool disassemble_activity_actor::try_start_single( player_activity &act, Character &who )
{
    if( targets.empty() ) {
        return false;
    }
    const iuse_location &target = targets.front();
    if( !target.loc ) {
        debugmsg( "Lost target of ACT_DISASSEMBLE" );
        targets.clear();
        return false;
    }
    const item &itm = *target.loc;
    const recipe &dis = recipe_dictionary::get_uncraft( itm.typeId() );

    // Have to check here again in case we ran out of tools
    const ret_val<bool> can_do = crafting::can_disassemble( who, itm, who.crafting_inventory() );
    if( !can_do.success() ) {
        who.add_msg_if_player( m_info, "%s", can_do.c_str() );
        return false;
    }

    int moves_needed = dis.time * target.count;

    act.moves_total = moves_needed;
    act.moves_left = moves_needed;
    return true;
}

int disassemble_activity_actor::calc_num_targets() const
{
    return static_cast<int>( targets.size() );
}

void disassemble_activity_actor::start( player_activity &act, Character &who )
{
    if( !who.is_avatar() ) {
        debugmsg( "ACT_DISASSEMBLE is not implemented for NPCs" );
        act.set_to_null();
    } else if( !try_start_single( act, who ) ) {
        act.set_to_null();
    }
    initial_num_targets = calc_num_targets();
}

void disassemble_activity_actor::finish( player_activity &act, Character &who )
{
    const iuse_location &target = targets.front();
    if( !target.loc ) {
        debugmsg( "Lost target of ACT_DISASSEMBLY" );
    } else {
        crafting::complete_disassemble( who, target, get_map().getlocal( pos.raw() ) );
    }
    targets.erase( targets.begin() );

    if( try_start_single( act, who ) ) {
        return;
    }

    // Make a copy to avoid use-after-free
    bool recurse = this->recursive;

    act.set_to_null();

    if( recurse ) {
        crafting::disassemble_all( *who.as_avatar(), recurse );
    }
}

void disassemble_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "targets", targets );
    jsout.member( "pos", pos );
    jsout.member( "recursive", recursive );
    jsout.member( "initial_num_targets", initial_num_targets );

    jsout.end_object();
}

std::unique_ptr<activity_actor> disassemble_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<disassemble_activity_actor> actor( new disassemble_activity_actor() );

    JsonObject data = jsin.get_object();

    data.read( "targets", actor->targets );
    data.read( "pos", actor->pos );
    data.read( "recursive", actor->recursive );
    data.read( "initial_num_targets", actor->initial_num_targets );

    return actor;
}

act_progress_message disassemble_activity_actor::get_progress_message(
    const player_activity &act, const Character & ) const
{
    std::string msg;

    const int percentage = ( ( act.moves_total - act.moves_left ) * 100 ) / act.moves_total;

    msg = string_format( "%d%%", percentage );

    if( initial_num_targets != 1 ) {
        constexpr int width = 20; // An arbitrary value
        std::string item_name_trimmed = trim_by_length( targets.front().loc->display_name(), width );

        msg += string_format(
                   _( "\n%d out of %d, working on %-20s" ),
                   initial_num_targets - calc_num_targets() + 1,
                   initial_num_targets,
                   item_name_trimmed
               );
    }

    return act_progress_message::make_extra_info( std::move( msg ) );
}

drop_activity_actor::drop_activity_actor( Character &ch, const drop_locations &items,
        bool force_ground, const tripoint &relpos )
    : force_ground( force_ground ), relpos( relpos )
{
    this->items = pickup::reorder_for_dropping( ch, items );
}

void drop_activity_actor::start( player_activity &act, Character & )
{
    // Set moves_left to value other than zero to indicate ongoing activity
    act.moves_total = 1;
    act.moves_left = 1;
}

void drop_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "items", items );
    jsout.member( "force_ground", force_ground );
    jsout.member( "relpos", relpos );

    jsout.end_object();
}

std::unique_ptr<activity_actor> drop_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<drop_activity_actor> actor( new drop_activity_actor() );

    JsonObject data = jsin.get_object();

    data.read( "items", actor->items );
    data.read( "force_ground", actor->force_ground );
    data.read( "relpos", actor->relpos );

    return actor;
}

void hacking_activity_actor::start( player_activity &act, Character & )
{
    act.moves_total = to_moves<int>( 5_minutes );
    act.moves_left = to_moves<int>( 5_minutes );
}

enum hack_result {
    HACK_UNABLE,
    HACK_FAIL,
    HACK_NOTHING,
    HACK_SUCCESS
};

enum hack_type {
    HACK_SAFE,
    HACK_DOOR,
    HACK_GAS,
    HACK_NULL
};

static int hack_level( const Character &who )
{
    ///\EFFECT_COMPUTER increases success chance of hacking card readers
    // odds go up with int>8, down with int<8
    // 4 int stat is worth 1 computer skill here
    ///\EFFECT_INT increases success chance of hacking card readers
    return who.get_skill_level( skill_computer ) + who.int_cur / 2 - 8;
}

static hack_result hack_attempt( Character &who, const bool using_bionic )
{
    // TODO: Remove this once player -> Character migration is complete
    {
        player *p = dynamic_cast<player *>( &who );
        p->practice( skill_computer, 20 );
    }

    // only skilled supergenius never cause short circuits, but the odds are low for people
    // with moderate skills
    const int hack_stddev = 5;
    int success = std::ceil( normal_roll( hack_level( who ), hack_stddev ) );
    if( success < 0 ) {
        who.add_msg_if_player( _( "You cause a short circuit!" ) );
        if( using_bionic ) {
            who.mod_power_level( -25_kJ );
        } else {
            who.use_charges( itype_electrohack, 25 );
        }

        if( success <= -5 ) {
            if( !using_bionic ) {
                who.add_msg_if_player( m_bad, _( "Your electrohack is ruined!" ) );
                who.use_amount( itype_electrohack, 1 );
            } else {
                who.add_msg_if_player( m_bad, _( "Your power is drained!" ) );
                who.mod_power_level( units::from_kilojoule( -rng( 25,
                                     units::to_kilojoule( who.get_power_level() ) ) ) );
            }
        }
        return HACK_FAIL;
    } else if( success < 6 ) {
        return HACK_NOTHING;
    } else {
        return HACK_SUCCESS;
    }
}

static hack_type get_hack_type( tripoint examp )
{
    hack_type type = HACK_NULL;
    const map &here = get_map();
    const furn_t &xfurn_t = here.furn( examp ).obj();
    const ter_t &xter_t = here.ter( examp ).obj();
    if( xter_t.examine == &iexamine::pay_gas || xfurn_t.examine == &iexamine::pay_gas ) {
        type = HACK_GAS;
    } else if( xter_t.examine == &iexamine::cardreader || xfurn_t.examine == &iexamine::cardreader ) {
        type = HACK_DOOR;
    } else if( xter_t.examine == &iexamine::gunsafe_el || xfurn_t.examine == &iexamine::gunsafe_el ) {
        type = HACK_SAFE;
    }
    return type;
}

hacking_activity_actor::hacking_activity_actor( use_bionic )
    : using_bionic( true )
{
}

void hacking_activity_actor::finish( player_activity &act, Character &who )
{
    tripoint examp = act.placement;
    hack_type type = get_hack_type( examp );
    map &here = get_map();
    switch( hack_attempt( who, using_bionic ) ) {
        case HACK_UNABLE:
            who.add_msg_if_player( _( "You cannot hack this." ) );
            break;
        case HACK_FAIL:
            // currently all things that can be hacked have equivalent alarm failure states.
            // this may not always be the case with new hackable things.
            g->events().send<event_type::triggers_alarm>( who.getID() );
            sounds::sound( who.pos(), 60, sounds::sound_t::music, _( "an alarm sound!" ), true, "environment",
                           "alarm" );
            if( examp.z > 0 && !g->timed_events.queued( TIMED_EVENT_WANTED ) ) {
                g->timed_events.add( TIMED_EVENT_WANTED, calendar::turn + 30_minutes, 0,
                                     who.global_sm_location() );
            }
            break;
        case HACK_NOTHING:
            who.add_msg_if_player( _( "You fail the hack, but no alarms are triggered." ) );
            break;
        case HACK_SUCCESS:
            if( type == HACK_GAS ) {
                int tankGasUnits;
                const std::optional<tripoint> pTank_ = iexamine::getNearFilledGasTank( examp, tankGasUnits );
                if( !pTank_ ) {
                    break;
                }
                const tripoint pTank = *pTank_;
                const std::optional<tripoint> pGasPump = iexamine::getGasPumpByNumber( examp,
                        uistate.ags_pay_gas_selected_pump );
                if( pGasPump && iexamine::toPumpFuel( pTank, *pGasPump, tankGasUnits ) ) {
                    who.add_msg_if_player( _( "You hack the terminal and route all available fuel to your pump!" ) );
                    sounds::sound( examp, 6, sounds::sound_t::activity,
                                   _( "Glug Glug Glug Glug Glug Glug Glug Glug Glug" ), true, "tool", "gaspump" );
                } else {
                    who.add_msg_if_player( _( "Nothing happens." ) );
                }
            } else if( type == HACK_SAFE ) {
                who.add_msg_if_player( m_good, _( "The door on the safe swings open." ) );
                here.furn_set( examp, furn_str_id( "f_safe_o" ) );
            } else if( type == HACK_DOOR ) {
                who.add_msg_if_player( _( "You activate the panel!" ) );
                who.add_msg_if_player( m_good, _( "The nearby doors unlock." ) );
                here.ter_set( examp, t_card_reader_broken );
                for( const tripoint &tmp : here.points_in_radius( ( examp ), 3 ) ) {
                    if( here.ter( tmp ) == t_door_metal_locked ) {
                        here.ter_set( tmp, t_door_metal_c );
                    }
                }
            }
            break;
    }
    act.set_to_null();
}

void hacking_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();
    jsout.member( "using_bionic", using_bionic );
    jsout.end_object();
}

std::unique_ptr<activity_actor> hacking_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<hacking_activity_actor> actor( new hacking_activity_actor() );
    if( jsin.test_null() ) {
        // Old saves might contain a null instead of an object.
        // Since we do not know whether a bionic or an item was chosen we assume
        // it was an item.
        actor->using_bionic = false;
    } else {
        JsonObject jsobj = jsin.get_object();
        jsobj.read( "using_bionic", actor->using_bionic );
    }
    return actor;
}

void move_items_activity_actor::do_turn( player_activity &act, Character &who )
{
    const tripoint dest = relative_destination + who.pos();

    while( who.moves > 0 && !target_items.empty() ) {
        safe_reference<item> target = std::move( target_items.back() );
        const int quantity = quantities.back();
        target_items.pop_back();
        quantities.pop_back();

        if( !target ) {
            //TODO!: might not be appropriate to debugmsg just because something was destroyed/unloaded
            debugmsg( "Lost target item of ACT_MOVE_ITEMS" );
            continue;
        }

        // Check that we can pick it up.
        if( target->made_of( LIQUID ) ) {
            continue;
        }

        // This is for hauling across zlevels, remove when going up and down stairs
        // is no longer teleportation
        if( target->is_owned_by( who, true ) ) {
            target->set_owner( who );
        } else {
            continue;
        }

        const tripoint src = target->position();
        detached_ptr<item> newit = quantity == 0 ? target->detach() : target->split( quantity );

        const int distance = src.z == dest.z ? std::max( rl_dist( src, dest ), 1 ) : 1;
        who.mod_moves( -pickup::cost_to_move_item( who, *newit ) * distance );

        std::vector<detached_ptr<item>> vec;
        vec.push_back( std::move( newit ) );
        if( to_vehicle ) {
            put_into_vehicle_or_drop( who, item_drop_reason::deliberate, vec, dest );
        } else {
            drop_on_map( who, item_drop_reason::deliberate, vec, dest );
        }
    }

    if( target_items.empty() ) {
        // Nuke the current activity, leaving the backlog alone.
        act.set_to_null();
        if( who.is_hauling() && !has_haulable_items( who.pos() ) ) {
            who.stop_hauling();
        }
    }
}

void move_items_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "target_items", target_items );
    jsout.member( "quantities", quantities );
    jsout.member( "to_vehicle", to_vehicle );
    jsout.member( "relative_destination", relative_destination );

    jsout.end_object();
}

std::unique_ptr<activity_actor> move_items_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<move_items_activity_actor> actor( new move_items_activity_actor( {}, {}, false,
            tripoint_zero ) );

    JsonObject data = jsin.get_object();

    data.read( "target_items", actor->target_items );
    data.read( "quantities", actor->quantities );
    data.read( "to_vehicle", actor->to_vehicle );
    data.read( "relative_destination", actor->relative_destination );

    return actor;
}

void pickup_activity_actor::do_turn( player_activity &act, Character &who )
{
    // If we don't have target items bail out
    if( target_items.empty() ) {
        who.cancel_activity();
        return;
    }

    // If the player moves while picking up (i.e.: in a moving vehicle) cancel
    // the activity, only populate starting_pos when grabbing from the ground
    if( starting_pos && *starting_pos != who.pos() ) {
        who.cancel_activity();
        who.add_msg_if_player( _( "Moving canceled auto-pickup." ) );
        return;
    }

    // Auto_resume implies autopickup.
    const bool autopickup = who.activity->auto_resume;

    // False indicates that the player canceled pickup when met with some prompt
    const bool keep_going = pickup::do_pickup( target_items, autopickup );

    // Check thievey witness
    npc *witness = nullptr;
    if( !act.str_values.empty() && act.str_values[0] == has_thievery_witness ) {
        for( npc &guy : g->all_npcs() ) {
            if( guy.get_attitude() == NPCATT_RECOVER_GOODS ) {
                witness = &guy;
                break;
            }
        }
    }

    // If there are items left we ran out of moves, so continue the activity
    // Otherwise, we are done.
    if( !keep_going || target_items.empty() || witness ) {
        who.cancel_activity();

        if( who.get_value( "THIEF_MODE_KEEP" ) != "YES" ) {
            who.set_value( "THIEF_MODE", "THIEF_ASK" );
        }

        if( !keep_going ) {
            // The user canceled the activity, so we're done
            // AIM might have more pickup activities pending, also cancel them.
            // TODO: Move this to advanced inventory instead of hacking it in here
            cancel_aim_processing();
        }

        if( witness ) {
            witness->talk_to_u();
            // Then remove "has_thievery_witness" from the activity
            act.str_values.clear();
        }
    }
}

void pickup_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "target_items", target_items );
    jsout.member( "starting_pos", starting_pos );

    jsout.end_object();
}

std::unique_ptr<activity_actor> pickup_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<pickup_activity_actor> actor( new pickup_activity_actor( {}, std::nullopt ) );

    JsonObject data = jsin.get_object();

    data.read( "target_items", actor->target_items );
    data.read( "starting_pos", actor->starting_pos );

    return actor;
}

std::unique_ptr<lockpick_activity_actor> lockpick_activity_actor::use_item(
    int moves_total,
    item &lockpick,
    const tripoint &target
)
{
    return std::unique_ptr<lockpick_activity_actor> ( new lockpick_activity_actor(
                moves_total,
                safe_reference<item>( lockpick ),
                detached_ptr<item>(),
                target
            ) );
}

std::unique_ptr<lockpick_activity_actor> lockpick_activity_actor::use_bionic(
    detached_ptr<item> &&fake_lockpick,
    const tripoint &target
)
{
    return std::unique_ptr<lockpick_activity_actor>( new lockpick_activity_actor(
                to_moves<int>( 5_seconds ),
                safe_reference<item>(),
                std::move( fake_lockpick ),
                target
            ) );
}

void lockpick_activity_actor::start( player_activity &act, Character & )
{
    act.moves_left = moves_total;
    act.moves_total = moves_total;
}

void lockpick_activity_actor::finish( player_activity &act, Character &who )
{
    act.set_to_null();

    item *it = nullptr;
    if( lockpick ) {
        it = &*lockpick;
    } else if( fake_lockpick ) {
        it = &*fake_lockpick;
    }

    if( !it ) {
        debugmsg( "Lost ACT_LOCKPICK item" );
        return;
    }

    const tripoint target = g->m.getlocal( this->target );
    const ter_id ter_type = g->m.ter( target );
    const furn_id furn_type = g->m.furn( target );
    ter_id new_ter_type = t_null;
    furn_id new_furn_type = f_null;
    std::string open_message = _( "The lock opens…" );

    if( g->m.has_furn( target ) ) {
        if( furn_type->lockpick_result.is_null() ) {
            debugmsg( "%s lockpick_result is null", furn_type.id().str() );
            return;
        }

        new_furn_type = furn_type->lockpick_result;
        if( !furn_type->lockpick_message.empty() ) {
            open_message = furn_type->lockpick_message.translated();
        }
    } else {
        if( ter_type->lockpick_result.is_null() ) {
            debugmsg( "%s lockpick_result is null", ter_type.id().str() );
            return;
        }

        new_ter_type = ter_type->lockpick_result;
        if( !ter_type->lockpick_message.empty() ) {
            open_message = ter_type->lockpick_message.translated();
        }
    }

    bool perfect = it->has_flag( flag_PERFECT_LOCKPICK );
    bool destroy = false;

    /** @EFFECT_DEX improves chances of successfully picking door lock, reduces chances of bad outcomes */
    /** @EFFECT_MECHANICS improves chances of successfully picking door lock, reduces chances of bad outcomes */
    int pick_roll = 5 *
                    ( std::pow( 1.3, who.get_skill_level( skill_mechanics ) ) +
                      it->get_quality( qual_LOCKPICK ) - it->damage() / 2000.0 ) +
                    who.dex_cur / 4.0;
    int lock_roll = rng( 1, 120 );
    int xp_gain = 0;
    if( perfect || ( pick_roll >= lock_roll ) ) {
        xp_gain += lock_roll;
        g->m.has_furn( target ) ?
        g->m.furn_set( target, new_furn_type ) :
        static_cast<void>( g->m.ter_set( target, new_ter_type ) );
        who.add_msg_if_player( m_good, open_message );
    } else if( furn_type == f_gunsafe_ml && lock_roll > ( 3 * pick_roll ) ) {
        who.add_msg_if_player( m_bad, _( "Your clumsy attempt jams the lock!" ) );
        g->m.furn_set( target, furn_str_id( "f_gunsafe_mj" ) );
    } else if( lock_roll > ( 1.5 * pick_roll ) ) {
        if( it->inc_damage() ) {
            who.add_msg_if_player( m_bad,
                                   _( "The lock stumps your efforts to pick it, and you destroy your tool." ) );
            destroy = true;
        } else {
            who.add_msg_if_player( m_bad,
                                   _( "The lock stumps your efforts to pick it, and you damage your tool." ) );
        }
    } else {
        who.add_msg_if_player( m_bad, _( "The lock stumps your efforts to pick it." ) );
    }

    if( !perfect ) {
        // You don't gain much skill since the item does all the hard work for you
        xp_gain += std::pow( 2, who.get_skill_level( skill_mechanics ) ) + 1;
    }
    who.practice( skill_mechanics, xp_gain );

    if( !perfect && g->m.has_flag( "ALARMED", target ) && ( lock_roll + dice( 1, 30 ) ) > pick_roll ) {
        sounds::sound( who.pos(), 40, sounds::sound_t::alarm, _( "an alarm sound!" ),
                       true, "environment", "alarm" );
        if( !g->timed_events.queued( TIMED_EVENT_WANTED ) ) {
            g->timed_events.add( TIMED_EVENT_WANTED, calendar::turn + 30_minutes, 0,
                                 who.global_sm_location() );
        }
    }

    if( destroy && lockpick ) {
        lockpick->detach();
    }
}

bool lockpick_activity_actor::is_pickable( const tripoint &p )
{
    return g->m.has_furn( p ) ? !g->m.furn( p )->lockpick_result.is_null() :
           !g->m.ter( p )->lockpick_result.is_null();
}

std::optional<tripoint> lockpick_activity_actor::select_location( avatar &you )
{
    if( you.is_mounted() ) {
        you.add_msg_if_player( m_info, _( "You cannot do that while mounted." ) );
        return std::nullopt;
    }

    const std::optional<tripoint> target = choose_adjacent_highlight(
            _( "Use your lockpick where?" ), _( "There is nothing to lockpick nearby." ), is_pickable, false );
    if( !target ) {
        return std::nullopt;
    }

    if( is_pickable( *target ) ) {
        return target;
    }

    const ter_id terr_type = g->m.ter( *target );
    if( *target == you.pos() ) {
        you.add_msg_if_player( m_info, _( "You pick your nose and your sinuses swing open." ) );
    } else if( g->critter_at<npc>( *target ) ) {
        you.add_msg_if_player( m_info,
                               _( "You can pick your friends, and you can pick your nose, but you can't pick your friend's nose." ) );
    } else if( !terr_type->open.is_null() ) {
        you.add_msg_if_player( m_info, _( "That door isn't locked." ) );
    } else {
        you.add_msg_if_player( m_info, _( "That cannot be picked." ) );
    }
    return std::nullopt;
}

void lockpick_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "moves_total", moves_total );
    jsout.member( "lockpick", lockpick );
    jsout.member( "fake_lockpick", fake_lockpick );
    jsout.member( "target", target );

    jsout.end_object();
}

std::unique_ptr<activity_actor> lockpick_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<lockpick_activity_actor> actor( new lockpick_activity_actor( 0,
            safe_reference<item>(), detached_ptr<item>(), tripoint_zero ) );

    JsonObject data = jsin.get_object();

    data.read( "moves_total", actor->moves_total );
    data.read( "lockpick", actor->lockpick );
    data.read( "fake_lockpick", actor->fake_lockpick );
    data.read( "target", actor->target );

    return actor;
}

void migration_cancel_activity_actor::do_turn( player_activity &act, Character &who )
{
    // Stop the activity
    act.set_to_null();

    // Ensure that neither avatars nor npcs end up in an invalid state
    if( who.is_npc() ) {
        npc &npc_who = dynamic_cast<npc &>( who );
        npc_who.revert_after_activity();
    } else {
        avatar &avatar_who = dynamic_cast<avatar &>( who );
        avatar_who.clear_destination();
        avatar_who.backlog.clear();
    }
}

void migration_cancel_activity_actor::serialize( JsonOut &jsout ) const
{
    // This will probably never be called, but write null to avoid invalid json in
    // the case that it is
    jsout.write_null();
}

std::unique_ptr<activity_actor> migration_cancel_activity_actor::deserialize( JsonIn & )
{
    return std::unique_ptr<migration_cancel_activity_actor>();
}

void toggle_gate_activity_actor::start( player_activity &act, Character & )
{
    act.moves_total = moves_total;
    act.moves_left = moves_total;
}

void toggle_gate_activity_actor::finish( player_activity &act, Character & )
{
    gates::toggle_gate( placement );
    act.set_to_null();
}

void toggle_gate_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "moves", moves_total );
    jsout.member( "placement", placement );

    jsout.end_object();
}

std::unique_ptr<activity_actor> toggle_gate_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<toggle_gate_activity_actor> actor( new toggle_gate_activity_actor( 0,
            tripoint_zero ) );

    JsonObject data = jsin.get_object();

    data.read( "moves", actor->moves_total );
    data.read( "placement", actor->placement );

    return actor;
}

void wash_activity_actor::start( player_activity &act, Character & )
{
    act.moves_total = moves_total;
    act.moves_left = moves_total;
}

stash_activity_actor::stash_activity_actor( Character &ch, const drop_locations &items,
        const tripoint &relpos ) : relpos( relpos )
{
    this->items = pickup::reorder_for_dropping( ch, items );
}

void stash_activity_actor::start( player_activity &act, Character & )
{
    // Set moves_left to value other than zero to indicate ongoing activity
    act.moves_total = 1;
    act.moves_left = 1;
}

void stash_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "items", items );
    jsout.member( "relpos", relpos );

    jsout.end_object();
}

std::unique_ptr<activity_actor> stash_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<stash_activity_actor> actor( new stash_activity_actor() );

    JsonObject data = jsin.get_object();

    data.read( "items", actor->items );
    data.read( "relpos", actor->relpos );

    return actor;
}

void throw_activity_actor::do_turn( player_activity &act, Character &who )
{
    // Make copies of relevant values since the class would
    // not be available after act.set_to_null()
    if( !target ) {
        debugmsg( "Lost weapon while throwing" );
        act.set_to_null();
        return;
    }

    item *it = &*target;
    std::optional<tripoint> blind_throw_pos = blind_throw_from_pos;

    // Stop the activity. Whether we will or will not throw doesn't matter.
    act.set_to_null();
    if( !who.is_avatar() ) {
        // Sanity check
        debugmsg( "ACT_THROW is not applicable for NPCs." );
        return;
    }

    // Shift our position to our "peeking" position, so that the UI
    // for picking a throw point lets us target the location we couldn't
    // otherwise see.
    const tripoint original_player_position = who.pos();
    if( blind_throw_pos ) {
        who.setpos( *blind_throw_pos );
    }

    target_handler::trajectory trajectory = target_handler::mode_throw( *who.as_avatar(), *it,
                                            blind_throw_pos.has_value() );

    // If we previously shifted our position, put ourselves back now that we've picked our target.
    if( blind_throw_pos ) {
        who.setpos( original_player_position );
    }

    if( trajectory.empty() ) {
        return;
    }

    if( it != &who.primary_weapon() ) {
        // This is to represent "implicit offhand wielding"
        int extra_cost = who.item_handling_cost( *it, true, INVENTORY_HANDLING_PENALTY / 2 );
        who.mod_moves( -extra_cost );
    }
    detached_ptr<item> det = target->split( 1 );
    ranged::throw_item( who, trajectory.back(), std::move( det ), blind_throw_pos );
}

void throw_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "target_loc", target );
    jsout.member( "blind_throw_from_pos", blind_throw_from_pos );

    jsout.end_object();
}

std::unique_ptr<activity_actor> throw_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<throw_activity_actor> actor;

    JsonObject data = jsin.get_object();

    data.read( "target_loc", actor->target );
    data.read( "blind_throw_from_pos", actor->blind_throw_from_pos );

    return actor;
}

void wash_activity_actor::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "targets", targets );
    jsout.member( "moves_total", moves_total );

    jsout.end_object();
}

std::unique_ptr<activity_actor> wash_activity_actor::deserialize( JsonIn &jsin )
{
    std::unique_ptr<wash_activity_actor> actor;

    JsonObject data = jsin.get_object();

    data.read( "targets", actor->targets );
    data.read( "moves_total", actor->moves_total );

    return actor;
}

namespace activity_actors
{

// Please keep this alphabetically sorted
const std::unordered_map<activity_id, std::unique_ptr<activity_actor>( * )( JsonIn & )>
deserialize_functions = {
    { activity_id( "ACT_AIM" ), &aim_activity_actor::deserialize },
    { activity_id( "ACT_AUTODRIVE" ), &autodrive_activity_actor::deserialize },
    { activity_id( "ACT_DIG" ), &dig_activity_actor::deserialize },
    { activity_id( "ACT_DIG_CHANNEL" ), &dig_channel_activity_actor::deserialize },
    { activity_id( "ACT_DROP" ), &drop_activity_actor::deserialize },
    { activity_id( "ACT_HACKING" ), &hacking_activity_actor::deserialize },
    { activity_id( "ACT_LOCKPICK" ), &lockpick_activity_actor::deserialize },
    { activity_id( "ACT_MIGRATION_CANCEL" ), &migration_cancel_activity_actor::deserialize },
    { activity_id( "ACT_MOVE_ITEMS" ), &move_items_activity_actor::deserialize },
    { activity_id( "ACT_TOGGLE_GATE" ), &toggle_gate_activity_actor::deserialize },
    { activity_id( "ACT_PICKUP" ), &pickup_activity_actor::deserialize },
    { activity_id( "ACT_STASH" ), &stash_activity_actor::deserialize },
    { activity_id( "ACT_THROW" ), &throw_activity_actor::deserialize },
    { activity_id( "ACT_WASH" ), &wash_activity_actor::deserialize },
};
} // namespace activity_actors

void serialize( const std::unique_ptr<activity_actor> &actor, JsonOut &jsout )
{
    if( !actor ) {
        jsout.write_null();
    } else {
        jsout.start_object();

        jsout.member( "actor_type", actor->get_type() );
        jsout.member( "actor_data", *actor );

        jsout.end_object();
    }
}

void deserialize( std::unique_ptr<activity_actor> &actor, JsonIn &jsin )
{
    if( jsin.test_null() ) {
        actor = nullptr;
    } else {
        JsonObject data = jsin.get_object();
        if( data.has_member( "actor_data" ) ) {
            activity_id actor_type;
            data.read( "actor_type", actor_type );
            auto deserializer = activity_actors::deserialize_functions.find( actor_type );
            if( deserializer != activity_actors::deserialize_functions.end() ) {
                actor = deserializer->second( *data.get_raw( "actor_data" ) );
            } else {
                debugmsg( "Failed to find activity actor deserializer for type \"%s\"", actor_type.c_str() );
                actor = nullptr;
            }
        } else {
            debugmsg( "Failed to load activity actor" );
            actor = nullptr;
        }
    }
}
