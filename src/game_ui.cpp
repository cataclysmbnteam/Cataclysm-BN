#include <optional>
#include "game_ui.h"
#include "input.h"
#include "point.h"
#include "cursesdef.h"

#if !defined(TILES)

void reinitialize_framebuffer( const bool /*force_invalidate*/ )
{
    return;
}

void to_map_font_dim_width( int & )
{
    return;
}

void to_map_font_dim_height( int & )
{
    return;
}

void to_map_font_dimension( int &, int & )
{
    return;
}
void from_map_font_dimension( int &, int & )
{
    return;
}
void to_overmap_font_dimension( int &, int & )
{
    return;
}

std::optional<point> input_context::get_coordinates_text( const catacurses::window & ) const
{
    // ncurses does not support mouse currently
    // curses build always returns "no mouse"
    return std::nullopt;
}

#endif
