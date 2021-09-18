#include "catch/catch.hpp"

#include <array>
#include <vector>
#include "dispersion.h"
#include "probability.h"

TEST_CASE( "probit math test", "[math]" )
{
    constexpr double probit_approx_eps = 0.01;
    CHECK( probit::rescaled_to_zero_to_one( 0.0 ) == Approx( 0.0 ).epsilon( probit_approx_eps ) );
    CHECK( probit::rescaled_to_zero_to_one( 0.5 ) == Approx( 0.5 ).epsilon( probit_approx_eps ) );
    CHECK( probit::rescaled_to_zero_to_one( 1.0 ) == Approx( 1.0 ).epsilon( probit_approx_eps ) );
}
