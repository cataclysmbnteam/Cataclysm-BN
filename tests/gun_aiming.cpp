#include "catch/catch.hpp"

#include "ammo.h"
#include "ballistics.h"
#include "dispersion.h"
#include "game.h"
#include "npc.h"
#include "item_factory.h"

static void test_distribution( const dispersion_sources &dispersion, int range, double target_size )
{
    const int N = 50000;
    std::array< std::pair<double, int>, 20 > bins;

    for( int i = 0; i < bins.size(); ++i ) {
        bins[i].first = ( double )( bins.size() - i ) / bins.size();
        bins[i].second = 0;
    }

    for( int i = 0; i < N; ++i ) {
        projectile_attack_aim aim = projectile_attack_roll( dispersion, range, target_size );
        for( int j = 0; j < bins.size() && aim.missed_by < bins[j].first; ++j ) {
            ++bins[j].second;
        }
    }

    for( int i = 0; i < bins.size(); ++i ) {
        CAPTURE( range );
        CAPTURE( dispersion.stddev() );
        CAPTURE( bins[i].first );
        CHECK( projectile_attack_chance( dispersion, range, bins[i].first,
                                         target_size ) == Approx( ( double )bins[i].second / N ).epsilon( 0.01 ) );
    }
}

static void test_internal( const npc &who, const std::vector<item> &guns )
{
    WHEN( "the target is bigger" ) {
        THEN( "chance to hit is greater" ) {
            for( const auto &gun : guns ) {
                CAPTURE( gun.tname() );
                double accuracy = 0.5;
                double recoil = MAX_RECOIL;
                double chance = 0.5;
                double range = who.weapon.gun_range();
                dispersion_sources dispersion( who.get_weapon_dispersion( gun ) );
                dispersion.add_range( recoil );

                double last_size = projectile_attack_chance( dispersion, range, accuracy, 0.1 );
                for( double target_size = 0.2; target_size <= 1.0; target_size += 0.1 ) {
                    CHECK( projectile_attack_chance( dispersion, range, accuracy, target_size ) >=
                           projectile_attack_chance( dispersion, range, accuracy, last_size ) );
                }
            }
        }
    }
}

TEST_CASE( "gun ranges and hit chances are sensibly calculated", "[gun] [aim]" )
{
    const int gun_skill    = 4; // marksmanship
    const int weapon_skill = 4; // relevant weapon (eg. rifle)

    // Note that GIVEN statements cannot appear within loops
    GIVEN( "A typical survivor with a loaded gun" ) {
        standard_npc who;
        who.setpos( tripoint( 0, 0, 0 ) );
        who.wear_item( item( "gloves_lsurvivor" ) );
        who.wear_item( item( "mask_lsurvivor" ) );
        who.set_skill_level( skill_id( "gun" ), gun_skill );

        WHEN( "many shots are fired at human-sized target" ) {
            THEN( "the distribution of accuracies is as expected" ) {
                double target_size = who.ranged_target_size();
                for( int range = 0; range <= 60; ++range ) {
                    for( int dispersion = 0; dispersion < 1200; dispersion += 50 ) {
                        test_distribution( dispersion, range, target_size );
                    }
                }
            }
        }

        WHEN( "the gun ranges are examined" ) {
            std::vector<item> guns;
            for( const itype *e : item_controller->all() ) {
                if( e->gun ) {
                    guns.emplace_back( e );
                    auto &gun = guns.back();
                    if( !gun.magazine_integral() ) {
                        gun.emplace_back( gun.magazine_default() );
                    }
                    gun.ammo_set( gun.ammo_type()->default_ammotype(), gun.ammo_capacity() );

                    who.set_skill_level( gun.gun_skill(), weapon_skill );

                    CAPTURE( gun.tname() );
                    CAPTURE( gun.ammo_current() );
                    REQUIRE( gun.is_gun() );
                    REQUIRE( gun.ammo_sufficient() );
                }
            }

            test_internal( who, guns );
        }

        // @todo acceptance tests here
    }
}

// If upper_bound is true, return last dispersion such that chance was >target
// otherwise return last dispersion such that chance was <target
static double bisect_dispersion( int range, double target, bool upper_bound )
{
    // Bisection:
    // start at 50% of max dispersion,
    // set step size to 25%,
    // step up or down based on whether we got too high or too low, halve step size
    // repeat the above until step size is too low or precision high enough
    const double disp_stddev_max = 10000.0;
    double disp_stddev = disp_stddev_max / 2;
    double step_size = disp_stddev_max / 4;
    double last_good = -1.0;
    while( step_size >= 0.5 ) {
        dispersion_sources dispersion( disp_stddev );
        double chance = projectile_attack_chance( dispersion, range, accuracy_goodhit, 0.5 );
        if( chance > target ) {
            if( !upper_bound ) {
                last_good = disp_stddev;
            }
            disp_stddev = disp_stddev + step_size;
        } else {
            if( upper_bound ) {
                last_good = disp_stddev;
            }
            disp_stddev = disp_stddev - step_size;
        }
        step_size /= 2;
    }

    return last_good;
}

TEST_CASE( "find_max_dispersion_for_range", "[.] [gun] [aim]" )
{
    for( int range = 1; range <= 60; range++ ) {
        // Find highest dispersion such that chance of goodhit is >50%
        int even = floor( bisect_dispersion( range, 0.5, true ) );
        int bad = ceil( bisect_dispersion( range, 0.1, false ) );

        printf( "Range %*d: >50%% good: %*d, <10%% good: %*d, ratio: %.1f\n", 2, range, 5, even, 5, bad, (float)bad/even );
    }
}