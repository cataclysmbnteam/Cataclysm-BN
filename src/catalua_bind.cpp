#ifdef LUA
#include "catalua_bind.h"
#include "catalua_bindings.h"

#include "avatar.h"
#include "character.h"
#include "creature.h"
#include "enum_conversions.h"
#include "faction.h"
#include "distribution_grid.h"
#include "item.h"
#include "itype.h"
#include "map.h"
#include "mapdata.h"
#include "monster.h"
#include "npc.h"
#include "player.h"
#include "point.h"
#include "popup.h"
#include "ui.h"

// These definitions help the doc generator
LUNA_DOC( bool, "bool" );
LUNA_DOC( int, "int" );
LUNA_DOC( float, "double" );
LUNA_DOC( double, "double" );
LUNA_DOC( void, "nil" );
LUNA_DOC( char, "char" );
LUNA_DOC( const char *, "string" );
LUNA_DOC( std::string, "string" );
LUNA_DOC( std::string_view, "string" );
LUNA_DOC( sol::lua_nil_t, "nil" );
LUNA_DOC( sol::variadic_args, "..." );

// These definitions are for the bindings generator
LUNA_VAL( color_id, "Color" );
LUNA_VAL( Creature, "Creature" );
LUNA_VAL( Character, "Character" );
LUNA_VAL( monster, "Monster" );
LUNA_VAL( npc, "Npc" );
LUNA_VAL( player, "Player" );
LUNA_VAL( avatar, "Avatar" );
LUNA_VAL( point, "Point" );
LUNA_VAL( tripoint, "Tripoint" );
LUNA_VAL( item, "Item" );
LUNA_VAL( map, "Map" );
LUNA_VAL( tinymap, "Tinymap" );
LUNA_VAL( item_stack, "ItemStack" );
LUNA_VAL( map_stack, "MapStack" );
LUNA_VAL( distribution_grid, "DistributionGrid" );
LUNA_VAL( distribution_grid_tracker, "DistributionGridTracker" );
LUNA_VAL( uilist, "UiList" );
LUNA_VAL( query_popup, "QueryPopup" );

// Ids for in-game objects
LUNA_ID( faction, "Faction" )
LUNA_ID( itype, "Itype" )
LUNA_ID( ter_t, "Ter" )
LUNA_ID( furn_t, "Furn" )

// Enums
LUNA_ENUM( game_message_type, "MsgType" )

