#include "catalua_bindings.h"

#include "avatar.h"
#include "catalua.h"
#include "catalua_bindings_utils.h"
#include "coordinates.h"
#include "mission.h"
#include "npc.h"
#include "itype.h"
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

    DOC( "Returns the mission's name as a string." );
    SET_FX( name );
    DOC( "Returns the mission type ID of this mission." );
    SET_FX( mission_id );
    DOC( "Returns true if the mission has a deadline." );
    SET_FX( has_deadline );
    DOC( "Returns the mission's deadline as a time_point." );
    SET_FX_T( get_deadline, time_point() const );
    DOC( "Returns the mission description." );
    SET_FX_T( get_description, std::string() const );
    DOC( "Returns true if the mission has a target." );
    SET_FX( has_target );
    // DOC("Returns the target of the mission (pointer to tripoint_abs_omt).");
    // SET_FX_T( &get_target, const tripoint_abs_omt() const );
    // DOC("Returns the mission type of the target (pointer to mission_type).");
    // //SET_FX_T( &get_type, const mission_type() const );
    DOC( "Returns true if the mission has a follow-up mission." );
    SET_FX( has_follow_up );
    DOC( "Returns the follow-up mission type ID." );
    SET_FX_T( get_follow_up, mission_type_id() const );
    DOC( "Returns the mission's value as an integer." );
    SET_FX( get_value );
    DOC( "Returns the mission's unique ID." );
    SET_FX( get_id );
    DOC( "Returns the item ID associated with the mission." );
    SET_FX_T( get_item_id, const itype_id & () const );
    DOC( "Returns the NPC character ID associated with the mission." );
    SET_FX_T( get_npc_id, character_id() const );
    //DOC("Returns likely rewards of the mission (reference to vector of <int, itype_id> pairs).");
    //SET_FX_T( &get_likely_rewards, const std::vector<std::pair<int, itype_id>>() const );
    DOC( "Returns true if the mission has generic rewards." );
    SET_FX( has_generic_rewards );
    DOC( "Returns true if the mission is currently assigned." );
    SET_FX( is_assigned );
    DOC( "Fails the mission." );
    SET_FX( fail );
    DOC( "Wraps up the mission successfully." );
    SET_FX( wrap_up );
    DOC( "Returns true if the mission has failed." );
    SET_FX( has_failed );
    DOC( "Returns true if the mission is currently in progress." );
    SET_FX( in_progress );
    DOC( "Marks a mission step as complete, taking an integer step index." );
    SET_FX( step_complete );

    DOC( "Assigns this mission to the given avatar." );
    luna::set_fx( ut, "assign",
    []( mission & m, avatar & u ) -> void {
        m.assign( u );
    } );

    DOC( "Reserves a new mission of the given type for the specified NPC. Returns the new mission." );
    luna::set_fx( ut, "reserve_new",
    []( const mission_type_id & type, const character_id & npc_id ) -> mission * {
        return mission::reserve_new( type, npc_id );
    } );

    DOC( "Reserves a random mission at the specified origin and position for the given NPC. Returns the new mission." );
    luna::set_fx( ut, "reserve_random",
    []( mission_origin origin, const tripoint & p, const character_id & npc_id ) -> mission * {
        return mission::reserve_random( origin, tripoint_abs_omt( p ), npc_id );
    } );

    reg_serde_functions( ut );

#undef UT_CLASS
};


void cata::detail::reg_mission_type( sol::state &lua )
{
#define UT_CLASS mission_type_id
    auto ut_id = luna::new_usertype<UT_CLASS>(
                     lua,
                     luna::no_bases,
                     luna::constructors <
                     mission_type_id( std::string )
                     > ()
                 );
#undef UT_CLASS
};