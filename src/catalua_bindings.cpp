#ifdef LUA
#include "catalua_bindings.h"

#include "avatar.h"
#include "bodypart.h"
#include "calendar.h"
#include "catalua_bindings_utils.h"
#include "catalua.h"
#include "catalua_impl.h"
#include "catalua_log.h"
#include "catalua_luna_doc.h"
#include "catalua_luna.h"
#include "character.h"
#include "creature.h"
#include "damage.h"
#include "distribution_grid.h"
#include "enum_conversions.h"
#include "enums.h"
#include "field.h"
#include "field_type.h"
#include "game.h"
#include "itype.h"
#include "map.h"
#include "messages.h"
#include "monfaction.h"
#include "monster.h"
#include "mtype.h"
#include "npc.h"
#include "player.h"
#include "popup.h"
#include "rng.h"
#include "skill.h"
#include "sounds.h"
#include "translations.h"
#include "type_id.h"
#include "ui.h"
#include "units_angle.h"
#include "units_energy.h"
#include "units_mass.h"
#include "units_volume.h"

std::string_view luna::detail::current_comment;

std::string cata::detail::fmt_lua_va( sol::variadic_args va )
{
    lua_State *L = va.lua_state();
    sol::state_view lua( L );

    std::string msg;
    for( auto it : va ) {
        msg += lua["tostring"]( it );
    }

    return msg;
}

namespace sol
{
template <>
struct is_container<item_stack> : std::false_type {};
template <>
struct is_container<map_stack> : std::false_type {};
} // namespace sol

struct item_stack_lua_it_state {
    item_stack *stack;
    size_t index;

    item_stack_lua_it_state( item_stack &stk )
        : stack( &stk ), index( 0 ) {
    }
};

static std::tuple<sol::object, sol::object>
item_stack_lua_next(
    sol::user<item_stack_lua_it_state &> user_it_state,
    sol::this_state l )
{
    // this gets called
    // to start the first iteration, and every
    // iteration there after

    // the state you passed in item_stack_lua_pairs is argument 1
    // the key value is argument 2, but we do not
    // care about the key value here
    item_stack_lua_it_state &it_state = user_it_state;
    if( it_state.index >= it_state.stack->size() ) {
        // return nil to signify that
        // there's nothing more to work with.
        return std::make_tuple( sol::object( sol::lua_nil ),
                                sol::object( sol::lua_nil ) );
    }
    auto it = it_state.stack->begin();
    std::advance( it, it_state.index );
    item *elem = *it;
    // 2 values are returned (pushed onto the stack):
    // the key and the value
    // the state is left alone
    auto r = std::make_tuple(
                 sol::object( l,  sol::in_place, it_state.index ),
                 sol::object( l, sol::in_place, elem ) );
    // the iterator must be moved forward one before we return
    it_state.index++;
    return r;
}

static auto item_stack_lua_pairs( item_stack &stk )
{
    // pairs expects 3 returns:
    // the "next" function on how to advance,
    // the "table" itself or some state,
    // and an initial key value (can be nil)

    // prepare our state
    item_stack_lua_it_state it_state( stk );
    // sol::user is a space/time optimization over regular
    // usertypes, it's incompatible with regular usertypes and
    // stores the type T directly in lua without any pretty
    // setup saves space allocation and a single dereference
    return std::make_tuple( &item_stack_lua_next,
                            sol::user<item_stack_lua_it_state>( std::move( it_state ) ),
                            sol::lua_nil );
}

