#include "ranged.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "activity_actor_definitions.h"
#include "avatar.h"
#include "ballistics.h"
#include "bodypart.h"
#include "calendar.h"
#include "cata_utility.h"
#include "catacharset.h"
#include "character.h"
#include "color.h"
#include "creature.h"
#include "cursesdef.h"
#include "damage.h"
#include "debug.h"
#include "dispersion.h"
#include "enums.h"
#include "event.h"
#include "event_bus.h"
#include "game.h"
#include "game_constants.h"
#include "gun_mode.h"
#include "input.h"
#include "item.h"
#include "item_location.h"
#include "itype.h"
#include "line.h"
#include "magic.h"
#include "map.h"
#include "material.h"
#include "math_defines.h"
#include "messages.h"
#include "monster.h"
#include "morale_types.h"
#include "mtype.h"
#include "npc.h"
#include "optional.h"
#include "options.h"
#include "output.h"
#include "panels.h"
#include "player.h"
#include "player_activity.h"
#include "point.h"
#include "projectile.h"
#include "rng.h"
#include "skill.h"
#include "sounds.h"
#include "string_formatter.h"
#include "string_id.h"
#include "translations.h"
#include "trap.h"
#include "type_id.h"
#include "ui_manager.h"
#include "units.h"
#include "units_angle.h"
#include "units_utility.h"
#include "value_ptr.h"
#include "vehicle.h"
#include "vpart_position.h"

struct ammo_effect;

using ammo_effect_str_id = string_id<ammo_effect>;

static const ammo_effect_str_id ammo_effect_ACT_ON_RANGED_HIT( "ACT_ON_RANGED_HIT" );
static const ammo_effect_str_id ammo_effect_BLACKPOWDER( "BLACKPOWDER" );
static const ammo_effect_str_id ammo_effect_BOUNCE( "BOUNCE" );
static const ammo_effect_str_id ammo_effect_BURST( "BURST" );
static const ammo_effect_str_id ammo_effect_CUSTOM_EXPLOSION( "CUSTOM_EXPLOSION" );
static const ammo_effect_str_id ammo_effect_EMP( "EMP" );
static const ammo_effect_str_id ammo_effect_EXPLOSIVE( "EXPLOSIVE" );
static const ammo_effect_str_id ammo_effect_HEAVY_HIT( "HEAVY_HIT" );
static const ammo_effect_str_id ammo_effect_IGNITE( "IGNITE" );
static const ammo_effect_str_id ammo_effect_LASER( "LASER" );
static const ammo_effect_str_id ammo_effect_LIGHTNING( "LIGHTNING" );
static const ammo_effect_str_id ammo_effect_NO_CRIT( "NO_CRIT" );
static const ammo_effect_str_id ammo_effect_NO_EMBED( "NO_EMBED" );
static const ammo_effect_str_id ammo_effect_NO_ITEM_DAMAGE( "NO_ITEM_DAMAGE" );
static const ammo_effect_str_id ammo_effect_NON_FOULING( "NON-FOULING" );
static const ammo_effect_str_id ammo_effect_PLASMA( "PLASMA" );
static const ammo_effect_str_id ammo_effect_RECYCLED( "RECYCLED" );
static const ammo_effect_str_id ammo_effect_SHATTER_SELF( "SHATTER_SELF" );
static const ammo_effect_str_id ammo_effect_SHOT( "SHOT" );
static const ammo_effect_str_id ammo_effect_TANGLE( "TANGLE" );
static const ammo_effect_str_id ammo_effect_WHIP( "WHIP" );
static const ammo_effect_str_id ammo_effect_WIDE( "WIDE" );

static const efftype_id effect_downed( "downed" );
static const efftype_id effect_hit_by_player( "hit_by_player" );
static const efftype_id effect_on_roof( "on_roof" );

static const itype_id itype_12mm( "12mm" );
static const itype_id itype_40x46mm( "40x46mm" );
static const itype_id itype_40x53mm( "40x53mm" );
static const itype_id itype_66mm( "66mm" );
static const itype_id itype_84x246mm( "84x246mm" );
static const itype_id itype_adv_UPS_off( "adv_UPS_off" );
static const itype_id itype_arrow( "arrow" );
static const itype_id itype_bolt( "bolt" );
static const itype_id itype_brass_catcher( "brass_catcher" );
static const itype_id itype_flammable( "flammable" );
static const itype_id itype_m235( "m235" );
static const itype_id itype_metal_rail( "metal_rail" );
static const itype_id itype_UPS( "UPS" );
static const itype_id itype_UPS_off( "UPS_off" );

static const trap_str_id tr_practice_target( "tr_practice_target" );

static const fault_id fault_gun_blackpowder( "fault_gun_blackpowder" );
static const fault_id fault_gun_chamber_spent( "fault_gun_chamber_spent" );
static const fault_id fault_gun_dirt( "fault_gun_dirt" );
static const fault_id fault_gun_unlubricated( "fault_gun_unlubricated" );

static const skill_id skill_dodge( "dodge" );
static const skill_id skill_driving( "driving" );
static const skill_id skill_gun( "gun" );
static const skill_id skill_launcher( "launcher" );
static const skill_id skill_throw( "throw" );

static const bionic_id bio_railgun( "bio_railgun" );
static const bionic_id bio_targeting( "bio_targeting" );
static const bionic_id bio_ups( "bio_ups" );

static const std::string flag_CONSUMABLE( "CONSUMABLE" );
static const std::string flag_FIRE_TWOHAND( "FIRE_TWOHAND" );
static const std::string flag_MOUNTABLE( "MOUNTABLE" );
static const std::string flag_MOUNTED_GUN( "MOUNTED_GUN" );
static const std::string flag_NEVER_JAMS( "NEVER_JAMS" );
static const std::string flag_NON_FOULING( "NON-FOULING" );
static const std::string flag_PRIMITIVE_RANGED_WEAPON( "PRIMITIVE_RANGED_WEAPON" );
static const std::string flag_PYROMANIAC_WEAPON( "PYROMANIAC_WEAPON" );
static const std::string flag_RELOAD_AND_SHOOT( "RELOAD_AND_SHOOT" );
static const std::string flag_RESTRICT_HANDS( "RESTRICT_HANDS" );
static const std::string flag_STR_DRAW( "STR_DRAW" );
static const std::string flag_UNDERWATER_GUN( "UNDERWATER_GUN" );
static const std::string flag_VEHICLE( "VEHICLE" );

static const trait_id trait_PYROMANIA( "PYROMANIA" );
static const trait_id trait_NORANGEDCRIT( "NO_RANGED_CRIT" );

// Maximum duration of aim-and-fire loop, in turns
static constexpr int AIF_DURATION_LIMIT = 10;

static projectile make_gun_projectile( const item &gun );
int time_to_attack( const Character &p, const itype &firing );
static void cycle_action( item &weap, const tripoint &pos );
void make_gun_sound_effect( const player &p, bool burst, item *weapon );
bool can_use_bipod( const map &m, const tripoint &pos );
dispersion_sources calculate_dispersion( const map &m, const player &p, const item &gun,
        int at_recoil, bool burst );

class target_ui
{
    public:
        /* None of the public members (except range) should be modified during execution */

        enum class TargetMode : int {
            Fire,
            Throw,
            ThrowBlind,
            Turrets,
            TurretManual,
            Reach,
            Spell,
            Shape
        };

        // Avatar
        avatar *you;
        // Interface mode
        TargetMode mode = TargetMode::Fire;
        // Weapon being fired/thrown
        item *relevant = nullptr;
        // Cached selection range from player's position
        int range = 0;
        // Turret being manually fired
        turret_data *turret = nullptr;
        // Turrets being fired (via vehicle controls)
        const std::vector<vehicle_part *> *vturrets = nullptr;
        // Vehicle that turrets belong to
        vehicle *veh = nullptr;
        // Spell being cast
        spell *casting = nullptr;
        // Spell cannot fail
        bool no_fail = false;
        // Spell does not require mana
        bool no_mana = false;
        // Relevant activity
        aim_activity_actor *activity = nullptr;
        // Generator of AoE shapes
        cata::optional<shape_factory> shape_gen;

        // Initialize UI and run the event loop
        target_handler::trajectory run();

    private:
        enum class ExitCode : int {
            Abort,
            Fire,
            Timeout,
            Reload
        };

        enum class Status : int {
            Good, // All UI elements are enabled
            BadTarget, // Bad 'dst' selected; forbid aiming/firing
            OutOfAmmo, // Selected gun mode is out of ammo; forbid moving cursor,aiming and firing
            OutOfRange // Selected target is out of range of current gun mode; forbid aiming/firing
        };

        // Ui status (affects which UI controls are temporarily disabled)
        Status status = Status::Good;

        // Cached current ammo to display
        const itype *ammo = nullptr;
        // Current trajectory
        std::vector<tripoint> traj;
        // Aiming source (player's position)
        tripoint src;
        // Aiming destination (cursor position)
        // Use set_cursor_pos() to modify
        tripoint dst;
        // Creature currently under cursor. nullptr if aiming at empty tile,
        // yourself or a creature you cannot see
        Creature *dst_critter = nullptr;
        // List of visible hostile targets
        std::vector<Creature *> targets;

        // 'true' if map has z levels and 3D fov is on
        bool allow_zlevel_shift = false;
        // Snap camera to cursor. Can be permanently toggled in settings
        // or temporarily in this window
        bool snap_to_target = false;
        // If true, LEVEL_UP, LEVEL_DOWN and directional keys
        // responsible for moving cursor will shift view instead.
        bool shifting_view = false;

        // Compact layout
        bool compact = false;
        // Tiny layout - when extremely short on space
        bool tiny = false;
        // Narrow layout - to keep in theme with
        // "compact" and "labels-narrow" sidebar styles.
        bool narrow = false;
        // Window
        catacurses::window w_target;
        // Input context
        input_context ctxt;

        /* These members are relevant for TargetMode::Fire */
        // Weapon sight dispersion
        int sight_dispersion = 0;
        // List of available weapon aim types
        std::vector<aim_type> aim_types;
        // Currently selected aim mode
        std::vector<aim_type>::iterator aim_mode;
        // 'Recoil' value the player will reach if they
        // start aiming at cursor position. Equals player's
        // 'recoil' while they are actively spending moves to aim,
        // but increases the further away the new aim point will be
        // relative to the current one.
        double predicted_recoil = 0;

        // For AOE spells, list of tiles affected by the spell
        // relevant for TargetMode::Spell
        std::set<tripoint> spell_aoe;

        // For shaped attacks, we want both points and coverage
        std::map<tripoint, double> shape_coverage;

        // Represents a turret and a straight line from that turret to target
        struct turret_with_lof {
            vehicle_part *turret;
            std::vector<tripoint> line;
        };

        // List of vehicle turrets in range (out of those listed in 'vturrets')
        std::vector<turret_with_lof> turrets_in_range;

        // If true, draws turret lines
        // relevant for TargetMode::Turrets
        bool draw_turret_lines = false;

        // Create window and set up input context
        void init_window_and_input();

        // Handle input related to cursor movement.
        // Returns 'true' if action was recognized and processed.
        // 'skip_redraw' is set to 'true' if there is no need to redraw the UI.
        bool handle_cursor_movement( const std::string &action, bool &skip_redraw );

        // Set cursor position. If new position is out of range,
        // selects closest position in range.
        // Returns 'false' if cursor position did not change
        bool set_cursor_pos( const tripoint &new_pos );

        // Called when range/ammo changes (or may have changed)
        void on_range_ammo_changed();

        // Updates 'targets' for current range
        void update_target_list();

        // Choose where to position the cursor when opening the ui
        tripoint choose_initial_target();

        /**
         * Try to re-acquire target for aim-and-fire.
         * @param critter whether were aiming at a critter, or a tile
         * @param new_dst where to move aim cursor (if e.g. critter moved)
         * @returns true on success
         */
        bool try_reacquire_target( bool critter, tripoint &new_dst );

        // Update 'status' variable
        void update_status();

        // Calculates distance from 'src'. For consistency, prefer using this over rl_dist.
        int dist_fn( const tripoint &p );

        // Set creature (or tile) under cursor as player's last target
        void set_last_target();

        // Prompts player to confirm attack on neutral NPC
        // Returns 'true' if attack should proceed
        bool confirm_non_enemy_target();

        // Prompts player to re-confirm an ongoing attack if
        // a non-hostile NPC / friendly creatures enters line of fire.
        // Returns 'true' if attack should proceed
        bool prompt_friendlies_in_lof();

        // List friendly creatures currently occupying line of fire.
        std::vector<weak_ptr_fast<Creature>> list_friendlies_in_lof();

        // Toggle snap-to-target
        void toggle_snap_to_target();

        // Cycle targets. 'direction' is either 1 or -1
        void cycle_targets( int direction );

        // Set new view offset. Updates map cache if necessary
        void set_view_offset( const tripoint &new_offset );

        // Updates 'turrets_in_range'
        void update_turrets_in_range();

        // Recalculate 'recoil' penalty. This should be called if
        // avatar's 'recoil' value has been modified
        // Relevant for TargetMode::Fire
        void recalc_aim_turning_penalty();

        // Apply penalty to avatar's 'recoil' value based on
        // how much they moved their aim point.
        // Relevant for TargetMode::Fire
        void apply_aim_turning_penalty();

        // Switch firing mode.
        void action_switch_mode();

        // Ensure we're using ranged gun mode.
        void ensure_ranged_gun_mode();

        // Update range & ammo from current gun mode
        void update_ammo_range_from_gun_mode();

        // Switch ammo. Returns 'false' if requires a reloading UI.
        bool action_switch_ammo();

        // Aim for 10 turns. Returns 'false' if ran out of moves
        bool action_aim();

        // Aim and shoot. Returns 'false' if ran out of moves
        bool action_aim_and_shoot( const std::string &action );

        // Draw UI-specific terrain overlays
        void draw_terrain_overlay();

        // Draw aiming window
        void draw_ui_window();

        // Generate ui window title
        std::string uitext_title();

        // Generate flavor text for 'Fire!' key
        std::string uitext_fire();

        void draw_window_title();
        void draw_help_notice();

        // Draw list of available controls at the bottom of the window.
        // text_y - first free line counting from the top
        void draw_controls_list( int text_y );

        void panel_cursor_info( int &text_y );
        void panel_gun_info( int &text_y );
        void panel_recoil( int &text_y );
        void panel_spell_info( int &text_y );
        void panel_target_info( int &text_y, bool fill_with_blank_if_no_target );
        void panel_fire_mode_aim( int &text_y );
        void panel_turret_list( int &text_y );

        // On-selected-as-target checks that act as if they are on-hit checks.
        // `harmful` is `false` if using a non-damaging spell
        void on_target_accepted( bool harmful );
};

target_handler::trajectory target_handler::mode_fire( avatar &you, aim_activity_actor &activity )
{
    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = target_ui::TargetMode::Fire;
    ui.activity = &activity;
    ui.relevant = activity.get_weapon();

    return ui.run();
}

target_handler::trajectory target_handler::mode_throw( avatar &you, item &relevant,
        bool blind_throwing )
{
    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = blind_throwing ? target_ui::TargetMode::ThrowBlind : target_ui::TargetMode::Throw;
    ui.relevant = &relevant;
    ui.range = you.throw_range( relevant );

    return ui.run();
}

target_handler::trajectory target_handler::mode_reach( avatar &you, item &weapon )
{
    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = target_ui::TargetMode::Reach;
    ui.relevant = &weapon;
    ui.range = weapon.reach_range( you );

    return ui.run();
}

target_handler::trajectory target_handler::mode_turret_manual( avatar &you, turret_data &turret )
{
    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = target_ui::TargetMode::TurretManual;
    ui.turret = &turret;
    ui.relevant = &*turret.base();

    return ui.run();
}

target_handler::trajectory target_handler::mode_turrets( avatar &you, vehicle &veh,
        const std::vector<vehicle_part *> &turrets )
{
    // Find radius of a circle centered at u encompassing all points turrets can aim at
    // FIXME: this calculation is fine for square distances, but results in an underestimation
    //        when used with real circles
    int range_total = 0;
    for( vehicle_part *t : turrets ) {
        int range = veh.turret_query( *t ).range();
        tripoint pos = veh.global_part_pos3( *t );

        int res = 0;
        res = std::max( res, rl_dist( you.pos(), pos + point( range, 0 ) ) );
        res = std::max( res, rl_dist( you.pos(), pos + point( -range, 0 ) ) );
        res = std::max( res, rl_dist( you.pos(), pos + point( 0, range ) ) );
        res = std::max( res, rl_dist( you.pos(), pos + point( 0, -range ) ) );
        range_total = std::max( range_total, res );
    }

    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = target_ui::TargetMode::Turrets;
    ui.veh = &veh;
    ui.vturrets = &turrets;
    ui.range = range_total;

    return ui.run();
}

target_handler::trajectory target_handler::mode_spell( avatar &you, spell &casting, bool no_fail,
        bool no_mana )
{
    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = target_ui::TargetMode::Spell;
    ui.casting = &casting;
    ui.range = casting.range();
    ui.no_fail = no_fail;
    ui.no_mana = no_mana;

    return ui.run();
}

target_handler::trajectory target_handler::mode_shaped( avatar &you, shape_factory &shape_fac,
        aim_activity_actor &activity )
{
    target_ui ui = target_ui();
    ui.you = &you;
    ui.mode = target_ui::TargetMode::Shape;
    ui.shape_gen = shape_fac;
    ui.range = shape_fac.get_range();
    ui.activity = &activity;
    ui.relevant = activity.get_weapon();

    return ui.run();
}

static double occupied_tile_fraction( m_size target_size )
{
    switch( target_size ) {
        case MS_TINY:
            return 0.1;
        case MS_SMALL:
            return 0.25;
        case MS_MEDIUM:
            return 0.5;
        case MS_LARGE:
            return 0.75;
        case MS_HUGE:
            return 1.0;
        default:
            break;
    }

    return 0.5;
}

