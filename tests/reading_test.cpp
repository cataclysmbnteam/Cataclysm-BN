#include "catch/catch.hpp"

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "avatar.h"
#include "bodypart.h"
#include "calendar.h"
#include "character_functions.h"
#include "item.h"
#include "itype.h"
#include "map.h"
#include "map_selector.h"
#include "map_helpers.h"
#include "morale_types.h"
#include "options.h"
#include "player_helpers.h"
#include "recipe.h"
#include "recipe_dictionary.h"
#include "skill.h"
#include "state_helpers.h"
#include "type_id.h"
#include "value_ptr.h"
#include "vehicle.h"
#include "vehicle_selector.h"

class player;

static const trait_id trait_HYPEROPIC( "HYPEROPIC" );
static const trait_id trait_ILLITERATE( "ILLITERATE" );
static const trait_id trait_LOVES_BOOKS( "LOVES_BOOKS" );
static const trait_id trait_SPIRITUAL( "SPIRITUAL" );

TEST_CASE( "identifying unread books", "[reading][book][identify]" )
{
    clear_all_state();
    avatar dummy;

    GIVEN( "player has some unidentified books" ) {
        item &book1 = dummy.i_add( item( "novel_western" ) );
        item &book2 = dummy.i_add( item( "mag_throwing" ) );

        REQUIRE_FALSE( dummy.has_identified( book1.typeId() ) );
        REQUIRE_FALSE( dummy.has_identified( book2.typeId() ) );

        WHEN( "they read the books for the first time" ) {
            dummy.do_read( item_location( dummy, &book1 ) );
            dummy.do_read( item_location( dummy, &book2 ) );

            THEN( "the books should be identified" ) {
                CHECK( dummy.has_identified( book1.typeId() ) );
                CHECK( dummy.has_identified( book2.typeId() ) );
            }
        }
    }
}

TEST_CASE( "reading a book for fun", "[reading][book][fun]" )
{
    clear_all_state();
    avatar dummy;

    GIVEN( "a fun book" ) {
        item &book = dummy.i_add( item( "novel_western" ) );
        REQUIRE( book.type->book );
        REQUIRE( book.type->book->fun > 0 );
        int book_fun = book.type->book->fun;

        WHEN( "player doesn't love books" ) {
            REQUIRE_FALSE( dummy.has_trait( trait_LOVES_BOOKS ) );

            THEN( "the book is a normal amount of fun" ) {
                CHECK( character_funcs::is_fun_to_read( dummy, book ) == true );
                CHECK( character_funcs::get_book_fun_for( dummy, book ) == book_fun );
            }
        }

        WHEN( "player loves books" ) {
            dummy.toggle_trait( trait_LOVES_BOOKS );
            REQUIRE( dummy.has_trait( trait_LOVES_BOOKS ) );

            THEN( "the book is extra fun" ) {
                CHECK( character_funcs::is_fun_to_read( dummy, book ) == true );
                CHECK( character_funcs::get_book_fun_for( dummy, book ) == book_fun + 1 );
            }
        }
    }

    GIVEN( "a fun book that is also inspirational" ) {
        item &book = dummy.i_add( item( "holybook_pastafarian" ) );
        REQUIRE( book.has_flag( "INSPIRATIONAL" ) );
        REQUIRE( book.type->book );
        REQUIRE( book.type->book->fun > 0 );
        int book_fun = book.type->book->fun;

        WHEN( "player is not spiritual" ) {
            REQUIRE_FALSE( dummy.has_trait( trait_SPIRITUAL ) );

            THEN( "the book is a normal amount of fun" ) {
                CHECK( character_funcs::is_fun_to_read( dummy, book ) == true );
                CHECK( character_funcs::get_book_fun_for( dummy, book ) == book_fun );
            }
        }

        WHEN( "player is spiritual" ) {
            dummy.toggle_trait( trait_SPIRITUAL );
            REQUIRE( dummy.has_trait( trait_SPIRITUAL ) );

            THEN( "the book is thrice the fun" ) {
                CHECK( character_funcs::is_fun_to_read( dummy, book ) == true );
                CHECK( character_funcs::get_book_fun_for( dummy, book ) == book_fun * 3 );
            }
        }
    }
}

