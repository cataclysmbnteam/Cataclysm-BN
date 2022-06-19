#include "catch/catch.hpp"
#include "stringmaker.h"

#include "avatar.h"
#include "map.h"
#include "map_helpers.h"
#include "map_setup_helpers.h"
#include "point.h"
#include "player_helpers.h"
#include "string_formatter.h"
#include "type_id.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vpart_position.h"
#include "vpart_range.h"
#include "units_utility.h"

#include <sstream>

namespace Catch
{
template<>
struct StringMaker<map_helpers::canvas> {
    static std::string convert( const map_helpers::canvas &c ) {
        return c.to_string();
    }
};
} // namespace Catch

static map_helpers::canvas_legend legend = {{
        { U'.', "t_pavement" },
        { U'x', "t_railroad_track" },
    }
};

enum class tcscope {
    full,
    no_back_turns,
    no_back_move
};

struct test_case {
    std::string veh_id;

    tcscope scope;

    point start_pos;
    units::angle start_dir;

    point end_pos_straight;
    units::angle end_dir_straight;

    point end_pos_left;
    units::angle end_dir_left;

    point end_pos_right;
    units::angle end_dir_right;

    map_helpers::canvas canvas;

    test_case( const std::string &veh_id, tcscope scope, units::angle start_dir,
               units::angle end_dir_straight, units::angle end_dir_left,
               units::angle end_dir_right, map_helpers::canvas &&canvas_arg ) :
        veh_id( veh_id ), scope( scope ), start_dir( start_dir ),
        end_dir_straight( end_dir_straight ), end_dir_left( end_dir_left ),
        end_dir_right( end_dir_right ), canvas( canvas_arg ) {
        start_pos = canvas.replace_unique( U'*', U'x' );
        end_pos_straight = canvas.replace_unique( U'o', U'x' );
        end_pos_left = canvas.replace_opt( U'l', U'x' ).value_or( end_pos_straight );
        end_pos_right = canvas.replace_opt( U'r', U'x' ).value_or( end_pos_straight );
    }
};

const efftype_id effect_blind( "blind" );

static void clear_game( const ter_id &terrain )
{
    // Set to turn 0 to prevent solars from producing power
    calendar::turn = calendar::turn_zero;
    clear_avatar();
    clear_creatures();
    clear_npcs();
    clear_vehicles();

    avatar &u = get_avatar();
    // Move player somewhere safe
    REQUIRE_FALSE( u.in_vehicle );
    u.setpos( tripoint_zero );
    // Blind the player to avoid needless drawing-related overhead
    u.add_effect( effect_blind, 365_days, num_bp );

    build_test_map( terrain );
}

static void build_map_from_canvas( const map_helpers::canvas &canvas, const tripoint &canvas_pos )
{
    auto adapter = map_helpers::canvas_adapter( legend )
    .with_setter( [canvas_pos]( const point & p, const std::string & s ) {
        get_map().ter_set( p + canvas_pos, ter_str_id( s ).id() );
    } )
    .with_getter( [canvas_pos]( const point & p ) {
        return get_map().ter( p + canvas_pos ).id().str();
    } );

    adapter.set_all( canvas );

    // Sanity check
    adapter.check_matches_expected( canvas, true );
}