double Creature::ranged_target_size() const
{
    if( has_flag( MF_HARDTOSHOOT ) ) {
        switch( get_size() ) {
            case MS_TINY:
            case MS_SMALL:
                return occupied_tile_fraction( MS_TINY );
            case MS_MEDIUM:
                return occupied_tile_fraction( MS_SMALL );
            case MS_LARGE:
                return occupied_tile_fraction( MS_MEDIUM );
            case MS_HUGE:
                return occupied_tile_fraction( MS_LARGE );
            default:
                break;
        }
    }
    return occupied_tile_fraction( get_size() );
}

int range_with_even_chance_of_good_hit( int dispersion )
{
    int even_chance_range = 0;
    while( static_cast<unsigned>( even_chance_range ) <
           Creature::dispersion_for_even_chance_of_good_hit.size() &&
           dispersion < Creature::dispersion_for_even_chance_of_good_hit[ even_chance_range ] ) {
        even_chance_range++;
    }
    return even_chance_range;
}

int player::gun_engagement_moves( const item &gun, int target, int start ) const
{
    int mv = 0;
    double penalty = start;

    while( penalty > target ) {
        double adj = aim_per_move( gun, penalty );
        if( adj <= 0 ) {
            break;
        }
        penalty -= adj;
        mv++;
    }

    return mv;
}

bool player::handle_gun_damage( item &it )
{
    // Below item (maximum dirt possible) should be greater than or equal to dirt range in item_group.cpp. Also keep in mind that monster drops can have specific ranges and these should be below the max!
    const double dirt_max_dbl = 10000;
    if( !it.is_gun() ) {
        debugmsg( "Tried to handle_gun_damage of a non-gun %s", it.tname() );
        return false;
    }

    int dirt = it.get_var( "dirt", 0 );
    int dirtadder = 0;
    double dirt_dbl = static_cast<double>( dirt );
    if( it.faults.count( fault_gun_chamber_spent ) ) {
        return false;
    }

    const auto &curammo_effects = it.ammo_effects();
    const islot_gun &firing = *it.type->gun;
    // misfire chance based on dirt accumulation. Formula is designed to make chance of jam highly unlikely at low dirt levels, but levels increase geometrically as the dirt level reaches max (10,000). The number used is just a figure I found reasonable after plugging the number into excel and changing it until the probability made sense at high, medium, and low levels of dirt.
    if( !it.has_flag( flag_NEVER_JAMS ) &&
        x_in_y( dirt_dbl * dirt_dbl * dirt_dbl,
                1000000000000.0 ) ) {
        add_msg_player_or_npc(
            _( "Your %s misfires with a muffled click!" ),
            _( "<npcname>'s %s misfires with a muffled click!" ),
            it.tname() );
        // at high dirt levels the chance to misfire gets to significant levels. 10,000 is max and 7800 is quite high so above that the player gets some relief in the form of exchanging time for some dirt reduction. Basically jiggling the parts loose to remove some dirt and get a few more shots out.
        if( dirt_dbl >
            7800 ) {
            add_msg_player_or_npc(
                _( "Perhaps taking the ammo out of your %s and reloading will help." ),
                _( "Perhaps taking the ammo out of <npcname>'s %s and reloading will help." ),
                it.tname() );
        }
        return false;
    }

    // Here we check if we're underwater and whether we should misfire.
    // As a result this causes no damage to the firearm, note that some guns are waterproof
    // and so are immune to this effect, note also that WATERPROOF_GUN status does not
    // mean the gun will actually be accurate underwater.
    int effective_durability = firing.durability;
    if( is_underwater() && !it.has_flag( "WATERPROOF_GUN" ) && one_in( effective_durability ) ) {
        add_msg_player_or_npc( _( "Your %s misfires with a wet click!" ),
                               _( "<npcname>'s %s misfires with a wet click!" ),
                               it.tname() );
        return false;
        // Here we check for a chance for the weapon to suffer a mechanical malfunction.
        // Note that some weapons never jam up 'NEVER_JAMS' and thus are immune to this
        // effect as current guns have a durability between 5 and 9 this results in
        // a chance of mechanical failure between 1/(64*3) and 1/(1024*3) on any given shot.
        // the malfunction can't cause damage
    } else if( one_in( ( 2 << effective_durability ) * 3 ) && !it.has_flag( flag_NEVER_JAMS ) ) {
        add_msg_player_or_npc( _( "Your %s malfunctions!" ),
                               _( "<npcname>'s %s malfunctions!" ),
                               it.tname() );
        return false;
        // Here we check for a chance for the weapon to suffer a misfire due to
        // using player-made 'RECYCLED' bullets. Note that not all forms of
        // player-made ammunition have this effect.
    } else if( curammo_effects.count( ammo_effect_RECYCLED ) && one_in( 256 ) ) {
        add_msg_player_or_npc( _( "Your %s misfires with a muffled click!" ),
                               _( "<npcname>'s %s misfires with a muffled click!" ),
                               it.tname() );
        return false;
        // Here we check for a chance for attached mods to get damaged if they are flagged as 'CONSUMABLE'.
        // This is mostly for crappy handmade expedient stuff  or things that rarely receive damage during normal usage.
        // Default chance is 1/10000 unless set via json, damage is proportional to caliber(see below).
        // Can be toned down with 'consume_divisor.'

    } else if( it.has_flag( flag_CONSUMABLE ) && !curammo_effects.count( ammo_effect_LASER ) &&
               !curammo_effects.count( ammo_effect_PLASMA ) && !curammo_effects.count( ammo_effect_EMP ) ) {
        int uncork = ( ( 10 * it.ammo_data()->ammo->loudness )
                       + ( it.ammo_data()->ammo->recoil / 2 ) ) / 100;
        uncork = std::pow( uncork, 3 ) * 6.5;
        for( auto mod : it.gunmods() ) {
            if( mod->has_flag( flag_CONSUMABLE ) ) {
                int dmgamt = uncork / mod->type->gunmod->consume_divisor;
                int modconsume = mod->type->gunmod->consume_chance;
                int initstate = it.damage();
                // fuzz damage if it's small
                if( dmgamt < 1000 ) {
                    dmgamt = rng( dmgamt, dmgamt + 200 );
                    // ignore damage if inconsequential.
                }
                if( dmgamt < 800 ) {
                    dmgamt = 0;
                }
                if( one_in( modconsume ) ) {
                    if( mod->mod_damage( dmgamt ) ) {
                        add_msg_player_or_npc( m_bad, _( "Your attached %s is destroyed by your shot!" ),
                                               _( "<npcname>'s attached %s is destroyed by their shot!" ),
                                               mod->tname() );
                        i_rem( mod );
                    } else if( it.damage() > initstate ) {
                        add_msg_player_or_npc( m_bad, _( "Your attached %s is damaged by your shot!" ),
                                               _( "<npcname>'s %s is damaged by their shot!" ),
                                               mod->tname() );
                    }
                }
            }
        }
    }
    if( it.has_fault( fault_gun_unlubricated ) &&
        x_in_y( dirt_dbl, dirt_max_dbl ) ) {
        add_msg_player_or_npc( m_bad, _( "Your %s emits a grimace-inducing screech!" ),
                               _( "<npcname>'s %s emits a grimace-inducing screech!" ),
                               it.tname() );
        it.inc_damage();
    }
    if( ( ( !curammo_effects.count( ammo_effect_NON_FOULING ) && !it.has_flag( flag_NON_FOULING ) ) ||
          ( it.has_fault( fault_gun_unlubricated ) ) ) &&
        !it.has_flag( flag_PRIMITIVE_RANGED_WEAPON ) ) {
        if( curammo_effects.count( ammo_effect_BLACKPOWDER ) ||
            it.has_fault( fault_gun_unlubricated ) ) {
            if( ( ( it.ammo_data()->ammo->recoil < firing.min_cycle_recoil ) ||
                  ( it.has_fault( fault_gun_unlubricated ) && one_in( 16 ) ) ) &&
                it.faults_potential().count( fault_gun_chamber_spent ) ) {
                add_msg_player_or_npc( m_bad, _( "Your %s fails to cycle!" ),
                                       _( "<npcname>'s %s fails to cycle!" ),
                                       it.tname() );
                it.faults.insert( fault_gun_chamber_spent );
                // Don't return false in this case; this shot happens, follow-up ones won't.
            }
        }
        // These are the dirtying/fouling mechanics
        if( !curammo_effects.count( ammo_effect_NON_FOULING ) && !it.has_flag( flag_NON_FOULING ) ) {
            if( dirt < static_cast<int>( dirt_max_dbl ) ) {
                dirtadder = curammo_effects.count( ammo_effect_BLACKPOWDER ) * ( 200 -
                            ( firing.blackpowder_tolerance *
                              2 ) );
                // dirtadder is the dirt-increasing number for shots fired with gunpowder-based ammo. Usually dirt level increases by 1, unless it's blackpowder, in which case it increases by a higher number, but there is a reduction for blackpowder resistance of a weapon.
                if( dirtadder < 0 ) {
                    dirtadder = 0;
                }
                // in addition to increasing dirt level faster, regular gunpowder fouling is also capped at 7,150, not 10,000. So firing with regular gunpowder can never make the gun quite as bad as firing it with black gunpowder. At 7,150 the chance to jam is significantly lower (though still significant) than it is at 10,000, the absolute cap.
                if( curammo_effects.count( ammo_effect_BLACKPOWDER ) ||
                    dirt < 7150 ) {
                    it.set_var( "dirt", std::min( static_cast<int>( dirt_max_dbl ), dirt + dirtadder + 1 ) );
                }
            }
            dirt = it.get_var( "dirt", 0 );
            dirt_dbl = static_cast<double>( dirt );
            if( dirt > 0 && !it.faults.count( fault_gun_blackpowder ) ) {
                it.faults.insert( fault_gun_dirt );
            }
            if( dirt > 0 && curammo_effects.count( ammo_effect_BLACKPOWDER ) ) {
                it.faults.erase( fault_gun_dirt );
                it.faults.insert( fault_gun_blackpowder );
            }
            // end fouling mechanics
        }
    }
    // chance to damage gun due to high levels of dirt. Very unlikely, especially at lower levels and impossible below 5,000. Lower than the chance of a jam at the same levels. 555555... is an arbitrary number that I came up with after playing with the formula in excel. It makes sense at low, medium, and high levels of dirt.
    if( dirt_dbl > 5000 &&
        x_in_y( dirt_dbl * dirt_dbl * dirt_dbl,
                5555555555555 ) ) {
        add_msg_player_or_npc( m_bad, _( "Your %s is damaged by the high pressure!" ),
                               _( "<npcname>'s %s is damaged by the high pressure!" ),
                               it.tname() );
        // Don't increment until after the message
        it.inc_damage();
    }
    return true;
}

void npc::pretend_fire( npc *source, int shots, item &gun )
{
    int curshot = 0;
    if( g->u.sees( *source ) && one_in( 50 ) ) {
        add_msg( m_info, _( "%s shoots something." ), source->disp_name() );
    }
    while( curshot != shots ) {
        if( gun.ammo_consume( gun.ammo_required(), pos() ) != gun.ammo_required() ) {
            debugmsg( "Unexpected shortage of ammo whilst firing %s", gun.tname().c_str() );
            break;
        }

        item *weapon = &gun;
        const auto data = weapon->gun_noise( shots > 1 );

        if( g->u.sees( *source ) ) {
            add_msg( m_warning, _( "You hear %s." ), data.sound );
        }
        curshot++;
        moves -= 100;
    }
}

bool can_use_bipod( const map &m, const tripoint &pos )
{
    // usage of any attached bipod is dependent upon terrain
    if( m.has_flag_ter_or_furn( "MOUNTABLE", pos ) ) {
        return true;
    }

    if( const optional_vpart_position vp = m.veh_at( pos ) ) {
        return vp->vehicle().has_part( pos, "MOUNTABLE" );
    }

    return false;
}

dispersion_sources calculate_dispersion( const map &m, const player &p, const item &gun,
        int at_recoil, bool burst )
{
    bool bipod = can_use_bipod( m, p.pos() );

    int gun_recoil = gun.gun_recoil( bipod );
    int eff_recoil = at_recoil + ( burst ? ranged::burst_penalty( p, gun, gun_recoil ) : 0 );
    dispersion_sources dispersion( p.get_weapon_dispersion( gun ) );
    dispersion.add_range( eff_recoil );
    return dispersion;
}

int player::fire_gun( const tripoint &target, int shots )
{
    return fire_gun( target, shots, weapon );
}

int player::fire_gun( const tripoint &target, const int max_shots, item &gun )
{
    if( !gun.is_gun() ) {
        debugmsg( "%s tried to fire non-gun (%s).", name, gun.tname() );
        return 0;
    }
    bool is_mech_weapon = false;
    if( is_mounted() &&
        mounted_creature->has_flag( MF_RIDEABLE_MECH ) ) {
        is_mech_weapon = true;
    }

    int shots = max_shots;
    // Number of shots to fire is limited by the amount of remaining ammo
    if( gun.ammo_required() ) {
        shots = std::min( shots, static_cast<int>( gun.ammo_remaining() / gun.ammo_required() ) );
    }

    // cap our maximum burst size by the amount of UPS power left
    if( !gun.has_flag( flag_VEHICLE ) && gun.get_gun_ups_drain() > 0 ) {
        shots = std::min( shots, static_cast<int>( charges_of( itype_UPS ) / gun.get_gun_ups_drain() ) );
    }

    if( shots <= 0 ) {
        debugmsg( "Attempted to fire zero or negative shots using %s", gun.tname() );
    }

    cata::optional<shape_factory> shape;
    if( gun.ammo_current() && gun.ammo_current()->ammo ) {
        shape = gun.ammo_current()->ammo->shape;
    }

    // Shaped attacks don't allow aiming, so they don't suffer from lack of aim either
    int character_recoil = shape ? recoil_vehicle() : recoil_total();
    // Penalty is (intentionally) based off mode shots, not ammo-limited.
    dispersion_sources dispersion = calculate_dispersion( g->m, *this, gun, character_recoil,
                                    max_shots > 1 );

    bool aoe_attack = gun.gun_skill() == skill_launcher || shape;
    tripoint aim = target;
    int curshot = 0;
    int hits = 0; // total shots on target
    while( curshot != shots ) {
        if( gun.faults.count( fault_gun_chamber_spent ) && curshot == 0 ) {
            moves -= 50;
            gun.faults.erase( fault_gun_chamber_spent );
            add_msg_if_player( _( "You cycle your %s manually." ), gun.tname() );
        }

        if( !handle_gun_damage( gun ) ) {
            break;
        }

        // If this is a vehicle mounted turret, which vehicle is it mounted on?
        const vehicle *in_veh = has_effect( effect_on_roof ) ? veh_pointer_or_null( g->m.veh_at(
                                    pos() ) ) : nullptr;
        projectile projectile = make_gun_projectile( gun );

        // Damage reduction from insufficient strength, if using a STR_DRAW weapon.
        projectile.impact.mult_damage( ranged::str_draw_damage_modifier( gun, *this ) );

        if( has_trait( trait_NORANGEDCRIT ) ) {
            projectile.add_effect( ammo_effect_NO_CRIT );
        }
        if( !shape ) {
            auto shot = projectile_attack( projectile, pos(), aim, dispersion, this, in_veh );
            if( shot.missed_by <= .1 ) {
                // TODO: check head existence for headshot
                g->events().send<event_type::character_gets_headshot>( getID() );
            }

            if( shot.hit_critter ) {
                hits++;
            }
        } else {
            // 30 degree cap, like for projectiles
            double angle_offset_arcmin = std::min( dispersion.roll(), 1800.0 ) * ( one_in( 2 ) ? 1 : -1 );
            double angle_offset = units::to_radians( units::from_arcmin( angle_offset_arcmin ) );
            double dx = aim.x - pos().x;
            double dy = aim.y - pos().y;
            double new_angle = atan2( dy, dx ) + angle_offset;
            // Always using trig here, rotations in maximum metric are weird
            double length = trig_dist( pos(), aim );
            rl_vec3d vec_pos( pos() );
            rl_vec3d new_aim = vec_pos + rl_vec3d( length, 0, 0 ).rotated( new_angle );
            ranged::execute_shaped_attack( *shape->create( vec_pos, new_aim ), projectile, *this );
        }
        curshot++;

        make_gun_sound_effect( *this, shots > 1, &gun );
        sfx::generate_gun_sound( *this, gun );

        cycle_action( gun, pos() );

        if( has_trait( trait_PYROMANIA ) && !has_morale( MORALE_PYROMANIA_STARTFIRE ) ) {
            if( gun.has_flag( flag_PYROMANIAC_WEAPON ) ) {
                add_msg_if_player( m_good, _( "You feel a surge of euphoria as flames roar out of the %s!" ),
                                   gun.tname() );
                add_morale( MORALE_PYROMANIA_STARTFIRE, 15, 15, 8_hours, 6_hours );
                rem_morale( MORALE_PYROMANIA_NOFIRE );
            }
        }

        if( gun.ammo_consume( gun.ammo_required(), pos() ) != gun.ammo_required() ) {
            debugmsg( "Unexpected shortage of ammo whilst firing %s", gun.tname() );
            break;
        }

        if( !gun.has_flag( flag_VEHICLE ) ) {
            use_charges( itype_UPS, gun.get_gun_ups_drain() );
        }

        if( aoe_attack ) {
            continue; // skip retargeting for launchers
        }
    }

    if( gun.has_flag( flag_RELOAD_AND_SHOOT ) ) {
        // Reset aim for bows and other reload-and-shoot weapons.
        recoil = MAX_RECOIL;
    } else {
        // Now actually apply recoil for the future shots
        // But only for one shot, because bursts kinda suck
        int gun_recoil = gun.gun_recoil( can_use_bipod( g->m, pos() ) );

        // If user is currently able to fire a mounted gun freely, penalize recoil based on size class.
        if( gun.has_flag( flag_MOUNTED_GUN ) && !can_use_bipod( g->m, pos() ) ) {
            if( get_size() == MS_HUGE ) {
                gun_recoil = gun_recoil * 2;
            } else {
                gun_recoil = gun_recoil * 3;
            }
        }

        recoil += gun_recoil;
        if( is_mech_weapon ) {
            // mechs can handle recoil far better. they are built around their main gun.
            recoil = recoil / 2;
        }
        recoil = std::min( MAX_RECOIL, recoil );
    }

    // Use different amounts of time depending on the type of gun and our skill
    moves -= time_to_attack( *this, *gun.type );

    // Practice the base gun skill proportionally to number of hits, but always by one.
    practice( skill_gun, ( hits + 1 ) * 5 );
    // launchers train weapon skill for both hits and misses.
    int practice_units = aoe_attack ? curshot : hits;
    practice( gun.gun_skill(), ( practice_units + 1 ) * 5 );

    return curshot;
}

