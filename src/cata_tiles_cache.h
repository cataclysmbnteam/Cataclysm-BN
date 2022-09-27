#pragma once
#ifndef CATA_SRC_CATA_TILES_CACHE_H
#define CATA_SRC_CATA_TILES_CACHE_H

#include <vector>
#include <string>

struct tile_type;
enum TILE_CATEGORY;

using tile_ptr = const tile_type*;

struct cata_tiles_cache {
    public:
        void build_cache();

        static tile_ptr find_tile(
            const std::string &tile_id
        );
        static tile_ptr find_tile(
            const std::string &tile_id,
            TILE_CATEGORY category
        );
        static tile_ptr find_tile(
            const std::string &tile_id,
            TILE_CATEGORY category,
            const std::string &subcategory
        );

    private:
        std::vector<tile_ptr> ascii;
        std::vector<tile_ptr> ui_elems;
        std::vector<tile_ptr> fld;
        std::vector<tile_ptr> trp;
        std::vector<tile_ptr> ter;
        std::vector<tile_ptr> furn;
        std::vector<tile_ptr> mon;
        std::vector<tile_ptr> mon_corpse;
        std::vector<tile_ptr> mon_rid;
        std::vector<tile_ptr> itm;
        std::vector<tile_ptr> itm_worn;
        std::vector<tile_ptr> itm_wielded;
        std::vector<tile_ptr> om_ter;
        std::vector<tile_ptr> trait_passive_m;
        std::vector<tile_ptr> trait_passive_f;
        std::vector<tile_ptr> trait_active_m;
        std::vector<tile_ptr> trait_active_f;
        std::vector<tile_ptr> bionic_passive_m;
        std::vector<tile_ptr> bionic_passive_f;
        std::vector<tile_ptr> bionic_active_m;
        std::vector<tile_ptr> bionic_active_f;
        std::vector<tile_ptr> effect_overlay;
        std::vector<tile_ptr> weather_ter;
        std::vector<tile_ptr> weather_om;
        std::vector<tile_ptr> spell_aoe;
        std::vector<tile_ptr> om_hordes;
        std::vector<tile_ptr> vp;
        std::vector<tile_ptr> vp_open;
        std::vector<tile_ptr> vp_broken;
        // TODO: overmap notes?
        // TODO: explicit per-category fallbacks?
};

#endif // CATA_SRC_CATA_TILES_CACHE_H
