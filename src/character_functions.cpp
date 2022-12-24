#include "character_functions.h"

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
static const trait_id trait_CHLOROMORPH( "CHLOROMORPH" );
static const trait_id trait_DEBUG_NODMG( "DEBUG_NODMG" );
static const trait_id trait_EASYSLEEPER( "EASYSLEEPER" );
static const trait_id trait_EASYSLEEPER2( "EASYSLEEPER2" );
static const trait_id trait_INSOMNIA( "INSOMNIA" );
static const trait_id trait_LOVES_BOOKS( "LOVES_BOOKS" );
static const trait_id trait_M_SKIN3( "M_SKIN3" );
static const trait_id trait_NOPAIN( "NOPAIN" );
static const trait_id trait_PER_SLIME_OK( "PER_SLIME_OK" );
static const trait_id trait_PSYCHOPATH( "PSYCHOPATH" );
static const trait_id trait_SAPIOVORE( "SAPIOVORE" );
static const trait_id trait_SHELL2( "SHELL2" );
static const trait_id trait_SPIRITUAL( "SPIRITUAL" );
static const trait_id trait_THRESH_SPIDER( "THRESH_SPIDER" );
static const trait_id trait_WATERSLEEP( "WATERSLEEP" );
static const trait_id trait_WEB_SPINNER( "WEB_SPINNER" );
static const trait_id trait_WEB_WALKER( "WEB_WALKER" );
static const trait_id trait_WEB_WEAVER( "WEB_WEAVER" );

static const std::string flag_FUNGUS( "FUNGUS" );
static const std::string flag_SWIMMABLE( "SWIMMABLE" );

static const efftype_id effect_boomered( "boomered" );
static const efftype_id effect_darkness( "darkness" );
static const efftype_id effect_meth( "meth" );

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

bool can_interface_armor( const Character &who )
{
    bool okay = std::any_of( who.my_bionics->begin(), who.my_bionics->end(),
    []( const bionic & b ) {
        return b.powered && b.info().has_flag( STATIC( flag_str_id( "BIONIC_ARMOR_INTERFACE" ) ) );
    } );
    return okay;
}

std::string fmt_wielded_weapon( const Character &who )
{
    if( !who.is_armed() ) {
        return _( "fists" );
    }
    const item &weapon = who.weapon;
    if( weapon.is_gun() ) {
        std::string str = string_format( "(%d) [%s] %s", weapon.ammo_remaining(),
                                         weapon.gun_current_mode().tname(), weapon.type_name() );
        // Is either the base item or at least one auxiliary gunmod loaded (includes empty magazines)
        bool base = weapon.ammo_capacity() > 0 && !weapon.has_flag( "RELOAD_AND_SHOOT" );

        const auto mods = weapon.gunmods();
        bool aux = std::any_of( mods.begin(), mods.end(), [&]( const item * e ) {
            return e->is_gun() && e->ammo_capacity() > 0 && !e->has_flag( "RELOAD_AND_SHOOT" );
        } );

        if( base || aux ) {
            for( auto e : mods ) {
                if( e->is_gun() && e->ammo_capacity() > 0 && !e->has_flag( "RELOAD_AND_SHOOT" ) ) {
                    str += " (" + std::to_string( e->ammo_remaining() );
                    if( e->magazine_integral() ) {
                        str += "/" + std::to_string( e->ammo_capacity() );
                    }
                    str += ")";
                }
            }
        }
        return str;

    } else if( weapon.is_container() && weapon.contents.num_item_stacks() == 1 ) {
        return string_format( "%s (%d)", weapon.tname(),
                              weapon.contents.front().charges );

    } else {
        return weapon.tname();
    }
}

void add_pain_msg( const Character &who, int val, body_part bp )
{
    if( who.has_trait( trait_NOPAIN ) ) {
        return;
    }
    if( bp == num_bp ) {
        if( val > 20 ) {
            who.add_msg_if_player( _( "Your body is wracked with excruciating pain!" ) );
        } else if( val > 10 ) {
            who.add_msg_if_player( _( "Your body is wracked with terrible pain!" ) );
        } else if( val > 5 ) {
            who.add_msg_if_player( _( "Your body is wracked with pain!" ) );
        } else if( val > 1 ) {
            who.add_msg_if_player( _( "Your body pains you!" ) );
        } else {
            who.add_msg_if_player( _( "Your body aches." ) );
        }
    } else {
        if( val > 20 ) {
            who.add_msg_if_player( _( "Your %s is wracked with excruciating pain!" ),
                                   body_part_name_accusative( bp ) );
        } else if( val > 10 ) {
            who.add_msg_if_player( _( "Your %s is wracked with terrible pain!" ),
                                   body_part_name_accusative( bp ) );
        } else if( val > 5 ) {
            who.add_msg_if_player( _( "Your %s is wracked with pain!" ),
                                   body_part_name_accusative( bp ) );
        } else if( val > 1 ) {
            who.add_msg_if_player( _( "Your %s pains you!" ),
                                   body_part_name_accusative( bp ) );
        } else {
            who.add_msg_if_player( _( "Your %s aches." ),
                                   body_part_name_accusative( bp ) );
        }
    }
}