TEST_CASE( "character reading speed", "[reading][character][speed]" )
{
    clear_all_state();
    avatar dummy;

    // Note: read_speed() returns number of moves;
    // 6000 == 60 seconds

    WHEN( "player has average intelligence" ) {
        REQUIRE( dummy.get_int() == 8 );

        THEN( "reading speed is normal" ) {
            CHECK( dummy.read_speed() == 6000 );
        }
    }

    WHEN( "player has below-average intelligence" ) {

        THEN( "reading speed gets slower as intelligence decreases" ) {
            dummy.int_max = 7;
            CHECK( dummy.read_speed() == 6300 );
            dummy.int_max = 6;
            CHECK( dummy.read_speed() == 6600 );
            dummy.int_max = 5;
            CHECK( dummy.read_speed() == 6900 );
            dummy.int_max = 4;
            CHECK( dummy.read_speed() == 7200 );
        }
    }

    WHEN( "player has above-average intelligence" ) {

        THEN( "reading speed gets faster as intelligence increases" ) {
            dummy.int_max = 9;
            CHECK( dummy.read_speed() == 5700 );
            dummy.int_max = 10;
            CHECK( dummy.read_speed() == 5400 );
            dummy.int_max = 12;
            CHECK( dummy.read_speed() == 4800 );
            dummy.int_max = 14;
            CHECK( dummy.read_speed() == 4200 );
        }
    }
}

TEST_CASE( "estimated reading time for a book", "[reading][book][time]" )
{
    clear_all_state();
    avatar dummy;

    // Easy, medium, and hard books
    item &child = dummy.i_add( item( "child_book" ) );
    item &western = dummy.i_add( item( "novel_western" ) );
    item &alpha = dummy.i_add( item( "recipe_alpha" ) );

    // Ensure the books are actually books
    REQUIRE( child.type->book );
    REQUIRE( western.type->book );
    REQUIRE( alpha.type->book );

    // Convert time to read from minutes to moves, for easier comparison later
    int moves_child = child.type->book->time * to_moves<int>( 1_minutes );
    int moves_western = western.type->book->time * to_moves<int>( 1_minutes );
    int moves_alpha = alpha.type->book->time * to_moves<int>( 1_minutes );

    GIVEN( "some identified books and plenty of light" ) {
        // Identify the books
        dummy.do_read( item_location( dummy, &child ) );
        dummy.do_read( item_location( dummy, &western ) );
        dummy.do_read( item_location( dummy, &alpha ) );
        REQUIRE( dummy.has_identified( child.typeId() ) );
        REQUIRE( dummy.has_identified( western.typeId() ) );
        REQUIRE( dummy.has_identified( alpha.typeId() ) );

        // Get some light
        dummy.i_add( item( "atomic_lamp" ) );
        REQUIRE( character_funcs::fine_detail_vision_mod( dummy ) == character_funcs::FINE_VISION_PERFECT );

        WHEN( "player has average intelligence" ) {
            dummy.int_max = 8;
            REQUIRE( dummy.get_int() == 8 );
            REQUIRE( dummy.read_speed() == 6000 ); // 60s, "normal"

            THEN( "they can read books at their reading level in the normal amount time" ) {
                CHECK( dummy.time_to_read( child, dummy ) == moves_child );
                CHECK( dummy.time_to_read( western, dummy ) == moves_western );
            }
            AND_THEN( "they can read books above their reading level, but it takes longer" ) {
                CHECK( dummy.time_to_read( alpha, dummy ) > moves_alpha );
            }
        }

        WHEN( "player has below average intelligence" ) {
            dummy.int_max = 6;
            REQUIRE( dummy.get_int() == 6 );
            REQUIRE( dummy.read_speed() == 6600 ); // 66s

            THEN( "they take longer than average to read any book" ) {
                CHECK( dummy.time_to_read( child, dummy ) > moves_child );
                CHECK( dummy.time_to_read( western, dummy ) > moves_western );
                CHECK( dummy.time_to_read( alpha, dummy ) > moves_alpha );
            }
        }

        WHEN( "player has above average intelligence" ) {
            dummy.int_max = 10;
            REQUIRE( dummy.get_int() == 10 );
            REQUIRE( dummy.read_speed() == 5400 ); // 54s

            THEN( "they take less time than average to read any book" ) {
                CHECK( dummy.time_to_read( child, dummy ) < moves_child );
                CHECK( dummy.time_to_read( western, dummy ) < moves_western );
                CHECK( dummy.time_to_read( alpha, dummy ) < moves_alpha );
            }
        }
    }
}