void cata::detail::reg_units( sol::state &lua )
{
    {
        sol::usertype<units::angle> ut =
            luna::new_usertype<units::angle>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "from_radians", &units::from_radians<double> );
        luna::set_fx( ut, "to_radians", &units::to_radians );
        luna::set_fx( ut, "from_degrees", &units::from_degrees<double> );
        luna::set_fx( ut, "to_degrees", &units::to_degrees );
        luna::set_fx( ut, "from_arcmin", &units::from_arcmin<double> );
        luna::set_fx( ut, "to_arcmin", &units::to_arcmin );

        luna::set_fx( ut, sol::meta_function::equal_to, &units::angle::operator== );
        luna::set_fx( ut, sol::meta_function::less_than, &units::angle::operator< );
        luna::set_fx( ut, sol::meta_function::less_than_or_equal_to, &units::angle::operator<= );
    }
    {
        sol::usertype<units::energy> ut =
            luna::new_usertype<units::energy>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "from_joule", &units::from_joule<int> );
        luna::set_fx( ut, "to_joule", &units::to_joule<int> );
        luna::set_fx( ut, "from_kilojoule", &units::from_kilojoule<int> );
        luna::set_fx( ut, "to_kilojoule", &units::to_kilojoule<int> );

        luna::set_fx( ut, sol::meta_function::equal_to, &units::energy::operator== );
        luna::set_fx( ut, sol::meta_function::less_than, &units::energy::operator< );
        luna::set_fx( ut, sol::meta_function::less_than_or_equal_to, &units::energy::operator<= );
    }
    {
        sol::usertype<units::mass> ut =
            luna::new_usertype<units::mass>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "from_milligram", &units::from_milligram<std::int64_t> );
        luna::set_fx( ut, "to_milligram", &units::to_milligram<std::int64_t> );
        luna::set_fx( ut, "from_gram", &units::from_gram<std::int64_t> );
        luna::set_fx( ut, "to_gram", &units::to_gram<std::int64_t> );
        luna::set_fx( ut, "from_kilogram", &units::from_kilogram<std::int64_t> );
        luna::set_fx( ut, "to_kilogram", &units::to_kilogram<std::int64_t> );
        luna::set_fx( ut, "from_newton", &units::from_newton<std::int64_t> );
        luna::set_fx( ut, "to_newton", &units::to_newton<std::int64_t> );

        luna::set_fx( ut, sol::meta_function::equal_to, &units::mass::operator== );
        luna::set_fx( ut, sol::meta_function::less_than, &units::mass::operator< );
        luna::set_fx( ut, sol::meta_function::less_than_or_equal_to, &units::mass::operator<= );
    }
    {
        sol::usertype<units::volume> ut =
            luna::new_usertype<units::volume>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "from_milliliter", &units::from_milliliter<int> );
        luna::set_fx( ut, "from_liter", &units::from_liter<int> );
        luna::set_fx( ut, "to_milliliter", &units::to_milliliter<int> );
        luna::set_fx( ut, "to_liter", &units::to_liter );

        luna::set_fx( ut, sol::meta_function::equal_to, &units::volume::operator== );
        luna::set_fx( ut, sol::meta_function::less_than, &units::volume::operator< );
        luna::set_fx( ut, sol::meta_function::less_than_or_equal_to, &units::volume::operator<= );
    }
}

void cata::detail::reg_skill_level_map( sol::state &lua )
{
    {
        sol::usertype<SkillLevel> ut =
            luna::new_usertype<SkillLevel>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "is_training", &SkillLevel::isTraining );
        luna::set_fx( ut, "level", sol::resolve<int() const>( &SkillLevel::level ) );
        luna::set_fx( ut, "highest_level", &SkillLevel::highestLevel );
        luna::set_fx( ut, "train", &SkillLevel::train );
        luna::set_fx( ut, "can_train", &SkillLevel::can_train );
    }
    {
        sol::usertype<SkillLevelMap> ut =
            luna::new_usertype<SkillLevelMap>(
                lua,
                luna::bases<std::map<skill_id, SkillLevel>>(),
                luna::no_constructor
            );
        luna::set_fx( ut, "mod_skill_level", &SkillLevelMap::mod_skill_level );
        luna::set_fx( ut, "get_skill_level",
                      sol::resolve<int( const skill_id & ) const>
                      ( &SkillLevelMap::get_skill_level ) );
        luna::set_fx( ut, "get_skill_level_object",
                      sol::resolve<SkillLevel &( const skill_id & )>
                      ( &SkillLevelMap::get_skill_level_object ) );
    }
}

void cata::detail::reg_damage_instance( sol::state &lua )
{
#define UT_CLASS damage_unit
    {
        DOC( "Represents a damage amount" );
        DOC( "Constructors are:" );
        DOC( "new()" );
        DOC( "new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)" );
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors <
            UT_CLASS( damage_type, float, float, float, float )
            > ()
        );

        SET_MEMB( type );
        SET_MEMB( amount );
        SET_MEMB( res_pen );
        SET_MEMB( res_mult );
        SET_MEMB( damage_multiplier );

        luna::set_fx( ut, sol::meta_function::equal_to, &UT_CLASS::operator== );

    }
#undef UT_CLASS // #define UT_CLASS damage_unit
#define UT_CLASS damage_instance
    {
        DOC( "Represents a bunch of damage amounts" );
        DOC( "Constructors are:" );
        DOC( "new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)" );
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors <
            UT_CLASS(),
            UT_CLASS( damage_type, float, float, float, float )
            > ()
        );

        SET_MEMB( damage_units );

        SET_FX( mult_damage );
        SET_FX( type_damage );
        SET_FX( total_damage );
        SET_FX( clear );
        SET_FX( empty );
        SET_FX( add_damage );
        SET_FX_T( add, void( const damage_unit & ) );

        luna::set_fx( ut, sol::meta_function::equal_to, &UT_CLASS::operator== );
    }