static std::string fmt_lua_va( sol::variadic_args va )
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
    using it_t = item_stack::iterator;
    it_t it;
    it_t last;

    item_stack_lua_it_state( item_stack &stk )
        : it( stk.begin() ), last( stk.end() ) {
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
    auto &it = it_state.it;
    if( it == it_state.last ) {
        // return nil to signify that
        // there's nothing more to work with.
        return std::make_tuple( sol::object( sol::lua_nil ),
                                sol::object( sol::lua_nil ) );
    }
    item *elem = &*it;
    // 2 values are returned (pushed onto the stack):
    // the key and the value
    // the state is left alone
    auto r = std::make_tuple(
                 sol::object( l, sol::in_place, it ),
                 sol::object( l, sol::in_place, elem ) );
    // the iterator must be moved forward one before we return
    std::advance( it, 1 );
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

static void reg_creature_family( sol::state &lua )
{
    {
        // Specifying base classes here allows us to pass derived classes
        // from Lua to C++ functions that expect base class.
        sol::usertype<Creature> ut =
            luna::new_usertype<Creature>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        // TODO: typesafe coords
        ut["get_pos_ms"] = &Creature::pos;
    }

    {
        luna::new_usertype<monster>(
            lua,
            luna::bases<Creature>(),
            luna::no_constructor
        );
        luna::new_usertype<Character>(
            lua,
            luna::bases<Creature>(),
            luna::no_constructor
        );
        luna::new_usertype<player>(
            lua,
            luna::bases<Character, Creature>(),
            luna::no_constructor
        );
        luna::new_usertype<npc>(
            lua,
            luna::bases<player, Character, Creature>(),
            luna::no_constructor
        );
        luna::new_usertype<avatar>(
            lua,
            luna::bases<player, Character, Creature>(),
            luna::no_constructor
        );
    }
}

static void reg_point_tripoint( sol::state &lua )
{
    // Register 'point' class to be used in Lua
    {
        sol::usertype<point> ut =
            luna::new_usertype<point>(
                lua,
                luna::no_bases,
                luna::constructors <
                point(),
                point( const point & ),
                point( int, int )
                > ()
            );

        // Members
        luna::set( ut, "x", &point::x );
        luna::set( ut, "y", &point::y );

        // Methods
        luna::set_fx( ut, "abs", &point::abs );
        luna::set_fx( ut, "rotate", &point::rotate );

        // To string
        // We're using Lua meta function here to make it work seamlessly with native Lua tostring()
        luna::set_fx( ut, sol::meta_function::to_string, &point::to_string );

        // Equality operator
        // It's defined as inline friend function inside point class, we can't access it and so have to improvise
        luna::set_fx( ut, sol::meta_function::equal_to, []( const point & a, const point & b ) {
            return a == b;
        } );

        // Less-then operator
        // Same deal as with equality operator
        luna::set_fx( ut, sol::meta_function::less_than, []( const point & a, const point & b ) {
            return a < b;
        } );

        // Arithmetic operators
        // point + point
        luna::set_fx( ut, sol::meta_function::addition, &point::operator+ );
        // point - point
        // sol::resolve here makes it possible to specify which overload to use
        luna::set_fx( ut, sol::meta_function::subtraction, sol::resolve< point( const point & ) const >
                      ( &point::operator- ) );
        // point * int
        luna::set_fx( ut, sol::meta_function::multiplication, &point::operator* );
        // point / float
        luna::set_fx( ut, sol::meta_function::division, &point::operator/ );
        // point / int
        luna::set_fx( ut, sol::meta_function::floor_division, &point::operator/ );
        // -point
        // sol::resolve here makes it possible to specify which overload to use
        luna::set_fx( ut, sol::meta_function::unary_minus,
                      sol::resolve< point() const >( &point::operator- ) );
    }

    // Register 'tripoint' class to be used in Lua
    {
        sol::usertype<tripoint> ut =
            luna::new_usertype<tripoint>(
                lua,
                luna::no_bases,
                luna::constructors <
                tripoint(),
                tripoint( const point &, int ),
                tripoint( const tripoint & ),
                tripoint( int, int, int )
                > ()
            );

        // Members
        luna::set( ut, "x", &tripoint::x );
        luna::set( ut, "y", &tripoint::y );
        luna::set( ut, "z", &tripoint::z );

        // Methods
        luna::set_fx( ut, "abs", &tripoint::abs );
        luna::set_fx( ut, "xy", &tripoint::xy );
        luna::set_fx( ut, "rotate_2d", &tripoint::rotate_2d );

        // To string
        // We're using Lua meta function here to make it work seamlessly with native Lua tostring()
        luna::set_fx( ut, sol::meta_function::to_string, &tripoint::to_string );

        // Equality operator
        // It's defined as inline friend function inside point class, we can't access it and so have to improvise
        luna::set_fx( ut, sol::meta_function::equal_to, []( const tripoint & a, const tripoint & b ) {
            return a == b;
        } );

        // Less-then operator
        // Same deal as with equality operator
        luna::set_fx( ut, sol::meta_function::less_than, []( const tripoint & a, const tripoint & b ) {
            return a < b;
        } );

        // Arithmetic operators
        // tripoint + tripoint (overload 1)
        // tripoint + point (overload 2)
        luna::set_fx( ut, sol::meta_function::addition, sol::overload(
                          sol::resolve< tripoint( const tripoint & ) const > ( &tripoint::operator+ ),
                          sol::resolve< tripoint( const point & ) const > ( &tripoint::operator+ )
                      ) );
        // tripoint - tripoint (overload 1)
        // tripoint - point (overload 2)
        luna::set_fx( ut, sol::meta_function::subtraction, sol::overload(
                          sol::resolve< tripoint( const tripoint & ) const > ( &tripoint::operator- ),
                          sol::resolve< tripoint( const point & ) const > ( &tripoint::operator- )
                      ) );
        // tripoint * int
        luna::set_fx( ut, sol::meta_function::multiplication, &tripoint::operator* );
        // tripoint / float
        luna::set_fx( ut, sol::meta_function::division, &tripoint::operator/ );
        // tripoint / int
        luna::set_fx( ut, sol::meta_function::floor_division, &tripoint::operator/ );
        // -tripoint
        // sol::resolve here makes it possible to specify which overload to use
        luna::set_fx( ut, sol::meta_function::unary_minus,
                      sol::resolve< tripoint() const >( &tripoint::operator- ) );
    }
}

static void reg_item( sol::state &lua )
{
    {
        sol::usertype<item> ut = luna::new_usertype<item>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "get_type", &item::typeId );

        luna::set_fx( ut, "has_var", &item::has_var );
        luna::set_fx( ut, "erase_var", &item::erase_var );
        luna::set_fx( ut, "clear_vars", &item::clear_vars );

        luna::set_fx( ut, "get_var_str",
                      sol::resolve<std::string( const std::string &, const std::string & ) const>
                      ( &item::get_var ) );
        luna::set_fx( ut, "get_var_num",
                      sol::resolve<double( const std::string &, double ) const>( &item::get_var ) );
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

static void reg_map( sol::state &lua )
{
    // Register 'map' class to be used in Lua
    {
        sol::usertype<map> ut = luna::new_usertype<map>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "get_abs_ms", sol::resolve<tripoint( const tripoint & ) const>( &map::getabs ) );
        luna::set_fx( ut, "get_local_ms",
                      sol::resolve<tripoint( const tripoint & ) const>( &map::getlocal ) );

        luna::set_fx( ut, "get_map_size_in_submaps", &map::getmapsize );
        luna::set_fx( ut, "get_map_size", []( const map & m ) -> int {
            return m.getmapsize() * SEEX;
        } );

        luna::set_fx( ut, "has_items_at", &map::has_items );
        luna::set_fx( ut, "get_items_at", []( map & m, const tripoint & p ) -> std::unique_ptr<map_stack> {
            return std::make_unique<map_stack>( m.i_at( p ) );
        } );

        // TODO: make it work with int_ids
        luna::set_fx( ut, "get_ter_at", []( const map & m, const tripoint & p ) {
            return m.ter( p ).id();
        } );
        luna::set_fx( ut, "set_ter_at", []( map & m, const tripoint & p, const ter_str_id & id ) {
            m.ter_set( p, id.id() );
        } );

        luna::set_fx( ut, "get_furn_at", []( const map & m, const tripoint & p ) {
            return m.furn( p ).id();
        } );
        luna::set_fx( ut, "set_furn_at", []( map & m, const tripoint & p, const furn_str_id & id ) {
            m.furn_set( p, id.id() );
        } );
    }

    // Register 'tinymap' class to be used in Lua
    {
        luna::new_usertype<tinymap>( lua, luna::bases<map>(), luna::no_constructor );
    }

    // Register 'item_stack' class to be used in Lua
    {
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

static void reg_distribution_grid( sol::state &lua )
{
    {
        sol::usertype<distribution_grid> ut =
            luna::new_usertype<distribution_grid>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "get_resource", &distribution_grid::get_resource );
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

static void reg_ui_elements( sol::state &lua )
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
        luna::set_fx( ut, "add", []( uilist & ui, int retval, const std::string & text ) {
            ui.addentry( retval, true, MENU_AUTOASSIGN, text );
        } );
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
            popup.message( "%s", fmt_lua_va( va ) );
        } );
        luna::set_fx( ut, "message_color", []( query_popup & popup, color_id col ) {
            popup.default_color( get_all_colors().get( col ) );
        } );
        luna::set_fx( ut, "allow_any_key", []( query_popup & popup, bool val ) {
            popup.allow_anykey( val );
        } );
        luna::set_fx( ut, "query", []( query_popup & popup ) {
            return popup.query().action;
        } );
    }
}

