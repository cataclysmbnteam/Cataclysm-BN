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