namespace ranged
{

int throw_cost( const player &c, const item &to_throw )
{
    // Very similar to player::attack_cost
    // TODO: Extract into a function?
    // Differences:
    // Dex is more (2x) important for throwing speed
    // At 10 skill, the cost is down to 0.75%, not 0.66%
    const int base_move_cost = to_throw.attack_cost() / 2;
    const int throw_skill = std::min( MAX_SKILL, c.get_skill_level( skill_throw ) );
    ///\EFFECT_THROW increases throwing speed
    const int skill_cost = static_cast<int>( ( base_move_cost * ( 20 - throw_skill ) / 20 ) );
    ///\EFFECT_DEX increases throwing speed
    const int dexbonus = c.get_dex();
    const int encumbrance_penalty = c.encumb( bp_torso ) +
                                    ( c.encumb( bp_hand_l ) + c.encumb( bp_hand_r ) ) / 2;
    const float stamina_ratio = static_cast<float>( c.get_stamina() ) / c.get_stamina_max();
    const float stamina_penalty = 1.0 + std::max( ( 0.25f - stamina_ratio ) * 4.0f, 0.0f );

    int move_cost = base_move_cost;
    // Stamina penalty only affects base/2 and encumbrance parts of the cost
    move_cost += encumbrance_penalty;
    move_cost *= stamina_penalty;
    move_cost += skill_cost;
    move_cost -= dexbonus;
    move_cost *= c.mutation_value( "attackcost_modifier" );

    return std::max( 25, move_cost );
}

float get_str_draw_penalty( const item &it, const Character &p )
{
    // We only care if weapon has STR_DRAW, and that the user is weaker than required strength.
    // Also avoid dividing by zero, and skip if we'd just get a result of 1 anyway.
    if( !it.has_flag( flag_STR_DRAW ) || p.get_str() >= it.get_min_str() || it.get_min_str() <= 1 ) {
        return 1.0f;
    }
    // We also don't want to actually reduce values to zero, even if user is debuffed to zero strength.
    float archer_str = std::max( 1, p.get_str() );
    return ( archer_str / it.get_min_str() );
}

float str_draw_damage_modifier( const item &it, const Character &p )
{
    if( !it.has_flag( flag_STR_DRAW ) || p.get_str() >= it.get_min_str() || it.get_min_str() <= 1 ) {
        return 1.0f;
    }
    if( ranged::get_str_draw_penalty( it, p ) < 0.75f ) {
        return 0.5f;
    } else if( ranged::get_str_draw_penalty( it, p ) < 1.0f ) {
        return 0.75f;
    } else {
        return 1.0f;
    }
}

float str_draw_dispersion_modifier( const item &it, const Character &p )
{
    if( !it.has_flag( flag_STR_DRAW ) || p.get_str() >= it.get_min_str() || it.get_min_str() <= 1 ) {
        return 1.0f;
    }
    if( ranged::get_str_draw_penalty( it, p ) < 0.75f ) {
        return 0.5f;
    } else {
        return 1.0f;
    }
}

float str_draw_range_modifier( const item &it, const Character &p )
{
    if( !it.has_flag( flag_STR_DRAW ) || p.get_str() >= it.get_min_str() || it.get_min_str() <= 1 ) {
        return 1.0f;
    }
    if( ranged::get_str_draw_penalty( it, p ) < 0.75f ) {
        return 0.5f;
    } else if( ranged::get_str_draw_penalty( it, p ) < 1.0f ) {
        return 0.75f;
    } else {
        return 1.0f;
    }
}

int throw_dispersion_per_dodge( const Character &c, bool add_encumbrance )
{
    // +200 per dodge point at 0 dexterity
    // +100 at 8, +80 at 12, +66.6 at 16, +57 at 20, +50 at 24
    // Each 10 encumbrance on either hand is like -1 dex (can bring penalty to +400 per dodge)
    // Maybe TODO: Only use one hand
    const int encumbrance = add_encumbrance ? c.encumb( bp_hand_l ) + c.encumb( bp_hand_r ) : 0;
    ///\EFFECT_DEX increases throwing accuracy against targets with good dodge stat
    float effective_dex = 2 + c.get_dex() / 4.0f - ( encumbrance ) / 40.0f;
    return static_cast<int>( 100.0f / std::max( 1.0f, effective_dex ) );
}

// Perfect situation gives us 1000 dispersion at lvl 0
// This goes down linearly to 200  dispersion at lvl 10
int throwing_dispersion( const Character &c, const item &to_throw, Creature *critter,
                         bool is_blind_throw )
{
    units::mass weight = to_throw.weight();
    units::volume volume = to_throw.volume();
    if( to_throw.count_by_charges() && to_throw.charges > 1 ) {
        weight /= to_throw.charges;
        volume /= to_throw.charges;
    }

    int throw_difficulty = 1000;
    // 1000 penalty for every liter after the first
    // TODO: Except javelin type items
    throw_difficulty += std::max<int>( 0, units::to_milliliter( volume - 1_liter ) );
    // 1 penalty for gram above str*100 grams (at 0 skill)
    ///\EFFECT_STR decreases throwing dispersion when throwing heavy objects
    const int weight_in_gram = units::to_gram( weight );
    throw_difficulty += std::max( 0, weight_in_gram - c.get_str() * 100 );

    // Dispersion from difficult throws goes from 100% at lvl 0 to 20% at lvl 10
    ///\EFFECT_THROW increases throwing accuracy
    const int throw_skill = std::min( MAX_SKILL, c.get_skill_level( skill_throw ) );
    int dispersion = 10 * throw_difficulty / ( 6 * throw_skill + 20 );
    // If the target is a creature, it moves around and ruins aim
    // TODO: Inform projectile functions if the attacker actually aims for the critter or just the tile
    if( critter != nullptr ) {
        // It's easier to dodge at close range (thrower needs to adjust more)
        // Dodge x10 at point blank, x5 at 1 dist, then flat
        float effective_dodge = critter->get_dodge() * std::max( 1, 10 - 5 * rl_dist( c.pos(),
                                critter->pos() ) );
        dispersion += throw_dispersion_per_dodge( c, true ) * effective_dodge;
    }
    // 1 perception per 1 eye encumbrance
    ///\EFFECT_PER decreases throwing accuracy penalty from eye encumbrance
    dispersion += std::max( 0, ( c.encumb( bp_eyes ) - c.get_per() ) * 10 );

    // If throwing blind, we're assuming they mechanically can't achieve the
    // accuracy of a normal throw.
    if( is_blind_throw ) {
        dispersion *= 4;
    }

    return std::max( 0, dispersion );
}

} // namespace ranged

dealt_projectile_attack player::throw_item( const tripoint &target, const item &to_throw,
        const cata::optional<tripoint> &blind_throw_from_pos )
{
    // Copy the item, we may alter it before throwing
    item thrown = to_throw;

    const int move_cost = ranged::throw_cost( *this, to_throw );
    mod_moves( -move_cost );

    const int throwing_skill = get_skill_level( skill_throw );
    units::volume volume = to_throw.volume();
    units::mass weight = to_throw.weight();

    // Previously calculated as 2_gram * std::max( 1, str_cur )
    // using 16_gram normalizes it to 8 str. Same effort expenditure
    // for being able to throw farther.
    const int weight_cost = weight / ( 16_gram );
    const int encumbrance_cost = roll_remainder( ( encumb( bp_arm_l ) + encumb( bp_arm_r ) ) * 2.0f );
    const int stamina_cost = ( weight_cost + encumbrance_cost - throwing_skill + 50 ) * -1;

    bool throw_assist = false;
    int throw_assist_str = 0;
    if( is_mounted() ) {
        auto mons = mounted_creature.get();
        if( mons->mech_str_addition() != 0 ) {
            throw_assist = true;
            throw_assist_str = mons->mech_str_addition();
            mons->use_mech_power( -3 );
        }
    }
    if( !throw_assist ) {
        mod_stamina( stamina_cost );
    }

    const skill_id &skill_used = skill_throw;
    int skill_level = std::min( MAX_SKILL, get_skill_level( skill_throw ) );
    // if you are lying on the floor, you can't really throw that well
    if( has_effect( effect_downed ) ) {
        skill_level = std::max( 0, skill_level - 5 );
    }
    // We'll be constructing a projectile
    projectile proj;
    proj.impact = thrown.base_damage_thrown();
    proj.speed = 10 + skill_level;
    auto &impact = proj.impact;

    static const std::set<material_id> ferric = { material_id( "iron" ), material_id( "steel" ) };

    bool do_railgun = has_active_bionic( bio_railgun ) && thrown.made_of_any( ferric ) &&
                      !throw_assist;

    // The damage dealt due to item's weight, player's strength, and skill level
    // Up to str/2 or weight/100g (lower), so 10 str is 5 damage before multipliers
    // Railgun doubles the effective strength
    ///\EFFECT_STR increases throwing damage
    double stats_mod = do_railgun ? get_str() : ( get_str() / 2.0 );
    stats_mod = throw_assist ? throw_assist_str / 2.0 : stats_mod;
    // modify strength impact based on skill level, clamped to [0.15 - 1]
    // mod = mod * [ ( ( skill / max_skill ) * 0.85 ) + 0.15 ]
    stats_mod *= ( std::min( MAX_SKILL,
                             get_skill_level( skill_throw ) ) /
                   static_cast<double>( MAX_SKILL ) ) * 0.85 + 0.15;
    impact.add_damage( DT_BASH, std::min( weight / 100.0_gram, stats_mod ) );

    if( thrown.has_flag( "ACT_ON_RANGED_HIT" ) ) {
        proj.add_effect( ammo_effect_ACT_ON_RANGED_HIT );
        thrown.active = true;
    }

    // Item will shatter upon landing, destroying the item, dealing damage, and making noise
    /** @EFFECT_STR increases chance of shattering thrown glass items (NEGATIVE) */
    const bool shatter = !thrown.active && thrown.made_of( material_id( "glass" ) ) &&
                         rng( 0, units::to_milliliter( 2_liter - volume ) ) < get_str() * 100;

    // Item will burst upon landing, destroying the item, and spilling its contents
    const bool burst = thrown.has_property( "burst_when_filled" ) && thrown.is_container() &&
                       thrown.get_property_int64_t( "burst_when_filled" ) <= static_cast<double>
                       ( thrown.get_contained().volume().value() ) / thrown.get_container_capacity().value() * 100;

    // Add some flags to the projectile
    if( weight > 500_gram ) {
        proj.add_effect( ammo_effect_HEAVY_HIT );
    }

    proj.add_effect( ammo_effect_NO_ITEM_DAMAGE );

    if( thrown.active ) {
        // Can't have Molotovs embed into monsters
        // Monsters don't have inventory processing
        proj.add_effect( ammo_effect_NO_EMBED );
    }

    if( do_railgun ) {
        proj.add_effect( ammo_effect_LIGHTNING );
    }

    if( volume > 500_ml ) {
        proj.add_effect( ammo_effect_WIDE );
    }

    // Deal extra cut damage if the item breaks
    if( shatter ) {
        impact.add_damage( DT_CUT, units::to_milliliter( volume ) / 500.0f );
        proj.add_effect( ammo_effect_SHATTER_SELF );
    }

    // TODO: Add wet effect if other things care about that
    if( burst ) {
        proj.add_effect( ammo_effect_BURST );
    }

    // Some minor (skill/2) armor piercing for skillful throws
    // Not as much as in melee, though
    for( damage_unit &du : impact.damage_units ) {
        du.res_pen += skill_level / 2.0f;
    }
    // handling for tangling thrown items
    if( thrown.has_flag( "TANGLE" ) ) {
        proj.add_effect( ammo_effect_TANGLE );
    }

    Creature *critter = g->critter_at( target, true );
    const dispersion_sources dispersion( ranged::throwing_dispersion( *this, thrown, critter,
                                         blind_throw_from_pos.has_value() ) );
    const itype *thrown_type = thrown.type;

    // Put the item into the projectile
    proj.set_drop( std::move( thrown ) );
    if( thrown_type->has_flag( "CUSTOM_EXPLOSION" ) ) {
        proj.set_custom_explosion( thrown_type->explosion );
    }

    // Throw from the player's position, unless we're blind throwing, in which case
    // throw from the the blind throw position instead.
    const tripoint throw_from = blind_throw_from_pos ? *blind_throw_from_pos : pos();

    float range = rl_dist( throw_from, target );
    proj.range = range;
    int skill_lvl = get_skill_level( skill_used );
    // Avoid awarding tons of xp for lucky throws against hard to hit targets
    const float range_factor = std::min<float>( range, skill_lvl + 3 );
    // We're aiming to get a damaging hit, not just an accurate one - reward proper weapons
    const float damage_factor = 5.0f * std::sqrt( proj.impact.total_damage() / 5.0f );
    // This should generally have values below ~20*sqrt(skill_lvl)
    const float final_xp_mult = range_factor * damage_factor;

    auto dealt_attack = projectile_attack( proj, throw_from, target, dispersion, this );

    const double missed_by = dealt_attack.missed_by;
    if( missed_by <= 0.1 && dealt_attack.hit_critter != nullptr ) {
        practice( skill_used, final_xp_mult, MAX_SKILL );
        // TODO: Check target for existence of head
        g->events().send<event_type::character_gets_headshot>( getID() );
    } else if( dealt_attack.hit_critter != nullptr && missed_by > 0.0f ) {
        practice( skill_used, final_xp_mult / ( 1.0f + missed_by ), MAX_SKILL );
    } else {
        // Pure grindy practice - cap gain at lvl 2
        practice( skill_used, 5, 2 );
    }
    // Reset last target pos
    last_target_pos = cata::nullopt;
    recoil = MAX_RECOIL;

    return dealt_attack;
}

static void do_aim( player &p, const item &relevant, const double min_recoil )
{
    const double aim_amount = p.aim_per_move( relevant, p.recoil );
    if( aim_amount > 0 && p.recoil > min_recoil ) {
        // Increase aim at the cost of moves
        p.mod_moves( -1 );
        p.recoil = std::max( min_recoil, p.recoil - aim_amount );
    } else {
        // If aim is already maxed, we're just waiting, so pass the turn.
        p.set_moves( 0 );
    }
}

struct confidence_rating {
    double aim_level;
    char symbol;
    std::string color;
    std::string label;
};

static int print_steadiness( const catacurses::window &w, int line_number, double steadiness )
{
    const int window_width = getmaxx( w ) - 2; // Window width minus borders.

    if( get_option<std::string>( "ACCURACY_DISPLAY" ) == "numbers" ) {
        std::string steadiness_s = string_format( "%s: %d%%", _( "Steadiness" ),
                                   static_cast<int>( 100.0 * steadiness ) );
        mvwprintw( w, point( 1, line_number++ ), steadiness_s );
    } else {
        const std::string &steadiness_bar = get_labeled_bar( steadiness, window_width,
                                            _( "Steadiness" ), '*' );
        mvwprintw( w, point( 1, line_number++ ), steadiness_bar );
    }

    return line_number;
}

static double confidence_estimate( int range, double target_size,
                                   const dispersion_sources &dispersion )
{
    // This is a rough estimate of accuracy based on a linear distribution across min and max
    // dispersion.  It is highly inaccurate probability-wise.  The result gives the player
    // correct relative measures of chance to hit, and corresponds with the actual distribution at
    // min, max, and mean.
    if( range == 0 ) {
        return 2 * target_size;
    }
    const double max_lateral_offset =
        iso_tangent( range, units::from_arcmin( dispersion.max() ) );
    return 1 / ( max_lateral_offset / target_size );
}

static std::vector<aim_type> get_default_aim_type()
{
    std::vector<aim_type> aim_types;
    aim_types.push_back( aim_type { "", "", "", false, 0 } ); // dummy aim type for unaimed shots
    return aim_types;
}

using RatingVector = std::vector<std::tuple<double, char, std::string>>;
static std::string get_colored_bar( const double val, const int width, const std::string &label,
                                    RatingVector::iterator begin, RatingVector::iterator end )
{
    std::string result;

    result.reserve( width );
    if( !label.empty() ) {
        result += label;
        result += ' ';
    }
    const int bar_width = width - utf8_width( result ) - 2; // - 2 for the brackets

    result += '[';
    if( bar_width > 0 ) {
        int used_width = 0;
        for( auto it( begin ); it != end; ++it ) {
            const double factor = std::min( 1.0, std::max( 0.0, std::get<0>( *it ) * val ) );
            const int seg_width = static_cast<int>( factor * bar_width ) - used_width;

            if( seg_width <= 0 ) {
                continue;
            }
            used_width += seg_width;
            result += string_format( "<color_%s>", std::get<2>( *it ) );
            result.insert( result.end(), seg_width, std::get<1>( *it ) );
            result += "</color>";
        }
        result.insert( result.end(), bar_width - used_width, ' ' );
    }
    result += ']';

    return result;
}

