#include "player_activity.h"
#include "cata_utility.h"
#include "player_activity_ptr.h"

#include <algorithm>
#include <array>
#include <memory>
#include <utility>

#include "activity_actor.h"
#include "activity_actor_definitions.h"
#include "activity_handlers.h"
#include "activity_type.h"
#include "avatar.h"
#include "calendar.h"
#include "character.h"
#include "character_turn.h"
#include "color.h"
#include "construction_partial.h"
#include "crafting.h"
#include "distraction_manager.h"
#include "item.h"
#include "itype.h"
#include "map.h"
#include "player.h"
#include "rng.h"
#include "skill.h"
#include "sounds.h"
#include "string_formatter.h"
#include "string_id.h"
#include "translations.h"
#include "value_ptr.h"
#include "profile.h"
#include "veh_type.h"
#include "vehicle_part.h"
#include "vpart_position.h"
#include "character_functions.h"
#include "game.h"
#include "npc.h"


static const activity_id ACT_ADV_INVENTORY( "ACT_ADV_INVENTORY" );
static const activity_id ACT_AIM( "ACT_AIM" );
static const activity_id ACT_ARMOR_LAYERS( "ACT_ARMOR_LAYERS" );
static const activity_id ACT_ASSIST( "ACT_ASSIST" );
static const activity_id ACT_ATM( "ACT_ATM" );
static const activity_id ACT_BURROW( "ACT_BURROW" );
static const activity_id ACT_CHOP_LOGS( "ACT_CHOP_LOGS" );
static const activity_id ACT_CHOP_PLANKS( "ACT_CHOP_PLANKS" );
static const activity_id ACT_CHOP_TREE( "ACT_CHOP_TREE" );
static const activity_id ACT_CLEAR_RUBBLE( "ACT_CLEAR_RUBBLE" );
static const activity_id ACT_CONSUME_DRINK_MENU( "ACT_CONSUME_DRINK_MENU" );
static const activity_id ACT_CONSUME_FOOD_MENU( "ACT_CONSUME_FOOD_MENU" );
static const activity_id ACT_CONSUME_MEDS_MENU( "ACT_CONSUME_MEDS_MENU" );
static const activity_id ACT_CRAFT( "ACT_CRAFT" );
static const activity_id ACT_DIG( "ACT_DIG" );
static const activity_id ACT_DIG_CHANNEL( "ACT_DIG_CHANNEL" );
static const activity_id ACT_EAT_MENU( "ACT_EAT_MENU" );
static const activity_id ACT_FILL_PIT( "ACT_FILL_PIT" );
static const activity_id ACT_HACKSAW( "ACT_HACKSAW" );
static const activity_id ACT_JACKHAMMER( "ACT_JACKHAMMER" );
static const activity_id ACT_PICKAXE( "ACT_PICKAXE" );
static const activity_id ACT_READ( "ACT_READ" );
static const activity_id ACT_TRAVELLING( "ACT_TRAVELLING" );
static const activity_id ACT_VEHICLE( "ACT_VEHICLE" );
static const activity_id ACT_WAIT_STAMINA( "ACT_WAIT_STAMINA" );


static const skill_id stat_speech( "speech" );

static const quality_id qual_BUTCHER( "BUTCHER" );
static const quality_id qual_CUT_FINE( "CUT_FINE" );

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

inline float limit_factor( float factor, float min = 0.25f, float max = 2.0f )
{
    //constrain speed between min and max
    factor = std::min( max, factor );
    factor = std::max( min, factor );
    return factor;
}

inline float refine_factor( float speed, int denom = 1, float min = -75.0f, float max = 100.0f )
{
    speed = limit_factor( speed, min, max );
    denom = denom < 1.0f
            ? 1.0f
            : denom;
    speed /= denom;

    //speed to factor
    return speed / 100.0f;
}

