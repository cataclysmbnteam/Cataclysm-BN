#pragma once
#ifndef CATA_SRC_DEBUG_MENU_H
#define CATA_SRC_DEBUG_MENU_H

#include <optional>

struct tripoint;

class Character;
class Creature;
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
void character_edit_menu( Character &c );
void effect_edit_menu( Creature &c );
void wishitem( player *p = nullptr );
void wishitem( player *p, const tripoint & );
void wishmonster( const std::optional<tripoint> &p );
void wishmutate( player *p );
void wishbionics( Character &c );
void wishskill( player *p );
void mutation_wish();
void benchmark( int max_difference, bench_kind kind );

void debug();

} // namespace debug_menu

#endif // CATA_SRC_DEBUG_MENU_H