#undef UT_CLASS // #define UT_CLASS damage_instance
#define UT_CLASS dealt_damage_instance
    {
        DOC( "Represents the final dealt damage" );
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::no_constructor
        );

        SET_MEMB( dealt_dams );
        SET_MEMB( bp_hit );

        SET_FX( type_damage );
        SET_FX( total_damage );
    }
#undef UT_CLASS // #define UT_CLASS dealt_damage_instance
}

void cata::detail::reg_item( sol::state &lua )
{
    {
        sol::usertype<item> ut = luna::new_usertype<item>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "get_type", &item::typeId );

        DOC( "Check for variable of any type" );
        luna::set_fx( ut, "has_var", &item::has_var );
        DOC( "Erase variable" );
        luna::set_fx( ut, "erase_var", &item::erase_var );
        DOC( "Erase all variables" );
        luna::set_fx( ut, "clear_vars", &item::clear_vars );

        DOC( "Get variable as string" );
        luna::set_fx( ut, "get_var_str",
                      sol::resolve<std::string( const std::string &, const std::string & ) const>
                      ( &item::get_var ) );
        DOC( "Get variable as float number" );
        luna::set_fx( ut, "get_var_num",
                      sol::resolve<double( const std::string &, double ) const>( &item::get_var ) );
        DOC( "Get variable as tripoint" );
        luna::set_fx( ut, "get_var_tri",
                      sol::resolve<tripoint( const std::string &, const tripoint & ) const>
                      ( &item::get_var ) );

        luna::set_fx( ut, "set_var_str", sol::resolve<void( const std::string &, const std::string & )>
                      ( &item::set_var ) );
        luna::set_fx( ut, "set_var_num",
                      sol::resolve<void( const std::string &, double )>( &item::set_var ) );
        luna::set_fx( ut, "set_var_tri",
                      sol::resolve<void( const std::string &, const tripoint & )>( &item::set_var ) );
    }
}

void cata::detail::reg_map( sol::state &lua )
{
    // Register 'map' class to be used in Lua
    {
        sol::usertype<map> ut = luna::new_usertype<map>( lua, luna::no_bases, luna::no_constructor );

        DOC( "Convert local ms -> absolute ms" );
        luna::set_fx( ut, "get_abs_ms", sol::resolve<tripoint( const tripoint & ) const>( &map::getabs ) );
        DOC( "Convert absolute ms -> local ms" );
        luna::set_fx( ut, "get_local_ms",
                      sol::resolve<tripoint( const tripoint & ) const>( &map::getlocal ) );

        luna::set_fx( ut, "get_map_size_in_submaps", &map::getmapsize );
        DOC( "In map squares" );
        luna::set_fx( ut, "get_map_size", []( const map & m ) -> int {
            return m.getmapsize() * SEEX;
        } );

        luna::set_fx( ut, "has_items_at", &map::has_items );
        luna::set_fx( ut, "get_items_at", []( map & m, const tripoint & p ) -> std::unique_ptr<map_stack> {
            return std::make_unique<map_stack>( m.i_at( p ) );
        } );


        luna::set_fx( ut, "get_ter_at", sol::resolve<ter_id( const tripoint & )const>( &map::ter ) );
        luna::set_fx( ut, "set_ter_at",
                      sol::resolve<bool( const tripoint &, const ter_id & )>( &map::ter_set ) );

        luna::set_fx( ut, "get_furn_at", sol::resolve<furn_id( const tripoint & )const>( &map::furn ) );
        luna::set_fx( ut, "set_furn_at", []( map & m, const tripoint & p, const furn_id & id ) {
            m.furn_set( p, id );
        } );

        luna::set_fx( ut, "has_field_at", []( const map & m, const tripoint & p,
        const field_type_id & fid ) -> bool {
            return !!m.field_at( p ).find_field( fid );
        } );
        luna::set_fx( ut, "get_field_int_at", &map::get_field_intensity );
        luna::set_fx( ut, "get_field_age_at", &map::get_field_age );
        luna::set_fx( ut, "mod_field_int_at", &map::mod_field_intensity );
        luna::set_fx( ut, "mod_field_age_at", &map::mod_field_age );
        luna::set_fx( ut, "set_field_int_at", &map::set_field_intensity );
        luna::set_fx( ut, "set_field_age_at", &map::set_field_age );
        luna::set_fx( ut, "add_field_at", []( map & m, const tripoint & p, const field_type_id & fid,
        int intensity, const time_duration & age ) -> bool {
            return m.add_field( p, fid, intensity, age );
        } );
        luna::set_fx( ut, "remove_field_at", &map::remove_field );
    }

    // Register 'tinymap' class to be used in Lua
    {
        luna::new_usertype<tinymap>( lua, luna::bases<map>(), luna::no_constructor );
    }

    // Register 'item_stack' class to be used in Lua
    {
        DOC( "Iterate over this using pairs()" );
        sol::usertype<item_stack> ut = luna::new_usertype<item_stack>( lua, luna::no_bases,
                                       luna::no_constructor );

        luna::set_fx( ut, sol::meta_function::pairs, item_stack_lua_pairs );
    }

    // Register 'map_stack' class to be used in Lua
    {
        sol::usertype<map_stack> ut = luna::new_usertype<map_stack>( lua, luna::bases<item_stack>(),
                                      luna::no_constructor );

        luna::set_fx( ut, "as_item_stack", []( map_stack & ref ) -> item_stack& {
            return ref;
        } );
    }
}

