#include "player_activity.h"
#include "cata_utility.h"
#include "player_activity_ptr.h"

#include <algorithm>
#include <array>
#include <map>
#include <memory>
#include <utility>

#include "activity_actor.h"
#include "activity_handlers.h"
#include "activity_type.h"
#include "avatar.h"
#include "calendar.h"
#include "character.h"
#include "character_turn.h"
#include "color.h"
#include "construction.h"
#include "construction_partial.h"
#include "crafting.h"
#include "distraction_manager.h"
#include "item.h"
#include "itype.h"
#include "map.h"
#include "player.h"
#include "recipe.h"
#include "rng.h"
#include "skill.h"
#include "sounds.h"
#include "string_formatter.h"
#include "string_id.h"
#include "translations.h"
#include "value_ptr.h"
#include "profile.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vehicle_part.h"
#include "vehicle_selector.h"
#include "vpart_position.h"
#include <character_functions.h>

static const activity_id ACT_GAME( "ACT_GAME" );
static const activity_id ACT_PICKAXE( "ACT_PICKAXE" );
static const activity_id ACT_START_FIRE( "ACT_START_FIRE" );
static const activity_id ACT_HAND_CRANK( "ACT_HAND_CRANK" );
static const activity_id ACT_VIBE( "ACT_VIBE" );
static const activity_id ACT_OXYTORCH( "ACT_OXYTORCH" );
static const activity_id ACT_FISH( "ACT_FISH" );
static const activity_id ACT_ATM( "ACT_ATM" );
static const activity_id ACT_GUNMOD_ADD( "ACT_GUNMOD_ADD" );

player_activity::player_activity() : type( activity_id::NULL_ID() ) { }

player_activity::player_activity( activity_id t, int turns, int Index, int pos,
                                  const std::string &name_in ) :
    type( t ), moves_total( turns ), moves_left( turns ),
    index( Index ),
    position( pos ), name( name_in ),
    placement( tripoint_min ), auto_resume( false )
{
}

player_activity::player_activity( std::unique_ptr<activity_actor> &&actor ) : type(
        actor->get_type() ),
    actor( std::move( actor ) ), moves_total( 0 ), moves_left( 0 )
{
}

player_activity::~player_activity() = default;


void player_activity::resolve_active()
{
    if( active ) {
        active = false;
    } else {
        delete this;
    }
}

void player_activity::set_to_null()
{
    type = activity_id::NULL_ID();
    sfx::end_activity_sounds(); // kill activity sounds when activity is nullified
}


int player_activity::get_value( size_t index, int def ) const
{
    return index < values.size() ? values[index] : def;
}

std::string player_activity::get_str_value( size_t index, const std::string &def ) const
{
    return index < str_values.size() ? str_values[index] : def;
}

void player_activity::calc_moves( const Character &who )
{
    if( is_bench_affected() ) {
        speed.bench = calc_bench();
    }
    if( is_light_affected() ) {
        speed.light = calc_light( who );
    }
    if( is_speed_affected() ) {
        speed.p_speed = who.get_moves();
    }
    if( is_skill_affected() ) {
        speed.skills = calc_skill();
    }
    if( is_tools_affected() ) {
        speed.tools = calc_tools();
    }
    if( is_moral_affected() ) {
        speed.morale = calc_morale( who.get_morale_level() );
    }
}

float player_activity::calc_bench() const
{
    return 1.f;
}

float player_activity::calc_light( const Character &who ) const
{
    if( character_funcs::can_see_fine_details( who ) ) {
        return 1.0f;
    }

    // This value whould be within [0,1]
    const float darkness =
        (
            character_funcs::fine_detail_vision_mod( who ) -
            character_funcs::FINE_VISION_THRESHOLD
        ) / 7.0f;
    return 1.0f - darkness;
}

float player_activity::calc_skill() const
{
    return actor ? actor->calc_skill() : 1.f;
}

float player_activity::calc_tools() const
{
    return actor ? actor->calc_tools() : 1.f;
}

float player_activity::calc_morale( int morale ) const
{
    float ac_morale = actor ? actor->calc_morale( morale ) : -1.f;
    //Any morale mod above 0 is valid, else - use default morale calc
    if( ac_morale > 0 ) {
        return ac_morale;
    }

    //1% per 4 extra morale
    if( morale > 20 ) {
        return 0.95f + morale / 25.f;
    } else if( morale < -40 ) {
        return 1.10f + morale / 25.f;
    }
    return 1.0f;
}

std::string formatSpd( float level, std::string name )
{
    if( level == 1.f ) {
        return "";
    }
    int percent = static_cast<int>( std::roundf( level * 100.f ) );
    nc_color col = percent == 100
                   ? c_white
                   : percent > 100 ? c_green : c_red;
    std::string colorized = colorize( std::to_string( percent ) + '%', col );
    return string_format( _( "%s: %s\n" ), name, colorized );

}