static void test_rail_movement( const std::string &veh_id,
                                int move_dir,
                                tripoint vehicle_pos,
                                units::angle face_dir,
                                units::angle turn_delta,
                                tripoint expected_pos,
                                units::angle expected_dir )
{
    CAPTURE( vehicle_pos );
    CAPTURE( face_dir );
    CAPTURE( turn_delta );
    CAPTURE( expected_pos );
    CAPTURE( expected_dir );

    map &here = get_map();
    vehicle *veh_ptr = here.add_vehicle( vproto_id( veh_id ), vehicle_pos, face_dir, 100, 0 );

    REQUIRE( veh_ptr != nullptr );

    vehicle &veh = *veh_ptr;

    // Position passed to add_vehicle is the desired position of the vehicle's (0,0) part.
    // However, for ease of testing we want to deal with positions of pivot.
    // As such, shift the vehicle as necessary so vehicle_pos is the pivot pos.
    tripoint pivot_fix_delta = vehicle_pos - veh.global_pos3();
    bool displaced_ok = here.displace_vehicle( veh, pivot_fix_delta, true );
    if( !displaced_ok ) {
        CAPTURE( vehicle_pos );
        CAPTURE( veh.global_pos3() );
        CAPTURE( pivot_fix_delta );
        REQUIRE( displaced_ok );
    }

    // Check that pivot pos is right where we want it
    REQUIRE( veh.global_pos3() == vehicle_pos );

    // Remove all items from cargo to normalize weight.
    // Keep fuel in tanks to allow cruise control.
    for( const vpart_reference vp : veh.get_all_parts() ) {
        veh_ptr->get_items( vp.part_index() ).clear();
    }
    for( const vpart_reference vp : veh.get_avail_parts( "OPENABLE" ) ) {
        veh.close( vp.part_index() );
    }

    veh.refresh_insides();

    veh.tags.insert( "IN_CONTROL_OVERRIDE" );
    veh.engine_on = true;

    int tgt_velocity = 200 * move_dir;
    REQUIRE( veh.safe_velocity( true ) >= std::abs( tgt_velocity ) );
    veh.cruise_on = true;
    veh.cruise_velocity = tgt_velocity;
    veh.velocity = tgt_velocity;
    veh.vertical_velocity = 0;
    veh.turn_dir = normalize( face_dir + turn_delta );

    std::stringstream scan_log;
    scan_log << "\n";

    int cycles_left = 80;
    for( ;; ) {
        if( cycles_left == 0 ) {
            scan_log << "exceeded max scan cycles\n";
            break;
        }
        cycles_left -= 1;
        here.vehmove();
        veh.idle( true );
        // If the vehicle starts skidding, the effects become random and test is RUINED
        REQUIRE( !veh.skidding );
        for( const tripoint &pos : veh.get_points() ) {
            REQUIRE( here.ter( pos ) );
        }

        scan_log << string_format( "pos: %s dir: %d vel: %d/%d\n",
                                   veh.global_pos3().to_string(),
                                   static_cast<int>( units::to_degrees( veh.face.dir() ) ),
                                   veh.velocity,
                                   veh.vertical_velocity
                                 );

        if( veh.global_pos3() == expected_pos ) {
            break;
        }
    }

    tripoint got_pos = veh.global_pos3();
    units::angle got_dir = normalize( veh.face.dir() );
    CAPTURE( got_pos );
    CAPTURE( got_dir );

    if( units::to_degrees( got_dir ) != Approx( units::to_degrees( expected_dir ) ) ||
        got_pos != expected_pos ) {
        CAPTURE( scan_log.str() );
        FAIL( "direction and/or position mismatch" );
    } else {
        SUCCEED();
    }
}

constexpr units::angle turn_step = 15_degrees;

