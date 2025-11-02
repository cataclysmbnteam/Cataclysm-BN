#include "cata_tiles.h"
#if defined(TILES)

#include "map.h"
#include "monster.h"
#include "character.h"
#include "field.h"

auto cata_tiles::get_overmap_color(
    const overmapbuffer &o, const tripoint_abs_omt &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_terrain_color(
    const ter_t &t, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_furniture_color(
    const furn_t &f, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_graffiti_color(
    const map &m, const tripoint &p )-> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_trap_color(
    const trap &tr, const map &map, tripoint tripoint ) -> color_tint_pair
{
    return { std::nullopt, std::nullopt};
}

auto cata_tiles::get_field_color(
    const field &f, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_item_color(
    const item &i, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_vpart_color(
    const optional_vpart_position &vp, const map &m, const tripoint &p )-> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_monster_color(
    const monster &mon, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_character_color(
    const Character &ch, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_effect_color(
    const effect &eff, const Character &c, const map &m, const tripoint &p ) -> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_bionic_color(
    const bionic &bio, const Character &c, const map &m, const tripoint &p )-> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

auto cata_tiles::get_mutation_color(
    const mutation &mut, const Character &c, const map &m, const tripoint &p )-> color_tint_pair
{
    return {std::nullopt, std::nullopt};
}

#endif