void cata::detail::reg_distribution_grid( sol::state &lua )
{
    {
        sol::usertype<distribution_grid> ut =
            luna::new_usertype<distribution_grid>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        DOC( "Boolean argument controls recursive behavior" );
        luna::set_fx( ut, "get_resource", &distribution_grid::get_resource );
        DOC( "Boolean argument controls recursive behavior" );
        luna::set_fx( ut, "mod_resource", &distribution_grid::mod_resource );
    }

    {
        sol::usertype<distribution_grid_tracker> ut =
            luna::new_usertype<distribution_grid_tracker>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "get_grid_at_abs_ms", []( distribution_grid_tracker & tr, const tripoint & p )
        -> distribution_grid& {
            return tr.grid_at( tripoint_abs_ms( p ) );
        } );
    }

}

void cata::detail::reg_ui_elements( sol::state &lua )
{
    {
        sol::usertype<uilist> ut =
            luna::new_usertype<uilist>(
                lua,
                luna::no_bases,
                luna::constructors <
                uilist()
                > ()
            );
        luna::set_fx( ut, "title", []( uilist & ui, const std::string & text ) {
            ui.title = text;
        } );
        DOC( "Return value, text" );
        luna::set_fx( ut, "add", []( uilist & ui, int retval, const std::string & text ) {
            ui.addentry( retval, true, MENU_AUTOASSIGN, text );
        } );
        DOC( "Returns retval for selected entry, or a negative number on fail/cancel" );
        luna::set_fx( ut, "query", []( uilist & ui ) {
            ui.query();
            return ui.ret;
        } );
    }

    {
        sol::usertype<query_popup> ut =
            luna::new_usertype<query_popup>(
                lua,
                luna::no_bases,
                luna::constructors <
                query_popup()
                > ()
            );
        luna::set_fx( ut, "message", []( query_popup & popup, sol::variadic_args va ) {
            popup.message( "%s", cata::detail::fmt_lua_va( va ) );
        } );
        luna::set_fx( ut, "message_color", []( query_popup & popup, color_id col ) {
            popup.default_color( get_all_colors().get( col ) );
        } );
        DOC( "Set whether to allow any key" );
        luna::set_fx( ut, "allow_any_key", []( query_popup & popup, bool val ) {
            popup.allow_anykey( val );
        } );
        DOC( "Returns selected action" );
        luna::set_fx( ut, "query", []( query_popup & popup ) {
            return popup.query().action;
        } );
    }
}

void cata::detail::reg_constants( sol::state &lua )
{
    DOC( "Various game constants" );
    luna::userlib lib = luna::begin_lib( lua, "const" );

    luna::set( lib, "OM_OMT_SIZE", OMAPX );
    luna::set( lib, "OM_SM_SIZE", OMAPX * 2 );
    luna::set( lib, "OM_MS_SIZE", OMAPX * 2 * SEEX );
    luna::set( lib, "OMT_SM_SIZE", 2 );
    luna::set( lib, "OMT_MS_SIZE", SEEX * 2 );
    luna::set( lib, "SM_MS_SIZE", SEEX );

    luna::finalize_lib( lib );
}

static void lua_log_info_impl( sol::variadic_args va )
{
    std::string msg = cata::detail::fmt_lua_va( va );

    DebugLog( DL::Info, DC::Lua ) << msg;
    cata::get_lua_log_instance().add( cata::LuaLogLevel::Info, std::move( msg ) );
}