static void run_test_case_at_rotation( const test_case &t, int i_rot )
{
    CAPTURE( i_rot );
    map_helpers::canvas canvas = t.canvas.rotated( i_rot );
    tripoint canvas_pos = tripoint( ( point( MAPSIZE_X, MAPSIZE_Y ) - canvas.size() ) / 2, 0 );

    point sz = t.canvas.size();
    tripoint start_pos = canvas_pos + t.start_pos.rotate( i_rot, sz );
    tripoint end_pos_s = canvas_pos + t.end_pos_straight.rotate( i_rot, sz );
    tripoint end_pos_l = canvas_pos + t.end_pos_left.rotate( i_rot, sz );
    tripoint end_pos_r = canvas_pos + t.end_pos_right.rotate( i_rot, sz );

    units::angle rot = i_rot * 90_degrees;
    units::angle start_dir = normalize( t.start_dir + rot );
    units::angle end_dir_s = normalize( t.end_dir_straight + rot );
    units::angle end_dir_l = normalize( t.end_dir_left + rot );
    units::angle end_dir_r = normalize( t.end_dir_right + rot );

    clear_game( t_floor );
    build_map_from_canvas( canvas, canvas_pos );

    int i = 16 * i_rot;
    const auto sn = [&]( const char *s ) {
        // Catch breaks when same "leaf" sections are executed multiple times,
        // so we have to generate unique name for each invocation.
        return std::string( s ) + "   sid=" + std::to_string( i ) + t.veh_id;
    };
    WHEN( sn( "moving forward" ) ) {
        AND_WHEN( sn( "not trying to turn " ) ) {
            test_rail_movement( t.veh_id, 1, start_pos, start_dir,
                                0_degrees, end_pos_s, end_dir_s );
        }
        AND_WHEN( sn( "trying to turn right " ) ) {
            test_rail_movement( t.veh_id, 1, start_pos, start_dir,
                                turn_step, end_pos_r, end_dir_r );
        }
        AND_WHEN( sn( "trying to turn left " ) ) {
            test_rail_movement( t.veh_id, 1, start_pos, start_dir,
                                -turn_step, end_pos_l, end_dir_l );
        }
    }
    if( t.scope != tcscope::no_back_move ) {
        WHEN( sn( "moving backwards from straight path" ) ) {
            AND_WHEN( sn( "not trying to turn" ) ) {
                test_rail_movement( t.veh_id, -1, end_pos_s, end_dir_s,
                                    0_degrees, start_pos, start_dir );
            }
            if( t.scope != tcscope::no_back_turns ) {
                AND_WHEN( sn( "trying to turn right" ) ) {
                    test_rail_movement( t.veh_id, -1, end_pos_s, end_dir_s,
                                        turn_step, start_pos, start_dir );
                }
                AND_WHEN( sn( "trying to turn left" ) ) {
                    test_rail_movement( t.veh_id, -1, end_pos_s, end_dir_s,
                                        -turn_step, start_pos, start_dir );
                }
            }
        }
        if( end_pos_r != end_pos_s ) {
            WHEN( sn( "moving backwards from right path" ) ) {
                AND_WHEN( sn( "not trying to turn" ) ) {
                    test_rail_movement( t.veh_id, -1, end_pos_r, end_dir_r,
                                        0_degrees, start_pos, start_dir );
                }
                if( t.scope != tcscope::no_back_turns ) {
                    AND_WHEN( sn( "trying to turn right" ) ) {
                        test_rail_movement( t.veh_id, -1, end_pos_r, end_dir_r,
                                            turn_step, start_pos, start_dir );
                    }
                    AND_WHEN( sn( "trying to turn left" ) ) {
                        test_rail_movement( t.veh_id, -1, end_pos_r, end_dir_r,
                                            -turn_step, start_pos, start_dir );
                    }
                }
            }
        }
        if( end_pos_l != end_pos_s ) {
            WHEN( sn( "moving backwards from left path" ) ) {
                AND_WHEN( sn( "not trying to turn" ) ) {
                    test_rail_movement( t.veh_id, -1, end_pos_l, end_dir_l,
                                        0_degrees, start_pos, start_dir );
                }
                if( t.scope != tcscope::no_back_turns ) {
                    AND_WHEN( sn( "trying to turn right" ) ) {
                        test_rail_movement( t.veh_id, -1, end_pos_l, end_dir_l,
                                            turn_step, start_pos, start_dir );
                    }
                    AND_WHEN( sn( "trying to turn left" ) ) {
                        test_rail_movement( t.veh_id, -1, end_pos_l, end_dir_l,
                                            -turn_step, start_pos, start_dir );
                    }
                }
            }
        }
    }
}

static void run_test_case( const test_case &t )
{
    CAPTURE( t.veh_id );
    for( int i_rot = 0; i_rot < 4; i_rot++ ) {
        run_test_case_at_rotation( t, i_rot );
    }
}

map_helpers::canvas empty_terrain()
{
    return { {
            U".........",
            U".........",
            U".........",
            U".........",
            U"..l.o.r..",
            U".........",
            U".........",
            U".........",
            U".........",
            U".........",
            U".........",
            U".........",
            U".........",
            U".........",
            U".........",
            U"....*....",
            U".........",
            U".........",
            U".........",
            U".........",
        }
    };
}

map_helpers::canvas rails_straight()
{
    return { {
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..o..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..*..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
            U".x..x..x.",
        }
    };
}

map_helpers::canvas rails_diag_start()
{
    return { {
            U"................x..x..x.",
            U"...............x..x..x..",
            U"..............x..x..x...",
            U".............x..x..x....",
            U"............x..o..x.....",
            U"...........x..x..x......",
            U"..........x..x..x.......",
            U".........x..x..x........",
            U"........x..x..x.........",
            U".......x..x..x..........",
            U"......x..x..x...........",
            U".....x..x..x............",
            U"....x..x..x.............",
            U"...x..x..x..............",
            U"..x..x..x...............",
            U".x..x..x................",
            U".x..x..x................",
            U".x..x..x................",
            U".x..x..x................",
            U".x..x..x................",
            U".x..x..x................",
            U".x..*..x................",
            U".x..x..x................",
            U".x..x..x................",
            U".x..x..x................",
            U".x..x..x................",
        }
    };
}