void normalize( Character &who )
{
    who.martial_arts_data->reset_style();
    who.weapon = item();

    who.set_body();
    who.recalc_hp();

    who.temp_cur.fill( BODYTEMP_NORM );
    who.temp_conv.fill( BODYTEMP_NORM );
    who.set_stamina( who.get_stamina_max() );
}

void store_in_container( Character &who, item &container, item &put, bool penalties, int base_cost )
{
    who.moves -= who.item_store_cost( put, container, penalties, base_cost );
    container.put_in( who.i_rem( &put ) );
    who.reset_encumbrance();
}

bool try_wield_contents( Character &who, item &container, item *internal_item, bool penalties,
                         int base_cost )
{
    // if index not specified and container has multiple items then ask the player to choose one
    if( internal_item == nullptr ) {
        std::vector<std::string> opts;
        std::list<item *> container_contents = container.contents.all_items_top();
        std::transform( container_contents.begin(), container_contents.end(),
        std::back_inserter( opts ), []( const item * elem ) {
            return elem->display_name();
        } );
        if( opts.size() > 1 ) {
            int pos = uilist( _( "Wield what?" ), opts );
            if( pos < 0 ) {
                return false;
            }
            internal_item = *std::next( container_contents.begin(), pos );
        } else {
            internal_item = &container.contents.front();
        }
    }

    if( !container.has_item( *internal_item ) ) {
        debugmsg( "Tried to wield non-existent item from container (player::wield_contents)" );
        return false;
    }

    const ret_val<bool> ret = who.as_player()->can_wield( *internal_item );
    if( !ret.success() ) {
        who.add_msg_if_player( m_info, "%s", ret.c_str() );
        return false;
    }

    int mv = 0;

    if( who.is_armed() ) {
        if( !who.as_player()->unwield() ) {
            return false;
        }
        who.inv.unsort();
    }

    who.weapon = std::move( *internal_item );
    container.remove_item( *internal_item );
    container.on_contents_changed();

    item &weapon = who.weapon;

    who.inv.update_invlet( weapon );
    who.inv.update_cache_with_item( weapon );
    who.last_item = weapon.typeId();

    /**
     * @EFFECT_PISTOL decreases time taken to draw pistols from holsters
     * @EFFECT_SMG decreases time taken to draw smgs from holsters
     * @EFFECT_RIFLE decreases time taken to draw rifles from holsters
     * @EFFECT_SHOTGUN decreases time taken to draw shotguns from holsters
     * @EFFECT_LAUNCHER decreases time taken to draw launchers from holsters
     * @EFFECT_STABBING decreases time taken to draw stabbing weapons from sheathes
     * @EFFECT_CUTTING decreases time taken to draw cutting weapons from scabbards
     * @EFFECT_BASHING decreases time taken to draw bashing weapons from holsters
     */
    int lvl = who.get_skill_level( weapon.is_gun() ? weapon.gun_skill() : weapon.melee_skill() );
    mv += who.item_handling_cost( weapon, penalties, base_cost ) / ( ( lvl + 10.0f ) / 10.0f );

    who.moves -= mv;

    weapon.on_wield( *who.as_player(), mv );

    return true;
}

bool is_bp_immune_to( const Character &who, body_part bp, damage_unit dam )
{
    if( who.has_trait( trait_DEBUG_NODMG ) || who.is_immune_damage( dam.type ) ) {
        return true;
    }

    who.passive_absorb_hit( convert_bp( bp ).id(), dam );

    for( const item &cloth : who.worn ) {
        if( cloth.get_coverage() == 100 && cloth.covers( bp ) ) {
            cloth.mitigate_damage( dam );
        }
    }

    return dam.amount <= 0;
}

} // namespace character_funcs
