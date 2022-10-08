#pragma once
#ifndef CATA_SRC_PANELS_H
#define CATA_SRC_PANELS_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "coordinates.h"

class JsonIn;
class JsonOut;
class avatar;
struct point;
struct tripoint;

namespace catacurses
{
class window;
} // namespace catacurses
enum face_type : int {
    face_human = 0,
    face_bird,
    face_bear,
    face_cat,
    num_face_types
};

namespace overmap_ui
{
void draw_overmap_chunk( const catacurses::window &w_minimap, const avatar &you,
                         const tripoint_abs_omt &global_omt, const point &start, int width,
                         int height );
} // namespace overmap_ui

auto default_render() -> bool;

class window_panel
{
    public:
        window_panel( std::function<void( avatar &, const catacurses::window & )> draw_func,
                      const std::string &nm, int ht, int wd, bool default_toggle_,
                      std::function<bool()> render_func = default_render, bool force_draw = false );

        std::function<void( avatar &, const catacurses::window & )> draw;
        std::function<bool()> render;

        auto get_height() const -> int;
        auto get_width() const -> int;
        auto get_name() const -> std::string;
        bool toggle;
        bool always_draw;

    private:
        int height;
        int width;
        bool default_toggle;
        std::string name;
};

class panel_manager
{
    public:
        panel_manager();
        ~panel_manager() = default;
        panel_manager( panel_manager && ) = default;
        panel_manager( const panel_manager & ) = default;
        auto operator=( panel_manager && ) -> panel_manager & = default;
        auto operator=( const panel_manager & ) -> panel_manager & = default;

        static auto get_manager() -> panel_manager & {
            static panel_manager single_instance;
            return single_instance;
        }

        auto get_current_layout() -> std::vector<window_panel> &;
        auto get_current_layout_id() const -> std::string;
        auto get_width_right() -> int;
        auto get_width_left() -> int;

        void show_adm();

        void init();

    private:
        auto save() -> bool;
        auto load() -> bool;
        void serialize( JsonOut &json );
        void deserialize( JsonIn &jsin );
        // update the screen offsets so the game knows how to adjust the main window
        void update_offsets( int x );

        // The amount of screen space from each edge the sidebar takes up
        int width_right = 0;
        int width_left = 0;
        std::string current_layout_id;
        std::map<std::string, std::vector<window_panel>> layouts;

};

#endif // CATA_SRC_PANELS_H
