#if defined(TILES)

#include "vehicle_preview.h"

#include "cata_tiles.h"
#include "cursesport.h"
#include "game.h"
#include "output.h"
#include "sdltiles.h"
#include "units.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vehicle_part.h"

// Local empty string for tile_search_params
static const std::string empty_string;

// These functions are also defined in character_preview.cpp
// We provide our own static definitions here to avoid duplicate symbols
static int termx_to_pixel_value()
{
    return projected_window_width() / TERMX;
}

static int termy_to_pixel_value()
{
    return projected_window_height() / TERMY;
}

/**
 * Adapter class to access protected members of cata_tiles.
 * This pattern is used by character_preview.cpp as well.
 */
class veh_preview_adapter : public cata_tiles
{
    public:
        static veh_preview_adapter *convert( cata_tiles *ct ) {
            return static_cast<veh_preview_adapter *>( ct );
        }

        /**
         * Draw a vehicle part tile at the given pixel position.
         */
        void draw_vpart_tile( const vpart_id &vp_id, const point &pixel_pos,
                              int part_mod, int rotation_degrees, bool highlight ) {
            if( !vp_id.is_valid() ) {
                return;
            }

            const std::string vpname = "vp_" + vp_id.str();

            // part_mod: 0=normal, 1=open, 2=broken
            // Maps to subtile values used by cata_tiles
            int subtile = 0;
            if( part_mod == 1 ) {
                subtile = open_;
            } else if( part_mod == 2 ) {
                subtile = broken;
            }

            int height_3d = 0;
            const tile_search_params tile {
                vpname,
                C_VEHICLE_PART,
                empty_string,
                subtile,
                rotation_degrees
            };

            // Draw the tile
            // Using tripoint with pixel coordinates - the as_independent_entity flag
            // tells the renderer to treat these as absolute pixel positions
            draw_from_id_string(
                tile,
                tripoint( pixel_pos, 0 ),
                highlight ? SDL_Color{ 255, 255, 0, 255 } : std::optional<SDL_Color> {},
                std::nullopt,
                lit_level::BRIGHT,
                false,  // apply_visual_effects
                0,      // overlay_count
                true,   // as_independent_entity - critical for UI rendering
                height_3d
            );
        }

        /**
         * Draw a cursor tile at the given pixel position.
         */
        void draw_cursor_tile( const point &pixel_pos ) {
            int height_3d = 0;
            const tile_search_params tile {
                "cursor",
                C_NONE,
                empty_string,
                0,
                0
            };

            draw_from_id_string(
                tile,
                tripoint( pixel_pos, 0 ),
                std::nullopt,
                std::nullopt,
                lit_level::BRIGHT,
                false,
                0,
                true,  // as_independent_entity
                height_3d
            );
        }
};

void vehicle_preview_window::prepare( const catacurses::window &win )
{
    w_preview = win;
    win_pos = point( getbegx( win ), getbegy( win ) );
    win_cols = getmaxx( win );
    win_lines = getmaxy( win );

    termx_pixels = termx_to_pixel_value();
    termy_pixels = termy_to_pixel_value();

    // Apply current zoom level
    tilecontext->set_draw_scale( zoom );
}

point vehicle_preview_window::calc_window_center_pixels() const
{
    // Calculate center of window in pixel coordinates
    const int center_x = win_pos.x * termx_pixels + ( win_cols * termx_pixels ) / 2;
    const int center_y = win_pos.y * termy_pixels + ( win_lines * termy_pixels ) / 2;
    return point( center_x, center_y );
}

void vehicle_preview_window::zoom_in()
{
    if( zoom < MAX_ZOOM ) {
        zoom *= 2;
        if( zoom > MAX_ZOOM ) {
            zoom = MAX_ZOOM;
        }
        tilecontext->set_draw_scale( zoom );
    }
}

void vehicle_preview_window::zoom_out()
{
    if( zoom > MIN_ZOOM ) {
        zoom /= 2;
        if( zoom < MIN_ZOOM ) {
            zoom = MIN_ZOOM;
        }
        tilecontext->set_draw_scale( zoom );
    }
}

void vehicle_preview_window::draw_vpart_at_pixel( const vpart_id &vp_id, point pixel_pos,
        int part_mod, units::angle veh_facing, bool highlight )
{
    const int rotation_degrees = static_cast<int>( std::round( to_degrees( veh_facing ) ) );

    veh_preview_adapter *adapter = veh_preview_adapter::convert( &*tilecontext );
    adapter->draw_vpart_tile( vp_id, pixel_pos, part_mod, rotation_degrees, highlight );
}

void vehicle_preview_window::draw_cursor_at_pixel( point pixel_pos )
{
    veh_preview_adapter *adapter = veh_preview_adapter::convert( &*tilecontext );
    adapter->draw_cursor_tile( pixel_pos );
}

void vehicle_preview_window::display( const vehicle &veh, point cursor_offset, int highlight_part )
{
    const point center_px = calc_window_center_pixels();

    // Get tile dimensions at current zoom
    const int tile_w = tilecontext->get_tile_width();
    const int tile_h = tilecontext->get_tile_height();

    // Calculate window bounds in pixels for clipping
    const int win_left = win_pos.x * termx_pixels;
    const int win_top = win_pos.y * termy_pixels;
    const int win_width = win_cols * termx_pixels;
    const int win_height = win_lines * termy_pixels;

    // Set SDL clip rectangle to prevent drawing outside the window bounds
    const SDL_Renderer_Ptr &renderer = get_sdl_renderer();
    SDL_Rect clip_rect = { win_left, win_top, win_width, win_height };
    SDL_RenderSetClipRect( renderer.get(), &clip_rect );

    // Get all parts that should be displayed (one per tile)
    const std::vector<int> structural_parts = veh.all_standalone_parts();

    for( int part_idx : structural_parts ) {
        const vehicle_part &part = veh.cpart( part_idx );
        if( part.removed ) {
            continue;
        }

        const point mount = part.mount;

        // Calculate position relative to cursor
        // rotate(3) matches the rotation used in ASCII display_veh()
        const point rel = ( mount + cursor_offset ).rotate( 3 );

        // Convert to pixel position (centered in window)
        const point pixel_pos = center_px + point( rel.x * tile_w, rel.y * tile_h );

        // Get part rendering info
        char part_mod = 0;
        const vpart_id &vp_id = veh.part_id_string( part_idx, false, part_mod );

        const bool is_highlighted = ( part_idx == highlight_part ) ||
                                    ( mount == -cursor_offset ); // Cursor position

        draw_vpart_at_pixel( vp_id, pixel_pos, part_mod, veh.face.dir(), is_highlighted );
    }

    // Draw cursor at center (current cursor position)
    draw_cursor_at_pixel( center_px );

    // Clear the clip rectangle
    SDL_RenderSetClipRect( renderer.get(), nullptr );
}

void vehicle_preview_window::clear()
{
    // Restore default zoom level
    tilecontext->set_draw_scale( DEFAULT_TILESET_ZOOM );
}

#endif // TILES
