#include "game_ui.h"

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

std::optional<point> input_context::get_coordinates_text(const catacurses::window
    & capture_win) const
{
    // ncurses build lways returns "no mouse"
    return std::nullopt;
}

#endif