static int print_ranged_chance( const player &, const catacurses::window &w, int line_number,
                                input_context &ctxt, const item &,
                                const std::vector<aim_type> &aim_types,
                                const std::function<dispersion_sources( const aim_type & )> &dispersion_fun,
                                const std::function<int( const aim_type & )> &cost_fun,
                                const std::vector<confidence_rating> &confidence_config,
                                double range, double target_size )
{
    int window_width = getmaxx( w ) - 2; // Window width minus borders.
    std::string display_type = get_option<std::string>( "ACCURACY_DISPLAY" );
    std::string panel_type = panel_manager::get_manager().get_current_layout_id();
    const int bars_pad = 3; // Padding for "bars" to fit moves_to_fire value.
    if( ( panel_type == "compact" || panel_type == "labels-narrow" ) && display_type != "numbers" ) {
        window_width -= bars_pad;
    }

    std::string label_m = _( "Moves" );
    std::vector<std::string> t_aims( 4 ), t_confidence( 16 );
    int aim_iter = 0, conf_iter = 0;

    nc_color col = c_dark_gray;

    if( display_type != "numbers" ) {
        std::string symbols;
        int column_number = 1;
        if( !( panel_type == "compact" || panel_type == "labels-narrow" ) ) {
            std::string label = _( "Symbols:" );
            mvwprintw( w, point( column_number, line_number ), label );
            column_number += utf8_width( label ) + 1; // 1 for whitespace after 'Symbols:'
        }
        for( const confidence_rating &cr : confidence_config ) {
            std::string label = pgettext( "aim_confidence", cr.label.c_str() );
            std::string symbols = string_format( "<color_%s>%s</color> = %s", cr.color, cr.symbol,
                                                 label );
            int line_len = utf8_width( label ) + 5; // 5 for '# = ' and whitespace at end
            if( ( window_width + bars_pad - column_number ) < line_len ) {
                column_number = 1;
                line_number++;
            }
            print_colored_text( w, point( column_number, line_number ), col, col, symbols );
            column_number += line_len;
        }
        line_number++;
    }
    if( ( panel_type == "compact" || panel_type == "labels-narrow" ) && display_type == "numbers" ) {
        std::string symbols = _( " <color_green>Great</color> - <color_light_gray>Normal</color>"
                                 " - <color_magenta>Graze</color> - <color_light_blue>Moves</color>" );
        fold_and_print( w, point( 1, line_number++ ), window_width + bars_pad,
                        c_dark_gray, symbols );
        int len = utf8_width( symbols ) - 96; // 96 to subtract color codes
        if( len > window_width + bars_pad ) {
            line_number++;
        }
        for( int i = 0; i < window_width; i++ ) {
            mvwprintw( w, point( i + 1, line_number ), "-" );
        }
    }

    const auto front_or = [&]( const std::string & s, const char fallback ) {
        const auto keys = ctxt.keys_bound_to( s );
        return keys.empty() ? fallback : keys.front();
    };

    for( const aim_type &type : aim_types ) {
        dispersion_sources current_dispersion = dispersion_fun( type );
        std::string label = _( "Current" );
        std::string aim_l = _( "Aim" );
        if( type.has_threshold ) {
            label = type.name;
        }

        int moves_to_fire = cost_fun( type );

        auto hotkey = front_or( type.action.empty() ? "FIRE" : type.action, ' ' );
        if( ( panel_type == "compact" || panel_type == "labels-narrow" ) ) {
            if( display_type == "numbers" ) {
                t_aims[aim_iter] = string_format( "<color_dark_gray>%s:</color>", label );
                t_confidence[( aim_iter * 4 ) + 3] = string_format( "<color_light_blue>%d</color>", moves_to_fire );
            } else {
                print_colored_text( w, point( 1, line_number ), col, col, string_format( _( "%s %s:" ), label,
                                    aim_l ) );
                right_print( w, line_number++, 1, c_light_blue, _( "Moves" ) );
                right_print( w, line_number, 1, c_light_blue, string_format( "%d", moves_to_fire ) );
            }
        } else {
            print_colored_text( w, point( 1, line_number++ ), col, col,
                                string_format( _( "<color_white>[%s]</color> %s %s: Moves to fire: "
                                                  "<color_light_blue>%d</color>" ),
                                               hotkey, label, aim_l, moves_to_fire ) );
        }

        double confidence = confidence_estimate( range, target_size, current_dispersion );

        if( display_type == "numbers" ) {
            if( panel_type == "compact" || panel_type == "labels-narrow" ) {
                int last_chance = 0;
                for( const confidence_rating &cr : confidence_config ) {
                    int chance = std::min<int>( 100, 100.0 * ( cr.aim_level ) * confidence ) - last_chance;
                    last_chance += chance;
                    t_confidence[conf_iter] = string_format( "<color_%s>%3d%%</color>", cr.color, chance );
                    conf_iter++;
                    if( conf_iter == ( aim_iter * 4 ) + 3 ) {
                        conf_iter++;
                    }
                }
                aim_iter++;
            } else {
                int last_chance = 0;
                std::string confidence_s = enumerate_as_string( confidence_config.begin(), confidence_config.end(),
                [&]( const confidence_rating & config ) {
                    // TODO: Consider not printing 0 chances, but only if you can print something (at least miss 100% or so)
                    int chance = std::min<int>( 100, 100.0 * ( config.aim_level * confidence ) ) - last_chance;
                    last_chance += chance;
                    return string_format( "%s: <color_%s>%3d%%</color>", pgettext( "aim_confidence",
                                          config.label.c_str() ), config.color, chance );
                }, enumeration_conjunction::none );
                line_number += fold_and_print_from( w, point( 1, line_number ), window_width, 0,
                                                    c_dark_gray, confidence_s );
            }
        } else {
            std::vector<std::tuple<double, char, std::string>> confidence_ratings;
            std::transform( confidence_config.begin(), confidence_config.end(),
                            std::back_inserter( confidence_ratings ),
            [&]( const confidence_rating & config ) {
                return std::make_tuple( config.aim_level, config.symbol, config.color );
            }
                          );
            const std::string &confidence_bar = get_colored_bar( confidence, window_width, "",
                                                confidence_ratings.begin(),
                                                confidence_ratings.end() );

            print_colored_text( w, point( 1, line_number++ ), col, col, confidence_bar );
        }
    }

    // Draw tables for compact Numbers display
    if( ( panel_type == "compact" || panel_type == "labels-narrow" )
        && display_type == "numbers" ) {
        const std::string divider = "|";
        int left_pad = 10, columns = 4;
        insert_table( w, left_pad, ++line_number, columns, c_light_gray, divider, true, t_confidence );
        insert_table( w, 0, line_number, 1, c_light_gray, "", false, t_aims );
        line_number = line_number + 4; // 4 to account for the tables
    }
    return line_number;
}

// Whether player character knows creature's position and can roughly track it with the aim cursor
static bool pl_sees( const Creature &cr )
{
    Character &u = get_player_character();
    return u.sees( cr ) || u.sees_with_infrared( cr ) || u.sees_with_specials( cr );
}

// Handle capping aim level when the player cannot see the target tile or there is nothing to aim at.
static double calculate_aim_cap( const player &p, const tripoint &target )
{
    double min_recoil = 0.0;
    const Creature *victim = g->critter_at( target, true );
    // No p.sees_with_specials() here because special senses are not precise enough
    // to give creature's exact size & position, only which tile it occupies
    if( victim == nullptr || ( !p.sees( *victim ) && !p.sees_with_infrared( *victim ) ) ) {
        const int range = rl_dist( p.pos(), target );
        // Get angle of triangle that spans the target square.
        const double angle = atan2( 1, range );
        // Convert from radians to arcmin.
        min_recoil = 60 * 180 * angle / M_PI;
    }
    return min_recoil;
}

static int print_aim( const player &p, const catacurses::window &w, int line_number,
                      input_context &ctxt, const item &weapon,
                      const double target_size, const tripoint &pos, double predicted_recoil )
{
    // This is absolute accuracy for the player.
    // TODO: push the calculations duplicated from Creature::deal_projectile_attack() and
    // Creature::projectile_attack() into shared methods.
    // Dodge doesn't affect gun attacks

    dispersion_sources dispersion = p.get_weapon_dispersion( weapon );
    dispersion.add_range( p.recoil_vehicle() );

    const double min_recoil = calculate_aim_cap( p, pos );
    const double effective_recoil = p.effective_dispersion( p.weapon.sight_dispersion() );
    const double min_dispersion = std::max( min_recoil, effective_recoil );
    const double steadiness_range = MAX_RECOIL - min_dispersion;
    // This is a relative measure of how steady the player's aim is,
    // 0 is the best the player can do.
    const double steady_score = std::max( 0.0, predicted_recoil - min_dispersion );
    // Fairly arbitrary cap on steadiness...
    const double steadiness = 1.0 - ( steady_score / steadiness_range );

    // This could be extracted, to allow more/less verbose displays
    static const std::vector<confidence_rating> confidence_config = {{
            { accuracy_critical, '*', "green", translate_marker_context( "aim_confidence", "Great" ) },
            { accuracy_standard, '+', "light_gray", translate_marker_context( "aim_confidence", "Normal" ) },
            { accuracy_grazing, '|', "magenta", translate_marker_context( "aim_confidence", "Graze" ) }
        }
    };

    int shots = std::max( 1, weapon.gun_current_mode().qty );
    const auto dispersion_fun = [&]( const aim_type & at ) {
        int at_recoil = at.has_threshold ? at.threshold : static_cast<int>( predicted_recoil );
        return calculate_dispersion( g->m, p, weapon, at_recoil, shots > 1 );
    };
    const auto cost_fun = [&]( const aim_type & at ) {
        int at_recoil = at.has_threshold ? at.threshold : static_cast<int>( predicted_recoil );
        return p.gun_engagement_moves( weapon, at_recoil, p.recoil ) + time_to_attack( p,
                *weapon.type );
    };
    const double range = rl_dist( p.pos(), pos );
    line_number = print_steadiness( w, line_number, steadiness );
    return print_ranged_chance( p, w, line_number, ctxt, weapon, p.get_aim_types( weapon ),
                                dispersion_fun, cost_fun,
                                confidence_config,
                                range, target_size );
}

static int draw_throw_aim( const player &p, const catacurses::window &w, int line_number,
                           input_context &ctxt,
                           const item &weapon, const tripoint &target_pos, bool is_blind_throw )
{
    Creature *target = g->critter_at( target_pos, true );
    if( target != nullptr && !p.sees( *target ) ) {
        target = nullptr;
    }

    const dispersion_sources dispersion(
        ranged::throwing_dispersion( p, weapon, target, is_blind_throw ) );
    const double range = rl_dist( p.pos(), target_pos );

    const double target_size = target != nullptr ? target->ranged_target_size() : 1.0f;

    static const std::vector<confidence_rating> confidence_config_critter = {{
            { accuracy_critical, '*', "green", translate_marker_context( "aim_confidence", "Great" ) },
            { accuracy_standard, '+', "light_gray", translate_marker_context( "aim_confidence", "Normal" ) },
            { accuracy_grazing, '|', "magenta", translate_marker_context( "aim_confidence", "Graze" ) }
        }
    };
    static const std::vector<confidence_rating> confidence_config_object = {{
            { accuracy_grazing, '*', "white", translate_marker_context( "aim_confidence", "Hit" ) }
        }
    };
    const auto &confidence_config = target != nullptr ?
                                    confidence_config_critter : confidence_config_object;

    const auto dispersion_fun = [&]( const aim_type & ) {
        return dispersion;
    };
    const auto cost_fun = [&]( const aim_type & ) {
        return ranged::throw_cost( p, weapon );
    };
    return print_ranged_chance( p, w, line_number, ctxt,  weapon, get_default_aim_type(),
                                dispersion_fun, cost_fun,
                                confidence_config,
                                range, target_size );
}

std::vector<aim_type> Character::get_aim_types( const item &gun ) const
{
    std::vector<aim_type> aim_types = get_default_aim_type();
    if( !gun.is_gun() ) {
        return aim_types;
    }
    int sight_dispersion = effective_dispersion( gun.sight_dispersion() );
    // Aiming thresholds are dependent on weapon sight dispersion, attempting to place thresholds
    // at 10%, 5% and 0% of the difference between MAX_RECOIL and sight dispersion.
    std::vector<int> thresholds = {
        static_cast<int>( ( ( MAX_RECOIL - sight_dispersion ) / 10.0 ) + sight_dispersion ),
        static_cast<int>( ( ( MAX_RECOIL - sight_dispersion ) / 20.0 ) + sight_dispersion ),
        static_cast<int>( sight_dispersion )
    };
    // Remove duplicate thresholds.
    std::vector<int>::iterator thresholds_it = std::adjacent_find( thresholds.begin(),
            thresholds.end() );
    while( thresholds_it != thresholds.end() ) {
        thresholds.erase( thresholds_it );
        thresholds_it = std::adjacent_find( thresholds.begin(), thresholds.end() );
    }
    thresholds_it = thresholds.begin();
    aim_types.push_back( aim_type { _( "Regular" ), "AIMED_SHOT", _( "[%c] to aim and fire." ),
                                    true, *thresholds_it } );
    thresholds_it++;
    if( thresholds_it != thresholds.end() ) {
        aim_types.push_back( aim_type { _( "Careful" ), "CAREFUL_SHOT",
                                        _( "[%c] to take careful aim and fire." ), true,
                                        *thresholds_it } );
        thresholds_it++;
    }
    if( thresholds_it != thresholds.end() ) {
        aim_types.push_back( aim_type { _( "Precise" ), "PRECISE_SHOT",
                                        _( "[%c] to take precise aim and fire." ), true,
                                        *thresholds_it } );
    }
    return aim_types;
}

static projectile make_gun_projectile( const item &gun )
{
    projectile proj;
    proj.speed  = 1000;
    proj.impact = gun.gun_damage();
    proj.range = gun.gun_range();
    for( const ammo_effect_str_id &ae_id : gun.ammo_effects() ) {
        proj.add_effect( ae_id );
    }

    auto &fx = proj;

    if( ( gun.ammo_data() && gun.ammo_data()->phase == LIQUID ) ||
        fx.has_effect( ammo_effect_SHOT ) || fx.has_effect( ammo_effect_BOUNCE ) ) {
        fx.add_effect( ammo_effect_WIDE );
    }

    if( gun.ammo_data() ) {
        assert( gun.ammo_data()->ammo );
        const islot_ammo &ammo = *gun.ammo_data()->ammo;
        // Some projectiles have a chance of being recoverable
        bool recover = !one_in( ammo.dont_recover_one_in );

        if( recover && !fx.has_effect( ammo_effect_IGNITE ) && !fx.has_effect( ammo_effect_EXPLOSIVE ) ) {
            item drop( gun.ammo_current(), calendar::turn, 1 );
            drop.active = fx.has_effect( ammo_effect_ACT_ON_RANGED_HIT );
            proj.set_drop( drop );
        }

        if( ammo.drop ) {
            item drop( ammo.drop );
            if( ammo.drop_active ) {
                drop.activate();
            }
            proj.set_drop( drop );
        }

        if( fx.has_effect( ammo_effect_CUSTOM_EXPLOSION ) ) {
            proj.set_custom_explosion( gun.ammo_data()->explosion );
        }
    }

    return proj;
}

int time_to_attack( const Character &p, const itype &firing )
{
    const skill_id &skill_used = firing.gun->skill_used;
    const time_info_t &info = skill_used->time_to_attack();
    return std::max( info.min_time,
                     info.base_time - info.time_reduction_per_level * p.get_skill_level( skill_used ) );
}

static void cycle_action( item &weap, const tripoint &pos )
{
    // eject casings and linkages in random direction avoiding walls using player position as fallback
    std::vector<tripoint> tiles = closest_points_first( pos, 1 );
    tiles.erase( tiles.begin() );
    tiles.erase( std::remove_if( tiles.begin(), tiles.end(), [&]( const tripoint & e ) {
        return !g->m.passable( e );
    } ), tiles.end() );
    tripoint eject = tiles.empty() ? pos : random_entry( tiles );

    // for turrets try and drop casings or linkages directly to any CARGO part on the same tile
    const optional_vpart_position vp = g->m.veh_at( pos );
    std::vector<vehicle_part *> cargo;
    if( vp && weap.has_flag( "VEHICLE" ) ) {
        cargo = vp->vehicle().get_parts_at( pos, "CARGO", part_status_flag::any );
    }

    if( weap.ammo_data() && weap.ammo_data()->ammo->casing ) {
        const itype_id casing = *weap.ammo_data()->ammo->casing;
        if( weap.has_flag( "RELOAD_EJECT" ) || weap.gunmod_find( itype_brass_catcher ) ) {
            weap.put_in( item( casing ).set_flag( "CASING" ) );
        } else {
            if( cargo.empty() ) {
                g->m.add_item_or_charges( eject, item( casing ) );
            } else {
                vp->vehicle().add_item( *cargo.front(), item( casing ) );
            }

            sfx::play_variant_sound( "fire_gun", "brass_eject", sfx::get_heard_volume( eject ),
                                     sfx::get_heard_angle( eject ) );
        }
    }

    // some magazines also eject disintegrating linkages
    const auto mag = weap.magazine_current();
    if( mag && mag->type->magazine->linkage ) {
        item linkage( *mag->type->magazine->linkage, calendar::turn, 1 );
        if( weap.gunmod_find( itype_brass_catcher ) ) {
            linkage.set_flag( "CASING" );
            weap.put_in( linkage );
        } else if( cargo.empty() ) {
            g->m.add_item_or_charges( eject, linkage );
        } else {
            vp->vehicle().add_item( *cargo.front(), linkage );
        }
    }
}