TEST_CASE( "reasons for not being able to read", "[reading][reasons]" )
{
    clear_all_state();
    avatar dummy;
    std::vector<std::string> reasons;
    std::vector<std::string> expect_reasons;

    item &child = dummy.i_add( item( "child_book" ) );
    item &western = dummy.i_add( item( "novel_western" ) );
    item &alpha = dummy.i_add( item( "recipe_alpha" ) );

    SECTION( "you cannot read what is not readable" ) {
        item &rag = dummy.i_add( item( "rag" ) );
        REQUIRE_FALSE( rag.is_book() );

        CHECK( dummy.get_book_reader( rag, reasons ) == nullptr );
        expect_reasons = { "Your rag is not good reading material." };
        CHECK( reasons == expect_reasons );
    }

    SECTION( "you cannot read in darkness" ) {
        dummy.add_env_effect( efftype_id( "darkness" ), bp_eyes, 3, 1_hours );
        REQUIRE( !character_funcs::can_see_fine_details( dummy ) );

        CHECK( dummy.get_book_reader( child, reasons ) == nullptr );
        expect_reasons = { "It's too dark to read!" };
        CHECK( reasons == expect_reasons );
    }

    GIVEN( "some identified books and plenty of light" ) {
        // Identify the books
        dummy.do_read( item_location( dummy, &child ) );
        dummy.do_read( item_location( dummy, &western ) );
        dummy.do_read( item_location( dummy, &alpha ) );

        // Get some light
        dummy.i_add( item( "atomic_lamp" ) );
        REQUIRE( character_funcs::fine_detail_vision_mod( dummy ) == character_funcs::FINE_VISION_PERFECT );

        THEN( "you cannot read while illiterate" ) {
            dummy.toggle_trait( trait_ILLITERATE );
            REQUIRE( dummy.has_trait( trait_ILLITERATE ) );

            CHECK( dummy.get_book_reader( western, reasons ) == nullptr );
            expect_reasons = { "You're illiterate!" };
            CHECK( reasons == expect_reasons );
        }

        THEN( "you cannot read while farsighted without reading glasses" ) {
            dummy.toggle_trait( trait_HYPEROPIC );
            REQUIRE( dummy.has_trait( trait_HYPEROPIC ) );

            CHECK( dummy.get_book_reader( western, reasons ) == nullptr );
            expect_reasons = { "Your eyes won't focus without reading glasses." };
            CHECK( reasons == expect_reasons );
        }

        THEN( "you cannot read without enough skill to understand the book" ) {
            dummy.set_skill_level( skill_id( "cooking" ), 7 );

            CHECK( dummy.get_book_reader( alpha, reasons ) == nullptr );
            expect_reasons = { "cooking 8 needed to understand.  You have 7" };
            CHECK( reasons == expect_reasons );
        }

        THEN( "you cannot read boring books when your morale is too low" ) {
            dummy.add_morale( MORALE_FEELING_BAD, -50, -100 );
            REQUIRE_FALSE( dummy.has_morale_to_read() );

            CHECK( dummy.get_book_reader( alpha, reasons ) == nullptr );
            expect_reasons = { "What's the point of studying?  (Your morale is too low!)" };
            CHECK( reasons == expect_reasons );
        }

        WHEN( "there is nothing preventing you from reading" ) {
            REQUIRE_FALSE( dummy.has_trait( trait_ILLITERATE ) );
            REQUIRE_FALSE( dummy.has_trait( trait_HYPEROPIC ) );
            REQUIRE_FALSE( dummy.in_vehicle );
            REQUIRE( dummy.has_morale_to_read() );

            THEN( "you can read!" ) {
                CHECK( dummy.get_book_reader( western, reasons ) != nullptr );
                expect_reasons = {};
                CHECK( reasons == expect_reasons );
            }
        }
    }
}

