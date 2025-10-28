#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

#include "color.h"
#include "sdl_wrappers.h"
#include "cached_options.h"

using color_pixel_function_pointer = SDL_Color( * )( const SDL_Color &color );
using color_pixel_function_map = std::unordered_map<std::string, color_pixel_function_pointer>;

color_pixel_function_pointer get_color_pixel_function( const std::string &name );

SDL_Color adjust_color_brightness( const SDL_Color &color, int percent );

SDL_Color mix_colors( const SDL_Color &first, const SDL_Color &second, int second_percent );

inline bool is_black( const SDL_Color &color )
{
    return
        color.r == 0x00 &&
        color.g == 0x00 &&
        color.b == 0x00;
}

template<bool RT = true, bool CR = false, bool VP = false, bool DC = false, bool BM = false>
struct sdl_render_state {
    using tRT = std::conditional_t<RT, std::tuple<SDL_Texture*>, std::tuple<>>;
    using tCR = std::conditional_t<CR, std::tuple<SDL_Rect>, std::tuple<>>;
    using tVP = std::conditional_t<VP, std::tuple<SDL_Rect>, std::tuple<>>;
    using tDC = std::conditional_t<DC, std::tuple<SDL_Color>, std::tuple<>>;
    using tBM = std::conditional_t<BM, std::tuple<SDL_BlendMode>, std::tuple<>>;

    constexpr static size_t RT_IDX = 0;
    constexpr static size_t CR_IDX = CR ? (RT_IDX + 1) : RT_IDX;
    constexpr static size_t VP_IDX = VP ? (CR_IDX + 1) : CR_IDX;
    constexpr static size_t DC_IDX = DC ? (VP_IDX + 1) : VP_IDX;
    constexpr static size_t BM_IDX = BM ? (DC_IDX + 1) : DC_IDX;

    using tuple_type = decltype(std::tuple_cat(tRT{}, tCR{}, tVP{}, tDC{}, tBM{}));

    tuple_type value;
};

template<bool RT = true, bool CR = false, bool VP = false, bool DC = false, bool BM = false>
auto sdl_save_render_state( SDL_Renderer* r ) -> sdl_render_state<RT, CR, VP, DC, BM > {

    using type = sdl_render_state<RT, CR, VP, DC, BM >;

    typename type::tuple_type res;
    if constexpr (RT) {
        std::get<type::RT_IDX>(res) = SDL_GetRenderTarget( r );
    }
    if constexpr (CR) {
        SDL_Rect& v = std::get<type::CR_IDX>(res);
        SDL_RenderGetClipRect( r, &v );
    }
    if constexpr (VP) {
        SDL_Rect& v = std::get<type::VP_IDX>(res);
        SDL_RenderGetViewport( r, &v );
    }
    if constexpr (DC) {
        SDL_Color& v = std::get<type::DC_IDX>(res);
        SDL_GetRenderDrawColor(r, &v.r, &v.g, &v.b, &v.a);
    }
    if constexpr (BM) {
        SDL_BlendMode& v = std::get<type::BM_IDX>(res);
        SDL_GetRenderDrawBlendMode(r, &v);
    }
    return type{res};
}

template<bool RT = true, bool CR = false, bool VP = false, bool DC = false, bool BM = false>
auto sdl_restore_render_state( SDL_Renderer* r, const sdl_render_state<RT, CR, VP, DC, BM> &state ) {
    auto& t = state.value;
    using type = sdl_render_state<RT, CR, VP, DC, BM>;
    if constexpr (RT) {
        const auto i = type::RT_IDX;
        SDL_Texture* v = std::get<type::RT_IDX>(t);
        SDL_SetRenderTarget(r, v);
    }
    if constexpr (CR) {
        const auto i = type::CR_IDX;
        const SDL_Rect& v = std::get<type::CR_IDX>(t);
        SDL_RenderSetClipRect(r, &v);
    }
    if constexpr (VP) {
        const auto i = type::VP_IDX;
        const SDL_Rect& v = std::get<type::VP_IDX>(t);
        SDL_RenderSetViewport(r, &v);
    }
    if constexpr (DC) {
        const auto i = type::DC_IDX;
        const SDL_Color& v = std::get<type::DC_IDX>(t);
        SDL_SetRenderDrawColor(r, v.r, v.g, v.b, v.a);
    }
    if constexpr (BM) {
        const auto i = type::BM_IDX;
        const SDL_BlendMode& v = std::get<type::BM_IDX>(t);
        SDL_SetRenderDrawBlendMode(r, v);
    }
}

inline Uint8 average_pixel_color( const SDL_Color &color )
{
    return 85 * ( color.r + color.g + color.b ) >> 8; // 85/256 ~ 1/3
}

SDL_Color color_pixel_grayscale( const SDL_Color &color );

SDL_Color color_pixel_nightvision( const SDL_Color &color );

SDL_Color color_pixel_overexposed( const SDL_Color &color );

SDL_Color color_pixel_underwater( const SDL_Color &color );

SDL_Color color_pixel_underwater_dark( const SDL_Color &color );

SDL_Color color_pixel_darken( const SDL_Color &color );

SDL_Color color_pixel_sepia( const SDL_Color &color );

SDL_Color color_pixel_z_overlay( const SDL_Color &color );

SDL_Color curses_color_to_SDL( const nc_color &color );

///@throws std::exception upon errors.
///@returns Always a valid pointer.
SDL_Surface_Ptr create_surface_32( int w, int h );

SDL_Rect fit_rect_inside( const SDL_Rect &inner, const SDL_Rect &outer );

/** Linearly interpolate intermediate colors between two given colors.
 * @param start_color: The color to start with.
 * @param end_color: The color that ends the interpolation.
 * @param additional_steps: Number of steps between the start and stop colors.
 * @return A vector of colors containing: the start color, all intermediate colors and finally the end color.
 * @note start with white (r=255, g=255, b=255) and end with blue (r=0, g=0, b=255) and use 2 additional steps:
 *     - The first intermediate color is: (r=170, g=170, b=255)
 *     - The second intermediate color is: (r=85, g=85, b=255)
 * Obviously the more intermediate steps there are, the harder it is to differentiate the intermediate colors.
 */
std::vector<SDL_Color> color_linear_interpolate( const SDL_Color &start_color,
        const SDL_Color &end_color, unsigned additional_steps );


