#include "character_turn.h"

#include "bionics.h"
#include "calendar.h"
#include "character_martial_arts.h"
#include "character.h"
#include "creature.h"
#include "handle_liquid.h"
#include "itype.h"
#include "make_static.h"
#include "map_iterator.h"
#include "player.h"
#include "rng.h"
#include "submap.h"
#include "trap.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vpart_position.h"
#include "weather_gen.h"
#include "weather.h"

static const trait_id trait_CANNIBAL( "CANNIBAL" );
static const trait_id trait_CENOBITE( "CENOBITE" );
static const trait_id trait_CHITIN_FUR( "CHITIN_FUR" );
static const trait_id trait_CHITIN_FUR2( "CHITIN_FUR2" );
static const trait_id trait_CHITIN_FUR3( "CHITIN_FUR3" );
static const trait_id trait_CHLOROMORPH( "CHLOROMORPH" );
static const trait_id trait_EASYSLEEPER( "EASYSLEEPER" );
static const trait_id trait_EASYSLEEPER2( "EASYSLEEPER2" );
static const trait_id trait_FELINE_FUR( "FELINE_FUR" );
static const trait_id trait_FUR( "FUR" );
static const trait_id trait_INSOMNIA( "INSOMNIA" );
static const trait_id trait_INT_SLIME( "INT_SLIME" );
static const trait_id trait_LIGHTFUR( "LIGHTFUR" );
static const trait_id trait_LOVES_BOOKS( "LOVES_BOOKS" );
static const trait_id trait_LUPINE_FUR( "LUPINE_FUR" );
static const trait_id trait_M_SKIN3( "M_SKIN3" );
static const trait_id trait_NAUSEA( "NAUSEA" );
static const trait_id trait_PER_SLIME_OK( "PER_SLIME_OK" );
static const trait_id trait_PSYCHOPATH( "PSYCHOPATH" );
static const trait_id trait_SAPIOVORE( "SAPIOVORE" );
static const trait_id trait_SHELL2( "SHELL2" );
static const trait_id trait_SLIMY( "SLIMY" );
static const trait_id trait_SPIRITUAL( "SPIRITUAL" );
static const trait_id trait_STRONGSTOMACH( "STRONGSTOMACH" );
static const trait_id trait_THRESH_SPIDER( "THRESH_SPIDER" );
static const trait_id trait_URSINE_FUR( "URSINE_FUR" );
static const trait_id trait_VOMITOUS( "VOMITOUS" );
static const trait_id trait_WATERSLEEP( "WATERSLEEP" );
static const trait_id trait_WEAKSTOMACH( "WEAKSTOMACH" );
static const trait_id trait_WEB_SPINNER( "WEB_SPINNER" );
static const trait_id trait_WEB_WALKER( "WEB_WALKER" );
static const trait_id trait_WEB_WEAVER( "WEB_WEAVER" );

static const std::string flag_FUNGUS( "FUNGUS" );
static const std::string flag_SWIMMABLE( "SWIMMABLE" );

static const efftype_id effect_boomered( "boomered" );
static const efftype_id effect_darkness( "darkness" );
static const efftype_id effect_downed( "downed" );
static const efftype_id effect_drunk( "drunk" );
static const efftype_id effect_meth( "meth" );
static const efftype_id effect_nausea( "nausea" );
static const efftype_id effect_onfire( "onfire" );
static const efftype_id effect_weed_high( "weed_high" );

static const skill_id skill_swimming( "swimming" );

static const bionic_id bio_ground_sonar( "bio_ground_sonar" );
static const bionic_id bio_soporific( "bio_soporific" );

static const itype_id itype_cookbook_human( "cookbook_human" );

