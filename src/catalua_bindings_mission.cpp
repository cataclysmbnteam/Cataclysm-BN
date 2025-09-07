#include "catalua_bindings.h"

#include "avatar.h"
#include "catalua.h"
#include "catalua_bindings_utils.h"
#include "coordinates.h"
#include "mission.h"
#include "npc.h"
#include "translations.h"
#include "catalua_impl.h"
#include "catalua_log.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

void cata::detail::reg_mission_family( sol::state &lua )
{
    reg_mission( lua );
    reg_mission_type( lua );
}

void cata::detail::reg_mission( sol::state &lua )
{
#define UT_CLASS mission
    sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors <
            // Define your actual constructors here
            mission()
            > ()
        );

    //SET_FX_T( mission_id, mission_id const );


    DOC( "Returns true if the mission has a deadline." );
    SET_FX_T( name,         std::string() );
    SET_FX_T( mission_id,         mission_type_id() );
    SET_FX_T( has_deadline,         bool() const );
    SET_FX_T( get_deadline,         time_point() const );
    SET_FX_T( get_description,         std::string() const );
    SET_FX_T( has_target,         bool() const );
    //SET_FX_T( get_target,         const tripoint_abs_omt() const );
    //SET_FX_T( get_type,         const mission_type() const );
    SET_FX_T( has_follow_up,         bool() const );
    SET_FX_T( get_follow_up,         mission_type_id() const );
    SET_FX_T( get_value,         int() const );
    SET_FX_T( get_id,         int() const );
    //SET_FX_T( &get_item_id,         const itype_id() const );
    SET_FX_T( get_npc_id,         character_id() const );
    //SET_FX_T( &get_likely_rewards,         const std::vector<std::pair<int, itype_id>>() const );
    SET_FX_T( has_generic_rewards,         bool() const );
    SET_FX_T( is_assigned,         bool() const );
    SET_FX_T( fail,         void() );
    SET_FX_T( wrap_up,         void() );
    SET_FX_T( has_failed,         bool() const );
    SET_FX_T( in_progress,         bool() const );
    SET_FX_T( step_complete, void( int ) );

    luna::set_fx( ut, "assign",
    []( mission & m, avatar & u ) -> void {
        m.assign( u );
    } );

    luna::set_fx( ut, "reserve_new",
    []( const mission_type_id & type, const character_id & npc_id ) -> mission * {
        return mission::reserve_new( type, npc_id );
    } );

    luna::set_fx( ut, "reserve_random",
    []( mission_origin origin, const tripoint & p, const character_id & npc_id ) -> mission * {
        return mission::reserve_random( origin, tripoint_abs_omt( p ), npc_id );
    } );

    // Add (de-)serialization functions so we can carry
    // our horde over the save/load boundary
    reg_serde_functions( ut );

    // Add more stuff like arithmetic operators, to_string operator, etc.
#undef UT_CLASS
};


void cata::detail::reg_mission_type( sol::state &lua )
{
#define UT_CLASS mission_type_id
    // mission_type_id
    auto ut_id = luna::new_usertype<mission_type_id>(
                     lua,
                     luna::no_bases,
                     luna::constructors <
                     mission_type_id( std::string )
                     > ()
                 );
#undef UT_CLASS
};