inline std::vector<npc *> &player_activity::assistants()
{
    if( !assistants_ids_.empty() && assistants_.empty() ) {
        for( npc &guy : g->all_npcs() ) {
            if( assistants_ids_.contains( guy.getID().get_value() ) ) {
                assistants_.push_back( &guy );
            }
        }
    }
    return assistants_;
}

void player_activity::calc_moves( const Character &who )
{
    if( is_light_affected() ) {
        speed.light = calc_light_factor( who );
    }
    if( is_speed_affected() ) {
        speed.player_speed = who.get_speed() / 100.0f;
    }
    if( is_stats_affected() ) {
        speed.stats = calc_stats_factors( who );
    }
    if( is_morale_affected() ) {
        speed.morale = calc_morale_factor( who.get_morale_level() );
    }
}

void player_activity::recalc_all_moves( Character &who )
{
    if( is_bench_affected() ) {
        find_best_bench( who.pos() );
        speed.bench = calc_bench_factor( who );
    }
    if( is_tools_affected() ) {
        speed.tools = calc_tools_factor( who );
    }
    if( is_skill_affected() ) {
        speed.skills = calc_skill_factor( who );
    }
    if( is_assistable() ) {
        if( assistants().empty() ) {
            get_assistants( who );
        }
        speed.assist = calc_assistants_factor( who );
    }
    calc_moves( who );
}

void player_activity::recalc_all_moves( Character &who, activity_reqs_adapter &reqs )
{
    if( is_bench_affected() ) {
        find_best_bench( who.pos() );
        speed.bench = calc_bench_factor( who );
    }
    if( is_tools_affected() ) {
        speed.tools = calc_tools_factor( who, reqs.qualities );
    }
    if( is_skill_affected() ) {
        speed.skills = calc_skill_factor( who, reqs.skills );
    }
    if( is_assistable() ) {
        if( assistants().empty() ) {
            get_assistants( who );
        }
        speed.assist = calc_assistants_factor( who );
    }
    calc_moves( who );
}

void player_activity::get_assistants( const Character &who,
                                      unsigned short max )
{
    max = type->max_assistants();
    if( max == 0 ) {
        return;
    }
    int n = 0;
    assistants_ = g->get_npcs_if( [&]( const npc & guy ) {
        if( n >= max ) {
            return false;
        }
        // NPCs can help craft if awake, taking orders, within pickup range and have clear path
        bool ok = guy.is_npc() && !guy.in_sleep_state() && guy.is_obeying( who ) &&
                  guy.activity->id() != ACT_ASSIST &&
                  rl_dist( guy.pos(), who.pos() ) < PICKUP_RANGE &&
                  get_map().clear_path( who.pos(), guy.pos(), PICKUP_RANGE, 1, 100 );
        if( ok ) {
            n++;
        }
        return ok;
    } );
    for( Character *guy : assistants_ ) {
        guy->assign_activity( std::make_unique<player_activity>
                              ( std::make_unique<assist_activity_actor>() ) );
        assistants_ids_.insert( guy->getID().get_value() );
    }
}

float player_activity::calc_assistants_factor( const Character &who )
{
    int x = assistants().size();
    if( x == 0 ) {
        return 1.0f;
    }

    float f = 0.5f * std::pow( x, 3 )
              - 7 * std::pow( x, 2 )
              + 45 * x;

    // range [0.8:1.2] based on speech
    f *= 0.8f + 0.04f * who.get_skill_level( stat_speech ) ;

    return 1.0f + refine_factor( f, 1, 0.0f, 200.0f );
}

float player_activity::calc_bench_factor( const Character &who ) const
{
    float ac_f = actor ? actor->calc_bench_factor( who, bench ) : -1.0f;
    //Any factor above 0 is valid, else - use default calc
    if( ac_f > 0 ) {
        return ac_f;
    }

    return bench.has_value()
           ? bench->wb_info.multiplier
           : 1.0f;
}