namespace character_funcs
{

void update_body_wetness( Character &who, const w_point &weather )
{
    // Average number of turns to go from completely soaked to fully dry
    // assuming average temperature and humidity
    constexpr time_duration average_drying = 2_hours;

    // A modifier on drying time
    double delay = 1.0;
    // Weather slows down drying
    delay += ( ( weather.humidity - 66 ) - ( units::to_fahrenheit( weather.temperature ) - 65 ) ) / 100;
    delay = std::max( 0.1, delay );
    // Fur/slime retains moisture
    if( who.has_trait( trait_LIGHTFUR ) ||
        who.has_trait( trait_FUR ) ||
        who.has_trait( trait_FELINE_FUR ) ||
        who.has_trait( trait_LUPINE_FUR ) ||
        who.has_trait( trait_CHITIN_FUR ) ||
        who.has_trait( trait_CHITIN_FUR2 ) ||
        who.has_trait( trait_CHITIN_FUR3 ) ) {
        delay = delay * 6 / 5;
    }
    if( who.has_trait( trait_URSINE_FUR ) || who.has_trait( trait_SLIMY ) ) {
        delay *= 1.5;
    }

    if( !x_in_y( 1, to_turns<int>( average_drying * delay / 100.0 ) ) ) {
        // No drying this turn
        return;
    }

    // Now per-body-part stuff
    // To make drying uniform, make just one roll and reuse it
    const int drying_roll = rng( 1, 80 );

    for( const body_part bp : all_body_parts ) {
        if( who.body_wetness[bp] == 0 ) {
            continue;
        }
        // This is to normalize drying times
        int drying_chance = who.drench_capacity[bp];
        // Body temperature affects duration of wetness
        // Note: Using temp_conv rather than temp_cur, to better approximate environment
        if( who.temp_conv[bp] >= BODYTEMP_SCORCHING ) {
            drying_chance *= 2;
        } else if( who.temp_conv[bp] >= BODYTEMP_VERY_HOT ) {
            drying_chance = drying_chance * 3 / 2;
        } else if( who.temp_conv[bp] >= BODYTEMP_HOT ) {
            drying_chance = drying_chance * 4 / 3;
        } else if( who.temp_conv[bp] > BODYTEMP_COLD ) {
            // Comfortable, doesn't need any changes
        } else {
            // Evaporation doesn't change that much at lower temp
            drying_chance = drying_chance * 3 / 4;
        }

        if( drying_chance < 1 ) {
            drying_chance = 1;
        }

        // TODO: Make evaporation reduce body heat
        if( drying_chance >= drying_roll ) {
            who.body_wetness[bp] -= 1;
            if( who.body_wetness[bp] < 0 ) {
                who.body_wetness[bp] = 0;
            }
        }
    }
    // TODO: Make clothing slow down drying
}

void do_pause( Character &who )
{
    map &here = get_map();

    who.moves = 0;
    who.recoil = MAX_RECOIL;

    // Train swimming if underwater
    if( !who.in_vehicle ) {
        if( who.is_underwater() ) {
            who.as_player()->practice( skill_swimming, 1 );
            who.drench( 100, { {
                    bp_leg_l, bp_leg_r, bp_torso, bp_arm_l,
                    bp_arm_r, bp_head, bp_eyes, bp_mouth,
                    bp_foot_l, bp_foot_r, bp_hand_l, bp_hand_r
                }
            }, true );
        } else if( here.has_flag( TFLAG_DEEP_WATER, who.pos() ) ) {
            who.as_player()->practice( skill_swimming, 1 );
            // Same as above, except no head/eyes/mouth
            who.drench( 100, { {
                    bp_leg_l, bp_leg_r, bp_torso, bp_arm_l,
                    bp_arm_r, bp_foot_l, bp_foot_r, bp_hand_l,
                    bp_hand_r
                }
            }, true );
        } else if( here.has_flag( "SWIMMABLE", who.pos() ) ) {
            who.drench( 40, { { bp_foot_l, bp_foot_r, bp_leg_l, bp_leg_r } }, false );
        }
    }

    // Try to put out clothing/hair fire
    if( who.has_effect( effect_onfire ) ) {
        time_duration total_removed = 0_turns;
        time_duration total_left = 0_turns;
        bool on_ground = who.has_effect( effect_downed );
        for( const body_part bp : all_body_parts ) {
            effect &eff = who.get_effect( effect_onfire, bp );
            if( eff.is_null() ) {
                continue;
            }

            // TODO: Tools and skills
            total_left += eff.get_duration();
            // Being on the ground will smother the fire much faster because you can roll
            const time_duration dur_removed = on_ground ? eff.get_duration() / 2 + 2_turns : 1_turns;
            eff.mod_duration( -dur_removed );
            total_removed += dur_removed;
        }

        // Don't drop on the ground when the ground is on fire
        if( total_left > 1_minutes && !who.is_dangerous_fields( here.field_at( who.pos() ) ) ) {
            who.add_effect( effect_downed, 2_turns, num_bp, 0, true );
            who.add_msg_player_or_npc( m_warning,
                                       _( "You roll on the ground, trying to smother the fire!" ),
                                       _( "<npcname> rolls on the ground!" ) );
        } else if( total_removed > 0_turns ) {
            who.add_msg_player_or_npc( m_warning,
                                       _( "You attempt to put out the fire on you!" ),
                                       _( "<npcname> attempts to put out the fire on them!" ) );
        }
    }

    // on-pause effects for martial arts
    who.martial_arts_data->ma_onpause_effects( who );

    if( who.is_npc() ) {
        // The stuff below doesn't apply to NPCs
        // search_surroundings should eventually do, though
        return;
    }

    if( who.in_vehicle && one_in( 8 ) ) {
        VehicleList vehs = here.get_vehicles();
        vehicle *veh = nullptr;
        for( auto &v : vehs ) {
            veh = v.v;
            if( veh && veh->is_moving() && veh->player_in_control( who ) ) {
                double exp_temp = 1 + veh->total_mass() / 400.0_kilogram +
                                  std::abs( veh->velocity / 3200.0 );
                int experience = static_cast<int>( exp_temp );
                if( exp_temp - experience > 0 && x_in_y( exp_temp - experience, 1.0 ) ) {
                    experience++;
                }
                who.as_player()->practice( skill_id( "driving" ), experience );
                break;
            }
        }
    }

    search_surroundings( who );
    who.wait_effects();
}

void search_surroundings( Character &who )
{
    if( who.controlling_vehicle ) {
        return;
    }
    const map &here = get_map();
    // Search for traps in a larger area than before because this is the only
    // way we can "find" traps that aren't marked as visible.
    // Detection formula takes care of likelihood of seeing within this range.
    for( const tripoint &tp : here.points_in_radius( who.pos(), 5 ) ) {
        const trap &tr = here.tr_at( tp );
        if( tr.is_null() || tp == who.pos() ) {
            continue;
        }
        if( who.has_active_bionic( bio_ground_sonar ) && !who.knows_trap( tp ) &&
            ( tr.loadid == tr_beartrap_buried ||
              tr.loadid == tr_landmine_buried || tr.loadid == tr_sinkhole ) ) {
            const std::string direction = direction_name( direction_from( who.pos(), tp ) );
            who.add_msg_if_player( m_warning, _( "Your ground sonar detected a %1$s to the %2$s!" ),
                                   tr.name(), direction );
            who.add_known_trap( tp, tr );
        }
        if( !who.sees( tp ) ) {
            continue;
        }
        if( tr.is_always_invisible() || tr.can_see( tp, who ) ) {
            // Already seen, or can never be seen
            continue;
        }
        // Chance to detect traps we haven't yet seen.
        if( tr.detect_trap( tp, who ) ) {
            if( tr.get_visibility() > 0 ) {
                // Only bug player about traps that aren't trivial to spot.
                const std::string direction = direction_name(
                                                  direction_from( who.pos(), tp ) );
                who.add_msg_if_player( _( "You've spotted a %1$s to the %2$s!" ),
                                       tr.name(), direction );
            }
            who.add_known_trap( tp, tr );
        }
    }
}

} // namespace character_funcs