// Now that's an ugly test
TEST_CASE( "Learning recipes from books", "[reading][book][recipe]" )
{
    clear_all_state();
    avatar dummy;
    item &alpha = dummy.i_add( item( "recipe_alpha" ) );
    auto mutagen_iter = std::find_if( recipe_dict.begin(),
    recipe_dict.end(), []( const std::pair<recipe_id, recipe> &p ) {
        return p.second.result() == itype_id( "mutagen_alpha" );
    } );

    REQUIRE( mutagen_iter != recipe_dict.end() );
    REQUIRE( get_option<bool>( "ALLOW_LEARNING_BOOK_RECIPES" ) );

    const recipe *rec = &( mutagen_iter->second );

    REQUIRE( alpha.type->book );
    const auto book_recipes = alpha.type->book->recipes;
    bool book_has_recipe = std::find_if( book_recipes.begin(),
    book_recipes.end(), [rec]( const islot_book::recipe_with_description_t &rec_d ) {
        return rec_d.recipe == rec;
    } ) != book_recipes.end();
    REQUIRE( book_has_recipe );

    REQUIRE_FALSE( dummy.knows_recipe( rec ) );
    // Just skim
    // TODO: Do without it somehow
    dummy.do_read( item_location( dummy, &alpha ) );

    SECTION( "You do not have the skills to understand the recipe in the book" ) {
        REQUIRE_FALSE( dummy.has_recipe_requirements( *rec ) );
        AND_WHEN( "You read the book" ) {
            dummy.do_read( item_location( dummy, &alpha ) );
            THEN( "You still don't know the recipe" ) {
                CHECK_FALSE( dummy.knows_recipe( rec ) );
            }
        }
    }

    SECTION( "You do have enough skills to understand the recipe in the book" ) {
        for( const Skill &s : Skill::skills ) {
            dummy.set_skill_level( s.ident(), 10 );
        }
        REQUIRE( dummy.has_recipe_requirements( *rec ) );
        AND_WHEN( "You read the book" ) {
            dummy.do_read( item_location( dummy, &alpha ) );
            THEN( "You know the recipe now" ) {
                CHECK( dummy.knows_recipe( rec ) );
            }
        }
    }
}

static void destroyed_book_test_helper( avatar &u, item_location loc )
{
    std::vector<std::string> reasons_cant_read;
    const player *reader = u.get_book_reader( *loc, reasons_cant_read );
    CAPTURE( reasons_cant_read );
    REQUIRE( reader != nullptr );
    WHEN( "You start reading the book" ) {
        REQUIRE( u.activity.is_null() );
        bool did_read = u.read( loc );
        REQUIRE( did_read );
        REQUIRE( !u.activity.is_null() );
        AND_WHEN( "The book is destroyed" ) {
            loc.remove_item();
            AND_WHEN( "A turn passes for you" ) {
                u.process_turn();
                CHECK( !u.activity.is_null() );
                process_activity( u );
                THEN( "The reading job is cancelled" ) {
                    CHECK( u.activity.is_null() );
                }
            }
        }
    }
}

TEST_CASE( "Losing book during reading", "[reading][book]" )
{
    clear_all_state();
    set_time( calendar::turn_zero + 12_hours );
    avatar &u = get_avatar();
    SECTION( "Book in inventory" ) {
        item &alpha = u.i_add( item( "novel_western" ) );
        item_location loc( u, &alpha );
        destroyed_book_test_helper( u, loc );
    }

    SECTION( "Book below player" ) {
        item &alpha = get_map().add_item( u.pos(), item( "novel_western" ) );
        REQUIRE( !alpha.is_null() );
        item_location loc( map_cursor( u.pos() ), &alpha );
        destroyed_book_test_helper( u, loc );
    }

    SECTION( "Book in car" ) {
        vehicle *veh = get_map().add_vehicle( vproto_id( "car" ), u.pos(), 0_degrees, 0, 0 );
        REQUIRE( veh != nullptr );
        int part = veh->part_with_feature( point_zero, "CARGO", true );
        REQUIRE( part >= 0 );
        auto iter = veh->add_item( part, item( "novel_western" ) );
        REQUIRE( iter );
        item_location loc( vehicle_cursor( *veh, part ), &( **iter ) );
        destroyed_book_test_helper( u, loc );
    }
}