static void lua_log_warn_impl( sol::variadic_args va )
{
    std::string msg = cata::detail::fmt_lua_va( va );

    DebugLog( DL::Warn, DC::Lua ) << msg;
    cata::get_lua_log_instance().add( cata::LuaLogLevel::Warn, std::move( msg ) );
}

static void lua_log_error_impl( sol::variadic_args va )
{
    std::string msg = cata::detail::fmt_lua_va( va );

    DebugLog( DL::Error, DC::Lua ) << msg;
    cata::get_lua_log_instance().add( cata::LuaLogLevel::Error, std::move( msg ) );
}

static void lua_debugmsg_impl( sol::variadic_args va )
{
    std::string msg = cata::detail::fmt_lua_va( va );

    debugmsg( "%s", msg );
    cata::get_lua_log_instance().add( cata::LuaLogLevel::DebugMsg, std::move( msg ) );
}

void cata::detail::reg_debug_api( sol::state &lua )
{
    DOC( "Debugging and logging API." );
    luna::userlib lib = luna::begin_lib( lua, "gdebug" );

    luna::set_fx( lib, "log_info", &lua_log_info_impl );
    luna::set_fx( lib, "log_warn", &lua_log_warn_impl );
    luna::set_fx( lib, "log_error", &lua_log_error_impl );
    luna::set_fx( lib, "debugmsg", &lua_debugmsg_impl );
    luna::set_fx( lib, "clear_lua_log", []() {
        cata::get_lua_log_instance().clear();
    } );
    luna::set_fx( lib, "set_log_capacity", []( int v ) {
        cata::get_lua_log_instance().set_log_capacity( v );
    } );
    luna::set_fx( lib, "reload_lua_code", &cata::reload_lua_code );
    luna::set_fx( lib, "save_game", []() -> bool {
        return g->save( false );
    } );

    luna::finalize_lib( lib );
}

void cata::detail::override_default_print( sol::state &lua )
{
    lua.globals()["print"] = &lua_log_info_impl;
}

void cata::detail::forbid_unsafe_functions( sol::state &lua )
{
    auto g = lua.globals();
    g["dofile"] = sol::nil;
    g["loadfile"] = sol::nil;
    g["load"] = sol::nil;
    g["loadstring"] = sol::nil;
}

static void add_msg_lua( game_message_type t, sol::variadic_args va )
{
    if( va.size() == 0 ) {
        // Nothing to print
        return;
    }

    std::string msg = cata::detail::fmt_lua_va( va );
    add_msg( t, msg );
}

