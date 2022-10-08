#include "flag.h"

#include <unordered_map>

#include "debug.h"
#include "json.h"
#include "type_id.h"
#include "generic_factory.h"

const flag_str_id flag_NULL = flag_str_id( "null" ); // intentionally invalid flag

namespace
{
generic_factory<json_flag> json_flags_all( "json_flags" );
} // namespace

/** @relates int_id */
template<>
auto flag_id ::is_valid() const -> bool
{
    return json_flags_all.is_valid( *this );
}

/** @relates int_id */
template<>
auto flag_id::obj() const -> const json_flag &
{
    return json_flags_all.obj( *this );
}

/** @relates int_id */
template<>
auto flag_id::id() const -> const flag_str_id &
{
    return json_flags_all.convert( *this );
}

/** @relates string_id */
template<>
auto flag_str_id ::is_valid() const -> bool
{
    return json_flags_all.is_valid( *this );
}

/** @relates string_id */
template<>
auto flag_str_id::obj() const -> const json_flag &
{
    return json_flags_all.obj( *this );
}

/** @relates string_id */
template<>
auto flag_str_id::id() const -> flag_id
{
    return json_flags_all.convert( *this, flag_id( -1 ) );
}

/** @relates int_id */
template<>
flag_id::int_id( const flag_str_id &id ) : _id( id.id() )
{
}

json_flag::operator bool() const
{
    return id.is_valid();
}

auto json_flag::get( const std::string &id ) -> const json_flag &
{
    static const json_flag null_value = json_flag();
    const flag_str_id f_id( id );
    return f_id.is_valid() ? *f_id : null_value;
}

void json_flag::load( const JsonObject &jo, const std::string & )
{
    // TODO: mark fields as mandatory where appropriate
    optional( jo, was_loaded, "info", info_ );
    optional( jo, was_loaded, "conflicts", conflicts_ );
    optional( jo, was_loaded, "inherit", inherit_, true );
    optional( jo, was_loaded, "craft_inherit", craft_inherit_, false );
    optional( jo, was_loaded, "requires_flag", requires_flag_ );
    optional( jo, was_loaded, "taste_mod", taste_mod_ );
    optional( jo, was_loaded, "restriction", restriction_ );

    // FIXME: most flags have a "context" field that isn't used for anything
    // Test for it here to avoid errors about unvisited members
    jo.get_member( "context" );
}

void json_flag::check_consistency()
{
    json_flags_all.check();
}

void json_flag::reset()
{
    json_flags_all.reset();
}

void json_flag::load_all( const JsonObject &jo, const std::string &src )
{
    json_flags_all.load( jo, src );
}

void json_flag::check() const
{
    for( const auto &conflicting : conflicts_ ) {
        if( !flag_str_id( conflicting ).is_valid() ) {
            debugmsg( "flag definition %s specifies unknown conflicting field %s", id.str(),
                      conflicting );
        }
    }
}

void json_flag::finalize_all()
{
    json_flags_all.finalize();
}

auto json_flag::is_ready() -> bool
{
    return !json_flags_all.empty();
}

auto json_flag::get_all() -> const std::vector<json_flag> &
{
    return json_flags_all.get_all();
}