std::optional<std::string> player_activity::get_progress_message( const avatar &u ) const
{
    if( !type || get_verb().empty() ) {
        return std::optional<std::string>();
    }
    if( is_verbose_tooltip() ) {

        std::string time_desc = string_format( _( "Time left: %s" ),
                                               to_string( time_duration::from_turns( moves_left / speed.total() ) ) );

        std::string mults_desc = string_format( _( "Speed multipliers:\n" ),
                                                get_verb().translated() );
        mults_desc += formatSpd( speed.total(), "Total" );
        mults_desc += formatSpd( speed.assist, "Assistants" );
        mults_desc += formatSpd( speed.tools, "Tools" );
        mults_desc += formatSpd( speed.bench, "Workbench" );
        mults_desc += formatSpd( speed.light, "Light" );
        mults_desc += formatSpd( speed.morale, "Morale" );
        mults_desc += formatSpd( speed.p_speed, "Speed" );
        mults_desc += formatSpd( speed.skills, "Skills" );
        mults_desc += formatSpd( speed.tools, "Tools" );

        return string_format( _( "%s: %s\n\n%s" ), get_verb().translated(),
                              time_desc,
                              mults_desc );
    }

    if( actor ) {
        act_progress_message msg = actor->get_progress_message( *this, u );
        if( msg.implemented ) {
            if( msg.msg_full ) {
                return *msg.msg_full;
            } else if( msg.msg_extra_info ) {
                return string_format( _( "%s: %s" ), get_verb().translated(), *msg.msg_extra_info );
            } else {
                return std::nullopt;
            }
        }
    }

    if( type == activity_id( "ACT_ADV_INVENTORY" ) ||
        type == activity_id( "ACT_AIM" ) ||
        type == activity_id( "ACT_ARMOR_LAYERS" ) ||
        type == activity_id( "ACT_ATM" ) ||
        type == activity_id( "ACT_CONSUME_DRINK_MENU" ) ||
        type == activity_id( "ACT_CONSUME_FOOD_MENU" ) ||
        type == activity_id( "ACT_CONSUME_MEDS_MENU" ) ||
        type == activity_id( "ACT_EAT_MENU" ) ) {
        return std::nullopt;
    }

    std::string extra_info;
    if( type == activity_id( "ACT_READ" ) ) {
        if( const item *book = &*targets.front() ) {
            if( const auto &reading = book->type->book ) {
                const skill_id &skill = reading->skill;
                if( skill && u.get_skill_level( skill ) < reading->level &&
                    u.get_skill_level_object( skill ).can_train() ) {
                    const SkillLevel &skill_level = u.get_skill_level_object( skill );
                    //~ skill_name current_skill_level -> next_skill_level (% to next level)
                    extra_info = string_format( pgettext( "reading progress", "%s %d -> %d (%d%%)" ),
                                                skill.obj().name(),
                                                skill_level.level(),
                                                skill_level.level() + 1,
                                                skill_level.exercise() );
                }
            }
        }
    } else if( moves_total > 0 ) {
        if( type == activity_id( "ACT_BURROW" ) ||
            type == activity_id( "ACT_HACKSAW" ) ||
            type == activity_id( "ACT_JACKHAMMER" ) ||
            type == activity_id( "ACT_PICKAXE" ) ||
            type == activity_id( "ACT_VEHICLE" ) ||
            type == activity_id( "ACT_FILL_PIT" ) ||
            type == activity_id( "ACT_DIG" ) ||
            type == activity_id( "ACT_DIG_CHANNEL" ) ||
            type == activity_id( "ACT_CHOP_TREE" ) ||
            type == activity_id( "ACT_CHOP_LOGS" ) ||
            type == activity_id( "ACT_CHOP_PLANKS" )
          ) {
            const int percentage = ( ( moves_total - moves_left ) * 100 ) / moves_total;

            extra_info = string_format( "%d%%", percentage );
        }

        if( type == activity_id( "ACT_BUILD" ) ) {
            partial_con *pc = get_map().partial_con_at( get_map().getlocal( u.activity->placement ) );
            if( pc ) {
                int counter = std::min( pc->counter, 10000000 );
                const int percentage = counter / 100000;

                extra_info = string_format( "%d%%", percentage );
            }
        }
    }

    return extra_info.empty() ? string_format( _( "%s…" ),
            get_verb().translated() ) : string_format( _( "%s: %s" ),
                    get_verb().translated(), extra_info );
}

