#include "avatar_functions.h"

#include "activity_handlers.h"
#include "avatar.h"
#include "character_functions.h"
#include "fault.h"
#include "field_type.h"
#include "game.h"
#include "game_inventory.h"
#include "handle_liquid.h"
#include "itype.h"
#include "map.h"
#include "mapdata.h"
#include "messages.h"
#include "npc.h"
#include "options.h"
#include "output.h"
#include "skill.h"
#include "trap.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vpart_position.h"

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

static const itype_id itype_brass_catcher( "brass_catcher" );
static const itype_id itype_large_repairkit( "large_repairkit" );
static const itype_id itype_plut_cell( "plut_cell" );
static const itype_id itype_small_repairkit( "small_repairkit" );

static const skill_id skill_weapon( "weapon" );

static const std::string flag_SPLINT( "SPLINT" );

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


static bool has_mod( const item &gun, const item &mod )
{
    for( const item *toolmod : gun.gunmods() ) {
        if( &mod == toolmod ) {
            return true;
        }
    }
    return false;
}

void gunmod_add( avatar &you, item &gun, item &mod )
{
    if( !gun.is_gunmod_compatible( mod ).success() ) {
        debugmsg( "Tried to add incompatible gunmod" );
        return;
    }

    if( !you.has_item( gun ) && !you.has_item( mod ) ) {
        debugmsg( "Tried gunmod installation but mod/gun not in player possession" );
        return;
    }

    // first check at least the minimum requirements are met
    if( !you.has_trait( trait_DEBUG_HS ) && !you.can_use( mod, gun ) ) {
        return;
    }

    // any (optional) tool charges that are used during installation
    auto odds = gunmod_installation_odds( you, gun, mod );
    int roll = odds.first;
    int risk = odds.second;

    std::string tool;
    int qty = 0;
    bool requery = false;

    item modded = gun;
    modded.put_in( mod );
    bool no_magazines = modded.common_ammo_default().is_null();

    std::string query_msg = mod.is_irremovable()
                            ? _( "<color_yellow>Permanently</color> install your %1$s in your %2$s?" )
                            : _( "Attach your %1$s to your %2$s?" );
    if( no_magazines ) {
        query_msg += "\n";
        query_msg += colorize(
                         _( "Warning: after installing this mod, a magazine adapter mod will be required to load it!" ),
                         c_red );
    }

    uilist prompt;
    prompt.text = string_format( query_msg,
                                 colorize( mod.tname(), mod.color_in_inventory() ),
                                 colorize( gun.tname(), gun.color_in_inventory() ) );

    std::vector<std::function<void()>> actions;

    if( roll < 100 ) {
        prompt.addentry( -1, true, 'w',
                         string_format( _( "Try without tools (%i%%) risking damage (%i%%)" ), roll, risk ) );
        actions.emplace_back( [&] {} );

        prompt.addentry( -1, you.has_charges( itype_small_repairkit, 100 ), 'f',
                         string_format( _( "Use 100 charges of firearm repair kit (%i%%)" ), std::min( roll * 2, 100 ) ) );

        actions.emplace_back( [&] {
            tool = "small_repairkit";
            qty = 100;
            roll *= 2; // firearm repair kit improves success...
            risk /= 2; // ...and reduces the risk of damage upon failure
        } );

        prompt.addentry( -1, you.has_charges( itype_large_repairkit, 25 ), 'g',
                         string_format( _( "Use 25 charges of gunsmith repair kit (%i%%)" ), std::min( roll * 3, 100 ) ) );

        actions.emplace_back( [&] {
            tool = "large_repairkit";
            qty = 25;
            roll *= 3; // gunsmith repair kit improves success markedly...
            risk = 0;  // ...and entirely prevents damage upon failure
        } );
    } else {
        prompt.addentry( -1, true, 'w', _( "Install without tools" ) );
        actions.emplace_back( [&] {} );
    }

    prompt.addentry( -1, true, 'c', _( "Compare before/after installation" ) );
    actions.emplace_back( [&] {
        requery = true;
        game_menus::inv::compare( gun, modded );
    } );

    do {
        requery = false;
        prompt.query();
        if( prompt.ret < 0 ) {
            you.add_msg_if_player( _( "Never mind." ) );
            return; // player canceled installation
        }
        actions[ prompt.ret ]();
    } while( requery );

    const int moves = !you.has_trait( trait_DEBUG_HS ) ? mod.type->gunmod->install_time : 0;

    you.assign_activity( activity_id( "ACT_GUNMOD_ADD" ), moves, -1, 0, tool );
    you.activity.targets.push_back( item_location( you, &gun ) );
    you.activity.targets.push_back( item_location( you, &mod ) );
    you.activity.values.push_back( 0 ); // dummy value
    you.activity.values.push_back( roll ); // chance of success (%)
    you.activity.values.push_back( risk ); // chance of damage (%)
    you.activity.values.push_back( qty ); // tool charges
}

