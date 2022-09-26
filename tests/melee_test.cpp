#include "catch/catch.hpp"

#include <cstddef>
#include <sstream>
#include <string>

#include "avatar.h"
#include "bodypart.h"
#include "character_martial_arts.h"
#include "creature.h"
#include "game.h"
#include "game_constants.h"
#include "item.h"
#include "player_helpers.h"
#include "map.h"
#include "map_helpers.h"
#include "monattack.h"
#include "monster.h"
#include "npc.h"
#include "player.h"
#include "player_helpers.h"
#include "point.h"
#include "rng.h"
#include "skill.h"
#include "type_id.h"
#include "itype.h"

static constexpr tripoint player_pos( 60, 60, 0 );
static constexpr int iterations = 80;

static float brute_probability( Creature &attacker, Creature &target, const size_t iters )
{
    // Note: not using deal_melee_attack because it trains dodge, which causes problems here
    size_t hits = 0;
    for( size_t i = 0; i < iters; i++ ) {
        const int spread = attacker.hit_roll() - target.dodge_roll();
        if( spread > 0 ) {
            hits++;
        }
    }

    return static_cast<float>( hits ) / iters;
}

static float brute_special_probability( monster &attacker, Creature &target, const size_t iters )
{
    size_t hits = 0;
    for( size_t i = 0; i < iters; i++ ) {
        if( !mattack::dodge_check( &attacker, &target ) ) {
            hits++;
        }
    }

    return static_cast<float>( hits ) / iters;
}

static std::string full_attack_details( const player &dude )
{
    std::stringstream ss;
    ss << "Details for " << dude.disp_name() << std::endl;
    ss << "get_hit() == " << dude.get_hit() << std::endl;
    ss << "get_melee_hit_base() == " << dude.get_melee_hit_base() << std::endl;
    ss << "get_hit_weapon() == " << dude.get_hit_weapon( dude.weapon ) << std::endl;
    return ss.str();
}

inline std::string percent_string( const float f )
{
    // Using stringstream for prettier precision printing
    std::stringstream ss;
    ss << 100.0f * f << "%";
    return ss.str();
}

static void check_near( float prob, const float expected, const float tolerance )
{
    const float low = expected - tolerance;
    const float high = expected + tolerance;
    THEN( "The chance to hit is between " + percent_string( low ) +
          " and " + percent_string( high ) ) {
        REQUIRE( prob > low );
        REQUIRE( prob < high );
    }
}

static const int num_iters = 10;

static constexpr tripoint dude_pos( HALF_MAPSIZE_X, HALF_MAPSIZE_Y, 0 );

TEST_CASE( "Character attacking a zombie", "[.melee]" )
{
    monster zed( mtype_id( "mon_zombie" ) );
    INFO( "Zombie has get_dodge() == " + std::to_string( zed.get_dodge() ) );

    SECTION( "8/8/8/8, no skills, unarmed" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 0, 8, 8, 8, 8 );
        const float prob = brute_probability( dude, zed, num_iters );
        INFO( full_attack_details( dude ) );
        check_near( prob, 0.6f, 0.1f );
    }

    SECTION( "8/8/8/8, 3 all skills, two-by-four" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 3, 8, 8, 8, 8 );
        dude.weapon = item( "2x4" );
        const float prob = brute_probability( dude, zed, num_iters );
        INFO( full_attack_details( dude ) );
        check_near( prob, 0.8f, 0.05f );
    }

    SECTION( "10/10/10/10, 8 all skills, katana" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 8, 10, 10, 10, 10 );
        dude.weapon = item( "katana" );
        const float prob = brute_probability( dude, zed, num_iters );
        INFO( full_attack_details( dude ) );
        check_near( prob, 0.975f, 0.025f );
    }
}