float player_activity::calc_light_factor( const Character &who ) const
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
    return limit_factor( 1.0f - darkness, 0.0f );
}


float player_activity::calc_skill_factor( const Character &who,
        const std::vector<activity_req<skill_id>> &skill_req ) const
{
    float ac_f = actor ? actor->calc_skill_factor( who, skill_req ) : -1.0f;
    //Any factor above 0 is valid, else - use default calc
    if( ac_f > 0 ) {
        return ac_f;
    }

    float f = 1.0f;
    std::vector<float> factors;
    for( const auto &skill : skill_req ) {
        int who_eff_skill = who.get_skill_level( skill.req ) - skill.threshold;
        float bonus = 0;
        if( who_eff_skill != 0 ) {
            bonus = 0.02f * std::pow( who_eff_skill, 3 )
                    -  0.5f * std::pow( who_eff_skill, 2 )
                    +  6.0f * who_eff_skill + skill.mod;
        }

        factors.push_back( bonus );
    }
    std::sort( factors.begin(), factors.end(), std::greater<>() );

    int denom = 0;
    for( const auto &factor : factors ) {
        f += refine_factor( factor, ++denom * 0.8f ) ;
    }

    return limit_factor( f );
}

std::pair<character_stat, float> player_activity::calc_single_stat( const Character &who,
        const activity_req<character_stat> &stat ) const
{
    int who_stat = 0;
    switch( stat.req ) {
        case character_stat::STRENGTH:
            who_stat = who.get_str();
            break;
        case character_stat::DEXTERITY:
            who_stat = who.get_dex();
            break;
        case character_stat::INTELLIGENCE:
            who_stat = who.get_int();
            break;
        case character_stat::PERCEPTION:
            who_stat = who.get_per();
            break;
        default:
            return std::pair<character_stat, float>( character_stat::DUMMY_STAT, 1.0f );
    }
    float f = 1.0f + refine_factor( stat.mod * ( who_stat - stat.threshold ) );
    return std::pair<character_stat, float>( stat.req, f );
}


std::vector<std::pair<character_stat, float>> player_activity::calc_stats_factors(
            const Character &who ) const
{
    auto f = actor
             ? actor->calc_stats_factors( who, type->stats )
             : std::vector<std::pair<character_stat, float>> {};

    if( !f.empty() ) {
        return f;
    }

    for( auto &stat : type->stats ) {
        f.emplace_back( calc_single_stat( who, stat ) );
    }
    return f;
}

float player_activity::get_best_qual_mod( const activity_req<quality_id> &q,
        const inventory &inv )
{
    int q_level = 0;
    inv.visit_items( [&q, &q_level]( const item * itm ) {
        int new_q = itm->get_quality( q.req );
        if( new_q > q_level ) {
            q_level = new_q;
        }
        return VisitResponse::NEXT;
    } );
    q_level = q_level - q.threshold;

    if( q.req == qual_CUT_FINE ) {
        float cut_fine_f =  2.0f * std::pow( q_level, 3 )
                            - 10.0f * std::pow( q_level, 2 )
                            + 32.0f * q_level + q.mod ;
        return cut_fine_f;
    }

    if( q_level == 0 ) {
        return 0.0f;
    }

    if( q.req == qual_BUTCHER ) {
        return q_level * q.mod;
    }

    return  q.mod * q_level / ( q_level + 1.75f );
}

float player_activity::calc_tools_factor( Character &who,
        const std::vector<activity_req<quality_id>> &quality_reqs ) const
{
    auto &inv = who.crafting_inventory();
    float ac_f = actor ? actor->calc_tools_factor( quality_reqs, inv ) : -1.0f;
    //Any factor above 0 is valid, else - use default calc
    if( ac_f > 0 ) {
        return ac_f;
    }

    float f = 1;
    std::vector<float> factors;
    for( const auto &q : quality_reqs ) {
        factors.push_back( get_best_qual_mod( q, inv ) );
    }
    std::sort( factors.begin(), factors.end(), std::greater<>() );

    int denom = 0;
    for( const auto &factor : factors ) {
        f += refine_factor( factor, ++denom * 0.8f ) ;
    }

    return limit_factor( f );
}