void player_activity::find_best_bench( const Character &who )
{
    bench_l best_bench = bench_l(
                             workbench_info_wrapper(
                                 * string_id<furn_t>( "f_ground_crafting_spot" ).obj().workbench.get() ),
                             bench_type::ground,
                             who.pos() );
    std::vector<tripoint> reachable( PICKUP_RANGE * PICKUP_RANGE );
    get_map().reachable_flood_steps( reachable, who.pos(), PICKUP_RANGE, 1, 100 );
    for( const tripoint &adj : reachable ) {
        if( auto wb = get_map().furn( adj ).obj().workbench ) {
            if( wb->multiplier > best_bench.wb_info.multiplier ) {
                best_bench = bench_l( workbench_info_wrapper( *wb.get() ), bench_type::furniture, adj );
            }
        }

        if( const std::optional<vpart_reference> vp = get_map().veh_at(
                    adj ).part_with_feature( "WORKBENCH", true ) ) {
            if( const std::optional<vpslot_workbench> &wb_info = vp->part().info().get_workbench_info() ) {
                if( wb_info->multiplier > best_bench.wb_info.multiplier ) {
                    best_bench = bench_l( workbench_info_wrapper( wb_info.value() ), bench_type::furniture, adj );
                }
            } else {
                debugmsg( "part '%S' with WORKBENCH flag has no workbench info", vp->part().name() );
            }
        }
    }

    bench = &best_bench;
}

void player_activity::start_or_resume( Character &who, bool resuming )
{
    if( actor && !resuming ) {
        if( is_bench_affected() ) {
            find_best_bench( who );
        }
        actor->start( *this, who );
    }
    if( rooted() ) {
        who.rooted_message();
    }
}

void player_activity::do_turn( player &p )
{
    ZoneScopedN( "player_activity::do_turn" );
    active = true;
    on_out_of_scope _resolve_on_return( [this]() {
        this->resolve_active();
    } );

    /*
    * Auto-needs block
    * Should happen before activity or it may fail du to 0 moves
    */
    if( *this && type->will_refuel_fires() ) {
        try_fuel_fire( *this, p );
    }
    if( calendar::once_every( 30_minutes ) ) {
        no_food_nearby_for_auto_consume = false;
        no_drink_nearby_for_auto_consume = false;
    }
    if( *this && !p.is_npc() && type->valid_auto_needs() && !no_food_nearby_for_auto_consume ) {
        if( p.get_kcal_percent() < 0.95f ) {
            if( !find_auto_consume( p, consume_type::FOOD ) ) {
                no_food_nearby_for_auto_consume = true;
            }
        }
        if( p.get_thirst() > thirst_levels::thirsty && !no_drink_nearby_for_auto_consume ) {
            if( !find_auto_consume( p, consume_type::DRINK ) ) {
                no_drink_nearby_for_auto_consume = true;
            }
        }
    }

    /*
    * Stamina block
    */
    int previous_stamina = p.get_stamina();
    if( p.is_npc() && p.restore_outbounds_activity() ) {
        // npc might be operating at the edge of the reality bubble.
        // or just now reloaded back into it, and their activity target might
        // be still unloaded, can cause infinite loops.
        set_to_null();
        p.drop_invalid_inventory();
        return;
    }
    const bool travel_activity = id() == activity_id( "ACT_TRAVELLING" );
    // Activities should never excessively drain stamina.
    // adjusted stamina because
    // autotravel doesn't reduce stamina after do_turn()
    // it just sets a destination, clears the activity, then moves afterwards
    // so set stamina -1 if that is the case
    // to simulate that the next step will surely use up some stamina anyway
    // this is to ensure that resting will occur when traveling overburdened
    const int adjusted_stamina = travel_activity ? p.get_stamina() - 1 : p.get_stamina();
    if( adjusted_stamina < previous_stamina && p.get_stamina() < p.get_stamina_max() / 3 ) {
        if( one_in( 50 ) ) {
            p.add_msg_if_player( _( "You pause for a moment to catch your breath." ) );
        }
        auto_resume = true;
        std::unique_ptr<player_activity> new_act = std::make_unique<player_activity>
                ( activity_id( "ACT_WAIT_STAMINA" ), to_moves<int>( 1_minutes ) );
        new_act->values.push_back( 200 + p.get_stamina_max() / 3 );
        p.assign_activity( std::move( new_act ) );
        return;
    }
    if( *this && type->rooted() ) {
        p.rooted();
        character_funcs::do_pause( p );
    }

    /*
     * Moves block
     * This might finish the activity (set it to null)
    */
    if( type->based_on() == based_on_type::TIME ) {
        if( moves_left >= 100 ) {
            moves_left -= 100;
            p.moves = 0;
        } else {
            p.moves -= moves_left ;
            moves_left = 0;
        }
    }
    // Leave as is till full migration to actors for "NEITHER"
    else if( type->based_on() != based_on_type::NEITHER ) {
        calc_moves( p );
        int moves_total = speed.totalMoves();
        if( moves_left >= moves_total ) {
            moves_left -= moves_total;
            p.moves = 0;
        } else {
            p.moves -= std::round( ( moves_total - moves_left ) * 100.f / moves_total );
            moves_left = 0;
        }
    }

    if( actor ) {
        actor->do_turn( *this, p );
    } else {
        // Use the legacy turn function
        type->call_do_turn( this, &p );
    }

    if( *this && moves_left <= 0 ) {
        // Note: For some activities "finish" is a misnomer; that's why we explicitly check if the
        // type is ACT_NULL below.
        if( actor ) {
            actor->finish( *this, p );
        } else {
            if( !type->call_finish( this, &p ) ) {
                // "Finish" is never a misnomer for any activity without a finish function
                set_to_null();
            }
        }
    }
    if( !p.activity ) {
        // Make sure data of previous activity is cleared
        p.activity = std::make_unique<player_activity>();
        p.resume_backlog_activity();

        // If whatever activity we were doing forced us to pick something up to
        // handle it, drop any overflow that may have caused
        p.drop_invalid_inventory();
    }
}

