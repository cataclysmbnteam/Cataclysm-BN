#pragma once
#ifndef CATA_SRC_CHARACTER_DISPLAY_H
#define CATA_SRC_CHARACTER_DISPLAY_H

#include "character_stat.h"

class Character;
class item;
class avatar;

namespace catacurses
{
class window;
} // namespace catacurses

namespace character_display
{

/**
 * Formats and prints encumbrance info to specified window
 */
void print_encumbrance( const catacurses::window &win, const Character &ch, int line = -1,
                        const item *selected_clothing = nullptr );

/**
 * @brief Handles and displays detailed character info for the '@' screen.
 *
 * @param ch Character to display info for. Has to be non-const reference
 * to allow toggling skills and upgrading stats for stats-through-x mods.
 */
void disp_info( Character &ch );

/**
 * Handles upgrade of avatar stats.
 */
void upgrade_stat_prompt( avatar &you, const character_stat &stat );

} // namespace character_display

#endif // CATA_SRC_CHARACTER_DISPLAY_H