float player_activity::calc_morale_factor( int morale ) const
{
    float ac_morale = actor ? actor->calc_morale_factor( morale ) : -1.0f;
    //Any morale mod above 0 is valid, else - use default morale calc
    if( ac_morale > 0 ) {
        return ac_morale;
    }

    //1% per 4 extra morale
    if( morale > 20 ) {
        return 0.95f + morale / 400.0f;
    }
    // 1% per 1 insuff morale
    else if( morale < -20 ) {
        return 1.20f + morale / 100.0f;
    }
    return 1.0f;
}

static std::string craft_progress_message( const avatar &u, const player_activity &act )
{
    const item *craft = &*act.targets.front();
    if( craft == nullptr ) {
        // Should never happen (?)
        return string_format( _( "%s…" ), act.get_verb().translated() );
    }

    // Horrid copypaste warning! TODO: Functions
    const recipe &rec = craft->get_making();
    const tripoint bench_pos = act.coords.front();
    // Ugly
    bench_type bench_t = bench_type( act.values[1] );

    const bench_location bench{ bench_t, bench_pos };

    const float light_mult = lighting_crafting_speed_multiplier( u, rec );
    const float bench_mult = workbench_crafting_speed_multiplier( *craft, bench );
    const float morale_mult = morale_crafting_speed_multiplier( u, rec );
    const int assistants = u.available_assistant_count( craft->get_making() );
    const float base_total_moves = std::max( 1, rec.batch_time( craft->charges, 1.0f, 0 ) );
    const float assist_total_moves = std::max( 1, rec.batch_time( craft->charges, 1.0f, assistants ) );
    const float assist_mult = base_total_moves / assist_total_moves;
    const float speed_mult = u.get_speed() / 100.0f;
    const float total_mult = light_mult * bench_mult * morale_mult * assist_mult * speed_mult;

    const double remaining_percentage = 1.0 - craft->item_counter / 10'000'000.0;
    int remaining_turns = remaining_percentage * base_total_moves / 100 / std::max( 0.01f, total_mult );
    std::string time_desc = string_format( _( "Time left: %s" ),
                                           to_string( time_duration::from_turns( remaining_turns ) ) );

    const std::array<std::pair<float, std::string>, 6> mults_with_data = { {
            { total_mult, _( "Total" ) },
            { speed_mult, _( "Speed" ) },
            { light_mult, _( "Light" ) },
            { bench_mult, _( "Workbench" ) },
            { morale_mult, _( "Morale" ) },
            { assist_mult, _( "Assistants" ) },
        }
    };
    std::string mults_desc = _( "Crafting speed multipliers:\n" );
    // Hack to make sure total always shows
    bool first = true;
    for( const std::pair<float, std::string> &p : mults_with_data ) {
        int percent = static_cast<int>( p.first * 100 );
        if( first || percent != 100 ) {
            nc_color col = percent > 100 ? c_green : c_red;
            std::string colorized = colorize( std::to_string( percent ) + '%', col );
            mults_desc += string_format( _( "%s: %s\n" ), p.second, colorized );
        }
        first = false;
    }

    return string_format( _( "%s: %s\n\n%s\n\n%s" ), act.get_verb().translated(), craft->tname(),
                          time_desc,
                          mults_desc );
}

static std::string format_spd( float level, std::string name, int indent = 0,
                               bool force_show = false )
{
    if( !force_show && level == 1.0f ) {
        return "";
    }
    int percent = static_cast<int>( std::roundf( level * 100.0f ) );
    nc_color col = percent == 100
                   ? c_white
                   : percent > 100 ? c_green : c_red;
    std::string spaces = "";
    std::string colorized = colorize( std::to_string( percent ) + '%', col );
    return string_format( _( " %s- %s: %s\n" ), spaces.insert( 0, indent, ' ' ), name, colorized );
}

