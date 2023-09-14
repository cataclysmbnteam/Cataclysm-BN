#include "catch/catch.hpp"

#include <cstdlib>
#include <map>
#include <utility>

#include "creature.h"
#include "monster.h"
#include "mtype.h"
#include "test_statistics.h"
#include "bodypart.h"
#include "rng.h"

float expected_weights_base[][12] = { { 20, 0,   0,   0, 15, 15, 0, 0, 25, 25, 0, 0 },
    { 33.33, 2.4, 0.32, 0, 10.65, 10.65, 0, 0, 9.91, 9.91, 0, 0 },
    { 36.57, 6.09,   .55,  0, 13.57, 13.57, 0, 0, 4.99, 4.99, 0, 0 }
};

float expected_weights_max[][12] = { { 2000, 0,   0,   0, 1191.49, 1191.49, 0, 0, 2228.12, 2228.12, 0, 0 },
    { 3333, 1516.92, 78.99, 0, 465.08, 465.08, 0, 0, 625.41, 625.41, 0, 0 },
    { 3657, 3842.91,   139.72,  0, 374.07, 374.07, 0, 0, 315.09, 315.09, 0, 0 }
};

static void calculate_bodypart_distribution( const enum m_size asize, const enum m_size dsize,
        const int hit_roll, float ( &expected )[12] )
{
    INFO( "hit roll = " << hit_roll );
    std::map<body_part, int> selected_part_histogram = {
        { bp_torso, 0 }, { bp_head, 0 }, { bp_eyes, 0 }, { bp_mouth, 0 }, { bp_arm_l, 0 }, { bp_arm_r, 0 },
        { bp_hand_l, 0 }, { bp_hand_r, 0 }, { bp_leg_l, 0 }, { bp_leg_r, 0 }, { bp_foot_l, 0 }, { bp_foot_r, 0 }
    };

    mtype atype;
    atype.size = asize;
    monster attacker;
    attacker.type = &atype;
    mtype dtype;
    dtype.size = dsize;
    monster defender;
    defender.type = &dtype;

    const int num_tests = 15000;

    for( int i = 0; i < num_tests; ++i ) {
        selected_part_histogram[defender.select_body_part( &attacker, hit_roll )]++;
    }

    float total_weight = 0.0;
    for( float w : expected ) {
        total_weight += w;
    }

    for( auto weight : selected_part_histogram ) {
        INFO( body_part_name( weight.first ) );
        const double expected_proportion = expected[weight.first] / total_weight;
        CHECK_THAT( weight.second, IsBinomialObservation( num_tests, expected_proportion ) );
    }
}

TEST_CASE( "Check distribution of attacks to body parts for same sized opponents." )
{
    rng_set_engine_seed( 4242424242 );

    calculate_bodypart_distribution( MS_SMALL, MS_SMALL, 0, expected_weights_base[1] );
    calculate_bodypart_distribution( MS_SMALL, MS_SMALL, 1, expected_weights_base[1] );
    calculate_bodypart_distribution( MS_SMALL, MS_SMALL, 100, expected_weights_max[1] );
}

TEST_CASE( "Check distribution of attacks to body parts for smaller attacker." )
{
    rng_set_engine_seed( 4242424242 );

    calculate_bodypart_distribution( MS_SMALL, MS_MEDIUM, 0, expected_weights_base[0] );
    calculate_bodypart_distribution( MS_SMALL, MS_MEDIUM, 1, expected_weights_base[0] );
    calculate_bodypart_distribution( MS_SMALL, MS_MEDIUM, 100, expected_weights_max[0] );
}

TEST_CASE( "Check distribution of attacks to body parts for larger attacker." )
{
    rng_set_engine_seed( 4242424242 );

    calculate_bodypart_distribution( MS_MEDIUM, MS_SMALL, 0, expected_weights_base[2] );
    calculate_bodypart_distribution( MS_MEDIUM, MS_SMALL, 1, expected_weights_base[2] );
    calculate_bodypart_distribution( MS_MEDIUM, MS_SMALL, 100, expected_weights_max[2] );
}