map_helpers::canvas rails_diag_end()
{
    return { {
            U"..............................",
            U".....................xxxxxxxxx",
            U"....................x.........",
            U"...................x..........",
            U"..................x..xxxxoxxxx",
            U".................x..x.........",
            U"................x..x..........",
            U"...............x..x..xxxxxxxxx",
            U"..............x..x..x.........",
            U".............x..x..x..........",
            U"............x..x..x...........",
            U"...........x..x..x............",
            U"..........x..x..x.............",
            U".........x..x..x..............",
            U"........x..x..x...............",
            U".......x..x..x................",
            U"......x..x..x.................",
            U".....x..x..x..................",
            U"....x..*..x...................",
            U"...x..x..x....................",
            U"..x..x..x.....................",
            U".x..x..x......................",
            U"x..x..x.......................",
            U"..x..x........................",
            U".x..x.........................",
            U"x..x..........................",
            U"..x...........................",
            U".x............................",
            U"x.............................",
            U"..............................",
        }
    };
}

map_helpers::canvas rails_cross()
{
    return { {
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..o..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..*..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
            U"..............x..x..x.............",
        }
    };
}

map_helpers::canvas rails_tee_straight()
{
    return { {
            U".x..x..x........x..x..x................",
            U"..x..x..x.......x..x..x................",
            U"...x..x..x......x..x..x................",
            U"....x..x..x.....x..x..x................",
            U".....x..l..x....x..o..x................",
            U"......x..x..x...x..x..x................",
            U".......x..x..x..x..x..x................",
            U"........x..x..x.x..x..x................",
            U".........x..x..xx..x..x................",
            U"..........x..x..x..x..x................",
            U"...........x..x.xx.x..x................",
            U"............x..xx.xx..x................",
            U".............x..x..x..x........x..x..x.",
            U"..............x.xx.xx.x.......x..x..x..",
            U"...............xx.xx.xx......x..x..x...",
            U"................x..x..x.....x..x..x....",
            U"................x..x..x....x..r..x.....",
            U"................x..x..x...x..x..x......",
            U"................x..x..x..x..x..x.......",
            U"................x..x..x.x..x..x........",
            U"................x..x..xx..x..x.........",
            U"................x..x..x..x..x..........",
            U"................x..x.xx.x..x...........",
            U"................x..xx.xx..x............",
            U"................x..x..x..x.............",
            U"................x.xx.xx.x..............",
            U"................xx.xx.xx...............",
            U"................x..x..x................",
            U"................x..x..x................",
            U"................x..*..x................",
            U"................x..x..x................",
            U"................x..x..x................",
            U"................x..x..x................",
            U"................x..x..x................",
        }
    };
}

map_helpers::canvas rails_tee_diag()
{
    return { {
            U"..................x..x..x.........x..x..x..",
            U"..................x..x..x........x..x..x...",
            U"..................x..x..x.......x..x..x....",
            U"..................x..x..x......x..x..x.....",
            U"..................x..l..x.....x..o..x......",
            U"..................x..x..x....x..x..x.......",
            U"..................x..x..x...x..x..x........",
            U"..................x..x..x..x..x..x.........",
            U"..................x..x..x.x..x..x..........",
            U"..................x..x..xx..x..x...........",
            U"..................x..x..x..x..x............",
            U"..................x..x.xx.x..x.............",
            U"..................x..xx.xx..x..............",
            U"..................x..x..x..x...............",
            U"..................x.xx.xx.x................",
            U"..................xx.xx.xx.................",
            U"..................x..x..x..................",
            U".................x..x..x...................",
            U"................x..x..x....................",
            U"...............x..x..x.....................",
            U"..............x..x..x......................",
            U".............xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            U"............x..x..x........................",
            U"...........x..x..x.........................",
            U"..........x..xxxxxxxxxxxxxxxxxxxxxxxxrxxxxx",
            U".........x..x..x...........................",
            U"........x..x..x............................",
            U".......x..x..xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            U"......x..x..x..............................",
            U".....x..x..x...............................",
            U"....x..*..x................................",
            U"...x..x..x.................................",
            U"..x..x..x..................................",
            U".x..x..x...................................",
            U"x..x..x....................................",
            U"..x..x.....................................",
            U".x..x......................................",
            U"x..x.......................................",
            U"..x........................................",
            U".x.........................................",
            U"x..........................................",
            U"...........................................",
        }
    };
}

map_helpers::canvas rails_straight_shifting()
{
    return { {
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..o..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"...x..x..x.",
            U"..x..x..x..",
            U"..x..x..x..",
            U"..x..x..x..",
            U"..x..x..x..",
            U"..x..x..x..",
            U"..x..x..x..",
            U".x..x..x...",
            U".x..x..x...",
            U".x..x..x...",
            U".x..x..x...",
            U".x..x..x...",
            U".x..x..x...",
            U".x..*..x...",
            U".x..x..x...",
            U".x..x..x...",
            U".x..x..x...",
            U".x..x..x...",
        }
    };
}

