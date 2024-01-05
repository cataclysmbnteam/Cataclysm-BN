#ifdef LUA
#include "catalua_bindings.h"

#include "activity_type.h"
#include "bionics.h"
#include "bodypart.h"
#include "catalua_luna_doc.h"
#include "catalua_luna.h"
#include "disease.h"
#include "effect.h"
#include "faction.h"
#include "field_type.h"
#include "flag.h"
#include "flag_trait.h"
#include "itype.h"
#include "json.h"
#include "mapdata.h"
#include "martialarts.h"
#include "monfaction.h"
#include "monstergenerator.h"
#include "morale_types.h"
#include "mutation.h"
#include "recipe.h"
#include "skill.h"
#include "type_id.h"

template<typename T, bool do_int_id>
void reg_id( sol::state &lua )
{
    using SID = string_id<T>;
    using IID = int_id<T>;
    {
        // Register string_id class under given name
        sol::usertype<SID> ut;
        if constexpr( do_int_id ) {
            ut = luna::new_usertype<SID>( lua, luna::no_bases, luna::constructors <
                                          SID(),
                                          SID( const SID & ),
                                          SID( const IID & ),
                                          SID( std::string )
                                          > ()
                                        );
        } else {
            ut = luna::new_usertype<SID>( lua, luna::no_bases, luna::constructors <
                                          SID(),
                                          SID( const SID & ),
                                          SID( std::string )
                                          > ()
                                        );
        }

        luna::set_fx( ut, "obj", []( const SID & sid ) -> const T* {
            return &sid.obj();
        } );
        if constexpr( do_int_id ) {
            luna::set_fx( ut, "int_id", &SID::id );
            luna::set_fx( ut, "implements_int_id", []() {
                return true;
            } );
        } else {
            luna::set_fx( ut, "implements_int_id", []() {
                return false;
            } );
        }
        luna::set_fx( ut, "is_null", &SID::is_null );
        luna::set_fx( ut, "is_valid", &SID::is_valid );
        luna::set_fx( ut, "str", &SID::c_str );
        luna::set_fx( ut, "NULL_ID", &SID::NULL_ID );
        luna::set_fx( ut, sol::meta_function::to_string, []( const SID & id ) -> std::string {
            return string_format( "%s[%s]", luna::detail::luna_traits<SID>::name, id.c_str() );
        } );

        // (De-)Serialization
        luna::set_fx( ut, "serialize", []( const SID & ut, JsonOut & jsout ) {
            jsout.write( ut.str() );
        } );
        luna::set_fx( ut, "deserialize", []( SID & ut, JsonIn & jsin ) {
            ut = SID( jsin.get_string() );
        } );
    }
    if constexpr( do_int_id ) {
        // Register int_id class under given name
        sol::usertype<IID> ut = luna::new_usertype<IID>( lua, luna::no_bases, luna::constructors <
                                IID(),
                                IID( const IID & ),
                                IID( const SID & )
                                > ()
                                                       );

        luna::set_fx( ut, "obj", []( const IID & iid ) -> const T* {
            return &iid.obj();
        } );
        luna::set_fx( ut, "str_id", &IID::id );
        luna::set_fx( ut, "is_valid", &IID::is_valid );
        luna::set_fx( ut, sol::meta_function::to_string, []( const IID & id ) -> std::string {
            return string_format( "%s[%d][%s]", luna::detail::luna_traits<IID>::name, id.to_i(), id.is_valid() ? id.id().c_str() : "<invalid>" );
        } );
    }
}

void cata::detail::reg_game_ids( sol::state &lua )
{
    // Don't forget to define comparison operators for your target type!
    // Some already may have them, but for the rest you can do it
    // with LUA_TYPE_OPS macro.

    reg_id<activity_type, false>( lua );
    reg_id<bionic_data, false>( lua );
    reg_id<body_part_type, true>( lua );
    reg_id<disease_type, false>( lua );
    reg_id<effect_type, false>( lua );
    reg_id<faction, false>( lua );
    reg_id<field_type, true>( lua );
    reg_id<furn_t, true>( lua );
    reg_id<itype, false>( lua );
    reg_id<json_flag, false>( lua );
    reg_id<json_trait_flag, false>( lua );
    reg_id<ma_buff, false>( lua );
    reg_id<monfaction, true>( lua );
    reg_id<morale_type_data, false>( lua );
    reg_id<mutation_branch, false>( lua );
    reg_id<mutation_category_trait, false>( lua );
    reg_id<recipe, false>( lua );
    reg_id<Skill, false>( lua );
    reg_id<species_type, false>( lua );
    reg_id<ter_t, true>( lua );

}

void cata::detail::reg_types( sol::state &lua )
{
    {
        sol::usertype<faction> ut =
            luna::new_usertype<faction>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "str_id", []( const faction & x ) -> faction_id {
            return x.id;
        } );

        // Factions are a pain because they _inherit_ from their type, not reference it by id.
        // This causes various weirdness, so let's omit the fields for now.
    }
    {
        sol::usertype<ter_t> ut =
            luna::new_usertype<ter_t>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "str_id", []( const ter_t & x ) -> ter_str_id {
            return x.id;
        } );
        luna::set_fx( ut, "int_id", []( const ter_t & x ) -> ter_id {
            return x.id.id();
        } );

        luna::set( ut, "open", &ter_t::open );
        luna::set( ut, "close", &ter_t::close );
        luna::set( ut, "trap_id_str", &ter_t::trap_id_str );
        luna::set( ut, "transforms_into", &ter_t::transforms_into );
        luna::set( ut, "roof", &ter_t::roof );
        luna::set( ut, "heat_radiation", &ter_t::heat_radiation );
    }
    {
        sol::usertype<furn_t> ut =
            luna::new_usertype<furn_t>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "str_id", []( const furn_t &x ) -> furn_str_id {
            return x.id;
        } );
        luna::set_fx( ut, "int_id", []( const furn_t &x ) -> furn_id {
            return x.id.id();
        } );

        luna::set( ut, "open", &furn_t::open );
        luna::set( ut, "close", &furn_t::close );
        luna::set( ut, "transforms_into", &furn_t::transforms_into );
    }
}

#endif