void make_gun_sound_effect( const player &p, bool burst, item *weapon )
{
    const auto data = weapon->gun_noise( burst );
    if( data.volume > 0 ) {
        sounds::sound( p.pos(), data.volume, sounds::sound_t::combat,
                       data.sound.empty() ? _( "Bang!" ) : data.sound );
    }
}

item::sound_data item::gun_noise( const bool burst ) const
{
    if( !is_gun() ) {
        return { 0, "" };
    }

    int noise = type->gun->loudness;
    for( const auto mod : gunmods() ) {
        noise += mod->type->gunmod->loudness;
    }
    if( ammo_data() ) {
        noise += ammo_data()->ammo->loudness;
    }

    noise = std::max( noise, 0 );

    if( ammo_current() == itype_40x46mm || ammo_current() == itype_40x53mm ) {
        // Grenade launchers
        return { 8, _( "Thunk!" ) };

    } else if( ammo_current() == itype_12mm || ammo_current() == itype_metal_rail ) {
        // Railguns
        return { 24, _( "tz-CRACKck!" ) };

    } else if( ammo_current() == itype_flammable || ammo_current() == itype_66mm ||
               ammo_current() == itype_84x246mm || ammo_current() == itype_m235 ) {
        // Rocket launchers and flamethrowers
        return { 4, _( "Fwoosh!" ) };
    } else if( ammo_current() == itype_arrow ) {
        return { noise, _( "whizz!" ) };
    } else if( ammo_current() == itype_bolt ) {
        return { noise, _( "thonk!" ) };
    }

    auto fx = ammo_effects();

    if( fx.count( ammo_effect_LASER ) || fx.count( ammo_effect_PLASMA ) ) {
        if( noise < 20 ) {
            return { noise, _( "Fzzt!" ) };
        } else if( noise < 40 ) {
            return { noise, _( "Pew!" ) };
        } else if( noise < 60 ) {
            return { noise, _( "Tsewww!" ) };
        } else {
            return { noise, _( "Kra-kow!" ) };
        }

    } else if( fx.count( ammo_effect_LIGHTNING ) ) {
        if( noise < 20 ) {
            return { noise, _( "Bzzt!" ) };
        } else if( noise < 40 ) {
            return { noise, _( "Bzap!" ) };
        } else if( noise < 60 ) {
            return { noise, _( "Bzaapp!" ) };
        } else {
            return { noise, _( "Kra-koom!" ) };
        }

    } else if( fx.count( ammo_effect_WHIP ) ) {
        return { noise, _( "Crack!" ) };

    } else if( noise > 0 ) {
        if( noise < 10 ) {
            return { noise, burst ? _( "Brrrip!" ) : _( "plink!" ) };
        } else if( noise < 150 ) {
            return { noise, burst ? _( "Brrrap!" ) : _( "bang!" ) };
        } else if( noise < 175 ) {
            return { noise, burst ? _( "P-p-p-pow!" ) : _( "blam!" ) };
        } else {
            return { noise, burst ? _( "Kaboom!" ) : _( "kerblam!" ) };
        }
    }

    return { 0, "" }; // silent weapons
}

static bool is_driving( const player &p )
{
    const optional_vpart_position vp = g->m.veh_at( p.pos() );
    return vp && vp->vehicle().is_moving() && vp->vehicle().player_in_control( p );
}

static double dispersion_from_skill( double skill, double weapon_dispersion )
{
    if( skill >= MAX_SKILL ) {
        return 0.0;
    }
    double skill_shortfall = double( MAX_SKILL ) - skill;
    double dispersion_penalty = 3 * skill_shortfall;
    double skill_threshold = 5;
    if( skill >= skill_threshold ) {
        double post_threshold_skill_shortfall = double( MAX_SKILL ) - skill;
        // Lack of mastery multiplies the dispersion of the weapon.
        return dispersion_penalty + ( weapon_dispersion * post_threshold_skill_shortfall * 1.25 ) /
               ( double( MAX_SKILL ) - skill_threshold );
    }
    // Unskilled shooters suffer greater penalties, still scaling with weapon penalties.
    double pre_threshold_skill_shortfall = skill_threshold - skill;
    dispersion_penalty += weapon_dispersion *
                          ( 1.25 + pre_threshold_skill_shortfall * 3.75 / skill_threshold );

    return dispersion_penalty;
}

// utility functions for projectile_attack
dispersion_sources player::get_weapon_dispersion( const item &obj ) const
{
    int weapon_dispersion = obj.gun_dispersion();
    dispersion_sources dispersion( weapon_dispersion );
    dispersion.add_range( ranged_dex_mod() );

    dispersion.add_range( ( encumb( bp_arm_l ) + encumb( bp_arm_r ) ) / 5.0 );

    if( is_driving( *this ) ) {
        // get volume of gun (or for auxiliary gunmods the parent gun)
        const item *parent = has_item( obj ) ? find_parent( obj ) : nullptr;
        const int vol = ( parent ? parent->volume() : obj.volume() ) / 250_ml;

        /** @EFFECT_DRIVING reduces the inaccuracy penalty when using guns whilst driving */
        dispersion.add_range( std::max( vol - get_skill_level( skill_driving ), 1 ) * 20 );
    }

    /** @EFFECT_GUN improves usage of accurate weapons and sights */
    double avgSkill = static_cast<double>( get_skill_level( skill_gun ) +
                                           get_skill_level( obj.gun_skill() ) ) / 2.0;
    avgSkill = std::min( avgSkill, static_cast<double>( MAX_SKILL ) );

    dispersion.add_range( dispersion_from_skill( avgSkill, weapon_dispersion ) );

    if( has_bionic( bio_targeting ) ) {
        dispersion.add_multiplier( 0.75 );
    }

    // If using a bow you lack the strength for, increase based on how much weaker shooter is.
    dispersion.add_multiplier( 1 / ranged::str_draw_dispersion_modifier( obj, *this ) );

    // Range is effectively four times longer when shooting unflagged/flagged guns underwater/out of water.
    if( is_underwater() != obj.has_flag( flag_UNDERWATER_GUN ) ) {
        // Adding dispersion for additional debuff
        dispersion.add_range( 150 );
        dispersion.add_multiplier( 4 );
    }

    // If user is currently able to fire a mounted gun freely, penalize dispersion based on size class.
    if( obj.has_flag( flag_MOUNTED_GUN ) && !can_use_bipod( g->m, pos() ) ) {
        if( get_size() == MS_HUGE ) {
            dispersion.add_multiplier( 2 );
        } else {
            dispersion.add_multiplier( 3 );
        }
    }

    return dispersion;
}

double player::gun_value( const item &weap, int ammo ) const
{
    // TODO: Mods
    // TODO: Allow using a specified type of ammo rather than default or current
    if( !weap.type->gun ) {
        return 0.0;
    }

    if( ammo <= 0 ) {
        return 0.0;
    }

    const islot_gun &gun = *weap.type->gun;
    itype_id ammo_type;
    if( !weap.ammo_current().is_null() ) {
        ammo_type = weap.ammo_current();
    } else if( weap.magazine_current() ) {
        ammo_type = weap.common_ammo_default();
    } else {
        ammo_type = weap.ammo_default();
    }
    const itype *def_ammo_i = ammo_type.is_null() ? nullptr : &*ammo_type;

    damage_instance gun_damage = weap.gun_damage();
    item tmp = weap;
    tmp.ammo_set( ammo_type );
    int total_dispersion = get_weapon_dispersion( tmp ).max() +
                           effective_dispersion( tmp.sight_dispersion() );

    if( def_ammo_i != nullptr && def_ammo_i->ammo ) {
        const islot_ammo &def_ammo = *def_ammo_i->ammo;
        gun_damage.add( def_ammo.damage );
        total_dispersion += def_ammo.dispersion;
    }

    float damage_factor = gun_damage.total_damage();
    if( damage_factor > 0 ) {
        // TODO: Multiple damage types
        damage_factor += 0.5f * gun_damage.damage_units.front().res_pen;
    }

    int move_cost = time_to_attack( *this, *weap.type );
    if( gun.clip != 0 && gun.clip < 10 ) {
        // TODO: RELOAD_ONE should get a penalty here
        int reload_cost = gun.reload_time + encumb( bp_hand_l ) + encumb( bp_hand_r );
        reload_cost /= gun.clip;
        move_cost += reload_cost;
    }

    // "Medium range" below means 9 tiles, "short range" means 4
    // Those are guarantees (assuming maximum time spent aiming)
    static const std::vector<std::pair<float, float>> dispersion_thresholds = {{
            // Headshots all the time
            { 0.0f, 5.0f },
            // Critical at medium range
            { 100.0f, 4.5f },
            // Critical at short range or good hit at medium
            { 200.0f, 3.5f },
            // OK hits at medium
            { 300.0f, 3.0f },
            // Point blank headshots
            { 450.0f, 2.5f },
            // OK hits at short
            { 700.0f, 1.5f },
            // Glances at medium, criticals at point blank
            { 1000.0f, 1.0f },
            // Nothing guaranteed, pure gamble
            { 2000.0f, 0.1f },
        }
    };

    static const std::vector<std::pair<float, float>> move_cost_thresholds = {{
            { 10.0f, 4.0f },
            { 25.0f, 3.0f },
            { 100.0f, 1.0f },
            { 500.0f, 5.0f },
        }
    };

    float move_cost_factor = multi_lerp( move_cost_thresholds, move_cost );

    // Penalty for dodging in melee makes the gun unusable in melee
    // Until NPCs get proper kiting, at least
    int melee_penalty = weapon.volume() / 250_ml - get_skill_level( skill_dodge );
    if( melee_penalty <= 0 ) {
        // Dispersion matters less if you can just use the gun in melee
        total_dispersion = std::min<int>( total_dispersion / move_cost_factor, total_dispersion );
    }

    float dispersion_factor = multi_lerp( dispersion_thresholds, total_dispersion );

    float damage_and_accuracy = damage_factor * dispersion_factor;

    // TODO: Some better approximation of the ability to keep on shooting
    static const std::vector<std::pair<float, float>> capacity_thresholds = {{
            { 1.0f, 0.5f },
            { 5.0f, 1.0f },
            { 10.0f, 1.5f },
            { 20.0f, 2.0f },
            { 50.0f, 3.0f },
        }
    };

    // How much until reload
    float capacity = gun.clip > 0 ? std::min<float>( gun.clip, ammo ) : ammo;
    // How much until dry and a new weapon is needed
    capacity += std::min<float>( 1.0, ammo / 20.0 );
    double capacity_factor = multi_lerp( capacity_thresholds, capacity );

    double gun_value = damage_and_accuracy * capacity_factor;

    add_msg( m_debug, "%s as gun: %.1f total, %.1f dispersion, %.1f damage, %.1f capacity",
             weap.type->get_id().str(), gun_value, dispersion_factor, damage_factor,
             capacity_factor );
    return std::max( 0.0, gun_value );
}

double Character::recoil_vehicle() const
{
    // TODO: vary penalty dependent upon vehicle part on which player is boarded

    if( in_vehicle ) {
        if( const optional_vpart_position vp = g->m.veh_at( pos() ) ) {
            return static_cast<double>( std::abs( vp->vehicle().velocity ) ) * 3 / 100;
        }
    }
    return 0;
}

double Character::recoil_total() const
{
    return recoil + recoil_vehicle();
}

namespace ranged
{

std::vector<Creature *> targetable_creatures( const Character &c, const int range )
{
    return targetable_creatures( c, range, turret_data() );
}

std::vector<Creature *> targetable_creatures( const Character &c, const int range,
        const turret_data &turret )
{
    const vehicle *veh_from_turret = turret ? turret.get_veh() : nullptr;
    return g->get_creatures_if( [&c, range, veh_from_turret]( const Creature & critter ) -> bool {
        if( std::round( rl_dist_exact( c.pos(), critter.pos() ) ) > range )
        {
            return false;
        }

        if( !c.sees( critter ) && !c.sees_with_infrared( critter ) )
        {
            return false;
        }

        // TODO: get rid of fake npcs (pos() check)
        if( &c == &critter || c.pos() == critter.pos() || c.attitude_to( critter ) == Creature::Attitude::A_FRIENDLY )
        {
            return false;
        }

        // TODO: It should use projectile passability checks when finding path, not vision checks.
        std::vector<tripoint> path = g->m.find_clear_path( c.pos(), critter.pos() );
        for( const tripoint &point : path )
        {
            if( g->m.passable( point ) ) {
                // If it's passable, it doesn't block bullets
                continue;
            }

            const vehicle *veh_at_point = veh_pointer_or_null( g->m.veh_at( point ) );
            if( veh_at_point && veh_at_point != veh_from_turret ) {
                // Vehicles don't have impassable-but-shootable-through parts
                return false;
            }
            if( !g->m.has_flag_ter( TFLAG_TRANSPARENT, point ) ) {
                // If it's transparent, it's either glass (fine) or reinforced glass (not fine)
                // Hack it with the more common case for now
                // TODO: Handle armored glass
                return false;
            }
        }

        return true;
    } );
}

int burst_penalty( const Character &p, const item &gun, int gun_recoil )
{
    ///\EFFECT_DEX reduces burst penalty by flat amount
    int dex_effect = p.get_dex() * 10;
    ///\EFFECT_STR reduces burst fire penalty
    float str_effect = p.get_str() * 0.5f;

    /** @EFFECT_PISTOL reduces burst fire penalty */
    /** @EFFECT_SMG reduces burst fire penalty */
    /** @EFFECT_RIFLE reduces burst fire penalty */
    /** @EFFECT_SHOTGUN reduces burst fire penalty */
    int skill_lvl = std::min( p.get_skill_level( gun.gun_skill() ), MAX_SKILL );

    return std::max<int>( 0, 3 * ( gun_recoil - dex_effect ) / std::max( 1.0f,
                          str_effect + skill_lvl ) );
}

} // namespace ranged

target_handler::trajectory target_ui::run()
{
    if( mode == TargetMode::Spell && !no_mana && !casting->can_cast( *you ) ) {
        you->add_msg_if_player( m_bad, _( "You don't have enough %s to cast this spell" ),
                                casting->energy_string() );
    }
    if( mode == TargetMode::Fire || mode == TargetMode::TurretManual || mode == TargetMode::Shape ) {
        ensure_ranged_gun_mode();
        update_ammo_range_from_gun_mode();
        if( mode == TargetMode::Fire ) {
            sight_dispersion = you->effective_dispersion( relevant->sight_dispersion() );
        }
    }

    map &here = get_map();
    // Load settings
    allow_zlevel_shift = here.has_zlevels() && get_option<bool>( "FOV_3D" );
    snap_to_target = get_option<bool>( "SNAP_TO_TARGET" );
    if( mode == TargetMode::Turrets ) {
        // Due to how cluttered the display would become, disable it by default
        // unless aiming a single turret.
        draw_turret_lines = vturrets->size() == 1;
    }

    avatar &player_character = *you;
    on_out_of_scope cleanup( [&here, &player_character]() {
        here.invalidate_map_cache( player_character.pos().z + player_character.view_offset.z );
    } );
    restore_on_out_of_scope<tripoint> view_offset_prev( player_character.view_offset );

    shared_ptr_fast<game::draw_callback_t> target_ui_cb = make_shared_fast<game::draw_callback_t>(
    [&]() {
        draw_terrain_overlay();
    } );
    g->add_draw_callback( target_ui_cb );

    ui_adaptor ui;
    ui.on_screen_resize( [&]( ui_adaptor & ui ) {
        init_window_and_input();
        ui.position_from_window( w_target );
    } );
    ui.mark_resize();

    ui.on_redraw( [&]( const ui_adaptor & ) {
        draw_ui_window();
    } );

    // Handle multi-turn aiming
    std::string action;
    bool attack_was_confirmed = false;
    bool reentered = false;
    bool resume_critter = false;
    if( mode == TargetMode::Fire && !activity->first_turn ) {
        // We were in this UI during previous turn...
        reentered = true;
        std::string act_data = activity->action;
        if( act_data == "AIM" ) {
            // ...and ran out of moves while aiming.
        } else {
            // ...and selected 'aim and shoot', but ran out of moves.
            // So, skip retrieving input and go straight to the action.
            action = act_data;
            attack_was_confirmed = true;
        }
        // Load state to keep the ui consistent across turns
        snap_to_target = activity->snap_to_target;
        shifting_view = activity->shifting_view;
        resume_critter = activity->aiming_at_critter;
    }

    // Initialize cursor position
    src = you->pos();
    update_target_list();

    tripoint initial_dst = src;
    if( reentered ) {
        if( !try_reacquire_target( resume_critter, initial_dst ) ) {
            // Target lost
            action.clear();
            attack_was_confirmed = false;
        }
    } else {
        initial_dst = choose_initial_target();
    }
    set_cursor_pos( initial_dst );
    if( dst != initial_dst ) {
        // Our target moved out of range
        action.clear();
        attack_was_confirmed = false;
    }
    if( mode == TargetMode::Fire ) {
        if( activity->aif_duration > AIF_DURATION_LIMIT ) {
            // Break long (potentially infinite) aim-and-fire loop.
            // May happen if e.g. avatar tries to get 'precise' shot while being
            // attacked by multiple zombies, which triggers dodges and corresponding aim loss.
            action.clear();
            attack_was_confirmed = false;
        }
        if( !activity->first_turn && !action.empty() && !prompt_friendlies_in_lof() ) {
            // A friendly creature moved into line of fire during aim-and-shoot,
            // and player decided to stop aiming
            action.clear();
            attack_was_confirmed = false;
        }
        activity->acceptable_losses.clear();
        if( action.empty() ) {
            activity->aif_duration = 0;
        } else {
            activity->aif_duration += 1;
        }
    }

    // Event loop!
    ExitCode loop_exit_code;
    std::string timed_out_action;
    bool skip_redraw = false;
    for( ;; action.clear() ) {
        if( !skip_redraw ) {
            g->invalidate_main_ui_adaptor();
            ui_manager::redraw();
        }
        skip_redraw = false;

        // Wait for user input (or use value retrieved from activity)
        if( action.empty() ) {
            int timeout = get_option<int>( "EDGE_SCROLL" );
            action = ctxt.handle_input( timeout );
        }

        // If an aiming mode is selected, use "*_SHOT" instead of "FIRE"
        if( mode == TargetMode::Fire && action == "FIRE" && aim_mode->has_threshold ) {
            action = aim_mode->action;
        }

        // Handle received input
        if( handle_cursor_movement( action, skip_redraw ) ) {
            continue;
        } else if( action == "TOGGLE_SNAP_TO_TARGET" ) {
            toggle_snap_to_target();
        } else if( action == "TOGGLE_TURRET_LINES" ) {
            draw_turret_lines = !draw_turret_lines;
        } else if( action == "TOGGLE_MOVE_CURSOR_VIEW" ) {
            if( snap_to_target ) {
                toggle_snap_to_target();
            }
            shifting_view = !shifting_view;
        } else if( action == "zoom_in" ) {
            g->zoom_in();
            g->mark_main_ui_adaptor_resize();
        } else if( action == "zoom_out" ) {
            g->zoom_out();
            g->mark_main_ui_adaptor_resize();
        } else if( action == "QUIT" ) {
            loop_exit_code = ExitCode::Abort;
            break;
        } else if( action == "SWITCH_MODE" ) {
            action_switch_mode();
        } else if( action == "SWITCH_AMMO" ) {
            if( !action_switch_ammo() ) {
                loop_exit_code = ExitCode::Reload;
                break;
            }
        } else if( action == "FIRE" ) {
            if( status != Status::Good ) {
                continue;
            }
            bool can_skip_confirm = ( mode == TargetMode::Spell && casting->damage() <= 0 );
            if( !can_skip_confirm && !confirm_non_enemy_target() ) {
                continue;
            }
            set_last_target();
            loop_exit_code = ExitCode::Fire;
            break;
        } else if( action == "AIM" ) {
            if( status != Status::Good ) {
                continue;
            }

            // No confirm_non_enemy_target here because we have not initiated the firing.
            // Aiming can be stopped / aborted at any time.

            if( !action_aim() ) {
                timed_out_action = "AIM";
                loop_exit_code = ExitCode::Timeout;
                break;
            }
        } else if( action == "AIMED_SHOT" || action == "CAREFUL_SHOT" || action == "PRECISE_SHOT" ) {
            if( status != Status::Good ) {
                continue;
            }

            // This action basically means "Fire" as well; the actual firing may be delayed
            // through aiming, but there is usually no means to abort it. Therefore we query now
            if( !attack_was_confirmed && !confirm_non_enemy_target() ) {
                continue;
            }

            if( action_aim_and_shoot( action ) ) {
                loop_exit_code = ExitCode::Fire;
            } else {
                timed_out_action = action;
                loop_exit_code = ExitCode::Timeout;
            }
            break;
        }
    } // for(;;)

    switch( loop_exit_code ) {
        case ExitCode::Abort: {
            traj.clear();
            if( mode == TargetMode::Fire || mode == TargetMode::Shape ) {
                activity->aborted = true;
            }
            break;
        }
        case ExitCode::Fire: {
            bool harmful = !( mode == TargetMode::Spell && casting->damage() <= 0 );
            on_target_accepted( harmful );
            break;
        }
        case ExitCode::Timeout: {
            // We've ran out of moves, save UI state
            activity->acceptable_losses = list_friendlies_in_lof();
            traj.clear();
            activity->action = timed_out_action;
            activity->snap_to_target = snap_to_target;
            activity->shifting_view = shifting_view;
            activity->aiming_at_critter = !!dst_critter;
            break;
        }
        case ExitCode::Reload: {
            traj.clear();
            activity->aborted = true;
            activity->reload_requested = true;
            break;
        }
    }

    return traj;
}