bool gunmod_remove( avatar &you, item &gun, item &mod )
{
    if( !has_mod( gun, mod ) ) {
        debugmsg( "Cannot remove non-existent gunmod" );
        return false;
    }

    item_location loc = item_location( you, &mod );
    if( mod.ammo_remaining() && !avatar_funcs::unload_item( you, loc ) ) {
        return false;
    }

    gun.gun_set_mode( gun_mode_id( "DEFAULT" ) );
    //TODO: add activity for removing gunmods

    if( mod.typeId() == itype_brass_catcher ) {
        gun.casings_handle( [&]( item & e ) {
            return you.i_add_or_drop( e );
        } );
    }

    const itype *modtype = mod.type;

    you.i_add_or_drop( mod );
    gun.remove_item( mod );

    //If the removed gunmod added mod locations, check to see if any mods are in invalid locations
    if( !modtype->gunmod->add_mod.empty() ) {
        std::map<gunmod_location, int> mod_locations = gun.get_mod_locations();
        for( const auto &slot : mod_locations ) {
            int free_slots = gun.get_free_mod_locations( slot.first );

            for( auto the_mod : gun.gunmods() ) {
                if( the_mod->type->gunmod->location == slot.first && free_slots < 0 ) {
                    gunmod_remove( you, gun, *the_mod );
                    free_slots++;
                } else if( mod_locations.find( the_mod->type->gunmod->location ) ==
                           mod_locations.end() ) {
                    gunmod_remove( you, gun, *the_mod );
                }
            }
        }
    }

    you.add_msg_if_player(
        //~ %1$s - gunmod, %2$s - gun.
        _( "You remove your %1$s from your %2$s." ),
        modtype->nname( 1 ), gun.tname()
    );

    return true;
}

std::pair<int, int> gunmod_installation_odds( const avatar &you, const item &gun,
        const item &mod )
{
    // Mods with INSTALL_DIFFICULT have a chance to fail, potentially damaging the gun
    if( !mod.has_flag( "INSTALL_DIFFICULT" ) || you.has_trait( trait_DEBUG_HS ) ) {
        return std::make_pair( 100, 0 );
    }

    int roll = 100; // chance of success (%)
    int risk = 0;   // chance of failure (%)
    int chances = 1; // start with 1 in 6 (~17% chance)

    for( const auto &e : mod.type->min_skills ) {
        // gain an additional chance for every level above the minimum requirement
        skill_id sk = e.first == skill_weapon ? gun.gun_skill() : e.first;
        chances += std::max( you.get_skill_level( sk ) - e.second, 0 );
    }
    // cap success from skill alone to 1 in 5 (~83% chance)
    roll = std::min( static_cast<double>( chances ), 5.0 ) / 6.0 * 100;
    // focus is either a penalty or bonus of at most +/-10%
    roll += ( std::max( std::min( you.focus_pool, 140 ), 60 ) - 100 ) / 4;
    // dexterity and intelligence give +/-2% for each point above or below 12
    roll += ( you.get_dex() - 12 ) * 2;
    roll += ( you.get_int() - 12 ) * 2;
    // each level of damage to the base gun reduces success by 10%
    roll -= std::max( gun.damage_level( 4 ), 0 ) * 10;
    roll = std::min( std::max( roll, 0 ), 100 );

    // risk of causing damage on failure increases with less durable guns
    risk = ( 100 - roll ) * ( ( 10.0 - std::min( gun.type->gun->durability, 9 ) ) / 10.0 );

    return std::make_pair( roll, risk );
}

