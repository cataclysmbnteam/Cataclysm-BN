#pragma once
#ifndef CATA_SRC_DEBUG_MENU_H
#define CATA_SRC_DEBUG_MENU_H

struct tripoint;

namespace cata
{
template<typename T>
class optional;
} // namespace cata

class player;

namespace debug_menu
{
enum bench_kind {
    DRAW,
    FPS
};

void teleport_short();
void teleport_long();
void teleport_overmap( bool specific_coordinates = false );

void spawn_nested_mapgen();
void character_edit_menu();
void wishitem( player *p = nullptr );
void wishitem( player *p, const tripoint & );
void wishmonster( const cata::optional<tripoint> &p );
void wishmutate( player *p );
void wishskill( player *p );
void mutation_wish();
void benchmark( int max_difference, bench_kind kind );

void debug();

} // namespace debug_menu

#endif // CATA_SRC_DEBUG_MENU_H