void cata::detail::reg_game_api( sol::state &lua )
{
    DOC( "Global game methods" );
    luna::userlib lib = luna::begin_lib( lua, "gapi" );

    luna::set_fx( lib, "get_avatar", &get_avatar );
    luna::set_fx( lib, "get_map", &get_map );
    luna::set_fx( lib, "get_distribution_grid_tracker", &get_distribution_grid_tracker );
    luna::set_fx( lib, "add_msg", sol::overload(
                      add_msg_lua,
    []( sol::variadic_args va ) {
        add_msg_lua( game_message_type::m_neutral, va );
    }
                  ) );
    luna::set_fx( lib, "place_player_overmap_at", []( const tripoint & p ) -> void {
        g->place_player_overmap( tripoint_abs_omt( p ) );
    } );
    luna::set_fx( lib, "current_turn", []() -> time_point { return calendar::turn; } );
    luna::set_fx( lib, "turn_zero", []() -> time_point { return calendar::turn_zero; } );
    luna::set_fx( lib, "before_time_starts", []() -> time_point { return calendar::before_time_starts; } );
    luna::set_fx( lib, "rng", sol::resolve<int( int, int )>( &rng ) );
    luna::set_fx( lib, "add_on_every_x_hook", []( sol::this_state lua_this, time_duration interval,
    sol::protected_function f ) {
        sol::state_view lua( lua_this );
        std::vector<on_every_x_hooks> &hooks = lua["game"]["cata_internal"]["on_every_x_hooks"];
        for( auto &entry : hooks ) {
            if( entry.interval == interval ) {
                entry.functions.push_back( f );
                return;
            }
        }
        std::vector<sol::protected_function> vec;
        vec.push_back( f );
        hooks.push_back( on_every_x_hooks{ interval, vec } );
    } );

    luna::set_fx( lib, "get_creature_at", []( const tripoint & p,
    sol::optional<bool> allow_hallucination ) -> Creature * {
        if( allow_hallucination.has_value() )
        {
            return g->critter_at<Creature>( p, *allow_hallucination );
        }
        return g->critter_at<Creature>( p );
    } );
    luna::set_fx( lib, "get_monster_at", []( const tripoint & p,
    sol::optional<bool> allow_hallucination ) -> monster * {
        if( allow_hallucination.has_value() )
        {
            return g->critter_at<monster>( p, *allow_hallucination );
        }
        return g->critter_at<monster>( p );
    } );
    luna::set_fx( lib, "get_character_at", []( const tripoint & p,
    sol::optional<bool> allow_hallucination ) -> Character * {
        if( allow_hallucination.has_value() )
        {
            return g->critter_at<Character>( p, *allow_hallucination );
        }
        return g->critter_at<Character>( p );
    } );
    luna::set_fx( lib, "get_npc_at", []( const tripoint & p,
    sol::optional<bool> allow_hallucination ) -> npc * {
        if( allow_hallucination.has_value() )
        {
            return g->critter_at<npc>( p, *allow_hallucination );
        }
        return g->critter_at<npc>( p );
    } );

    luna::set_fx( lib, "choose_adjacent", []( const std::string & message,
    sol::optional<bool> allow_vertical ) -> sol::optional<tripoint> {
        std::optional<tripoint> stdOpt;
        if( allow_vertical.has_value() )
        {
            stdOpt = choose_adjacent( message, *allow_vertical );
        } else
        {
            stdOpt = choose_adjacent( message );
        }
        if( stdOpt.has_value() )
        {
            return sol::optional<tripoint>( *stdOpt );
        }
        return sol::optional<tripoint>();
    } );
    luna::set_fx( lib, "choose_direction", []( const std::string & message,
    sol::optional<bool> allow_vertical ) -> sol::optional<tripoint> {
        std::optional<tripoint> stdOpt;
        if( allow_vertical.has_value() )
        {
            stdOpt = choose_direction( message, *allow_vertical );
        } else
        {
            stdOpt = choose_direction( message );
        }
        if( stdOpt.has_value() )
        {
            return sol::optional<tripoint>( *stdOpt );
        }
        return sol::optional<tripoint>();
    } );
    luna::set_fx( lib, "look_around", []() {
        auto result = g->look_around();
        if( result.has_value() ) {
            return sol::optional<tripoint>( *result );
        }
        return sol::optional<tripoint>();
    } );

    luna::set_fx( lib, "play_variant_sound",
                  sol::overload(
                      sol::resolve<void( const std::string &, const std::string &, int )>( &sfx::play_variant_sound ),
                      sol::resolve<void( const std::string &, const std::string &, int,
                                         units::angle, double, double )>( &sfx::play_variant_sound )
                  ) );
    luna::set_fx( lib, "play_ambient_variant_sound", &sfx::play_ambient_variant_sound );

    luna::set_fx( lib, "add_npc_follower", []( npc & p ) {
        g->add_npc_follower( p.getID() );
    } );
    luna::set_fx( lib, "remove_npc_follower", []( npc & p ) {
        g->remove_npc_follower( p.getID() );
    } );

    luna::finalize_lib( lib );
}

// We have to alias the function here because VS compiler
// confuses namespaces 'detail' and 'cata::detail'
const auto &gettext_raw = sol::resolve<const char *( const char * )>
                          ( &detail::_translate_internal );

void cata::detail::reg_locale_api( sol::state &lua )
{
    DOC( "Localization API." );
    luna::userlib lib = luna::begin_lib( lua, "locale" );

    DOC( "Expects english source string, returns translated string." );
    luna::set_fx( lib, "gettext", gettext_raw );
    DOC( "First is english singular string, second is english plural string. Number is amount to translate for." );
    luna::set_fx( lib, "vgettext", &vgettext );
    DOC( "First is context string. Second is english source string." );
    luna::set_fx( lib, "pgettext", &pgettext );
    DOC( "First is context string. Second is english singular string. third is english plural. Number is amount to translate for." );
    luna::set_fx( lib, "vpgettext", &vpgettext );

    luna::finalize_lib( lib );
}

template<typename E>
void reg_enum( sol::state &lua )
{
    // Sol2 has new_enum<E>(...) function, but it needs to know all value-string
    // pairs at compile time, so we can't use it with io::enum_to_string.
    //
    // As such, hack it by creating read-only table.

    luna::userenum<E> et = luna::begin_enum<E>( lua );

    using Int = std::underlying_type_t<E>;
    constexpr Int max = static_cast<Int>( enum_traits<E>::last );

    for( Int i = 0; i < max; ++i ) {
        E e = static_cast<E>( i );
        std::string key = io::enum_to_string<E>( e );
        luna::add_val( et, key, e );
    }

    luna::finalize_enum( et );
}

