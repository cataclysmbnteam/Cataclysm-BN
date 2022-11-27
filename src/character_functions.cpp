#include "character_functions.h"
#include "character_effects.h"

#include "calendar.h"
#include "character.h"
#include "creature.h"
#include "handle_liquid.h"
#include "itype.h"
#include "rng.h"
#include "vpart_position.h"
#include "submap.h"
#include "vehicle.h"
#include "trap.h"
#include "veh_type.h"
#include "weather.h"
#include "weather_gen.h"

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
static const efftype_id effect_meth( "meth" );
static const efftype_id effect_nausea( "nausea" );
static const efftype_id effect_weed_high( "weed_high" );

static const bionic_id bio_soporific( "bio_soporific" );

static const itype_id itype_cookbook_human( "cookbook_human" );

namespace character_funcs
{

time_duration estimate_effect_dur( int skill_lvl, const efftype_id &target_effect,
                                   const time_duration &error_magnitude, int threshold, const Creature &target )
{
    const time_duration zero_duration = 0_turns;

    time_duration estimate = std::max( zero_duration, target.get_effect_dur( target_effect ) +
                                       rng( -1, 1 ) * error_magnitude *
                                       rng( 0, std::max( 0, threshold - skill_lvl ) ) );
    return estimate;
}

void siphon( Character &ch, vehicle &veh, const itype_id &desired_liquid )
{
    if( !ch.is_avatar() ) {
        // TODO: implement for NPCs
        debugmsg( "Siphoning not implemented for NPCs." );
        return;
    }
    auto qty = veh.fuel_left( desired_liquid );
    if( qty <= 0 ) {
        ch.add_msg_if_player( m_bad, _( "There is not enough %s left to siphon it." ),
                              item::nname( desired_liquid ) );
        return;
    }

    item liquid( desired_liquid, calendar::turn, qty );
    if( liquid_handler::handle_liquid( liquid, nullptr, 1, nullptr, &veh ) ) {
        veh.drain( desired_liquid, qty - liquid.charges );
    }
}

bool is_fun_to_read( const Character &ch, const item &book )
{
    // If you don't have a problem with eating humans, To Serve Man becomes rewarding
    if( ( ch.has_trait( trait_CANNIBAL ) || ch.has_trait( trait_PSYCHOPATH ) ||
          ch.has_trait( trait_SAPIOVORE ) ) &&
        book.typeId() == itype_cookbook_human ) {
        return true;
    } else if( ch.has_trait( trait_SPIRITUAL ) && book.has_flag( "INSPIRATIONAL" ) ) {
        return true;
    } else {
        return get_book_fun_for( ch, book ) > 0;
    }
}

int get_book_fun_for( const Character &ch, const item &book )
{
    int fun_bonus = book.type->book->fun;
    if( !book.is_book() ) {
        debugmsg( "called avatar::book_fun_for with non-book" );
        return 0;
    }

    // If you don't have a problem with eating humans, To Serve Man becomes rewarding
    if( ( ch.has_trait( trait_CANNIBAL ) ||
          ch.has_trait( trait_PSYCHOPATH ) ||
          ch.has_trait( trait_SAPIOVORE ) ) &&
        book.typeId() == itype_cookbook_human ) {
        fun_bonus = std::abs( fun_bonus );
    } else if( ch.has_trait( trait_SPIRITUAL ) && book.has_flag( "INSPIRATIONAL" ) ) {
        fun_bonus = std::abs( fun_bonus * 3 );
    }

    if( ch.has_trait( trait_LOVES_BOOKS ) ) {
        fun_bonus++;
    }

    if( fun_bonus > 1 && book.get_chapters() > 0 && book.get_remaining_chapters( ch ) == 0 ) {
        fun_bonus /= 2;
    }

    return fun_bonus;
}

float fine_detail_vision_mod( const Character &who )
{
    return fine_detail_vision_mod( who, who.pos() );
}

float fine_detail_vision_mod( const Character &who, const tripoint &p )
{
    // PER_SLIME_OK implies you can get enough eyes around the bile
    // that you can generally see.  There still will be the haze, but
    // it's annoying rather than limiting.
    if( who.is_blind() ||
        ( ( who.has_effect( effect_boomered ) || who.has_effect( effect_darkness ) ) &&
          !who.has_trait( trait_PER_SLIME_OK ) ) ) {
        return 11.0;
    }
    // Scale linearly as light level approaches LIGHT_AMBIENT_LIT.
    // If we're actually a source of light, assume we can direct it where we need it.
    // Therefore give a hefty bonus relative to ambient light.
    float own_light = std::max( 1.0f, LIGHT_AMBIENT_LIT - who.active_light() - 2.0f );

    // Same calculation as above, but with a result 3 lower.
    float ambient_light = std::max( 1.0f, LIGHT_AMBIENT_LIT - get_map().ambient_light_at( p ) + 1.0f );

    return std::min( own_light, ambient_light );
}

bool can_see_fine_details( const Character &who )
{
    return can_see_fine_details( who, who.pos() );
}

bool can_see_fine_details( const Character &who, const tripoint &p )
{
    return fine_detail_vision_mod( who, p ) <= FINE_VISION_THRESHOLD;
}

comfort_response_t base_comfort_value( const Character &who, const tripoint &p )
{
    // Comfort of sleeping spots is "objective", while sleep_spot( p ) is "subjective"
    // As in the latter also checks for fatigue and other variables while this function
    // only looks at the base comfyness of something. It's still subjective, in a sense,
    // as arachnids who sleep in webs will find most places comfortable for instance.
    int comfort = 0;

    comfort_response_t comfort_response;

    bool plantsleep = who.has_trait( trait_CHLOROMORPH );
    bool fungaloid_cosplay = who.has_trait( trait_M_SKIN3 );
    bool websleep = who.has_trait( trait_WEB_WALKER );
    bool webforce = who.has_trait( trait_THRESH_SPIDER ) && ( who.has_trait( trait_WEB_SPINNER ) ||
                    ( who.has_trait( trait_WEB_WEAVER ) ) );
    bool in_shell = who.has_active_mutation( trait_SHELL2 );
    bool watersleep = who.has_trait( trait_WATERSLEEP );

    map &here = get_map();
    const optional_vpart_position vp = here.veh_at( p );
    const maptile tile = here.maptile_at( p );
    const trap &trap_at_pos = tile.get_trap_t();
    const ter_id ter_at_pos = tile.get_ter();
    const furn_id furn_at_pos = tile.get_furn();

    int web = here.get_field_intensity( p, fd_web );

    // Some mutants have different comfort needs
    if( !plantsleep && !webforce ) {
        if( in_shell ) {
            comfort += 1 + static_cast<int>( comfort_level::slightly_comfortable );
            // Note: shelled individuals can still use sleeping aids!
        } else if( vp ) {
            const cata::optional<vpart_reference> carg = vp.part_with_feature( "CARGO", false );
            const cata::optional<vpart_reference> board = vp.part_with_feature( "BOARDABLE", true );
            if( carg ) {
                const vehicle_stack items = vp->vehicle().get_items( carg->part_index() );
                for( const item &items_it : items ) {
                    if( items_it.has_flag( "SLEEP_AID" ) ) {
                        // Note: BED + SLEEP_AID = 9 pts, or 1 pt below very_comfortable
                        comfort += 1 + static_cast<int>( comfort_level::slightly_comfortable );
                        comfort_response.aid = &items_it;
                        break; // prevents using more than 1 sleep aid
                    }
                }
            }
            if( board ) {
                comfort += board->info().comfort;
            } else {
                comfort -= here.move_cost( p );
            }
        }
        // Not in a vehicle, start checking furniture/terrain/traps at this point in decreasing order
        else if( furn_at_pos != f_null ) {
            comfort += 0 + furn_at_pos.obj().comfort;
        }
        // Web sleepers can use their webs if better furniture isn't available
        else if( websleep && web >= 3 ) {
            comfort += 1 + static_cast<int>( comfort_level::slightly_comfortable );
        } else if( ter_at_pos == t_improvised_shelter ) {
            comfort += 0 + static_cast<int>( comfort_level::slightly_comfortable );
        } else if( ter_at_pos == t_floor || ter_at_pos == t_floor_waxed ||
                   ter_at_pos == t_carpet_red || ter_at_pos == t_carpet_yellow ||
                   ter_at_pos == t_carpet_green || ter_at_pos == t_carpet_purple ) {
            comfort += 1 + static_cast<int>( comfort_level::neutral );
        } else if( !trap_at_pos.is_null() ) {
            comfort += 0 + trap_at_pos.comfort;
        } else {
            // Not a comfortable sleeping spot
            comfort -= here.move_cost( p );
        }

        if( comfort_response.aid == nullptr ) {
            const map_stack items = here.i_at( p );
            for( const item &items_it : items ) {
                if( items_it.has_flag( "SLEEP_AID" ) ) {
                    // Note: BED + SLEEP_AID = 9 pts, or 1 pt below very_comfortable
                    comfort += 1 + static_cast<int>( comfort_level::slightly_comfortable );
                    comfort_response.aid = &items_it;
                    break; // prevents using more than 1 sleep aid
                }
            }
        }
        if( fungaloid_cosplay && here.has_flag_ter_or_furn( flag_FUNGUS, p ) ) {
            comfort += static_cast<int>( comfort_level::very_comfortable );
        } else if( watersleep && here.has_flag_ter( flag_SWIMMABLE, p ) ) {
            comfort += static_cast<int>( comfort_level::very_comfortable );
        }
    } else if( plantsleep ) {
        if( vp || furn_at_pos != f_null ) {
            // Sleep ain't happening in a vehicle or on furniture
            comfort = static_cast<int>( comfort_level::impossible );
        } else {
            // It's very easy for Chloromorphs to get to sleep on soil!
            if( ter_at_pos == t_dirt || ter_at_pos == t_pit || ter_at_pos == t_dirtmound ||
                ter_at_pos == t_pit_shallow ) {
                comfort += static_cast<int>( comfort_level::very_comfortable );
            }
            // Not as much if you have to dig through stuff first
            else if( ter_at_pos == t_grass ) {
                comfort += static_cast<int>( comfort_level::comfortable );
            }
            // Sleep ain't happening
            else {
                comfort = static_cast<int>( comfort_level::impossible );
            }
        }
        // Has webforce
    } else {
        if( web >= 3 ) {
            // Thick Web and you're good to go
            comfort += static_cast<int>( comfort_level::very_comfortable );
        } else {
            comfort = static_cast<int>( comfort_level::impossible );
        }
    }

    if( comfort > static_cast<int>( comfort_level::comfortable ) ) {
        comfort_response.level = comfort_level::very_comfortable;
    } else if( comfort > static_cast<int>( comfort_level::slightly_comfortable ) ) {
        comfort_response.level = comfort_level::comfortable;
    } else if( comfort > static_cast<int>( comfort_level::neutral ) ) {
        comfort_response.level = comfort_level::slightly_comfortable;
    } else if( comfort == static_cast<int>( comfort_level::neutral ) ) {
        comfort_response.level = comfort_level::neutral;
    } else {
        comfort_response.level = comfort_level::uncomfortable;
    }
    return comfort_response;
}

int rate_sleep_spot( const Character &who, const tripoint &p )
{
    const int current_stim = who.get_stim();
    const comfort_response_t comfort_info = base_comfort_value( who, p );
    if( comfort_info.aid != nullptr ) {
        who.add_msg_if_player( m_info, _( "You use your %s for comfort." ), comfort_info.aid->tname() );
    }

    int sleepy = static_cast<int>( comfort_info.level );
    bool watersleep = who.has_trait( trait_WATERSLEEP );

    if( who.has_addiction( add_type::SLEEP ) ) {
        sleepy -= 4;
    }
    if( who.has_trait( trait_INSOMNIA ) ) {
        // 12.5 points is the difference between "tired" and "dead tired"
        sleepy -= 12;
    }
    if( who.has_trait( trait_EASYSLEEPER ) ) {
        // Low fatigue (being rested) has a much stronger effect than high fatigue
        // so it's OK for the value to be that much higher
        sleepy += 40;
    }
    if( who.has_active_bionic( bio_soporific ) ) {
        sleepy += 30;
    }
    if( who.has_trait( trait_EASYSLEEPER2 ) ) {
        // At this point, the only limit to sleep is tiredness
        sleepy += 100;
    }
    if( watersleep && get_map().has_flag_ter( "SWIMMABLE", p ) ) {
        sleepy += 10; //comfy water!
    }

    if( who.get_fatigue() < fatigue_levels::tired + 1 ) {
        sleepy -= static_cast<int>( ( fatigue_levels::tired + 1 - who.get_fatigue() ) / 4 );
    } else {
        sleepy += static_cast<int>( ( who.get_fatigue() - fatigue_levels::tired + 1 ) / 16 );
    }

    if( current_stim > 0 || !who.has_trait( trait_INSOMNIA ) ) {
        sleepy -= 2 * current_stim;
    } else {
        // Make it harder for insomniac to get around the trait
        sleepy -= current_stim;
    }

    return sleepy;
}

bool roll_can_sleep( Character &who )
{
    if( who.has_effect( effect_meth ) ) {
        // Sleep ain't happening until that meth wears off completely.
        return false;
    }

    // Since there's a bit of randomness to falling asleep, we want to
    // prevent exploiting this if can_sleep() gets called over and over.
    // Only actually check if we can fall asleep no more frequently than
    // every 30 minutes.  We're assuming that if we return true, we'll
    // immediately be falling asleep after that.
    //
    // Also if player debug menu'd time backwards this breaks, just do the
    // check anyway, this will reset the timer if 'dur' is negative.
    const time_point now = calendar::turn;
    const time_duration dur = now - who.last_sleep_check;
    if( dur >= 0_turns && dur < 30_minutes ) {
        return false;
    }
    who.last_sleep_check = now;

    int sleepy = character_funcs::rate_sleep_spot( who, who.pos() );
    sleepy += rng( -8, 8 );
    bool result = sleepy > 0;

    if( who.has_active_bionic( bio_soporific ) ) {
        if( who.bio_soporific_powered_at_last_sleep_check && !who.has_power() ) {
            who.add_msg_if_player( m_bad, _( "Your soporific inducer runs out of power!" ) );
        } else if( !who.bio_soporific_powered_at_last_sleep_check && who.has_power() ) {
            who.add_msg_if_player( m_good, _( "Your soporific inducer starts back up." ) );
        }
        who.bio_soporific_powered_at_last_sleep_check = who.has_power();
    }

    return result;
}

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

} // namespace character_funcs

