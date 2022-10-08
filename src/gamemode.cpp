#include "gamemode.h"
#include "gamemode_defense.h"
#include "gamemode_tutorial.h"

#include "debug.h"
#include "translations.h"

auto special_game_name( special_game_id id ) -> std::string
{
    switch( id ) {
        case SGAME_NULL:
        case NUM_SPECIAL_GAMES:
            return "Bug (gamemode.cpp:special_game_name)";
        case SGAME_TUTORIAL:
            return _( "Tutorial" );
        case SGAME_DEFENSE:
            return _( "Defense" );
        default:
            return "Uncoded (gamemode.cpp:special_game_name)";
    }
}

auto get_special_game( special_game_id id ) -> std::unique_ptr<special_game>
{
    std::unique_ptr<special_game> ret;
    switch( id ) {
        case SGAME_NULL:
            ret = std::make_unique<special_game>();
            break;
        case SGAME_TUTORIAL:
            ret = std::make_unique<tutorial_game>();
            break;
        case SGAME_DEFENSE:
            ret = std::make_unique<defense_game>();
            break;
        default:
            debugmsg( "Missing something in gamemode.cpp:get_special_game()?" );
            ret = std::make_unique<special_game>();
            break;
    }

    return ret;
}