void target_ui::init_window_and_input()
{
    std::string display_type = get_option<std::string>( "ACCURACY_DISPLAY" );
    std::string panel_type = panel_manager::get_manager().get_current_layout_id();
    narrow = ( panel_type == "compact" || panel_type == "labels-narrow" );

    int top = 0;
    int width;
    int height;
    if( narrow ) {
        // Narrow layout removes the list of controls. This allows us
        // to have small window size and not suffer from it.
        width = 34;
        height = 24;
        compact = true;
    } else {
        width = 55;
        compact = TERMY < 41;
        tiny = TERMY < 28;
        bool use_whole_sidebar = TERMY < 32;
        if( use_whole_sidebar ) {
            // If we're extremely short on space, use the whole sidebar.
            height = TERMY;
        } else if( compact ) {
            // Cover up more low-value ui elements if we're tight on space.
            height = 28;
        } else {
            // Go all out
            height = 32;
        }
    }

    w_target = catacurses::newwin( height, width, point( TERMX - width, top ) );

    ctxt = input_context( "TARGET" );
    ctxt.set_iso( true );
    ctxt.register_directions();
    ctxt.register_action( "COORDINATE" );
    ctxt.register_action( "SELECT" );
    ctxt.register_action( "FIRE" );
    ctxt.register_action( "NEXT_TARGET" );
    ctxt.register_action( "PREV_TARGET" );
    ctxt.register_action( "CENTER" );
    ctxt.register_action( "TOGGLE_SNAP_TO_TARGET" );
    ctxt.register_action( "HELP_KEYBINDINGS" );
    ctxt.register_action( "QUIT" );
    ctxt.register_action( "MOUSE_MOVE" );
    ctxt.register_action( "zoom_out" );
    ctxt.register_action( "zoom_in" );
    ctxt.register_action( "TOGGLE_MOVE_CURSOR_VIEW" );
    if( allow_zlevel_shift ) {
        ctxt.register_action( "LEVEL_UP" );
        ctxt.register_action( "LEVEL_DOWN" );
    }
    if( mode == TargetMode::Fire || mode == TargetMode::TurretManual ) {
        ctxt.register_action( "SWITCH_MODE" );
        ctxt.register_action( "SWITCH_AMMO" );
    }
    if( mode == TargetMode::Fire ) {
        ctxt.register_action( "AIM" );

        aim_types = you->get_aim_types( *relevant );
        for( aim_type &type : aim_types ) {
            if( type.has_threshold ) {
                ctxt.register_action( type.action );
            }
        }
        aim_mode = aim_types.begin();
        for( auto it = aim_types.begin(); it != aim_types.end(); ++it ) {
            if( you->preferred_aiming_mode == it->action ) {
                aim_mode = it; // default to persisted mode if possible
            }
        }
    }
    if( mode == TargetMode::Turrets ) {
        ctxt.register_action( "TOGGLE_TURRET_LINES" );
    }
}

bool target_ui::handle_cursor_movement( const std::string &action, bool &skip_redraw )
{
    cata::optional<tripoint> mouse_pos;
    const auto shift_view_or_cursor = [this]( const tripoint & delta ) {
        if( this->shifting_view ) {
            this->set_view_offset( this->you->view_offset + delta );
        } else {
            this->set_cursor_pos( dst + delta );
        }
    };

    if( action == "MOUSE_MOVE" || action == "TIMEOUT" ) {
        // Shift pos and/or view via edge scrolling
        tripoint edge_scroll = g->mouse_edge_scrolling_terrain( ctxt );
        if( edge_scroll == tripoint_zero ) {
            skip_redraw = true;
        } else {
            if( action == "MOUSE_MOVE" ) {
                edge_scroll *= 2;
            }
            if( snap_to_target ) {
                set_cursor_pos( dst + edge_scroll );
            } else {
                set_view_offset( you->view_offset + edge_scroll );
            }
        }
    } else if( const cata::optional<tripoint> delta = ctxt.get_direction( action ) ) {
        // Shift view/cursor with directional keys
        shift_view_or_cursor( *delta );
    } else if( action == "SELECT" && ( mouse_pos = ctxt.get_coordinates( g->w_terrain ) ) ) {
        // Set pos by clicking with mouse
        mouse_pos->z = you->pos().z + you->view_offset.z;
        set_cursor_pos( *mouse_pos );
    } else if( action == "LEVEL_UP" || action == "LEVEL_DOWN" ) {
        // Shift view/cursor up/down one z level
        tripoint delta = tripoint(
                             0,
                             0,
                             action == "LEVEL_UP" ? 1 : -1
                         );
        shift_view_or_cursor( delta );
    } else if( action == "NEXT_TARGET" ) {
        cycle_targets( 1 );
    } else if( action == "PREV_TARGET" ) {
        cycle_targets( -1 );
    } else if( action == "CENTER" ) {
        if( shifting_view ) {
            set_view_offset( tripoint_zero );
        } else {
            set_cursor_pos( src );
        }
    } else {
        return false;
    }

    return true;
}

bool target_ui::set_cursor_pos( const tripoint &new_pos )
{
    if( dst == new_pos ) {
        return false;
    }
    if( status == Status::OutOfAmmo && new_pos != src ) {
        // range == 0, no sense in moving cursor
        return false;
    }

    // Make sure new position is valid or find a closest valid position
    std::vector<tripoint> new_traj;
    tripoint valid_pos = new_pos;
    map &here = get_map();
    if( new_pos != src ) {
        // On Z axis, make sure we do not exceed map boundaries
        valid_pos.z = clamp( valid_pos.z, -OVERMAP_DEPTH, OVERMAP_HEIGHT );
        // Or current view range
        valid_pos.z = clamp( valid_pos.z - src.z, -fov_3d_z_range, fov_3d_z_range ) + src.z;

        new_traj = here.find_clear_path( src, valid_pos );
        if( range == 1 ) {
            // We should always be able to hit adjacent squares
            if( square_dist( src, valid_pos ) > 1 ) {
                valid_pos = new_traj[0];
            }
        } else if( trigdist ) {
            if( dist_fn( valid_pos ) > range ) {
                // Find the farthest point that is still in range
                for( size_t i = new_traj.size(); i > 0; i-- ) {
                    if( dist_fn( new_traj[i - 1] ) <= range ) {
                        valid_pos = new_traj[i - 1];
                        break;
                    }
                }
                // Sanity check
                if( dist_fn( valid_pos ) > range ) {
                    debugmsg( "Calculated trajectory exceeds allowed range!" );
                    valid_pos = src;
                }
            }
        } else {
            tripoint delta = valid_pos - src;
            valid_pos = src + tripoint(
                            clamp( delta.x, -range, range ),
                            clamp( delta.y, -range, range ),
                            clamp( delta.z, -range, range )
                        );
        }
    } else {
        new_traj.push_back( src );
    }

    if( valid_pos == dst ) {
        // We don't need to move the cursor after all
        return false;
    } else if( new_pos == valid_pos ) {
        // We can reuse new_traj
        dst = valid_pos;
        traj = new_traj;
    } else {
        dst = valid_pos;
        traj = here.find_clear_path( src, dst );
    }

    if( snap_to_target ) {
        set_view_offset( dst - src );
    }

    // Make player's sprite flip to face the current target
    point d( dst.xy() - src.xy() );
    if( !tile_iso ) {

        if( d.x > 0 ) {
            you->facing = FacingDirection::FD_RIGHT;
        } else if( d.x < 0 ) {
            you->facing = FacingDirection::FD_LEFT;
        }
    } else {
        if( d.x >= 0 && d.y >= 0 ) {
            you->facing = FacingDirection::FD_RIGHT;
        }
        if( d.y <= 0 && d.x <= 0 ) {
            you->facing = FacingDirection::FD_LEFT;
        }
    }

    // Cache creature under cursor
    if( src != dst ) {
        Creature *cr = g->critter_at( dst, true );
        if( cr && pl_sees( *cr ) ) {
            dst_critter = cr;
        } else {
            dst_critter = nullptr;
        }
    } else {
        dst_critter = nullptr;
    }

    // Update mode-specific stuff
    if( mode == TargetMode::Fire ) {
        recalc_aim_turning_penalty();
    } else if( mode == TargetMode::Spell ) {
        const std::string fx = casting->effect();
        if( fx == "target_attack" || fx == "projectile_attack" || fx == "ter_transform" ) {
            spell_aoe = spell_effect::spell_effect_blast( *casting, src, dst, casting->aoe(), true );
        } else if( fx == "cone_attack" ) {
            spell_aoe = spell_effect::spell_effect_cone( *casting, src, dst, casting->aoe(), true );
        } else if( fx == "line_attack" ) {
            spell_aoe = spell_effect::spell_effect_line( *casting, src, dst, casting->aoe(), true );
        } else {
            spell_aoe.clear();
        }
    } else if( mode == TargetMode::Turrets ) {
        update_turrets_in_range();
    } else if( mode == TargetMode::Shape ) {
        std::shared_ptr<shape> sh = shape_gen->create( src, dst );
        projectile proj = make_gun_projectile( *relevant );
        // Same as in map::shoot (should probably be a function!)
        int expected_bash_force = std::accumulate( proj.impact.begin(), proj.impact.end(), 0.0,
        []( double acc, const damage_unit & du ) {
            return acc + du.amount + du.res_pen;
        } );
        shape_coverage = ranged::expected_coverage( *sh, here, expected_bash_force );
    }

    // Update UI controls & colors
    update_status();

    return true;
}

void target_ui::on_range_ammo_changed()
{
    update_status();
    update_target_list();
}

void target_ui::update_target_list()
{
    if( range == 0 ) {
        targets.clear();
        return;
    }

    // Get targets in range and sort them by distance (targets[0] is the closest)

    if( mode == TargetMode::TurretManual ) {
        targets = ranged::targetable_creatures( *you, range, *turret );
    } else {
        targets = ranged::targetable_creatures( *you, range );
    }

    std::sort( targets.begin(), targets.end(), [&]( const Creature * lhs, const Creature * rhs ) {
        return rl_dist_exact( lhs->pos(), you->pos() ) < rl_dist_exact( rhs->pos(), you->pos() );
    } );
}

tripoint target_ui::choose_initial_target()
{
    // Try previously targeted creature
    shared_ptr_fast<Creature> cr = you->last_target.lock();
    if( cr && pl_sees( *cr ) && dist_fn( cr->pos() ) <= range ) {
        return cr->pos();
    }

    // Try closest creature
    if( !targets.empty() ) {
        return targets[0]->pos();
    }

    // Try closest practice target
    map &here = get_map();
    const std::vector<tripoint> nearby = closest_points_first( src, range );
    const auto target_spot = std::find_if( nearby.begin(), nearby.end(),
    [this, &here]( const tripoint & pt ) {
        return here.tr_at( pt ).id == tr_practice_target && this->you->sees( pt );
    } );
    if( target_spot != nearby.end() ) {
        return *target_spot;
    }

    // We've got nothing.
    return src;
}

bool target_ui::try_reacquire_target( bool critter, tripoint &new_dst )
{
    if( critter ) {
        // Try to re-acquire the creature
        shared_ptr_fast<Creature> cr = you->last_target.lock();
        if( cr && pl_sees( *cr ) && dist_fn( cr->pos() ) <= range ) {
            new_dst = cr->pos();
            return true;
        }
    }

    if( !you->last_target_pos.has_value() ) {
        // This shouldn't happen
        return false;
    }

    // Try to re-acquire target tile or tile where the target creature used to be
    tripoint local_lt = get_map().getlocal( *you->last_target_pos );
    if( dist_fn( local_lt ) <= range ) {
        new_dst = local_lt;
        // Abort aiming if a creature moved in
        return !critter && !g->critter_at( local_lt, true );
    }

    // We moved out of range
    return false;
}

void target_ui::update_status()
{
    std::vector<std::string> msgbuf;
    if( mode == TargetMode::Turrets && turrets_in_range.empty() ) {
        // None of the turrets are in range
        status = Status::OutOfRange;
    } else if( mode == TargetMode::Fire &&
               ( !ranged::gunmode_checks_common( *you, get_map(), msgbuf, relevant->gun_current_mode() ) ||
                 !ranged::gunmode_checks_weapon( *you, get_map(), msgbuf, relevant->gun_current_mode() ) ) ) {
        // Selected gun mode is empty
        // TODO: it might be some other error, but that's highly unlikely to happen, so a catch-all 'Out of ammo' is fine
        status = Status::OutOfAmmo;
    } else if( mode == TargetMode::TurretManual && ( turret->query() != turret_data::status::ready ||
               !ranged::gunmode_checks_common( *you, get_map(), msgbuf, relevant->gun_current_mode() ) ) ) {
        status = Status::OutOfAmmo;
    } else if( ( src == dst ) && !( mode == TargetMode::Spell &&
                                    casting->is_valid_target( valid_target::target_self ) ) ) {
        // TODO: consider allowing targeting yourself with turrets
        status = Status::BadTarget;
    } else if( dist_fn( dst ) > range ) {
        // We're out of range. This can happen if we switch from long-ranged
        // gun mode to short-ranged. We can, of course, move the cursor into range automatically,
        // but that would be rude. Instead, wait for directional keys/etc. and *then* move the cursor.
        status = Status::OutOfRange;
    } else {
        status = Status::Good;
    }
}

int target_ui::dist_fn( const tripoint &p )
{
    return static_cast<int>( std::round( rl_dist_exact( src, p ) ) );
}

void target_ui::set_last_target()
{
    you->last_target_pos = get_map().getabs( dst );
    if( dst_critter ) {
        you->last_target = g->shared_from( *dst_critter );
    } else {
        you->last_target.reset();
    }
}

bool target_ui::confirm_non_enemy_target()
{
    npc *const who = dynamic_cast<npc *>( dst_critter );
    if( who && !who->guaranteed_hostile() ) {
        return query_yn( _( "Really attack %s?" ), who->name.c_str() );
    }
    return true;
}

