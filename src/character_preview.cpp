#if defined(TILES)
#include "character_preview.h"
#include "type_id.h"
#include "character.h"
#include "profession.h"
#include "sdltiles.h"
#include "output.h"
#include "cata_tiles.h"
#include "cursesport.h"
#include "game.h"

auto termx_to_pixel_value() -> int
{
    return projected_window_width() / TERMX;
}

auto termy_to_pixel_value() -> int
{
    return projected_window_height() / TERMY;
}

void character_preview_window::init( Character *character )
{
    this->character = character;

    // Setting bionics
    for( const bionic_id &bio : character->prof->CBMs() ) {
        character->add_bionic( bio );
    }

    // Collecting profession clothes
    std::vector<detached_ptr<item>> prof_items = character->prof->items( character->male,
                                 character->get_mutations() );
    for( detached_ptr<item> &it : prof_items ) {
        if( it->is_armor() ) {
            clothes.push_back( std::move( it ) );
        }
    }
    toggle_clothes();
}


void character_preview_window::prepare( const int nlines, const int ncols,
                                        const Orientation *orientation, const int hide_below_ncols )
{
    zoom = DEFAULT_ZOOM;
    tilecontext->set_draw_scale( zoom );
    termx_pixels = termx_to_pixel_value();
    termy_pixels = termy_to_pixel_value();
    this->hide_below_ncols = hide_below_ncols;

    // Trying to ensure that tile will fit in border
    const int win_width = ncols * termx_pixels;
    const int win_height = nlines * termy_pixels;
    int t_width = tilecontext->get_tile_width();
    int t_height = tilecontext->get_tile_height();
    while( zoom != MIN_ZOOM && ( win_width < t_width || win_height < t_height ) ) {
        zoom_out();
        t_width = tilecontext->get_tile_width();
        t_height = tilecontext->get_tile_height();
    }

    // Final size of character preview window
    const int box_ncols = t_width / termx_pixels + 4;
    const int box_nlines = t_height / termy_pixels + 3;

    // Setting window just a little bit more than a tile itself
    point start;
    switch( orientation->type ) {
        case( TOP_LEFT ):
            start = point_zero;
            break;
        case( TOP_RIGHT ):
            start = point{TERMX - box_ncols, 0};
            break;
        case( BOTTOM_LEFT ):
            start = point{0, TERMY - box_nlines};
            break;
        case( BOTTOM_RIGHT ):
            start = point{TERMX - box_ncols, TERMY - box_nlines};
            break;
    }

    start.x += orientation->margin.left - orientation->margin.right;
    start.y += orientation->margin.top - orientation->margin.bottom;
    w_preview = catacurses::newwin( box_nlines, box_ncols, start );
    ncols_width = box_ncols;
    nlines_width = box_nlines;
    pos = start;
}

auto character_preview_window::calc_character_pos() const -> point
{
    const int t_width = tilecontext->get_tile_width();
    const int t_height = tilecontext->get_tile_height();
    return point(
               pos.x * termx_pixels + ncols_width * termx_pixels / 2 - t_width / 2,
               pos.y * termy_pixels + nlines_width * termy_pixels / 2 - t_height / 2
           );
}

void character_preview_window::zoom_in()
{
    zoom = zoom * 2 % ( MAX_ZOOM * 2 );
    if( zoom == 0 ) {
        zoom = MIN_ZOOM;
    }
    tilecontext->set_draw_scale( zoom );
}

void character_preview_window::zoom_out()
{
    zoom = zoom / 2;
    if( zoom < MIN_ZOOM ) {
        zoom = MAX_ZOOM;
    }
    tilecontext->set_draw_scale( zoom );
}

void character_preview_window::toggle_clothes()
{
    if( !show_clothes ) {
        character->worn.clear();
    } else {
        for( detached_ptr<item> &it : clothes ) {
            character->wear_item( item::spawn( *std::move( it ) ), false );
        }
    }
    show_clothes = !show_clothes;
}

void character_preview_window::display() const
{
    // If device width is too small - ignore display
    if( TERMX - ncols_width < hide_below_ncols ) {
        return;
    }

    // Drawing UI across character tile
    werase( w_preview );
    draw_border( w_preview, BORDER_COLOR, _( "CHARACTER PREVIEW" ), BORDER_COLOR );
    wnoutrefresh( w_preview );

    // Drawing character itself
    const point pos = calc_character_pos();
    tilecontext->display_character( *character, pos );
}

void character_preview_window::clear() const
{
    character->worn.clear();
    character->clear_bionics();
    tilecontext->set_draw_scale( DEFAULT_TILESET_ZOOM );
}

auto character_preview_window::clothes_showing() const -> bool
{
    return !show_clothes;
}

#endif // TILES