void player_activity::canceled( Character &who )
{
    if( *this && actor ) {
        actor->canceled( *this, who );
    }
}

template <typename T>
bool containers_equal( const T &left, const T &right )
{
    if( left.size() != right.size() ) {
        return false;
    }

    return std::equal( left.begin(), left.end(), right.begin() );
}

bool player_activity::can_resume_with( const player_activity &other, const Character &who ) const
{
    // Should be used for relative positions
    // And to forbid resuming now-invalid crafting

    if( !*this || !other || type->no_resume() ) {
        return false;
    }

    if( id() != other.id() ) {
        return false;
    }

    // if actor XOR other.actor then id() != other.id() so
    // we will correctly return false based on final return statement
    if( actor && other.actor ) {
        return actor->can_resume_with( *other.actor, who );
    }

    if( id() == activity_id( "ACT_CLEAR_RUBBLE" ) ) {
        if( other.coords.empty() || other.coords[0] != coords[0] ) {
            return false;
        }
    } else if( id() == activity_id( "ACT_READ" ) ) {
        // Return false if any NPCs joined or left the study session
        // the vector {1, 2} != {2, 1}, so we'll have to check manually
        if( values.size() != other.values.size() ) {
            return false;
        }
        for( int foo : other.values ) {
            if( std::find( values.begin(), values.end(), foo ) == values.end() ) {
                return false;
            }
        }
        if( targets.empty() || other.targets.empty() || targets[0] != other.targets[0] ) {
            return false;
        }
    } else if( id() == activity_id( "ACT_VEHICLE" ) ) {
        if( values != other.values || str_values != other.str_values ) {
            return false;
        }
    }

    return !auto_resume && index == other.index &&
           position == other.position && name == other.name && targets == other.targets;
}

bool player_activity::is_distraction_ignored( distraction_type type ) const
{
    return ( get_distraction_manager().is_ignored( type ) ||
             ignored_distractions.find( type ) != ignored_distractions.end() );
}

void player_activity::ignore_distraction( distraction_type type )
{
    ignored_distractions.emplace( type );
}

void player_activity::allow_distractions()
{
    ignored_distractions.clear();
}

void player_activity::inherit_distractions( const player_activity &other )
{
    for( auto &type : other.ignored_distractions ) {
        ignore_distraction( type );
    }
}


activity_ptr::activity_ptr() : act( std::make_unique<player_activity>() ) {}

activity_ptr::activity_ptr( activity_ptr && )  noexcept = default;
activity_ptr::activity_ptr( std::unique_ptr<player_activity> &&source )
{
    check_active();
    act = std::move( source );
}
activity_ptr &activity_ptr::operator=( activity_ptr && )  noexcept = default;
activity_ptr &activity_ptr::operator=( std::unique_ptr<player_activity> &&source )
{
    check_active();
    act = std::move( source );
    return *this;
}

activity_ptr::~activity_ptr()
{
    check_active();
};

void activity_ptr::check_active()
{
    if( act && act->active ) {
        //If the activity is active then we're currently inside it's do_turn so it's not safe to delete it.
        //It will delete itself at the end of it's do_turn function.
        act->active = false;
        player_activity *ptr = act.release();
        ( void )ptr;
    }
}

std::unique_ptr<player_activity> activity_ptr::release()
{
    std::unique_ptr<player_activity> ret = std::move( act );
    act = std::make_unique < player_activity>();
    return ret;
}

activity_ptr::operator bool() const
{
    return !!*act;
}

void activity_ptr::serialize( JsonOut &json ) const
{
    act->serialize( json );
}

void activity_ptr::deserialize( JsonIn &jsin )
{
    act->deserialize( jsin );
}