bool target_ui::prompt_friendlies_in_lof()
{
    if( mode != TargetMode::Fire ) {
        debugmsg( "Not implemented" );
        return true;
    }

    std::vector<weak_ptr_fast<Creature>> in_lof = list_friendlies_in_lof();
    std::vector<Creature *> new_in_lof;
    for( const weak_ptr_fast<Creature> &cr_ptr : in_lof ) {
        bool found = false;
        shared_ptr_fast<Creature> ptr_lock = cr_ptr.lock();
        Creature *cr = ptr_lock.get();
        for( const weak_ptr_fast<Creature> &cr2_ptr : activity->acceptable_losses ) {
            shared_ptr_fast<Creature> ptr2_lock = cr2_ptr.lock();
            Creature *cr2 = ptr2_lock.get();
            if( cr == cr2 ) {
                found = true;
                break;
            }
        }
        if( !found ) {
            new_in_lof.push_back( cr );
        }
    }

    if( new_in_lof.empty() ) {
        return true;
    }

    std::string msg = _( "There are friendly creatures in line of fire:\n" );
    for( Creature *cr : new_in_lof ) {
        msg += "  " + cr->disp_name() + "\n";
    }
    msg += _( "Proceed with the attack?" );
    return query_yn( msg );
}

std::vector<weak_ptr_fast<Creature>> target_ui::list_friendlies_in_lof()
{
    std::vector<weak_ptr_fast<Creature>> ret;
    if( mode == TargetMode::Turrets || mode == TargetMode::Spell ) {
        debugmsg( "Not implemented" );
        return ret;
    }
    for( const tripoint &p : traj ) {
        if( p != dst && p != src ) {
            Creature *cr = g->critter_at( p, true );
            if( cr && you->sees( *cr ) ) {
                Creature::Attitude a = cr->attitude_to( *this->you );
                if(
                    ( cr->is_npc() && a != Creature::A_HOSTILE ) ||
                    ( !cr->is_npc() && a == Creature::A_FRIENDLY )
                ) {
                    ret.push_back( g->shared_from( *cr ) );
                }
            }
        }
    }
    return ret;
}

void target_ui::toggle_snap_to_target()
{
    shifting_view = false;
    if( snap_to_target ) {
        // Keep current view offset
    } else {
        set_view_offset( dst - src );
    }
    snap_to_target = !snap_to_target;
}

void target_ui::cycle_targets( int direction )
{
    if( targets.empty() ) {
        // Nothing to cycle
        return;
    }

    if( dst_critter ) {
        auto t = std::find( targets.begin(), targets.end(), dst_critter );
        size_t new_target = 0;
        if( t != targets.end() ) {
            size_t idx = std::distance( targets.begin(), t );
            new_target = ( idx + targets.size() + direction ) % targets.size();
            set_cursor_pos( targets[new_target]->pos() );
            return;
        }
    }

    // There is either no creature under the cursor or the player can't see it.
    // Use the closest/farthest target in this case
    if( direction == 1 ) {
        set_cursor_pos( targets.front()->pos() );
    } else {
        set_cursor_pos( targets.back()->pos() );
    }
}

void target_ui::set_view_offset( const tripoint &new_offset )
{
    tripoint new_( new_offset.xy(), clamp( new_offset.z, -fov_3d_z_range, fov_3d_z_range ) );
    new_.z = clamp( new_.z + src.z, -OVERMAP_DEPTH, OVERMAP_HEIGHT ) - src.z;

    bool changed_z = you->view_offset.z != new_.z;
    you->view_offset = new_;
    if( changed_z ) {
        // We need to do a bunch of cache updates since we're
        // looking at a different z-level.
        get_map().invalidate_map_cache( new_.z );
    }
}

void target_ui::update_turrets_in_range()
{
    turrets_in_range.clear();
    for( vehicle_part *t : *vturrets ) {
        turret_data td = veh->turret_query( *t );
        if( td.in_range( dst ) ) {
            tripoint src = veh->global_part_pos3( *t );
            turrets_in_range.push_back( { t, line_to( src, dst ) } );
        }
    }
}

void target_ui::recalc_aim_turning_penalty()
{
    if( status != Status::Good ) {
        // We don't care about invalid situations
        predicted_recoil = MAX_RECOIL;
        return;
    }

    double curr_recoil = you->recoil;
    tripoint curr_recoil_pos;
    shared_ptr_fast<Creature> ptr_lock = you->last_target.lock();
    const Creature *lt_ptr = ptr_lock.get();
    if( lt_ptr ) {
        curr_recoil_pos = lt_ptr->pos();
    } else if( you->last_target_pos ) {
        curr_recoil_pos = get_map().getlocal( *you->last_target_pos );
    } else {
        curr_recoil_pos = src;
    }

    if( curr_recoil_pos == dst ) {
        // We're aiming at that point right now, no penalty
        predicted_recoil = curr_recoil;
    } else if( curr_recoil_pos == src ) {
        // The player wasn't aiming anywhere, max it out
        predicted_recoil = MAX_RECOIL;
    } else {
        // Raise it proportionally to how much
        // the player has to turn from previous aiming point
        const double recoil_per_degree = MAX_RECOIL / 180.0;
        const units::angle angle_curr = coord_to_angle( src, curr_recoil_pos );
        const units::angle angle_desired = coord_to_angle( src, dst );
        const units::angle phi = normalize( angle_curr - angle_desired );
        const units::angle angle = std::min( phi, 360.0_degrees - phi );
        predicted_recoil =
            std::min( MAX_RECOIL, curr_recoil + to_degrees( angle ) * recoil_per_degree );
    }
}

void target_ui::apply_aim_turning_penalty()
{
    you->recoil = predicted_recoil;
}

void target_ui::action_switch_mode()
{
    uilist menu;
    menu.settext( _( "Select preferences" ) );

    std::vector<std::function<void()>> on_select;

    if( !aim_types.empty() ) {
        menu.addentry( -1, false, 0, "  " + std::string( _( "Default aiming mode" ) ) );
        menu.entries.back().force_color = true;
        menu.entries.back().text_color = c_cyan;

        for( auto it = aim_types.begin(); it != aim_types.end(); ++it ) {
            const bool is_active_aim_mode = aim_mode == it;
            const std::string text = ( it->name.empty() ? _( "Immediate" ) : it->name ) +
                                     ( is_active_aim_mode ? _( " (active)" ) : "" );

            menu.addentry( on_select.size(), true, MENU_AUTOASSIGN, text );
            on_select.emplace_back( [it, this]() {
                aim_mode = it;
                you->preferred_aiming_mode = it->action;
            } );
            if( is_active_aim_mode ) {
                menu.entries.back().text_color = c_light_green;
            }
        }
    }

    const std::map<gun_mode_id, gun_mode> &all_gun_modes = relevant->gun_all_modes();
    if( !all_gun_modes.empty() ) {
        menu.addentry( -1, false, 0, "  " + std::string( _( "Firing mode" ) ) );
        menu.entries.back().force_color = true;
        menu.entries.back().text_color = c_cyan;

        for( const auto &mode : all_gun_modes ) {
            if( mode.second.melee() ) {
                continue;
            }
            const bool active_gun_mode = relevant->gun_get_mode_id() == mode.first;

            // If gun mode is from a gunmod use gunmod's name, pay attention to the "->" on tname
            std::string text = ( mode.second.target == relevant )
                               ? mode.second.tname()
                               : mode.second->tname() + " (" + std::to_string( mode.second.qty ) + ")";

            text += ( active_gun_mode ? _( " (active)" ) : "" );

            menu.entries.emplace_back( static_cast<int>( on_select.size() ), true, MENU_AUTOASSIGN, text );
            on_select.emplace_back( [mode, this]() {
                relevant->gun_set_mode( mode.first );
            } );
            if( active_gun_mode ) {
                menu.entries.back().text_color = c_light_green;
                if( menu.selected == 0 ) {
                    menu.selected = menu.entries.size() - 1;
                }
            }
        }
    }

    menu.query();
    if( menu.ret >= 0 && menu.ret < static_cast<int>( on_select.size() ) ) {
        size_t i = static_cast<size_t>( menu.ret );
        on_select[i]();
    } // else - just refresh

    ensure_ranged_gun_mode();
    update_ammo_range_from_gun_mode();
    on_range_ammo_changed();
}

void target_ui::ensure_ranged_gun_mode()
{
    while( relevant->gun_current_mode().melee() ) {
        relevant->gun_cycle_mode();
    }
}

void target_ui::update_ammo_range_from_gun_mode()
{
    if( mode == TargetMode::TurretManual ) {
        itype_id ammo_current = turret->ammo_current();
        // Test no-ammo and not a UPS weapon
        if( !ammo_current && ( relevant->get_gun_ups_drain() == 0 ) ) {
            ammo = nullptr;
            range = 0;
        } else {
            ammo = &*ammo_current;
            range = turret->range();
        }
    } else {
        ammo = relevant->gun_current_mode().target->ammo_data();
        range = relevant->gun_current_mode().target->gun_range( you );
    }
}

bool target_ui::action_switch_ammo()
{
    if( mode == TargetMode::TurretManual ) {
        // For turrets that use vehicle tanks & can fire multiple liquids
        if( turret->ammo_options().size() > 1 ) {
            const auto opts = turret->ammo_options();
            auto iter = opts.find( turret->ammo_current() );
            turret->ammo_select( ++iter != opts.end() ? *iter : *opts.begin() );
            ammo = &*turret->ammo_current();
            range = turret->range();
        }
    } else {
        // Leave aiming UI and open reloading UI since
        // reloading annihilates our aim anyway
        return false;
    }
    on_range_ammo_changed();
    return true;
}

bool target_ui::action_aim()
{
    set_last_target();
    apply_aim_turning_penalty();
    const double min_recoil = calculate_aim_cap( *you, dst );
    for( int i = 0; i < 10; ++i ) {
        do_aim( *you, *relevant, min_recoil );
    }

    // We've changed pc.recoil, update penalty
    recalc_aim_turning_penalty();

    return you->moves > 0;
}

bool target_ui::action_aim_and_shoot( const std::string &action )
{
    std::vector<aim_type>::iterator it;
    for( it = aim_types.begin(); it != aim_types.end(); it++ ) {
        if( action == it->action ) {
            break;
        }
    }
    if( it == aim_types.end() ) {
        debugmsg( "Could not find a valid aim_type for %s", action.c_str() );
        aim_mode = aim_types.begin();
    }
    int aim_threshold = it->threshold;
    set_last_target();
    apply_aim_turning_penalty();
    const double min_recoil = calculate_aim_cap( *you, dst );
    do {
        do_aim( *you, relevant ? *relevant : null_item_reference(), min_recoil );
    } while( you->moves > 0 && you->recoil > aim_threshold &&
             you->recoil - sight_dispersion > min_recoil );

    // If we made it under the aim threshold, go ahead and fire.
    // Also fire if we're at our best aim level already.
    // If no critter is at dst then sight dispersion does not apply,
    // so it would lock into an infinite loop.
    bool done_aiming = you->recoil <= aim_threshold || you->recoil - sight_dispersion == min_recoil ||
                       ( !g->critter_at( dst ) && you->recoil == min_recoil );
    return done_aiming;
}

void target_ui::draw_terrain_overlay()
{
    tripoint center = you->pos() + you->view_offset;

    // Removes parts that don't belong to currently visible Z level
    const auto filter_this_z = [&center]( const std::vector<tripoint> &traj ) {
        std::vector<tripoint> this_z = traj;
        this_z.erase( std::remove_if( this_z.begin(), this_z.end(),
        [&center]( const tripoint & p ) {
            return p.z != center.z;
        } ), this_z.end() );
        return this_z;
    };

    // FIXME: TILES version of g->draw_line helpfully draws a cursor at last point.
    //        This creates a fake cursor if 'dst' is on a z-level we cannot see.

    // Draw approximate line of fire for each turret in range
    if( mode == TargetMode::Turrets && draw_turret_lines ) {
        // TODO: TILES version doesn't know how to draw more than 1 line at a time.
        //       We merge all lines together and draw them as a big malformed one
        std::set<tripoint> points;
        for( const turret_with_lof &it : turrets_in_range ) {
            std::vector<tripoint> this_z = filter_this_z( it.line );
            for( const tripoint &p : this_z ) {
                points.insert( p );
            }
        }
        // Since "trajectory" for each turret is just a straight line,
        // we can draw it even if the player can't see some parts
        points.erase( dst ); // Workaround for fake cursor on TILES
        std::vector<tripoint> l( points.begin(), points.end() );
        if( dst.z == center.z ) {
            // Workaround for fake cursor bug on TILES
            l.push_back( dst );
        }
        g->draw_line( src, center, l, true );
    }

    // Draw trajectory
    if( mode != TargetMode::Turrets && dst != src ) {
        std::vector<tripoint> this_z = filter_this_z( traj );

        // Draw a highlighted trajectory only if we can see the endpoint.
        // Provides feedback to the player, but avoids leaking information
        // about tiles they can't see.
        g->draw_line( dst, center, this_z );
    }

    // Since draw_line does nothing if destination is not visible,
    // cursor also disappears. Draw it explicitly.
    if( dst.z == center.z ) {
        g->draw_cursor( dst );
    }

    // Draw spell AOE
    if( mode == TargetMode::Spell ) {
        drawsq_params params = drawsq_params().highlight( true ).center( center );
        for( const tripoint &tile : spell_aoe ) {
            if( tile.z != center.z ) {
                continue;
            }
#ifdef TILES
            if( use_tiles ) {
                g->draw_highlight( tile );
            } else {
#endif
                get_map().drawsq( g->w_terrain, tile, params );
#ifdef TILES
            }
#endif
        }
    } else if( mode == TargetMode::Shape ) {
        drawsq_params params = drawsq_params().highlight( true ).center( center );
        for( const std::pair<const tripoint, double> &pr : shape_coverage ) {
            const tripoint &tile = pr.first;
#ifdef TILES
            if( use_tiles ) {
                g->draw_highlight( tile );
            } else {
#endif
                get_map().drawsq( g->w_terrain, tile, params );
                Creature *critter = g->critter_at( tile );
                if( critter != nullptr ) {
                    g->draw_critter_highlighted( *critter, center );
                }
#ifdef TILES
            }
#endif
        }
    }
}

void target_ui::draw_ui_window()
{
    // Clear target window and make it non-transparent.
    int width = getmaxx( w_target );
    int height = getmaxy( w_target );
    for( int y = 0; y < height; y++ ) {
        for( int x = 0; x < width; x++ ) {
            mvwputch( w_target, point( x, y ), c_white, ' ' );
        }
    }

    draw_border( w_target );
    draw_window_title();
    draw_help_notice();

    int text_y = 1; // Skip top border

    panel_cursor_info( text_y );
    text_y += compact ? 0 : 1;

    if( mode == TargetMode::Fire || mode == TargetMode::TurretManual ) {
        panel_gun_info( text_y );
        panel_recoil( text_y );
        text_y += compact ? 0 : 1;
    } else if( mode == TargetMode::Spell ) {
        panel_spell_info( text_y );
        text_y += compact ? 0 : 1;
    }

    bool fill_with_blank_if_no_target = !tiny;
    panel_target_info( text_y, fill_with_blank_if_no_target );
    text_y += compact ? 0 : 1;

    if( mode == TargetMode::Turrets ) {
        panel_turret_list( text_y );
    } else if( status == Status::Good ) {
        // TODO: these are old, consider refactoring
        if( mode == TargetMode::Fire ) {
            panel_fire_mode_aim( text_y );
        } else if( mode == TargetMode::Throw || mode == TargetMode::ThrowBlind ) {
            bool blind = ( mode == TargetMode::ThrowBlind );
            draw_throw_aim( *you, w_target, text_y, ctxt, *relevant, dst, blind );
        }
    }

    if( !narrow ) {
        draw_controls_list( text_y );
    }

    wnoutrefresh( w_target );
}

std::string target_ui::uitext_title()
{
    switch( mode ) {
        case TargetMode::Fire:
        case TargetMode::TurretManual:
            return string_format( _( "Firing %s" ), relevant->tname() );
        case TargetMode::Throw:
            return string_format( _( "Throwing %s" ), relevant->tname() );
        case TargetMode::ThrowBlind:
            return string_format( _( "Blind throwing %s" ), relevant->tname() );
        default:
            return _( "Set target" );
    }
}

std::string target_ui::uitext_fire()
{
    if( mode == TargetMode::Throw || mode == TargetMode::ThrowBlind ) {
        return to_translation( "[Hotkey] to throw", "to throw" ).translated();
    } else if( mode == TargetMode::Reach ) {
        return to_translation( "[Hotkey] to attack", "to attack" ).translated();
    } else if( mode == TargetMode::Spell ) {
        return to_translation( "[Hotkey] to cast the spell", "to cast" ).translated();
    } else {
        return to_translation( "[Hotkey] to fire", "to fire" ).translated();
    }
}

void target_ui::draw_window_title()
{
    mvwprintz( w_target, point( 2, 0 ), c_white, "< " );
    trim_and_print( w_target, point( 4, 0 ), getmaxx( w_target ) - 7, c_red, uitext_title() );
    wprintz( w_target, c_white, " >" );
}

void target_ui::draw_help_notice()
{
    int text_y = getmaxy( w_target ) - 1;
    int width = getmaxx( w_target );
    const std::string label_help = string_format(
                                       narrow ? _( "[%s] show help" ) : _( "[%s] show all controls" ),
                                       ctxt.get_desc( "HELP_KEYBINDINGS", 1 ) );
    int label_width = std::min( utf8_width( label_help ), width - 6 ); // 6 for borders and "< " + " >"
    int text_x = width - label_width - 6;
    mvwprintz( w_target, point( text_x + 1, text_y ), c_white, "< " );
    trim_and_print( w_target, point( text_x + 3, text_y ), label_width, c_white, label_help );
    wprintz( w_target, c_white, " >" );
}