TEST_CASE( "Character attacking a manhack", "[.melee]" )
{
    monster manhack( mtype_id( "mon_manhack" ) );
    INFO( "Manhack has get_dodge() == " + std::to_string( manhack.get_dodge() ) );

    SECTION( "8/8/8/8, no skills, unarmed" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 0, 8, 8, 8, 8 );
        const float prob = brute_probability( dude, manhack, num_iters );
        INFO( full_attack_details( dude ) );
        check_near( prob, 0.2f, 0.05f );
    }

    SECTION( "8/8/8/8, 3 all skills, two-by-four" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 3, 8, 8, 8, 8 );
        dude.weapon = item( "2x4" );
        const float prob = brute_probability( dude, manhack, num_iters );
        INFO( full_attack_details( dude ) );
        check_near( prob, 0.4f, 0.05f );
    }

    SECTION( "10/10/10/10, 8 all skills, katana" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 8, 10, 10, 10, 10 );
        dude.weapon = item( "katana" );
        const float prob = brute_probability( dude, manhack, num_iters );
        INFO( full_attack_details( dude ) );
        check_near( prob, 0.7f, 0.05f );
    }
}

TEST_CASE( "Zombie attacking a character", "[.melee]" )
{
    monster zed( mtype_id( "mon_zombie" ) );
    INFO( "Zombie has get_hit() == " + std::to_string( zed.get_hit() ) );

    SECTION( "8/8/8/8, no skills, unencumbered" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 0, 8, 8, 8, 8 );
        const float prob = brute_probability( zed, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        THEN( "Character has no significant dodge bonus or penalty" ) {
            REQUIRE( dude.get_dodge_bonus() < 0.5f );
            REQUIRE( dude.get_dodge_bonus() > -0.5f );
        }

        THEN( "Character's dodge skill is roughly equal to zombie's attack skill" ) {
            REQUIRE( dude.get_dodge() < zed.get_hit() + 0.5f );
            REQUIRE( dude.get_dodge() > zed.get_hit() - 0.5f );
        }

        check_near( prob, 0.5f, 0.05f );
    }

    SECTION( "10/10/10/10, 3 all skills, good cotton armor" ) {
        standard_npc dude( "TestCharacter", dude_pos,
        { "hoodie", "jeans", "long_underpants", "long_undertop", "longshirt" },
        3, 10, 10, 10, 10 );
        const float prob = brute_probability( zed, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        check_near( prob, 0.2f, 0.05f );
    }

    SECTION( "10/10/10/10, 8 all skills, survivor suit" ) {
        standard_npc dude( "TestCharacter", dude_pos, { "survivor_suit" }, 8, 10, 10, 10, 10 );
        const float prob = brute_probability( zed, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        check_near( prob, 0.025f, 0.0125f );
    }
}

TEST_CASE( "Manhack attacking a character", "[.melee]" )
{
    monster manhack( mtype_id( "mon_manhack" ) );
    INFO( "Manhack has get_hit() == " + std::to_string( manhack.get_hit() ) );

    SECTION( "8/8/8/8, no skills, unencumbered" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 0, 8, 8, 8, 8 );
        const float prob = brute_probability( manhack, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        THEN( "Character has no significant dodge bonus or penalty" ) {
            REQUIRE( dude.get_dodge_bonus() < 0.5f );
            REQUIRE( dude.get_dodge_bonus() > -0.5f );
        }

        check_near( prob, 0.9f, 0.05f );
    }

    SECTION( "10/10/10/10, 3 all skills, good cotton armor" ) {
        standard_npc dude( "TestCharacter", dude_pos,
        { "hoodie", "jeans", "long_underpants", "long_undertop", "longshirt" },
        3, 10, 10, 10, 10 );
        const float prob = brute_probability( manhack, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        check_near( prob, 0.6f, 0.05f );
    }

    SECTION( "10/10/10/10, 8 all skills, survivor suit" ) {
        standard_npc dude( "TestCharacter", dude_pos, { "survivor_suit" }, 8, 10, 10, 10, 10 );
        const float prob = brute_probability( manhack, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        check_near( prob, 0.25f, 0.05f );
    }
}

TEST_CASE( "Hulk smashing a character", "[.], [melee], [monattack]" )
{
    monster zed( mtype_id( "mon_zombie_hulk" ) );
    INFO( "Hulk has get_hit() == " + std::to_string( zed.get_hit() ) );

    SECTION( "8/8/8/8, no skills, unencumbered" ) {
        standard_npc dude( "TestCharacter", dude_pos, {}, 0, 8, 8, 8, 8 );
        const float prob = brute_special_probability( zed, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        THEN( "Character has no significant dodge bonus or penalty" ) {
            REQUIRE( dude.get_dodge_bonus() < 0.5f );
            REQUIRE( dude.get_dodge_bonus() > -0.5f );
        }

        check_near( prob, 0.95f, 0.05f );
    }

    SECTION( "10/10/10/10, 3 all skills, good cotton armor" ) {
        standard_npc dude( "TestCharacter", dude_pos,
        { "hoodie", "jeans", "long_underpants", "long_undertop", "longshirt" },
        3, 10, 10, 10, 10 );
        const float prob = brute_special_probability( zed, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        check_near( prob, 0.75f, 0.05f );
    }

    SECTION( "10/10/10/10, 8 all skills, survivor suit" ) {
        standard_npc dude( "TestCharacter", dude_pos, { "survivor_suit" }, 8, 10, 10, 10, 10 );
        const float prob = brute_special_probability( zed, dude, num_iters );
        INFO( "Has get_dodge() == " + std::to_string( dude.get_dodge() ) );
        check_near( prob, 0.2f, 0.05f );
    }
}


// BALANCE MARTIAL ARTS

TEST_CASE( "Strong character, 5 in all skills, using melee weapons against a kevlar hulk",
           "[.][melee][slow]" )
{
    // CHANGE THIS VALUES AS YOU NEED
    const std::vector<int> stats_level = { 12, 15, 18 }; // Will be tested by pair, eg 12-5, 15-7...
    const std::vector<int> skills_level = { 5, 7, 9 };
    // Cloths of character
    const std::vector<std::string> &clothing = { "hoodie", "jeans", "long_underpants", "long_undertop", "survivor_suit" };
    // attacks iterations between complete reset, for player/monster, don't get it too high or the player/monster will die. Default 3
    const int attacks_numbers = 3;
    // If testing a specific weapon, set one_weapon to true and replace the string below. Otherwise set it to false.
    // If you want to test an unarmed MA, your best bet is to set an unarmed weapon here. They are not marked as compatible weapons in JSON, but still work
    const bool one_weapon = false;
    itype_id tested_weapon_id( "to_replace" );
    // If you want the result damage for every weapons, of every MA
    const bool verbose = false;
    // Tests are for now very long, so set this to true to remove the less interesting weapons from MA who have a lot of them. Very, very WIP
    const bool reduce_weapons = true;
    // What monster(s) should be spawned?
    const std::vector<mtype_id> monster_ids = {
        mtype_id( "mon_zombie_kevlar_2" ),
        mtype_id( "mon_zombie_hulk" )
    };
    // The martial arts to test
    const std::vector<matype_id> tested_martial_arts = {
        { matype_id( "style_medievalpole" ) },
        { matype_id( "style_barbaran" ) },
        { matype_id( "style_biojutsu" ) },
        { matype_id( "style_ninjutsu" ) },
        { matype_id( "style_niten" ) },
        { matype_id( "style_swordsmanship" ) }
    };


    // Those will be used to store results and print them from best to worst
    struct struct_ma {
        matype_id ma_type_id;
        double best_weapon_dmg;
        std::string best_weapon;

        bool operator<( const struct_ma &sm ) const {
            return best_weapon_dmg > sm.best_weapon_dmg;
        }

    } STRUCT_MA;


    // Get player
    avatar &p = g->u;

    // For each monster
    for( size_t i = 0; i < monster_ids.size(); i++ ) {
        // For stats/skills couple
        for( size_t j = 0; j < stats_level.size(); j++ ) {
            // Set up tested MA
            std::vector<struct_ma> results_ma;
            for( const matype_id ma : tested_martial_arts ) {
                results_ma.push_back( { ma, 0.0,  "" } );
            }


            // Test performances with those weapons, with all martial arts
            cata_print_stdout( "\n\nStarting tests for -> STATS: " + std::to_string(
                                   stats_level[j] ) + " SKILLS: " + std::to_string( skills_level[j] ) + " M: " + monster_ids[i].str() +
                               "\n" );

            // For each MA to test
            for( const matype_id ma : tested_martial_arts ) {
                p.martial_arts_data->set_style( ma, true );

                matec_id special_counter( "tec_none" );
                double best_weapon_dmg = 0;
                std::vector<const itype *> compatible_weapons;
                std::string best_weapon = "";

                // Find compatible weapons with this MA
                for( const itype *it : find_weapons() ) {
                    // If we test only one weapon, override and use this one
                    if( one_weapon ) {
                        if( it->get_id() == tested_weapon_id ) {
                            compatible_weapons.push_back( it );
                        }
                    }
                    // Otherwise get weapons allowed for this MA
                    else {
                        if( p.martial_arts_data->selected_has_weapon( it->get_id() ) ) {
                            item player_wp( it );
                            compatible_weapons.push_back( it );
                        }
                    }
                }

                // Reduce number of weapons to 4 by MA, to greatly speed up tests. Keep the ones with the best DPS
                if( reduce_weapons ) {
                    monster dummy_m( monster_ids[i] );
                    std::sort( compatible_weapons.begin(), compatible_weapons.end(), [&]( const auto & l,
                    const auto & r ) {

                        item wp_l( l );
                        double interesting_coefficient_l = wp_l.damage_melee( damage_type::DT_BASH ) * 2.0;
                        interesting_coefficient_l += wp_l.damage_melee( damage_type::DT_CUT ) * 1.0;
                        interesting_coefficient_l += wp_l.damage_melee( damage_type::DT_STAB ) * 1.5;
                        //interesting_coefficient_l /= wp_l.attack_cost();
                        item wp_r( r );
                        double interesting_coefficient_r = wp_r.damage_melee( damage_type::DT_BASH ) * 2.0;
                        interesting_coefficient_r += wp_r.damage_melee( damage_type::DT_CUT ) * 1.0;
                        interesting_coefficient_r += wp_r.damage_melee( damage_type::DT_STAB ) * 1.5;
                        //interesting_coefficient_r /= wp_r.attack_cost();
                        return interesting_coefficient_l * wp_l.effective_dps( p,
                                dummy_m ) > interesting_coefficient_r * wp_r.effective_dps( p, dummy_m );
                    } );
                    while( compatible_weapons.size() > 11 ) {
                        compatible_weapons.pop_back();
                    }
                }

                // Go through weapons
                for( const itype *w : compatible_weapons ) {
                    item player_wp( w ); // Player weapon
                    int total_damage = 0;

                    for( size_t k = 0; k < iterations; k++ ) {
                        // Reset map
                        clear_map();
                        // Reset player
                        clear_character( p );
                        g->place_player( player_pos );
                        p.martial_arts_data->set_style( ma, true );
                        // Set stats
                        p.str_cur = stats_level[j];
                        p.str_max = stats_level[j];
                        p.dex_cur = stats_level[j];
                        p.dex_max = stats_level[j];
                        p.per_cur = stats_level[j];
                        p.per_max = stats_level[j];
                        p.int_cur = stats_level[j];
                        p.int_max = stats_level[j];
                        // Set all skills to 5 and MA
                        for( const Skill &e : Skill::skills ) {
                            p.set_skill_level( e.ident(), skills_level[j] );
                        }

                        // A bit hacky, but some martial arts benefits a lot from their counter. So until we can trigger correcly, we get and trigger them manually
                        // In a normal game, you would reach this by stacking armor
                        for( const auto &mat_id : ma->techniques ) {
                            const ma_technique &tec = mat_id.obj();
                            if( tec.block_counter == true ) {
                                special_counter = tec.id;
                                p.set_skill_level( skill_id( "dodge" ), 0 );
                                break;
                            }
                            if( tec.dodge_counter == true ) {
                                special_counter = tec.id;
                                break;
                            }
                        }


                        // Wield weapon
                        p.wield( player_wp );

                        // equip armor
                        for( const std::string &e : clothing ) {
                            p.wear_item( item( e ) );
                        }

                        // Place new monster (clear_map delete all monsters)
                        monster &z = spawn_test_monster( monster_ids[i].str(), p.pos() + point( 0, 1 ) );

                        // Place terrain around player and monster, to avoid knockback issues
                        // Nothing get through a resin wall
                        ter_id resin_wall( "t_wall_resin" );
                        // XXX top walls
                        g->m.ter_set( p.pos() + point( -1, 2 ), resin_wall );
                        g->m.ter_set( p.pos() + point( 0, 2 ), resin_wall );
                        g->m.ter_set( p.pos() + point( 1, 2 ), resin_wall );
                        // XmX monster level
                        g->m.ter_set( p.pos() + point( -1, 1 ), resin_wall );
                        g->m.ter_set( p.pos() + point( 1, 1 ), resin_wall );
                        // XPX player level
                        g->m.ter_set( p.pos() + point( -1, 0 ), resin_wall );
                        g->m.ter_set( p.pos() + point( 1, 0 ), resin_wall );
                        // XXX bottom walls
                        g->m.ter_set( p.pos() + point( -1, -1 ), resin_wall );
                        g->m.ter_set( p.pos() + point( 0, -1 ), resin_wall );
                        g->m.ter_set( p.pos() + point( 1, -1 ), resin_wall );


                        // Wait once for debuff (eg Barbaran montante)
                        // TODO does it work?
                        p.process_turn();

                        // For each fight (a few hits for each iteration, no reset between them)
                        for( size_t l = 0; l < attacks_numbers; l++ ) {

                            p.melee_attack( z, true );
                            // Important to trigger dodge counters/block counters, having enough moves means block/dodge counters left are reset
                            g->u.mod_moves( 300 );
                            p.process_turn();
                            z.melee_attack( p );
                            z.process_turn();

                            // Since counter attacks seems to trigger a much lower rate than in game (meaning close to never), we trigger them manually

                            if( ( k % 3 == 0 || one_in( 5 ) ) && special_counter->id.str() != "tec_none" ) {
                                p.melee_attack( z, true, special_counter );
                            }
                        }


                        total_damage += z.get_hp_max() - z.get_hp();
                    }

                    // Print average damage with this weapon and MA. Attack cost of weapon is taken into account
                    double attack_cost_factor = static_cast<double>( player_wp.attack_cost() ) / 100;
                    double average_damage = static_cast<double>( total_damage ) / ( iterations * attacks_numbers );
                    double corrected_average_damage = average_damage / attack_cost_factor;

                    // Store result in result_ma
                    for( size_t e = 0; e < results_ma.size(); e++ ) {
                        if( results_ma[e].ma_type_id == ma ) {
                            if( corrected_average_damage > results_ma[e].best_weapon_dmg ) {
                                results_ma[e].best_weapon_dmg = corrected_average_damage;
                                results_ma[e].best_weapon = player_wp.display_name();
                            }
                            break;
                        }
                    }


                    // Usually with one MA we want to see all weapons
                    if( tested_martial_arts.size() == 1 || verbose ) {
                        cata_printf( "%-30s : %-30s : %.1f\n", ma->name.translated( 1 ), player_wp.display_name(),
                                     corrected_average_damage );
                    }

                }

            }

            std::sort( results_ma.begin(), results_ma.end() );
            // Finally, print the MA results
            for( const struct_ma rma : results_ma ) {
                cata_printf( "%-30s : %-30s : %.1f\n", rma.ma_type_id->name.translated( 1 ), rma.best_weapon,
                             rma.best_weapon_dmg );
            }
        }
    }
}