std::optional<std::string> player_activity::get_progress_message( const avatar &u ) const
{
    if( !type || get_verb().empty() ) {
        return std::optional<std::string>();
    }
    if( !type->special() && is_verbose_tooltip() ) {

        /*
        * Progress block
        */
        std::string target = "";
        std::string progress_desc = "Progress: ";

        /*
         * TODO progress for targets
         * proper use of activity_actor::targets for all activities
         * must be implementated for proper work of multiple targets
         */
        if( actor ) {
            if( actor->progress.empty() ) {
                target = string_format( ": %s", actor->progress.front().target_name );
                progress_desc = "";
                //shouldn't ever happend actually
                debugmsg( "Progress counter is empty, despite activity using actor, total tasks %s",
                          actor->progress.get_total_tasks() );
            } else {
                target = string_format( ": %s", actor->progress.front().target_name );
                if( actor->progress.get_total_tasks() > 1 ) {
                    progress_desc += "\n - Total: ";
                    progress_desc += string_format( "%.1f%%\n",
                                                    ( 1.0f - float( actor->progress.get_moves_left() ) / actor->progress.get_moves_total() ) * 100.0f );
                    progress_desc += string_format( _( "  - Processing %s out of %s\n" ), actor->progress.get_index(),
                                                    actor->progress.get_total_tasks() );
                    progress_desc += string_format( _( "  - Estimated time: %s\n" ),
                                                    to_string( time_duration::from_turns( actor->progress.get_moves_left() / speed.total_moves() ) ) );
                    progress_desc += " - Current: ";
                }
                progress_desc += string_format( "%.1f%%\n",
                                                ( 1.0f - float( actor->progress.front().moves_left ) / actor->progress.front().moves_total ) *
                                                100.0f );
                if( actor->progress.get_total_tasks() > 1 ) {
                    progress_desc += "  - ";
                }
                progress_desc += string_format( _( "Time left: %s\n" ),
                                                to_string( time_duration::from_turns( actor->progress.front().moves_left /
                                                        speed.total_moves() ) ) );
            }
        } else {
            if( !targets.empty() && targets.front().is_accessible() ) {
                target = string_format( ": %s", targets.front()->tname( targets.front()->count() ) );
            }
            if( moves_total > 0 ) {
                progress_desc += string_format( "%.1f%%\n",
                                                ( 1.0f - float( moves_left ) / moves_total ) * 100.0f );
            }
            if( moves_left > 0 ) {
                progress_desc += string_format( _( "Time left: %s\n" ),
                                                to_string( time_duration::from_turns( moves_left / speed.total_moves() ) ) );
            }
            if( moves_total <= 0 && moves_left <= 0 ) {
                progress_desc = "";
            }
        }

        /*
        * Speed block
        */
        std::string mults_desc = string_format( _( "Speed multipliers:\n" ) );
        mults_desc += format_spd( speed.total(), "Total", 0, true );
        mults_desc += format_spd( speed.assist, "Assistants", 1 );
        mults_desc += format_spd( speed.light, "Light", 1 );
        mults_desc += format_spd( speed.morale, "Morale", 1 );
        mults_desc += format_spd( speed.player_speed, "Speed", 1 );
        mults_desc += format_spd( speed.skills, "Skills", 1 );
        mults_desc += format_spd( speed.tools, "Tools", 1 );
        mults_desc += format_spd( speed.bench, "Workbench", 1 );
        mults_desc += format_spd( speed.stats_total(), "Stats", 1 );

        for( auto &stat : speed.stats ) {
            mults_desc += format_spd( stat.second, get_stat_name( stat.first ), 2 );
        }



        return string_format( _( "%s%s\n%s\n%s" ), get_verb().translated(),
                              target,
                              progress_desc,
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

    if( type == ACT_ADV_INVENTORY ||
        type == ACT_AIM ||
        type == ACT_ARMOR_LAYERS ||
        type == ACT_ATM ||
        type == ACT_CONSUME_DRINK_MENU ||
        type == ACT_CONSUME_FOOD_MENU ||
        type == ACT_CONSUME_MEDS_MENU ||
        type == ACT_EAT_MENU ) {
        return std::nullopt;
    }

    std::string extra_info;
    if( type == ACT_CRAFT ) {
        return craft_progress_message( u, *this );
    } else if( type == ACT_READ ) {
        if( const item *book = &*targets.front() ) {
            if( const auto &reading = book->type->book ) {
                const skill_id &skill = reading->skill;
                if( skill && u.get_skill_level( skill ) < reading->level &&
                    u.get_skill_level_object( skill ).can_train() ) {
                    const SkillLevel &skill_level = u.get_skill_level_object( skill );
                    //~ skill_name current_skill_level -> next_skill_level (% to next level)
                    extra_info = string_format( pgettext( "reading progress", "%s %d -> %d (%d%%)" ),
                                                skill->name(),
                                                skill_level.level(),
                                                skill_level.level() + 1,
                                                skill_level.exercise() );
                }
            }
        }
    } else if( moves_total > 0 ) {
        if( type == ACT_BURROW ||
            type == ACT_HACKSAW ||
            type == ACT_JACKHAMMER ||
            type == ACT_PICKAXE ||
            type == ACT_VEHICLE ||
            type == ACT_FILL_PIT ||
            type == ACT_DIG ||
            type == ACT_DIG_CHANNEL ||
            type == ACT_CHOP_TREE ||
            type == ACT_CHOP_LOGS ||
            type == ACT_CHOP_PLANKS
          ) {
            const int percentage = ( ( moves_total - moves_left ) * 100 ) / moves_total;

            extra_info = string_format( "%d%%", percentage );
        }
    }

    return extra_info.empty()
           ? string_format( _( "%s…" ), get_verb().translated() )
           : string_format( _( "%s: %s" ), get_verb().translated(), extra_info );
}

void player_activity::find_best_bench( const tripoint &pos )
{
    bench_loc best_bench = bench_loc(
                               workbench_info_wrapper(
                                   * string_id<furn_t>( "f_ground_crafting_spot" ).obj().workbench.get() ),
                               bench_type::ground,
                               pos );
    std::vector<tripoint> reachable( PICKUP_RANGE * PICKUP_RANGE );
    get_map().reachable_flood_steps( reachable, pos, PICKUP_RANGE, 1, 100 );
    for( const tripoint &adj : reachable ) {
        if( auto wb = get_map().furn( adj ).obj().workbench ) {
            if( wb->multiplier > best_bench.wb_info.multiplier ) {
                best_bench = bench_loc( workbench_info_wrapper( *wb.get() ), bench_type::furniture, adj );
            }
        }

        if( const std::optional<vpart_reference> vp = get_map().veh_at(
                    adj ).part_with_feature( "WORKBENCH", true ) ) {
            if( const std::optional<vpslot_workbench> &wb_info = vp->part().info().get_workbench_info() ) {
                if( wb_info->multiplier > best_bench.wb_info.multiplier ) {
                    best_bench = bench_loc( workbench_info_wrapper( wb_info.value() ), bench_type::furniture, adj );
                }
            } else {
                debugmsg( "part '%' with WORKBENCH flag has no workbench info", vp->part().name() );
            }
        }
    }

    bench = best_bench;
}

void player_activity::start_or_resume( Character &who, bool resuming )
{
    if( actor && !resuming ) {
        actor->start( *this, who );
    }
    init_all_moves( who );
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

    if( p.is_npc() ) {
        if( p.restore_outbounds_activity() ) {
            // npc might be operating at the edge of the reality bubble.
            // or just now reloaded back into it, and their activity target might
            // be still unloaded, can cause infinite loops.
            set_to_null();
            p.drop_invalid_inventory();
            return;
        }
        if( p.activity->id() == ACT_ASSIST ) {
            p.moves = 0;
            return;
        }
    }

    /*
     * Moves block
     * This might finish the activity (set it to null)
     * Leave as is till full migration to actors for "NEITHER"
    */
    if( !type->special() ) {
        if( type->complex_moves() ) {
            if( calendar::once_every( 1_minutes ) ) {
                calc_moves( p );
            }

            int moves_total = speed.total_moves();

            //fancy new system
            if( actor ) {
                if( actor->progress.get_moves_left() >= moves_total ) {
                    actor->progress.mod_moves_left( - moves_total );
                    p.moves = 0;
                } else {
                    p.moves -= std::round( ( moves_total - actor->progress.get_moves_left() ) * 100.0f / moves_total );
                    actor->progress.mod_moves_left( -actor->progress.get_moves_left() );
                }
            }
            //old one
            else {
                if( moves_left >= moves_total ) {
                    moves_left -= moves_total;
                    p.moves = 0;
                } else {
                    p.moves -= std::round( ( moves_total - moves_left ) * 100.0f / moves_total );
                    moves_left = 0;
                }
            }
        } else {
            //fancy new system
            if( actor ) {
                if( actor->progress.get_moves_left() >= 100 ) {
                    actor->progress.mod_moves_left( - 100 );
                    p.moves = 0;
                } else {
                    p.moves -= actor->progress.get_moves_left();
                    actor->progress.mod_moves_left( -actor->progress.get_moves_left() );
                }
            }
            //old one
            else {
                if( moves_left >= 100 ) {
                    moves_left -= 100;
                    p.moves = 0;
                } else {
                    p.moves -= moves_left;
                    moves_left = 0;
                }
            }
        }
    }

    if( actor ) {
        actor->do_turn( *this, p );
    } else {
        // Use the legacy turn function
        type->call_do_turn( this, &p );
    }


    /*
    * Stamina block
    */
    // Activities should never excessively drain stamina.
    // adjusted stamina because
    // autotravel doesn't reduce stamina after do_turn()
    // it just sets a destination, clears the activity, then moves afterwards
    // so set stamina -1 if that is the case
    // to simulate that the next step will surely use up some stamina anyway
    // this is to ensure that resting will occur when traveling overburdened

    int previous_stamina = p.get_stamina();
    const bool travel_activity = id() == ACT_TRAVELLING;
    const int adjusted_stamina = travel_activity ? p.get_stamina() - 1 : p.get_stamina();
    if( adjusted_stamina < previous_stamina && p.get_stamina() < p.get_stamina_max() / 3 ) {
        if( one_in( 50 ) ) {
            p.add_msg_if_player( _( "You pause for a moment to catch your breath." ) );
        }
        auto_resume = true;
        std::unique_ptr<player_activity> new_act = std::make_unique<player_activity>
                ( ACT_WAIT_STAMINA, to_moves<int>( 1_minutes ) );
        new_act->values.push_back( 200 + p.get_stamina_max() / 3 );
        p.assign_activity( std::move( new_act ) );
        return;
    }

    if( *this && type->rooted() ) {
        p.rooted();
        character_funcs::do_pause( p );
    }

    if( *this && complete() ) {
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
        for( Character *npc : assistants() ) {
            npc->cancel_activity();
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
    for( Character *npc : assistants() ) {
        npc->cancel_activity();
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

    if( id() == ACT_CLEAR_RUBBLE ) {
        if( other.coords.empty() || other.coords[0] != coords[0] ) {
            return false;
        }
    } else if( id() == ACT_READ ) {
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
    } else if( id() == ACT_VEHICLE ) {
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