namespace character_effects
{

stat_mod get_pain_penalty( const Character &ch )
{
    stat_mod ret;
    int pain = ch.get_perceived_pain();
    if( pain <= 0 ) {
        return ret;
    }

    int stat_penalty = std::floor( std::pow( pain, 0.8f ) / 10.0f );

    bool ceno = ch.has_trait( trait_CENOBITE );
    if( !ceno ) {
        ret.strength = stat_penalty;
        ret.dexterity = stat_penalty;
    }

    if( !ch.has_trait( trait_INT_SLIME ) ) {
        ret.intelligence = stat_penalty;
    } else {
        ret.intelligence = pain / 5;
    }

    ret.perception = stat_penalty * 2 / 3;

    ret.speed = std::pow( pain, 0.7f );
    if( ceno ) {
        ret.speed /= 2;
    }

    ret.speed = std::min( ret.speed, 30 );
    return ret;
}

int get_kcal_speed_penalty( float kcal_percent )
{
    static const std::vector<std::pair<float, float>> starv_thresholds = { {
            std::make_pair( 0.0f, -90.0f ),
            std::make_pair( 0.1f, -50.f ),
            std::make_pair( 0.3f, -25.0f ),
            std::make_pair( 0.5f, 0.0f )
        }
    };
    if( kcal_percent > 0.95f ) {
        return 0;
    } else {
        return std::round( multi_lerp( starv_thresholds, kcal_percent ) );
    }
}

int get_thirst_speed_penalty( int thirst )
{
    // We die at 1200 thirst
    // Start by dropping speed really fast, but then level it off a bit
    static const std::vector<std::pair<float, float>> thirst_thresholds = {{
            std::make_pair( static_cast<float>( thirst_levels::very_thirsty ), 0.0f ),
            std::make_pair( static_cast<float>( thirst_levels::dehydrated ), -25.0f ),
            std::make_pair( static_cast<float>( thirst_levels::parched ), -50.0f ),
            std::make_pair( static_cast<float>( thirst_levels::dead ), -75.0f )
        }
    };
    return static_cast<int>( multi_lerp( thirst_thresholds, thirst ) );
}

int calc_morale_fatigue_cap( int fatigue )
{
    if( fatigue >= fatigue_levels::massive ) {
        return 20;
    } else if( fatigue >= fatigue_levels::exhausted ) {
        return 40;
    } else if( fatigue >= fatigue_levels::dead_tired ) {
        return 60;
    } else if( fatigue >= fatigue_levels::tired ) {
        return 80;
    }
    return 0;
}

double vomit_mod( const Character &ch )
{
    double mod = 1;
    if( ch.has_effect( effect_weed_high ) ) {
        mod *= .1;
    }
    if( ch.has_trait( trait_STRONGSTOMACH ) ) {
        mod *= .5;
    }
    if( ch.has_trait( trait_WEAKSTOMACH ) ) {
        mod *= 2;
    }
    if( ch.has_trait( trait_NAUSEA ) ) {
        mod *= 3;
    }
    if( ch.has_trait( trait_VOMITOUS ) ) {
        mod *= 3;
    }
    // If you're already nauseous, any food in your stomach greatly
    // increases chance of vomiting. Water doesn't provoke vomiting, though.
    if( ch.stomach.get_calories() > 0 && ch.has_effect( effect_nausea ) ) {
        mod *= 5 * ch.get_effect_int( effect_nausea );
    }
    return mod;
}

} // namespace character_effects
