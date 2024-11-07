#include <unordered_map>

#include "debug.h"
#include "flag_trait.h"
#include "json.h"
#include "type_id.h"
#include "generic_factory.h"

namespace
{
generic_factory<json_trait_flag> json_trait_flags_all( "json_trait_flags" );
} // namespace

/** @relates int_id */
template<>
bool trait_flag_id ::is_valid() const
{
    return json_trait_flags_all.is_valid( *this );
}

/** @relates int_id */
template<>
const json_trait_flag &trait_flag_id::obj() const
{
    return json_trait_flags_all.obj( *this );
}

/** @relates int_id */
template<>
const trait_flag_str_id &trait_flag_id::id() const
{
    return json_trait_flags_all.convert( *this );
}

/** @relates string_id */
template<>
bool trait_flag_str_id ::is_valid() const
{
    return json_trait_flags_all.is_valid( *this );
}

/** @relates string_id */
template<>
const json_trait_flag &trait_flag_str_id::obj() const
{
    return json_trait_flags_all.obj( *this );
}

/** @relates string_id */
template<>
trait_flag_id trait_flag_str_id::id() const
{
    return json_trait_flags_all.convert( *this, trait_flag_id( -1 ) );
}

/** @relates int_id */
template<>
trait_flag_id::int_id( const trait_flag_str_id &id ) : _id( id.id() )
{
}

json_trait_flag::operator bool() const
{
    return id.is_valid();
}

const json_trait_flag &json_trait_flag::get( const std::string &id )
{
    static const json_trait_flag null_value = json_trait_flag();
    const trait_flag_str_id f_id( id );
    return f_id.is_valid() ? *f_id : null_value;
}

void json_trait_flag::load( const JsonObject &, const std::string & )
{
}

void json_trait_flag::check_consistency()
{
    json_trait_flags_all.check();
}

void json_trait_flag::reset()
{
    json_trait_flags_all.reset();
}

void json_trait_flag::load_all( const JsonObject &jo, const std::string &src )
{
    json_trait_flags_all.load( jo, src );
}

void json_trait_flag::check() const
{
    for( const auto &conflicting : conflicts_ ) {
        if( !trait_flag_str_id( conflicting ).is_valid() ) {
            debugmsg( "trait flag definition %s specifies unknown conflicting field %s", id.str(),
                      conflicting );
        }
    }
}

void json_trait_flag::finalize_all()
{
    json_trait_flags_all.finalize();
}

bool json_trait_flag::is_ready()
{
    return !json_trait_flags_all.empty();
}

const std::vector<json_trait_flag> &json_trait_flag::get_all()
{
    return json_trait_flags_all.get_all();
}