map_helpers::canvas rails_diag_shifting()
{
    return { {
            U"....................x..x..x..",
            U"...................x..x..x...",
            U"..................x..x..x....",
            U".................x..x..x.....",
            U"................x..o..x......",
            U"...............x..x..x.......",
            U"..............x..x..x........",
            U".............x..x..x.........",
            U"............x..x..x..........",
            U"...........x..x..x...........",
            U"..........x..x..x............",
            U"..........x..x..x............",
            U".........x..x..x.............",
            U"........x..x..x..............",
            U".......x..x..x...............",
            U".......x..x..x...............",
            U"......x..x..x................",
            U".....x..x..x.................",
            U"....x..x..x..................",
            U"...x..x..x...................",
            U"..x..*..x....................",
            U".x..x..x.....................",
            U"x..x..x......................",
            U"..x..x.......................",
            U".x..x........................",
            U"x..x.........................",
            U"..x..........................",
            U".x...........................",
            U"x............................"
        }
    };
}

TEST_CASE( "vehicle_rail_movement", "[vehicle][railroad]" )
{
    SECTION( "no_rails" ) {
        // On normal ground rail vehicle behaves like normal vehicle
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::no_back_move,
            -90_degrees,
            -90_degrees,
            -90_degrees - turn_step,
            -90_degrees + turn_step,
            empty_terrain()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::no_back_move,
            -90_degrees,
            -90_degrees,
            -90_degrees - turn_step,
            -90_degrees + turn_step,
            empty_terrain()
        } );
    }
    SECTION( "straight_rails" ) {
        // Rail vehicle must follow straight rails regardless of desired turn dir
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            rails_straight()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            rails_straight()
        } );
    }
    SECTION( "enter_diagonal" ) {
        // Rail vehicle must follow tracks and turn regardless of desired turn dir
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::full,
            -90_degrees,
            -45_degrees,
            -45_degrees,
            -45_degrees,
            rails_diag_start()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -90_degrees,
            -45_degrees,
            -45_degrees,
            -45_degrees,
            rails_diag_start()
        } );
    }
    SECTION( "leave_diagonal" ) {
        // Rail vehicle must follow tracks and turn regardless of desired turn dir
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::full,
            -45_degrees,
            0_degrees,
            0_degrees,
            0_degrees,
            rails_diag_end()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -45_degrees,
            0_degrees,
            0_degrees,
            0_degrees,
            rails_diag_end()
        } );
    }
    SECTION( "rail_crossing" ) {
        // Rail vehicle must follow straight rails regardless of desired turn dir
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            rails_cross()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            rails_cross()
        } );
    }
    SECTION( "rails_tee_straight" ) {
        // Rail vehicle must follow straight rails by default,
        // but can switch tracks depending on desired turn dir
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::no_back_turns,
            -90_degrees,
            -90_degrees,
            -90_degrees - 45_degrees,
            -90_degrees + 45_degrees,
            rails_tee_straight()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees - 45_degrees,
            -90_degrees + 45_degrees,
            rails_tee_straight()
        } );
    }
    SECTION( "rails_tee_diag" ) {
        // Rail vehicle must follow straight rails by default,
        // but can switch tracks depending on desired turn dir
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::no_back_turns,
            -45_degrees,
            -45_degrees,
            -45_degrees - 45_degrees,
            -45_degrees + 45_degrees,
            rails_tee_diag()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -45_degrees,
            -45_degrees,
            -45_degrees - 45_degrees,
            -45_degrees + 45_degrees,
            rails_tee_diag()
        } );
    }

    SECTION( "rails_straight_shifting" ) {
        // Rail vehicle must shift by 1 tile left or right
        // if the rails shift left or right
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            rails_straight_shifting()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            -90_degrees,
            rails_straight_shifting()
        } );
    }
    SECTION( "rails_diag_shifting" ) {
        // Same as above, but for diagonal case
        run_test_case( test_case{
            "motorcycle_rail",
            tcscope::full,
            -45_degrees,
            -45_degrees,
            -45_degrees,
            -45_degrees,
            rails_diag_shifting()
        } );

        run_test_case( test_case{
            "motorized_draisine_trirail",
            tcscope::full,
            -45_degrees,
            -45_degrees,
            -45_degrees,
            -45_degrees,
            rails_diag_shifting()
        } );
    }
}