void toolmod_add( avatar &you, item_location tool, item_location mod )
{
    if( !tool && !mod ) {
        debugmsg( "Tried toolmod installation but mod/tool not available" );
        return;
    }
    // first check at least the minimum requirements are met
    if( !you.has_trait( trait_DEBUG_HS ) && !you.can_use( *mod, *tool ) ) {
        return;
    }

    if( !query_yn( _( "Permanently install your %1$s in your %2$s?" ),
                   colorize( mod->tname(), mod->color_in_inventory() ),
                   colorize( tool->tname(), tool->color_in_inventory() ) ) ) {
        you.add_msg_if_player( _( "Never mind." ) );
        return; // player canceled installation
    }

    you.assign_activity( activity_id( "ACT_TOOLMOD_ADD" ), 1, -1 );
    you.activity.targets.emplace_back( std::move( tool ) );
    you.activity.targets.emplace_back( std::move( mod ) );
}

static bool is_pet_food( const item &itm )
{
    return itm.type->can_use( "DOGFOOD" ) ||
           itm.type->can_use( "CATFOOD" ) ||
           itm.type->can_use( "BIRDFOOD" ) ||
           itm.type->can_use( "CATTLEFODDER" );
}

void use_item( avatar &you, item_location loc )
{
    item &used = *loc.get_item();

    if( used.is_null() ) {
        add_msg( m_info, _( "You do not have that item." ) );
        return;
    }

    you.last_item = used.typeId();

    if( used.is_tool() ) {
        if( !used.type->has_use() ) {
            add_msg( _( "You can't do anything interesting with your %s." ), used.tname() );
            return;
        }
        you.invoke_item( &used, loc.position() );

    } else if( is_pet_food( used ) ) {
        you.invoke_item( &used, loc.position() );

    } else if( !used.is_container_empty() && is_pet_food( used.get_contained() ) ) {
        unload_item( you, loc );

    } else if( !used.is_craft() && ( used.is_medication() || ( !used.type->has_use() &&
                                     ( used.is_food() ||
                                       used.get_contained().is_food() ||
                                       used.get_contained().is_medication() ) ) ) ) {
        you.consume( loc );

    } else if( used.is_book() ) {
        you.read( loc );
    } else if( used.type->has_use() ) {
        you.invoke_item( &used, loc.position() );
    } else if( used.has_flag( flag_SPLINT ) ) {
        ret_val<bool> need_splint = you.can_wear( used );
        if( need_splint.success() ) {
            you.wear_item( used );
            loc.remove_item();
        } else {
            add_msg( m_info, need_splint.str() );
        }
    } else {
        add_msg( m_info, _( "You can't do anything interesting with your %s." ),
                 used.tname() );
    }
    you.recalculate_enchantment_cache();
}

