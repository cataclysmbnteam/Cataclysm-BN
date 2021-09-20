#include "catch/catch.hpp"

#include <array>
#include <vector>
#include "dispersion.h"
#include "probability.h"
#include "rng.h"

TEST_CASE( "truncated probit math constants", "[math]" )
{
    constexpr double probit_approx_eps = 0.01;
    CHECK( truncated_probit::at( 0.0 ) == Approx( 0.0 ).epsilon( probit_approx_eps ) );
    CHECK( truncated_probit::at( 0.5 ) == Approx( 0.5 ).epsilon( probit_approx_eps ) );
    CHECK( truncated_probit::at( 1.0 ) == Approx( 1.0 ).epsilon( probit_approx_eps ) );
}

TEST_CASE( "truncated probit math invariants", "[math]" )
{
    for( size_t i = 0; i < truncated_probit::n; i++ ) {
        CAPTURE( i );
        CHECK( truncated_probit::lookup_table[i] >= 0 );
        CHECK( truncated_probit::lookup_table[i] <= 1 );
    }
    for( size_t i = 1; i < truncated_probit::n; i++ ) {
        CAPTURE( i );
        CHECK( truncated_probit::lookup_table[i] >= truncated_probit::lookup_table[i - 1] );
    }
}
