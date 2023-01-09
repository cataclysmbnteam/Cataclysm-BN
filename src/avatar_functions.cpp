#include "avatar_functions.h"

#include "avatar.h"
#include "character_functions.h"
#include "fault.h"
#include "field_type.h"
#include "map.h"
#include "mapdata.h"
#include "messages.h"
#include "output.h"
#include "skill.h"
#include "trap.h"
#include "vpart_position.h"
#include "vehicle.h"
#include "veh_type.h"

static const trait_id trait_CHLOROMORPH( "CHLOROMORPH" );
static const trait_id trait_DEBUG_HS( "DEBUG_HS" );
static const trait_id trait_M_SKIN3( "M_SKIN3" );
static const trait_id trait_SHELL2( "SHELL2" );
static const trait_id trait_THRESH_SPIDER( "THRESH_SPIDER" );
static const trait_id trait_WATERSLEEP( "WATERSLEEP" );
static const trait_id trait_WEB_SPINNER( "WEB_SPINNER" );
static const trait_id trait_WEB_WALKER( "WEB_WALKER" );
static const trait_id trait_WEB_WEAVER( "WEB_WEAVER" );

static const bionic_id bio_soporific( "bio_soporific" );

namespace avatar_funcs
{

void try_to_sleep( avatar &you, const time_duration &dur )
{
    map &here = get_map();
    const optional_vpart_position vp = here.veh_at( you.pos() );
    const trap &trap_at_pos = here.tr_at( you.pos() );
    const ter_id ter_at_pos = here.ter( you.pos() );
    const furn_id furn_at_pos = here.furn( you.pos() );
    bool plantsleep = false;
    bool fungaloid_cosplay = false;
    bool websleep = false;
    bool webforce = false;
    bool websleeping = false;
    bool in_shell = false;
    bool watersleep = false;
    if( you.has_trait( trait_CHLOROMORPH ) ) {
        plantsleep = true;
        if( ( ter_at_pos == t_dirt || ter_at_pos == t_pit ||
              ter_at_pos == t_dirtmound || ter_at_pos == t_pit_shallow ||
              ter_at_pos == t_grass ) && !vp &&
            furn_at_pos == f_null ) {
            you.add_msg_if_player( m_good, _( "You relax as your roots embrace the soil." ) );
        } else if( vp ) {
            you.add_msg_if_player( m_bad, _( "It's impossible to sleep in this wheeled pot!" ) );
        } else if( furn_at_pos != f_null ) {
            you.add_msg_if_player( m_bad,
                                   _( "The humans' furniture blocks your roots.  You can't get comfortable." ) );
        } else { // Floor problems
            you.add_msg_if_player( m_bad, _( "Your roots scrabble ineffectively at the unyielding surface." ) );
        }
    } else if( you.has_trait( trait_M_SKIN3 ) ) {
        fungaloid_cosplay = true;
        if( here.has_flag_ter_or_furn( "FUNGUS", you.pos() ) ) {
            you.add_msg_if_player( m_good,
                                   _( "Our fibers meld with the ground beneath us.  The gills on our neck begin to seed the air with spores as our awareness fades." ) );
        }
    }
    if( you.has_trait( trait_WEB_WALKER ) ) {
        websleep = true;
    }
    // Not sure how one would get Arachnid w/o web-making, but Just In Case
    if( you.has_trait( trait_THRESH_SPIDER ) && ( you.has_trait( trait_WEB_SPINNER ) ||
            ( you.has_trait( trait_WEB_WEAVER ) ) ) ) {
        webforce = true;
    }
    if( websleep || webforce ) {
        int web = here.get_field_intensity( you.pos(), fd_web );
        if( !webforce ) {
            // At this point, it's kinda weird, but surprisingly comfy...
            if( web >= 3 ) {
                you.add_msg_if_player( m_good,
                                       _( "These thick webs support your weight, and are strangely comfortable�" ) );
                websleeping = true;
            } else if( web > 0 ) {
                you.add_msg_if_player( m_info,
                                       _( "You try to sleep, but the webs get in the way.  You brush them aside." ) );
                here.remove_field( you.pos(), fd_web );
            }
        } else {
            // Here, you're just not comfortable outside a nice thick web.
            if( web >= 3 ) {
                you.add_msg_if_player( m_good, _( "You relax into your web." ) );
                websleeping = true;
            } else {
                you.add_msg_if_player( m_bad,
                                       _( "You try to sleep, but you feel exposed and your spinnerets keep twitching." ) );
                you.add_msg_if_player( m_info, _( "Maybe a nice thick web would help you sleep." ) );
            }
        }
    }
    if( you.has_active_mutation( trait_SHELL2 ) ) {
        // Your shell's interior is a comfortable place to sleep.
        in_shell = true;
    }
    if( you.has_trait( trait_WATERSLEEP ) ) {
        if( you.is_underwater() ) {
            you.add_msg_if_player( m_good,
                                   _( "You lay beneath the waves' embrace, gazing up through the water's surface�" ) );
            watersleep = true;
        } else if( here.has_flag_ter( "SWIMMABLE", you.pos() ) ) {
            you.add_msg_if_player( m_good, _( "You settle into the water and begin to drowse�" ) );
            watersleep = true;
        }
    }
    constexpr int confort_level_neutral = static_cast<int>( character_funcs::comfort_level::neutral );
    if( !plantsleep && ( furn_at_pos.obj().comfort > confort_level_neutral ||
                         ter_at_pos == t_improvised_shelter ||
                         trap_at_pos.comfort > confort_level_neutral ||
                         in_shell || websleeping || watersleep ||
                         vp.part_with_feature( "SEAT", true ) ||
                         vp.part_with_feature( "BED", true ) ) ) {
        you.add_msg_if_player( m_good, _( "This is a comfortable place to sleep." ) );
    } else if( !plantsleep && !fungaloid_cosplay && !watersleep ) {
        if( !vp && ter_at_pos != t_floor ) {
            you.add_msg_if_player( ter_at_pos.obj().movecost <= 2 ?
                                   _( "It's a little hard to get to sleep on this %s." ) :
                                   _( "It's hard to get to sleep on this %s." ),
                                   ter_at_pos.obj().name() );
        } else if( vp ) {
            if( vp->part_with_feature( VPFLAG_AISLE, true ) ) {
                you.add_msg_if_player(
                    //~ %1$s: vehicle name, %2$s: vehicle part name
                    _( "It's a little hard to get to sleep on this %2$s in %1$s." ),
                    vp->vehicle().disp_name(),
                    vp->part_with_feature( VPFLAG_AISLE, true )->part().name( false ) );
            } else {
                you.add_msg_if_player(
                    //~ %1$s: vehicle name
                    _( "It's hard to get to sleep in %1$s." ),
                    vp->vehicle().disp_name() );
            }
        }
    }
    you.add_msg_if_player( _( "You start trying to fall asleep." ) );
    if( you.has_active_bionic( bio_soporific ) ) {
        you.bio_soporific_powered_at_last_sleep_check = you.has_power();
        if( you.bio_soporific_powered_at_last_sleep_check ) {
            // The actual bonus is applied in sleep_spot( p ).
            you.add_msg_if_player( m_good, _( "Your soporific inducer starts working its magic." ) );
        } else {
            you.add_msg_if_player( m_bad, _( "Your soporific inducer doesn't have enough power to operate." ) );
        }
    }
    you.assign_activity( activity_id( "ACT_TRY_SLEEP" ), to_moves<int>( dur ) );
}

void mend_item( avatar &you, item_location &&obj, bool interactive )
{
    if( you.has_trait( trait_DEBUG_HS ) ) {
        uilist menu;
        menu.text = _( "Toggle which fault?" );
        std::vector<std::pair<fault_id, bool>> opts;
        for( const auto &f : obj->faults_potential() ) {
            opts.emplace_back( f, !!obj->faults.count( f ) );
            menu.addentry( -1, true, -1, string_format(
                               opts.back().second ? pgettext( "fault", "Mend: %s" ) : pgettext( "fault", "Set: %s" ),
                               f.obj().name() ) );
        }
        if( opts.empty() ) {
            add_msg( m_info, _( "The %s doesn't have any faults to toggle." ), obj->tname() );
            return;
        }
        menu.query();
        if( menu.ret >= 0 ) {
            if( opts[ menu.ret ].second ) {
                obj->faults.erase( opts[ menu.ret ].first );
            } else {
                obj->faults.insert( opts[ menu.ret ].first );
            }
        }
        return;
    }

    auto inv = you.crafting_inventory();

    struct mending_option {
        fault_id fault;
        std::reference_wrapper<const mending_method> method;
        bool doable;
    };

    std::vector<mending_option> mending_options;
    for( const fault_id &f : obj->faults ) {
        for( const auto &m : f->mending_methods() ) {
            mending_option opt { f, m.second, true };
            for( const auto &sk : m.second.skills ) {
                if( you.get_skill_level( sk.first ) < sk.second ) {
                    opt.doable = false;
                    break;
                }
            }
            opt.doable = opt.doable &&
                         m.second.requirements->can_make_with_inventory( inv, is_crafting_component );
            mending_options.emplace_back( opt );
        }
    }

    if( mending_options.empty() ) {
        if( interactive ) {
            add_msg( m_info, _( "The %s doesn't have any faults to mend." ), obj->tname() );
            if( obj->damage() > 0 ) {
                const std::set<itype_id> &rep = obj->repaired_with();
                if( !rep.empty() ) {
                    const std::string repair_options =
                    enumerate_as_string( rep.begin(), rep.end(), []( const itype_id & e ) {
                        return item::nname( e );
                    }, enumeration_conjunction::or_ );

                    add_msg( m_info, _( "It is damaged, and could be repaired with %s.  "
                                        "%s to use one of those items." ),
                             repair_options, press_x( ACTION_USE ) );
                }
            }
        }
        return;
    }

    int sel = 0;
    if( interactive ) {
        uilist menu;
        menu.text = _( "Mend which fault?" );
        menu.desc_enabled = true;
        menu.desc_lines_hint = 0; // Let uilist handle description height

        constexpr int fold_width = 80;

        for( const mending_option &opt : mending_options ) {
            const mending_method &method = opt.method;
            const nc_color col = opt.doable ? c_white : c_light_gray;

            auto reqs = method.requirements.obj();
            auto tools = reqs.get_folded_tools_list( fold_width, col, inv );
            auto comps = reqs.get_folded_components_list( fold_width, col, inv, is_crafting_component );

            std::string descr;
            if( method.turns_into ) {
                descr += string_format( _( "Turns into: <color_cyan>%s</color>\n" ),
                                        method.turns_into->obj().name() );
            }
            if( method.also_mends ) {
                descr += string_format( _( "Also mends: <color_cyan>%s</color>\n" ),
                                        method.also_mends->obj().name() );
            }
            descr += string_format( _( "Time required: <color_cyan>%s</color>\n" ),
                                    to_string_approx( method.time ) );
            if( method.skills.empty() ) {
                descr += string_format( _( "Skills: <color_cyan>none</color>\n" ) );
            } else {
                descr += string_format( _( "Skills: %s\n" ),
                                        enumerate_as_string( method.skills.begin(), method.skills.end(),
                [&]( const std::pair<skill_id, int> &sk ) -> std::string {
                    if( you.get_skill_level( sk.first ) >= sk.second )
                    {
                        return string_format( pgettext( "skill requirement",
                                                        //~ %1$s: skill name, %2$s: current skill level, %3$s: required skill level
                                                        "<color_cyan>%1$s</color> <color_green>(%2$d/%3$d)</color>" ),
                                              sk.first->name(), you.get_skill_level( sk.first ), sk.second );
                    } else
                    {
                        return string_format( pgettext( "skill requirement",
                                                        //~ %1$s: skill name, %2$s: current skill level, %3$s: required skill level
                                                        "<color_cyan>%1$s</color> <color_yellow>(%2$d/%3$d)</color>" ),
                                              sk.first->name(), you.get_skill_level( sk.first ), sk.second );
                    }
                } ) );
            }

            for( const std::string &line : tools ) {
                descr += line + "\n";
            }
            for( const std::string &line : comps ) {
                descr += line + "\n";
            }

            const std::string desc = method.description + "\n\n" + colorize( descr, col );
            menu.addentry_desc( -1, true, -1, method.name.translated(), desc );
        }
        menu.query();
        if( menu.ret < 0 ) {
            add_msg( _( "Never mind." ) );
            return;
        }
        sel = menu.ret;
    }

    if( sel >= 0 ) {
        const mending_option &opt = mending_options[sel];
        if( !opt.doable ) {
            if( interactive ) {
                add_msg( m_info, _( "You are currently unable to mend the %s this way." ), obj->tname() );
            }
            return;
        }

        const mending_method &method = opt.method;
        you.assign_activity( activity_id( "ACT_MEND_ITEM" ), to_moves<int>( method.time ) );
        you.activity.name = opt.fault.str();
        you.activity.str_values.emplace_back( method.id );
        you.activity.targets.push_back( std::move( obj ) );
    }
}

} // namespace avatar_funcs
