#pragma once
#ifndef CATA_SRC_CATALUA_BINDINGS_UTILS_H
#define CATA_SRC_CATALUA_BINDINGS_UTILS_H

#include "catalua_luna.h"
#include "json.h"

template<typename T>
void reg_serde_functions( sol::usertype<T> &ut )
{
    luna::set_fx( ut, "serialize", sol::resolve< void( JsonOut & ) const >( &T::serialize ) );
    luna::set_fx( ut, "deserialize", sol::resolve< void( JsonIn & ) >( &T::deserialize ) );
}

#define DOC( x ) luna::doc( x )

#endif // CATA_SRC_CATALUA_BINDINGS_UTILS_H
