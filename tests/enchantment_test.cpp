#include "catch/catch.hpp"

#include "magic_enchantment.h"
#include "map.h"
#include "map_helpers.h"
#include "item.h"
#include "player.h"
#include "player_helpers.h"

static trait_id trait_CARNIVORE( "CARNIVORE" );
static efftype_id effect_debug_clairvoyance( "debug_clairvoyance" );

static void advance_turn( Character &guy )
{
    guy.process_turn();
    calendar::turn += 1_turns;
}

static void give_item( Character &guy, const std::string &item_id )
{
    guy.i_add( item( item_id ) );
    guy.recalculate_enchantment_cache();
}

static void clear_items( Character &guy )
{
    guy.inv.clear();
    guy.recalculate_enchantment_cache();
}

TEST_CASE( "Enchantments grant mutations", "[magic][enchantment][trait][mutation]" )
{
    clear_map();
    Character &guy = get_player_character();
    clear_character( *guy.as_player(), true );

    guy.recalculate_enchantment_cache();
    advance_turn( guy );

    std::string s_relic = "test_relic_gives_trait";

    WHEN( "Character doesn't have trait" ) {
        REQUIRE( !guy.has_trait( trait_CARNIVORE ) );
        AND_WHEN( "Character receives relic" ) {
            give_item( guy, s_relic );
            THEN( "Character gains trait" ) {
                CHECK( guy.has_trait( trait_CARNIVORE ) );
            }
            AND_WHEN( "Turn passes" ) {
                advance_turn( guy );
                THEN( "Character still has trait" ) {
                    CHECK( guy.has_trait( trait_CARNIVORE ) );
                }
                AND_WHEN( "Character loses relic" ) {
                    clear_items( guy );
                    THEN( "Character loses trait" ) {
                        CHECK_FALSE( guy.has_trait( trait_CARNIVORE ) );
                    }
                    AND_WHEN( "Turn passes" ) {
                        advance_turn( guy );
                        THEN( "Character still has no trait" ) {
                            CHECK_FALSE( guy.has_trait( trait_CARNIVORE ) );
                        }
                    }
                }
            }
        }
    }

    WHEN( "Character has trait" ) {
        guy.set_mutation( trait_CARNIVORE );
        REQUIRE( guy.has_trait( trait_CARNIVORE ) );
        AND_WHEN( "Character receives relic" ) {
            give_item( guy, s_relic );
            THEN( "Nothing changes" ) {
                CHECK( guy.has_trait( trait_CARNIVORE ) );
            }
            AND_WHEN( "Turn passes" ) {
                advance_turn( guy );
                THEN( "Nothing changes" ) {
                    CHECK( guy.has_trait( trait_CARNIVORE ) );
                }
                AND_WHEN( "Character loses relic" ) {
                    clear_items( guy );
                    THEN( "Nothing changes" ) {
                        CHECK( guy.has_trait( trait_CARNIVORE ) );
                    }
                    AND_WHEN( "Turn passes" ) {
                        advance_turn( guy );
                        THEN( "Nothing changes" ) {
                            CHECK( guy.has_trait( trait_CARNIVORE ) );
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE( "Enchantments apply effects", "[magic][enchantment][effect]" )
{
    clear_map();
    Character &guy = get_player_character();
    clear_character( *guy.as_player(), true );

    guy.recalculate_enchantment_cache();
    advance_turn( guy );

    std::string s_relic = "architect_cube";

    // TODO: multiple enchantments apply same effect of
    //       same or different intensity
    // TODO: enchantments apply effect while char already has effect of
    //       same, stronger or weaker intensity

    WHEN( "Character doesn't have effect" ) {
        REQUIRE( !guy.has_effect( effect_debug_clairvoyance ) );
        AND_WHEN( "Character receives relic" ) {
            give_item( guy, s_relic );
            THEN( "Character still doesn't have effect" ) {
                CHECK_FALSE( guy.has_effect( effect_debug_clairvoyance ) );
            }
            AND_WHEN( "Turn passes" ) {
                advance_turn( guy );
                THEN( "Character receives effect" ) {
                    CHECK( guy.has_effect( effect_debug_clairvoyance ) );
                }
                AND_WHEN( "Character loses relic" ) {
                    clear_items( guy );
                    THEN( "Character still has effect" ) {
                        CHECK( guy.has_effect( effect_debug_clairvoyance ) );
                    }
                    AND_WHEN( "Turn passes" ) {
                        advance_turn( guy );

                        // FIXME: effects should go away after 1 turn!
                        CHECK( guy.has_effect( effect_debug_clairvoyance ) );
                        advance_turn( guy );

                        THEN( "Character loses effect" ) {
                            CHECK_FALSE( guy.has_effect( effect_debug_clairvoyance ) );
                        }
                    }
                }
            }
        }
    }
}

static void tests_stats( Character &guy, int s_base, int d_base, int p_base, int i_base, int s_exp,
                         int d_exp, int p_exp, int i_exp )
{
    guy.str_max = s_base;
    guy.dex_max = d_base;
    guy.per_max = p_base;
    guy.int_max = i_base;

    guy.recalculate_enchantment_cache();
    advance_turn( guy );

    auto check_stats = [&]( int s, int d, int p, int i ) {
        REQUIRE( guy.get_str_base() == s_base );
        REQUIRE( guy.get_dex_base() == d_base );
        REQUIRE( guy.get_per_base() == p_base );
        REQUIRE( guy.get_int_base() == i_base );

        CHECK( guy.get_str() == s );
        CHECK( guy.get_dex() == d );
        CHECK( guy.get_per() == p );
        CHECK( guy.get_int() == i );
    };
    auto check_stats_base = [&]() {
        check_stats( s_base, d_base, p_base, i_base );
    };

    check_stats_base();

    std::string s_relic = "test_relic_mods_stats";

    WHEN( "Character receives relic" ) {
        give_item( guy, s_relic );
        THEN( "Stats remain unchanged" ) {
            check_stats_base();
        }
        AND_WHEN( "Turn passes" ) {
            advance_turn( guy );
            THEN( "Stats are modified and don't overflow" ) {
                check_stats( s_exp, d_exp, p_exp, i_exp );
            }
            AND_WHEN( "Character loses relic" ) {
                clear_items( guy );
                THEN( "Stats remain unchanged" ) {
                    check_stats( s_exp, d_exp, p_exp, i_exp );
                }
                AND_WHEN( "Turn passes" ) {
                    advance_turn( guy );
                    THEN( "Stats return to normal" ) {
                        check_stats_base();
                    }
                }
            }
        }
    }
}

TEST_CASE( "Enchantments modify stats", "[magic][enchantment][character]" )
{
    clear_map();
    Character &guy = get_player_character();
    clear_character( *guy.as_player(), true );

    SECTION( "base stats 8" ) {
        tests_stats( guy, 8, 8, 8, 8, 20, 6, 5, 0 );
    }
    SECTION( "base stats 12" ) {
        tests_stats( guy, 12, 12, 12, 12, 28, 10, 7, 1 );
    }
    SECTION( "base stats 4" ) {
        tests_stats( guy, 4, 4, 4, 4, 12, 2, 3, 0 );
    }
}

static void tests_speed( Character &guy, int sp_base, int sp_exp )
{
    guy.recalculate_enchantment_cache();
    guy.set_speed_base( sp_base );
    guy.set_speed_bonus( 0 );

    guy.set_moves( 0 );
    advance_turn( guy );

    std::string s_relic = "test_relic_mods_speed";

    auto check_speed = [&]( int speed, int moves ) {
        REQUIRE( guy.get_speed_base() == sp_base );
        REQUIRE( guy.get_speed() == speed );
        REQUIRE( guy.get_moves() == moves );
    };
    auto check_speed_is_base = [&] {
        check_speed( sp_base, sp_base );
    };

    WHEN( "Character has no relics" ) {
        THEN( "Speed and moves gain equel base" ) {
            check_speed_is_base();
        }
    }
    WHEN( "Character receives relic" ) {
        give_item( guy, s_relic );
        THEN( "Nothing changes" ) {
            check_speed_is_base();
        }
        AND_WHEN( "Turn passes" ) {
            guy.set_moves( 0 );
            advance_turn( guy );
            THEN( "Speed changes, moves gain changes" ) {
                check_speed( sp_exp, sp_exp );
            }
            AND_WHEN( "Character loses relic" ) {
                clear_items( guy );
                THEN( "Nothing changes" ) {
                    check_speed( sp_exp, sp_exp );
                }
                AND_WHEN( "Turn passes" ) {
                    guy.set_moves( 0 );
                    advance_turn( guy );
                    THEN( "Speed and moves gain return to normal" ) {
                        check_speed_is_base();
                    }
                }
            }
        }
    }
    WHEN( "Character receives 10 relics" ) {
        for( int i = 0; i < 10; i++ ) {
            give_item( guy, s_relic );
        }
        THEN( "Nothing changes" ) {
            check_speed_is_base();
        }
        AND_WHEN( "Turn passes" ) {
            guy.set_moves( 0 );
            advance_turn( guy );
            THEN( "Speed and moves gain do not fall below 25% of base" ) {
                check_speed( sp_base / 4, sp_base / 4 );
            }
        }
    }
}

TEST_CASE( "Enchantments modify speed", "[magic][enchantment][speed]" )
{
    clear_map();
    Character &guy = get_player_character();
    clear_character( *guy.as_player(), true );

    SECTION( "base = 100" ) {
        tests_speed( guy, 100, 75 );
    }
    SECTION( "base = 80" ) {
        tests_speed( guy, 80, 65 );
    }
    SECTION( "base = 120" ) {
        tests_speed( guy, 120, 85 );
    }
}