static void reg_coords_library( sol::state &lua )
{
    luna::userlib lib = luna::begin_lib( lua, "coords" );

    luna::set_fx( lib, "ms_to_sm", []( const tripoint & raw ) -> std::tuple<tripoint, point> {
        tripoint_rel_ms fine( raw );
        tripoint_rel_sm rough;
        point_sm_ms remain;
        std::tie( rough, remain ) = coords::project_remain<coords::sm>( fine );
        return std::make_pair( rough.raw(), remain.raw() );
    } );
    luna::set_fx( lib, "ms_to_omt", []( const tripoint & raw ) -> std::tuple<tripoint, point> {
        tripoint_rel_ms fine( raw );
        tripoint_rel_omt rough;
        point_omt_ms remain;
        std::tie( rough, remain ) = coords::project_remain<coords::omt>( fine );
        return std::make_pair( rough.raw(), remain.raw() );
    } );
    luna::set_fx( lib, "ms_to_om", []( const tripoint & raw ) -> std::tuple<point, tripoint> {
        tripoint_rel_ms fine( raw );
        point_rel_om rough;
        coords::coord_point<tripoint, coords::origin::overmap, coords::ms> remain;
        std::tie( rough, remain ) = coords::project_remain<coords::om>( fine );
        return std::make_pair( rough.raw(), remain.raw() );
    } );

    luna::set_fx( lib, "sm_to_ms", []( const tripoint & raw_rough,
    sol::optional<const point &> raw_remain ) -> tripoint {
        tripoint_rel_sm rough( raw_rough );
        point_sm_ms remain( raw_remain ? *raw_remain : point_zero );
        tripoint_rel_ms fine = coords::project_combine( rough, remain );
        return fine.raw();
    } );
    luna::set_fx( lib, "omt_to_ms", []( const tripoint & raw_rough,
    sol::optional<const point &> raw_remain ) -> tripoint {
        tripoint_rel_omt rough( raw_rough );
        point_omt_ms remain( raw_remain ? *raw_remain : point_zero );
        tripoint_rel_ms fine = coords::project_combine( rough, remain );
        return fine.raw();
    } );
    luna::set_fx( lib, "om_to_ms", []( const point & raw_rough,
    sol::optional<const tripoint &> raw_remain ) -> tripoint {
        point_rel_om rough( raw_rough );
        coords::coord_point<tripoint, coords::origin::overmap, coords::ms> remain(
            raw_remain ? *raw_remain : tripoint_zero
        );
        tripoint_rel_ms fine = coords::project_combine( rough, remain );
        return fine.raw();
    } );

    luna::set_fx( lib, "rl_dist", sol::overload(
                      sol::resolve<int( const tripoint &, const tripoint & )>( rl_dist ),
                      sol::resolve<int( const point &, const point & )>( rl_dist )
                  ) );
    luna::set_fx( lib, "trig_dist", sol::overload(
                      sol::resolve<float( const tripoint &, const tripoint & )>( trig_dist ),
                      sol::resolve<float( const point &, const point & )>( trig_dist )
                  ) );
    luna::set_fx( lib, "square_dist", sol::overload(
                      sol::resolve<int( const tripoint &, const tripoint & )>( square_dist ),
                      sol::resolve<int( const point &, const point & )>( square_dist )
                  ) );

    luna::finalize_lib( lib );
}

