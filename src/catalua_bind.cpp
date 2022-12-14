#ifdef LUA
#include "catalua_bind.h"
#include "catalua_bindings.h"

#include "item.h"
#include "itype.h"
#include "point.h"

LUNA_VAL( bool, "bool" );
LUNA_VAL( int, "int" );
LUNA_VAL( float, "double" );
LUNA_VAL( double, "double" );
LUNA_VAL( void, "void" );
LUNA_VAL( std::string, "string" );

LUNA_VAL( point, "Point" );
LUNA_VAL( tripoint, "Tripoint" );
LUNA_VAL( item, "Item" );

void reg_docced_bindings( sol::state &lua )
{
    // Register 'point' class to be used in Lua
    {
        sol::usertype<point> ut =
            luna::new_usertype<point>(
                lua,
                sol::constructors <
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

    // Register 'item' class to be used in Lua
    {
        sol::usertype<item> ut = luna::new_usertype<item>( lua, sol::no_constructor );

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

#endif
