#include "catch/catch.hpp"

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
                dispersion_sources dispersion( who.get_weapon_dispersion( gun, RANGE_SOFT_CAP ) + recoil );

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
                // Don't test range above soft cap - there is no good approxmation for that yet
                for( int range = 0; range <= RANGE_SOFT_CAP; ++range ) {
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
                    gun.ammo_set( default_ammo( gun.ammo_type() ), gun.ammo_capacity() );

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