void target_ui::draw_controls_list( int text_y )
{
    // Change UI colors for visual feedback
    // TODO: Colorize keys inside brackets to be consistent with other UI windows
    nc_color col_enabled = c_white;
    nc_color col_disabled = c_light_gray;
    nc_color col_move = ( status != Status::OutOfAmmo ? col_enabled : col_disabled );
    nc_color col_fire = ( status == Status::Good ? col_enabled : col_disabled );

    // Get first key bound to given action OR ' ' if there are none.
    const auto bound_key = [this]( const std::string & s ) {
        const std::vector<char> keys = this->ctxt.keys_bound_to( s );
        return keys.empty() ? ' ' : keys.front();
    };
    const auto colored = [col_enabled]( nc_color color, const std::string & s ) {
        if( color == col_enabled ) {
            // col_enabled is the default one when printing
            return s;
        } else {
            return colorize( s, color );
        }
    };

    struct line {
        size_t order; // Lines with highest 'order' are removed first
        std::string str;
    };
    std::vector<line> lines;

    // Compile full list
    if( shifting_view ) {
        lines.push_back( { 8, colored( col_move, _( "Shift view with directional keys" ) ) } );
    } else {
        lines.push_back( { 8, colored( col_move, _( "Move cursor with directional keys" ) ) } );
    }
    if( is_mouse_enabled() ) {
        std::string move = _( "Mouse: LMB: Target, Wheel: Cycle," );
        std::string fire = _( "RMB: Fire" );
        lines.push_back( { 7, colored( col_move, move ) + " " + colored( col_fire, fire ) } );
    }
    {
        std::string cycle = string_format( _( "[%s] Cycle targets;" ), ctxt.get_desc( "NEXT_TARGET", 1 ) );
        std::string fire = string_format( _( "[%c] %s." ), bound_key( "FIRE" ), uitext_fire() );
        lines.push_back( { 0, colored( col_move, cycle ) + " " + colored( col_fire, fire ) } );
    }
    {
        std::string text = string_format( _( "[%c] target self; [%c] toggle snap-to-target" ),
                                          bound_key( "CENTER" ), bound_key( "TOGGLE_SNAP_TO_TARGET" ) );
        lines.push_back( { 3, colored( col_enabled, text ) } );
    }
    if( mode == TargetMode::Fire ) {
        std::string aim_and_fire;
        for( const auto &e : aim_types ) {
            if( e.has_threshold ) {
                aim_and_fire += string_format( "[%c] ", bound_key( e.action ) );
            }
        }
        aim_and_fire += _( "to aim and fire." );

        std::string aim = string_format( _( "[%c] to steady your aim.  (10 moves)" ),
                                         bound_key( "AIM" ) );

        lines.push_back( { 2, colored( col_fire, aim ) } );
        lines.push_back( { 4, colored( col_fire, aim_and_fire ) } );
    }
    if( mode == TargetMode::Fire || mode == TargetMode::TurretManual ) {
        lines.push_back( { 5, colored( col_enabled, string_format( _( "[%c] to switch firing modes." ),
                                       bound_key( "SWITCH_MODE" ) ) ) } );
        lines.push_back( { 6, colored( col_enabled, string_format( _( "[%c] to reload/switch ammo." ),
                                       bound_key( "SWITCH_AMMO" ) ) ) } );
    }
    if( mode == TargetMode::Turrets ) {
        const std::string label = draw_turret_lines
                                  ? _( "[%c] Hide lines of fire" )
                                  : _( "[%c] Show lines of fire" );
        lines.push_back( { 1, colored( col_enabled, string_format( label, bound_key( "TOGGLE_TURRET_LINES" ) ) ) } );
    }

    // Shrink the list until it fits
    int height = getmaxy( w_target );
    int available_lines = height - text_y - 1; // 1 for bottom border
    if( available_lines <= 0 ) {
        return;
    }
    while( lines.size() > static_cast<size_t>( available_lines ) ) {
        lines.erase( std::max_element( lines.begin(), lines.end(), []( const line & l1, const line & l2 ) {
            return l1.order < l2.order;
        } ) );
    }

    text_y = height - lines.size() - 1;
    for( const line &l : lines ) {
        nc_color col = col_enabled;
        print_colored_text( w_target, point( 1, text_y++ ), col, col, l.str );
    }
}

void target_ui::panel_cursor_info( int &text_y )
{
    std::string label_range;
    if( src == dst ) {
        label_range = string_format( "Range: %d", range );
    } else {
        label_range = string_format( "Range: %d/%d", dist_fn( dst ), range );
    }
    if( status == Status::OutOfRange && mode != TargetMode::Turrets ) {
        // Since each turret has its own range, highlighting cursor
        // range with red would be misleading
        label_range = colorize( label_range, c_red );
    }

    std::vector<std::string> labels;
    labels.push_back( label_range );
    if( allow_zlevel_shift ) {
        labels.push_back( string_format( _( "Elevation: %d" ), dst.z - src.z ) );
    }
    labels.push_back( string_format( _( "Targets: %d" ), targets.size() ) );

    nc_color col = c_light_gray;
    int width = getmaxx( w_target );
    int text_x = 1;
    for( const std::string &s : labels ) {
        int x_left = width - text_x - 1;
        int len = utf8_width( s, true );
        if( len > x_left ) {
            text_x = 1;
            text_y++;
        }
        print_colored_text( w_target, point( text_x, text_y ), col, col, s );
        text_x += len + 1; // 1 for space
    }
    text_y++;
}

void target_ui::panel_gun_info( int &text_y )
{
    gun_mode m = relevant->gun_current_mode();
    std::string mode_name = m.tname();
    std::string gunmod_name;
    if( m.target != relevant ) {
        // Gun mode comes from a gunmod, not base gun. Add gunmod's name
        gunmod_name = m->tname() + " ";
    }
    std::string str = string_format( _( "Firing mode: <color_cyan>%s%s (%d)</color>" ),
                                     gunmod_name, mode_name, m.qty
                                   );
    nc_color clr = c_light_gray;
    print_colored_text( w_target, point( 1, text_y++ ), clr, clr, str );

    if( status == Status::OutOfAmmo ) {
        mvwprintz( w_target, point( 1, text_y++ ), c_red, _( "OUT OF AMMO" ) );
    } else if( ammo ) {
        str = string_format( m->ammo_remaining() ? _( "Ammo: %s (%d/%d)" ) : _( "Ammo: %s" ),
                             colorize( ammo->nname( std::max( m->ammo_remaining(), 1 ) ), ammo->color ), m->ammo_remaining(),
                             m->ammo_capacity() );
        print_colored_text( w_target, point( 1, text_y++ ), clr, clr, str );
    } else {
        // Weapon doesn't use ammunition
        text_y++;
    }
}

void target_ui::panel_recoil( int &text_y )
{
    const int val = you->recoil_total();
    const int min_recoil = you->effective_dispersion( relevant->sight_dispersion() );
    const int recoil_range = MAX_RECOIL - min_recoil;
    std::string str;
    if( val >= min_recoil + ( recoil_range * 2 / 3 ) ) {
        str = pgettext( "amount of backward momentum", "<color_red>High</color>" );
    } else if( val >= min_recoil + ( recoil_range / 2 ) ) {
        str = pgettext( "amount of backward momentum", "<color_yellow>Medium</color>" );
    } else if( val >= min_recoil + ( recoil_range / 4 ) ) {
        str = pgettext( "amount of backward momentum", "<color_light_green>Low</color>" );
    } else {
        str = pgettext( "amount of backward momentum", "<color_cyan>None</color>" );
    }
    str = string_format( _( "Recoil: %s" ), str );
    nc_color clr = c_light_gray;
    print_colored_text( w_target, point( 1, text_y++ ), clr, clr, str );
}

void target_ui::panel_spell_info( int &text_y )
{
    nc_color clr = c_light_gray;

    mvwprintz( w_target, point( 1, text_y++ ), c_light_green, _( "Casting: %s (Level %u)" ),
               casting->name(),
               casting->get_level() );
    if( !no_mana || casting->energy_source() == energy_type::none_energy ) {
        if( casting->energy_source() == energy_type::hp_energy ) {
            text_y += fold_and_print( w_target, point( 1, text_y ), getmaxx( w_target ) - 2,
                                      clr,
                                      _( "Cost: %s %s" ), casting->energy_cost_string( *you ), casting->energy_string() );
        } else {
            text_y += fold_and_print( w_target, point( 1, text_y ), getmaxx( w_target ) - 2,
                                      clr,
                                      _( "Cost: %s %s (Current: %s)" ), casting->energy_cost_string( *you ), casting->energy_string(),
                                      casting->energy_cur_string( *you ) );
        }
    }

    std::string fail_str;
    if( no_fail ) {
        fail_str = colorize( _( "0.0 % Failure Chance" ), c_light_green );
    } else {
        fail_str = casting->colorized_fail_percent( *you );
    }
    print_colored_text( w_target, point( 1, text_y++ ), clr, clr, fail_str );

    if( casting->aoe() > 0 ) {
        nc_color color = c_light_gray;
        const std::string fx = casting->effect();
        const std::string aoes = casting->aoe_string();
        if( fx == "projectile_attack" || fx == "target_attack" ||
            fx == "area_pull" || fx == "area_push" ||
            fx == "ter_transform" ) {
            text_y += fold_and_print( w_target, point( 1, text_y ), getmaxx( w_target ) - 2, color,
                                      _( "Effective Spell Radius: %s%s" ), aoes,
                                      casting->in_aoe( src, dst ) ? colorize( _( " WARNING!  IN RANGE" ), c_red ) : "" );
        } else if( fx == "cone_attack" ) {
            text_y += fold_and_print( w_target, point( 1, text_y ), getmaxx( w_target ) - 2, color,
                                      _( "Cone Arc: %s degrees" ), aoes );
        } else if( fx == "line_attack" ) {
            text_y += fold_and_print( w_target, point( 1, text_y ), getmaxx( w_target ) - 2, color,
                                      _( "Line width: %s" ), aoes );
        }
    }

    mvwprintz( w_target, point( 1, text_y++ ), c_light_red, _( "Damage: %s" ),
               casting->damage_string() );

    text_y += fold_and_print( w_target, point( 1, text_y ), getmaxx( w_target ) - 2, clr,
                              casting->description() );
}

void target_ui::panel_target_info( int &text_y, bool fill_with_blank_if_no_target )
{
    int max_lines = 4;
    if( dst_critter ) {
        if( you->sees( *dst_critter ) ) {
            // FIXME: print_info doesn't really care about line limit
            //        and can always occupy up to 4 of them (or even more?).
            //        To make things consistent, we ask it for 2 lines
            //        and somewhat reliably get 4.
            int fix_for_print_info = max_lines - 2;
            dst_critter->print_info( w_target, text_y, fix_for_print_info, 1 );
            text_y += max_lines;
        } else {
            std::vector<std::string> buf;
            if( you->sees_with_infrared( *dst_critter ) ) {
                dst_critter->describe_infrared( buf );
            } else if( you->sees_with_specials( *dst_critter ) ) {
                dst_critter->describe_specials( buf );
            }
            for( size_t i = 0; i < static_cast<size_t>( max_lines ); i++, text_y++ ) {
                if( i >= buf.size() ) {
                    continue;
                }
                mvwprintw( w_target, point( 1, text_y ), buf[i] );
            }
        }
    } else if( fill_with_blank_if_no_target ) {
        // Fill with blank lines to prevent other panels from jumping around
        // when the cursor moves.
        text_y += max_lines;
        // TODO: print info about tile?
    }
}

void target_ui::panel_fire_mode_aim( int &text_y )
{
    // TODO: saving & restoring pc.recoil may actually be unnecessary
    double saved_pc_recoil = you->recoil;
    you->recoil = predicted_recoil;

    double predicted_recoil = you->recoil;
    int predicted_delay = 0;
    if( aim_mode->has_threshold && aim_mode->threshold < you->recoil ) {
        do {
            const double aim_amount = you->aim_per_move( *relevant, predicted_recoil );
            if( aim_amount > 0 ) {
                predicted_delay++;
                predicted_recoil = std::max( predicted_recoil - aim_amount, 0.0 );
            }
        } while( predicted_recoil > aim_mode->threshold &&
                 predicted_recoil - sight_dispersion > 0 );
    } else {
        predicted_recoil = you->recoil;
    }

    const double target_size = dst_critter ? dst_critter->ranged_target_size() :
                               occupied_tile_fraction( m_size::MS_MEDIUM );

    text_y = print_aim( *you, w_target, text_y, ctxt, *relevant->gun_current_mode(),
                        target_size, dst, predicted_recoil );

    if( aim_mode->has_threshold ) {
        mvwprintw( w_target, point( 1, text_y++ ), _( "%s Delay: %i" ), aim_mode->name,
                   predicted_delay );
    }

    you->recoil = saved_pc_recoil;
}

void target_ui::panel_turret_list( int &text_y )
{
    mvwprintw( w_target, point( 1, text_y++ ), _( "Turrets in range: %d/%d" ), turrets_in_range.size(),
               vturrets->size() );

    for( const turret_with_lof &it : turrets_in_range ) {
        std::string str = string_format( "* %s", it.turret->name() );
        nc_color clr = c_white;
        print_colored_text( w_target, point( 1, text_y++ ), clr, clr, str );
    }
}

void target_ui::on_target_accepted( bool harmful )
{
    // TODO: all of this should be moved into on-hit code
    const auto lt_ptr = you->last_target.lock();
    if( npc *const guy = dynamic_cast<npc *>( lt_ptr.get() ) ) {
        if( harmful ) {
            if( !guy->guaranteed_hostile() ) {
                // TODO: get rid of this. Or combine it with effect_hit_by_player
                guy->hit_by_player = true; // used for morale penalty
            }
            guy->make_angry();
        }
    } else if( monster *const mon = dynamic_cast<monster *>( lt_ptr.get() ) ) {
        mon->add_effect( effect_hit_by_player, 10_minutes );
    }
}

bool ranged::gunmode_checks_common( avatar &you, const map &m, std::vector<std::string> &messages,
                                    const gun_mode &gmode )
{
    bool result = true;

    // Check that passed gun mode is valid and we are able to use it
    if( !( gmode && you.can_use( *gmode ) ) ) {
        messages.push_back( string_format( _( "You can't currently fire your %s." ),
                                           gmode->tname() ) );
        result = false;
    }

    const optional_vpart_position vp = m.veh_at( you.pos() );
    if( vp && vp->vehicle().player_in_control( you ) && ( gmode->is_two_handed( you ) ||
            gmode->has_flag( flag_FIRE_TWOHAND ) ) ) {
        messages.push_back( string_format( _( "You can't fire your %s while driving." ),
                                           gmode->tname() ) );
        result = false;
    }

    if( gmode->has_flag( flag_FIRE_TWOHAND ) && ( !you.has_two_arms() ||
            you.worn_with_flag( flag_RESTRICT_HANDS ) ) ) {
        messages.push_back( string_format( _( "You need two free hands to fire your %s." ),
                                           gmode->tname() ) );
        result = false;
    }

    if( ranged::get_str_draw_penalty( *gmode, you ) < 0.5f ) {
        messages.push_back( string_format( _( "You don't have enough strength to fire your %s." ),
                                           gmode->tname() ) );
        result = false;
    }

    return result;
}

bool ranged::gunmode_checks_weapon( avatar &you, const map &m, std::vector<std::string> &messages,
                                    const gun_mode &gmode )
{
    bool result = true;

    if( !gmode->ammo_sufficient() && !gmode->has_flag( flag_RELOAD_AND_SHOOT ) ) {
        if( !gmode->ammo_remaining() ) {
            messages.push_back( string_format( _( "Your %s is empty!" ), gmode->tname() ) );
        } else {
            messages.push_back( string_format( _( "Your %s needs %i charges to fire!" ),
                                               gmode->tname(), gmode->ammo_required() ) );
        }
        result = false;
    }

    if( gmode->get_gun_ups_drain() > 0 ) {
        const int ups_drain = gmode->get_gun_ups_drain();
        const int adv_ups_drain = std::max( 1, ups_drain * 3 / 5 );
        bool is_mech_weapon = false;
        if( you.is_mounted() ) {
            monster *mons = get_player_character().mounted_creature.get();
            if( !mons->type->mech_weapon.is_empty() ) {
                is_mech_weapon = true;
            }
        }
        if( !is_mech_weapon ) {
            if( !( you.has_charges( itype_UPS_off, ups_drain ) ||
                   you.has_charges( itype_adv_UPS_off, adv_ups_drain ) ||
                   ( you.has_active_bionic( bio_ups ) &&
                     you.get_power_level() >= units::from_kilojoule( ups_drain ) ) ) ) {
                messages.push_back( string_format(
                                        _( "You need a UPS with at least %2$d charges or an advanced UPS with at least %3$d charges to fire the %1$s!" ),
                                        gmode->tname(), ups_drain, adv_ups_drain ) );
                result = false;
            }
        } else {
            if( !you.has_charges( itype_UPS, ups_drain ) ) {
                messages.push_back( string_format( _( "Your mech has an empty battery, its %s will not fire." ),
                                                   gmode->tname() ) );
                result = false;
            }
        }
    }

    if( gmode->has_flag( flag_MOUNTED_GUN ) ) {
        const bool v_mountable = static_cast<bool>( m.veh_at( you.pos() ).part_with_feature( "MOUNTABLE",
                                 true ) );
        bool t_mountable = m.has_flag_ter_or_furn( flag_MOUNTABLE, you.pos() );
        if( !t_mountable && !v_mountable && !( you.get_size() > MS_MEDIUM ) ) {
            messages.push_back( string_format(
                                    _( "You must stand near acceptable terrain or furniture to fire the %s.  A table, a mound of dirt, a broken window, etc." ),
                                    gmode->tname() ) );
            result = false;
        }
    }

    return result;
}
