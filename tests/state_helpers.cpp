#include "state_helpers.h"

#include "map_helpers.h"
#include "player_helpers.h"
#include "rng.h"
#include "calendar.h"
#include "weather.h"
#include "game.h"
#include "map.h"
#include "name.h"

void clear_all_state( )
{
    disable_mapgen = true;
    get_weather().weather_id = weather_type_id( "clear" );
    clear_map();
    clear_avatar();
    set_time( calendar::turn_zero );
    Name::clear();


    cleanup_arenas();
}