static bool add_or_drop_with_msg( avatar &you, item &it, bool unloading )
{
    if( it.made_of( LIQUID ) ) {
        liquid_handler::consume_liquid( it, 1 );
        return it.charges <= 0;
    }
    it.charges = you.i_add_to_container( it, unloading );
    if( it.is_ammo() && it.charges == 0 ) {
        return true;
    } else if( !you.can_pick_volume( it ) ) {
        put_into_vehicle_or_drop( you, item_drop_reason::too_large, { it } );
    } else if( !you.can_pick_weight( it, !get_option<bool>( "DANGEROUS_PICKUPS" ) ) ) {
        put_into_vehicle_or_drop( you, item_drop_reason::too_heavy, { it } );
    } else {
        auto &ni = you.i_add( it );
        add_msg( _( "You put the %s in your inventory." ), ni.tname() );
        add_msg( m_info, "%c - %s", ni.invlet == 0 ? ' ' : ni.invlet, ni.tname() );
    }
    return true;
}

bool unload_item( avatar &you, item_location loc )
{
    item &it = *loc.get_item();
    // Unload a container consuming moves per item successfully removed
    if( it.is_container() || it.is_bandolier() ) {
        if( it.contents.empty() ) {
            add_msg( m_info, _( "The %s is already empty!" ), it.tname() );
            return false;
        }
        if( !it.can_unload_liquid() ) {
            add_msg( m_info, _( "The liquid can't be unloaded in its current state!" ) );
            return false;
        }

        bool changed = false;
        for( item *contained : it.contents.all_items_top() ) {
            int old_charges = contained->charges;
            const bool consumed = add_or_drop_with_msg( you, *contained, true );
            changed = changed || consumed || contained->charges != old_charges;
            if( consumed ) {
                you.mod_moves( -you.item_handling_cost( *contained ) );
                it.remove_item( *contained );
            }
        }

        if( changed ) {
            it.on_contents_changed();
        }
        return true;
    }

    // If item can be unloaded more than once (currently only guns) prompt user to choose
    std::vector<std::string> msgs( 1, it.tname() );
    std::vector<item *> opts( 1, &it );

    for( auto e : it.gunmods() ) {
        if( e->is_gun() && !e->has_flag( "NO_UNLOAD" ) &&
            ( e->magazine_current() || e->ammo_remaining() > 0 || e->casings_count() > 0 ) ) {
            msgs.emplace_back( e->tname() );
            opts.emplace_back( e );
        }
    }

    item *target = nullptr;
    if( opts.size() > 1 ) {
        const int ret = uilist( _( "Unload what?" ), msgs );
        if( ret >= 0 ) {
            target = opts[ret];
        }
    } else {
        target = &it;
    }

    if( target == nullptr ) {
        return false;
    }

    // Next check for any reasons why the item cannot be unloaded
    if( target->ammo_types().empty() || target->ammo_capacity() <= 0 ) {
        add_msg( m_info, _( "You can't unload a %s!" ), target->tname() );
        return false;
    }

    if( target->has_flag( "NO_UNLOAD" ) ) {
        if( target->has_flag( "RECHARGE" ) || target->has_flag( "USE_UPS" ) ) {
            add_msg( m_info, _( "You can't unload a rechargeable %s!" ), target->tname() );
        } else {
            add_msg( m_info, _( "You can't unload a %s!" ), target->tname() );
        }
        return false;
    }

    if( !target->magazine_current() && target->ammo_remaining() <= 0 && target->casings_count() <= 0 ) {
        if( target->is_tool() ) {
            add_msg( m_info, _( "Your %s isn't charged." ), target->tname() );
        } else {
            add_msg( m_info, _( "Your %s isn't loaded." ), target->tname() );
        }
        return false;
    }

    target->casings_handle( [&]( item & e ) {
        return you.i_add_or_drop( e );
    } );

    if( target->is_magazine() ) {
        // Calculate the time to remove the contained ammo (consuming half as much time as required to load the magazine)
        int mv = 0;
        int qty = 0;
        std::vector<item *> remove_contained;
        for( item *contained : it.contents.all_items_top() ) {
            mv += you.item_reload_cost( it, *contained, contained->charges ) / 2;
            if( add_or_drop_with_msg( you, *contained, true ) ) {
                qty += contained->charges;
                remove_contained.push_back( contained );
            }
        }
        // remove the ammo leads in the belt
        for( item *remove : remove_contained ) {
            it.remove_item( *remove );
        }

        // remove the belt linkage
        if( it.is_ammo_belt() ) {
            if( it.type->magazine->linkage ) {
                item link( *it.type->magazine->linkage, calendar::turn, qty );
                add_or_drop_with_msg( you, link, true );
            }
            add_msg( _( "You disassemble your %s." ), it.tname() );
        } else {
            add_msg( _( "You unload your %s." ), it.tname() );
        }

        you.mod_moves( -std::min( 200, mv ) );
        if( loc->has_flag( "MAG_DESTROY" ) && loc->ammo_remaining() == 0 ) {
            loc.remove_item();
        }
        return true;

    } else if( target->magazine_current() ) {
        if( !add_or_drop_with_msg( you, *target->magazine_current(), true ) ) {
            return false;
        }
        // Eject magazine consuming half as much time as required to insert it
        you.moves -= you.item_reload_cost( *target, *target->magazine_current(), -1 ) / 2;

        target->remove_items_with( [&target]( const item & e ) {
            return target->magazine_current() == &e;
        } );

    } else if( target->ammo_remaining() ) {
        int qty = target->ammo_remaining();

        if( target->ammo_current() == itype_plut_cell ) {
            qty = target->ammo_remaining() / PLUTONIUM_CHARGES;
            if( qty > 0 ) {
                add_msg( _( "You recover %i unused plutonium." ), qty );
            } else {
                add_msg( m_info, _( "You can't remove partially depleted plutonium!" ) );
                return false;
            }
        }

        // Construct a new ammo item and try to drop it
        item ammo( target->ammo_current(), calendar::turn, qty );
        if( target->is_filthy() ) {
            ammo.set_flag( "FILTHY" );
        }

        if( ammo.made_of( LIQUID ) ) {
            if( !add_or_drop_with_msg( you, ammo, false ) ) {
                qty -= ammo.charges; // only handled part (or none) of the liquid
            }
            if( qty <= 0 ) {
                return false; // no liquid was moved
            }

        } else if( !add_or_drop_with_msg( you, ammo, qty > 1 ) ) {
            return false;
        }

        // If successful remove appropriate qty of ammo consuming half as much time as required to load it
        you.moves -= you.item_reload_cost( *target, ammo, qty ) / 2;

        if( target->ammo_current() == itype_plut_cell ) {
            qty *= PLUTONIUM_CHARGES;
        }

        target->ammo_set( target->ammo_current(), target->ammo_remaining() - qty );
    }

    // Turn off any active tools
    if( target->is_tool() && target->active && target->ammo_remaining() == 0 ) {
        target->type->invoke( you, *target, you.pos() );
    }

    add_msg( _( "You unload your %s." ), target->tname() );
    return true;

}

std::vector<npc *> list_potential_theft_witnesses( avatar &you, const faction_id &owners )
{
    std::vector<npc *> witnesses;
    for( npc &guy : g->all_npcs() ) {
        // Only owners care about theft of their property
        if( guy.get_faction() &&
            guy.get_faction()->id == owners &&
            rl_dist( guy.pos(), you.pos() ) < MAX_VIEW_DISTANCE &&
            guy.sees( you.pos() )
          ) {
            witnesses.push_back( &guy );
        }
    }
    return witnesses;
}

bool handle_theft_witnesses( avatar &you, const faction_id &owners )
{
    std::vector<npc *> witnesses = list_potential_theft_witnesses( you, owners );
    for( npc *guy : witnesses ) {
        guy->say( "<witnessed_thievery>", 7 );
    }
    if( !witnesses.empty() ) {
        if( you.add_faction_warning( owners ) ) {
            for( npc *guy : witnesses ) {
                guy->make_angry();
            }
        }
        return true;
    } else {
        return false;
    }
}

} // namespace avatar_funcs