void cata::detail::reg_colors( sol::state &lua )
{
    // Colors are not enums, we have to do them manually
    luna::userenum<color_id> et = luna::begin_enum<color_id>( lua );

    using Int = std::underlying_type_t<color_id>;
    constexpr Int max = static_cast<Int>( color_id::num_colors );

    for( Int i = 0; i < max; ++i ) {
        color_id e = static_cast<color_id>( i );
        std::string key = get_all_colors().id_to_name( e );
        luna::add_val( et, key, e );
    }

    luna::finalize_enum( et );
}

void cata::detail::reg_enums( sol::state &lua )
{
    reg_enum<add_type>( lua );
    reg_enum<Attitude>( lua );
    reg_enum<body_part>( lua );
    reg_enum<character_movemode>( lua );
    reg_enum<damage_type>( lua );
    reg_enum<game_message_type>( lua );
    reg_enum<mf_attitude>( lua );
    reg_enum<m_flag>( lua );
    reg_enum<monster_attitude>( lua );
    reg_enum<creature_size>( lua );
    reg_enum<npc_attitude>( lua );
    reg_enum<npc_need>( lua );
    reg_enum<sfx::channel>( lua );
}

void cata::detail::reg_hooks_examples( sol::state &lua )
{
    DOC( "Documentation for hooks" );
    luna::userlib lib = luna::begin_lib( lua, "hooks_doc" );

    DOC( "Called when game is about to save" );
    luna::set_fx( lib, "on_game_save", []() {} );
    DOC( "Called right after game has loaded" );
    luna::set_fx( lib, "on_game_load", []() {} );
    DOC( "Called every in-game period" );
    luna::set_fx( lib, "on_every_x", []() {} );
    DOC( "Called right after mapgen has completed. "
         "Map argument is the tinymap that represents 24x24 area (2x2 submaps, or 1x1 omt), "
         "tripoint is the absolute omt pos, and time_point is the current time (for time-based effects)."
       );
    luna::set_fx( lib, "on_mapgen_postprocess", []( map &, const tripoint &, const time_point & ) {} );

    luna::finalize_lib( lib );
}

