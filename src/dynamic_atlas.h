#pragma once

#if defined(TILES)

#include <optional>
#include <vector>

#include "sdl_wrappers.h"

class texture;

namespace detail
{
class texture_packer
{
    protected:
        SDL_Rect bounds;
        explicit texture_packer( const SDL_Rect &bounds ) : bounds( bounds ) {}
    public:
        virtual std::optional<SDL_Rect> pack( uint32_t w, uint32_t h ) = 0;
        virtual ~texture_packer() = 0;
};
} // namespace detail

using atlas_texture = std::pair<SDL_Texture_SharedPtr, SDL_Rect>;

class dynamic_atlas
{
        struct sprite_sheet {
            SDL_Texture_SharedPtr texture;
            std::unique_ptr<detail::texture_packer> packer;
            int atlas_width;
            int atlas_height;
            SDL_Surface_Ptr readback;
            bool dirty;
        };

    public:
        dynamic_atlas()
            : max_atlas_width( 0 ), max_atlas_height( 0 ), hint_sprite_width( 0 ), hint_sprite_height( 0 ) {}
        dynamic_atlas( const int w, const int h, const int sw = 0, const int sh = 0 )
            : max_atlas_width( w ), max_atlas_height( h ), hint_sprite_width( sw ), hint_sprite_height( sh ) {}

        auto get_surface( const texture &tex ) -> std::pair<SDL_Surface *, SDL_Rect>;
        auto allocate_sprite( int w, int h ) -> atlas_texture;
        void dump( const std::string &s ) ;
        void readback();
        void clear();

        auto begin() { return sheets.begin(); }
        auto end() { return sheets.end(); }

    private:
        std::vector<sprite_sheet> sheets;

        int max_atlas_width;
        int max_atlas_height;
        int hint_sprite_width;
        int hint_sprite_height;
};

#endif