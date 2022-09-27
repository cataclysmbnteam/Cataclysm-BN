#include "cata_tiles_ui_element.h"

#include "generic_factory.h"

namespace
{
generic_factory<tiles_ui_element> ui_element_factory( "ui_element" );
} // namespace

template<>
const tiles_ui_element &string_id<tiles_ui_element>::obj() const
{
    return ui_element_factory.obj( *this );
}

template<>
bool string_id<tiles_ui_element>::is_valid() const
{
    return ui_element_factory.is_valid( *this );
}

template<>
const tiles_ui_element &int_id<tiles_ui_element>::obj() const
{
    return ui_element_factory.obj( *this );
}

template<>
bool int_id<tiles_ui_element>::is_valid() const
{
    return ui_element_factory.is_valid( *this );
}

template<>
int_id<tiles_ui_element> string_id<tiles_ui_element>::id() const
{
    return ui_element_factory.convert( *this, int_id< tiles_ui_element>() );
}

template<>
int_id<tiles_ui_element>::int_id( const string_id<tiles_ui_element> &id ) : _id( id.id() )
{

}

void tiles_ui_element::reset()
{
    ui_element_factory.reset();
}

void tiles_ui_element::load_ui_element( const JsonObject &jo, const std::string &src )
{
    ui_element_factory.load( jo, src );
}

void tiles_ui_element::finalize()
{
    ui_element_factory.finalize();
}

const std::vector<tiles_ui_element> &tiles_ui_element::get_all()
{
    return ui_element_factory.get_all();
}

void tiles_ui_element::load( const JsonObject &jo, const std::string & )
{
    // No data to load
}