void cata::detail::reg_time_types( sol::state &lua )
{
    DOC( "Library for dealing with time primitives." );

    {
        DOC( "Represent fixed point in time" );
        sol::usertype<time_point> ut =
            luna::new_usertype<time_point>(
                lua,
                luna::no_bases,
                luna::constructors < time_point() > ()
            );

        // Constructor method
        luna::set_fx( ut, "from_turn", &time_point::from_turn );

        // Methods
        luna::set_fx( ut, "to_turn", []( const time_point & tp ) -> int {
            return to_turn<int>( tp );
        } );

        luna::set_fx( ut, "is_night", &is_night );
        luna::set_fx( ut, "is_day", &is_day );
        luna::set_fx( ut, "is_dusk", &is_dusk );
        luna::set_fx( ut, "is_dawn", &is_dawn );

        luna::set_fx( ut, "second_of_minute", []( const time_point & tp ) -> int {
            return to_turn<int>( tp ) % 60;
        } );
        luna::set_fx( ut, "minute_of_hour", []( const time_point & tp ) -> int {
            return minute_of_hour<int>( tp );
        } );
        luna::set_fx( ut, "hour_of_day", []( const time_point & tp ) -> int {
            return hour_of_day<int>( tp );
        } );

        // (De-)Serialization
        reg_serde_functions( ut );

        luna::set_fx( ut, "to_string_time_of_day", &to_string_time_of_day );

        // To string
        // We're using Lua meta function here to make it work seamlessly with native Lua tostring()
        luna::set_fx( ut, sol::meta_function::to_string,
                      sol::resolve<std::string( const time_point & )>( to_string ) );

        // Equality operator
        // It's defined as inline friend function inside point class, we can't access it and so have to improvise
        luna::set_fx( ut, sol::meta_function::equal_to, []( const time_point & a, const time_point & b ) {
            return a == b;
        } );

        // Less-then operator
        // Same deal as with equality operator
        luna::set_fx( ut, sol::meta_function::less_than, []( const time_point & a, const time_point & b ) {
            return a < b;
        } );

        // Arithmetic operators
        luna::set_fx( ut, sol::meta_function::addition,
        []( const time_point & a, const time_duration & b ) -> time_point {
            return a + b;
        }
                    );
        luna::set_fx( ut, sol::meta_function::subtraction,
                      sol::overload(
        []( const time_point & a, const time_point & b ) -> time_duration {
            return a - b;
        },
        []( const time_point & a, const time_duration & b ) -> time_point {
            return a - b;
        }
                      ) );
    }
    {
        DOC( "Represent duration between 2 fixed points in time" );
        sol::usertype<time_duration> ut =
            luna::new_usertype<time_duration>(
                lua,
                luna::no_bases,
                luna::constructors < time_duration() > ()
            );

        // Constructor methods
        luna::set_fx( ut, "from_turns", []( int t ) {
            return time_duration::from_turns( t );
        } );
        luna::set_fx( ut, "from_seconds", []( int t ) {
            return time_duration::from_seconds( t );
        } );
        luna::set_fx( ut, "from_minutes", []( int t ) {
            return time_duration::from_minutes( t );
        } );
        luna::set_fx( ut, "from_hours", []( int t ) {
            return time_duration::from_hours( t );
        } );
        luna::set_fx( ut, "from_days", []( int t ) {
            return time_duration::from_days( t );
        } );
        luna::set_fx( ut, "from_weeks", []( int t ) {
            return time_duration::from_weeks( t );
        } );

        luna::set_fx( ut, "make_random", []( const time_duration & lo, const time_duration & hi ) {
            return rng( lo, hi );
        } );

        luna::set_fx( ut, "to_turns", []( const time_duration & t ) -> int {
            return to_turns<int>( t );
        } );
        luna::set_fx( ut, "to_seconds", []( const time_duration & t ) -> int {
            return to_seconds<int>( t );
        } );
        luna::set_fx( ut, "to_minutes", []( const time_duration & t ) -> int {
            return to_minutes<int>( t );
        } );
        luna::set_fx( ut, "to_hours", []( const time_duration & t ) -> int {
            return to_hours<int>( t );
        } );
        luna::set_fx( ut, "to_days", []( const time_duration & t ) -> int {
            return to_days<int>( t );
        } );
        luna::set_fx( ut, "to_weeks", []( const time_duration & t ) -> int {
            return to_weeks<int>( t );
        } );

        // (De-)Serialization
        reg_serde_functions( ut );

        // To string
        // We're using Lua meta function here to make it work seamlessly with native Lua tostring()
        luna::set_fx( ut, sol::meta_function::to_string,
                      sol::resolve<std::string( const time_duration & )>( to_string ) );

        luna::set_fx( ut, sol::meta_function::addition, []( const time_duration & a,
        const time_duration & b ) {
            return a + b;
        } );
        luna::set_fx( ut, sol::meta_function::subtraction, []( const time_duration & a,
        const time_duration & b ) {
            return a - b;
        } );
        luna::set_fx( ut, sol::meta_function::multiplication, []( const time_duration & a, int b ) {
            return a * b;
        } );
        luna::set_fx( ut, sol::meta_function::division, []( const time_duration & a, int b ) {
            return a / b;
        } );
        luna::set_fx( ut, sol::meta_function::unary_minus, []( const time_duration & a ) {
            return -a;
        } );
    }
}

void cata::detail::reg_testing_library( sol::state &lua )
{
    DOC( "Library for testing purposes" );
    luna::userlib lib = luna::begin_lib( lua, "tests_lib" );

    // Regression test for https://github.com/ThePhD/sol2/issues/1444
    luna::set_fx( lib, "my_awesome_lambda_1", []() -> int {
        return 1;
    } );
    luna::set_fx( lib, "my_awesome_lambda_2", []() -> int {
        return 2;
    } );

    luna::finalize_lib( lib );
}

void cata::reg_all_bindings( sol::state &lua )
{
    using namespace detail;

    override_default_print( lua );
    forbid_unsafe_functions( lua );
    reg_debug_api( lua );
    reg_game_api( lua );
    reg_locale_api( lua );
    reg_units( lua );
    reg_skill_level_map( lua );
    reg_damage_instance( lua );
    reg_creature_family( lua );
    reg_point_tripoint( lua );
    reg_item( lua );
    reg_map( lua );
    reg_distribution_grid( lua );
    reg_ui_elements( lua );
    reg_colors( lua );
    reg_enums( lua );
    reg_game_ids( lua );
    reg_coords_library( lua );
    reg_constants( lua );
    reg_hooks_examples( lua );
    reg_types( lua );
    reg_time_types( lua );
    reg_testing_library( lua );
}

#endif