static void reg_constants( sol::state &lua )
{
    luna::userlib lib = luna::begin_lib( lua, "const" );

    luna::set( lib, "OM_OMT_SIZE", OMAPX );
    luna::set( lib, "OM_SM_SIZE", OMAPX * 2 );
    luna::set( lib, "OM_MS_SIZE", OMAPX * 2 * SEEX );
    luna::set( lib, "OMT_SM_SIZE", 2 );
    luna::set( lib, "OMT_MS_SIZE", SEEX * 2 );
    luna::set( lib, "SM_MS_SIZE", SEEX );

    luna::finalize_lib( lib );
}

template<typename T>
void reg_id( sol::state &lua )
{
    using SID = string_id<T>;

    // Register string_id class under given name
    sol::usertype<SID> ut = luna::new_usertype<SID>( lua, luna::no_bases, luna::constructors <
                            SID(),
                            SID( const SID & ),
                            SID( const char * ),
                            SID( std::string )
                            > ()
                                                   );

    luna::set_fx( ut, "is_null", &SID::is_null );
    luna::set_fx( ut, "is_valid", &SID::is_valid );
    luna::set_fx( ut, "str", &SID::c_str );
    luna::set_fx( ut, "NULL_ID", &SID::NULL_ID );
    luna::set_fx( ut, sol::meta_function::to_string, []( const SID & id ) -> std::string {
        return string_format( "%s[%s]", luna::detail::luna_traits<SID>::name, id.c_str() );
    } );
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

static void reg_colors( sol::state &lua )
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

static void reg_enums( sol::state &lua )
{
    reg_enum<game_message_type>( lua );
}

static void reg_string_ids( sol::state &lua )
{
    reg_id<faction>( lua );
    reg_id<itype>( lua );
    reg_id<ter_t>( lua );
    reg_id<furn_t>( lua );
}

void reg_docced_bindings( sol::state &lua )
{
    reg_creature_family( lua );
    reg_point_tripoint( lua );
    reg_item( lua );
    reg_map( lua );
    reg_distribution_grid( lua );
    reg_ui_elements( lua );
    reg_colors( lua );
    reg_enums( lua );
    reg_string_ids( lua );
    reg_coords_library( lua );
    reg_constants( lua );
}

#endif
