#pragma once
#ifndef CATA_SRC_GAME_INFO_H
#define CATA_SRC_GAME_INFO_H

#include <optional>
#include <string>

namespace game_info
{
/** Return the name of the current operating system. */
std::string operating_system();
/** Return a detailed version of the operating system; e.g. "Ubuntu 18.04" or "(Windows) 10 1809". */
std::string operating_system_version();
/** Return the "bitness" of the game (not necessarily of the operating system); either: 64, 32 or nullopt. */
std::optional<int> bitness();
/** Return the "bitness" string of the game (not necessarily of the operating system); either: 64-bit, 32-bit or Unknown. */
std::string bitness_string();
/** Return the game version, as in the entry screen. */
std::string game_version();
/** Return the underlying graphics version used by the game; either Tiles or Curses. */
std::string graphics_version();
/** Return a list of the loaded mods, including the mod full name and its id name in brackets, e.g. "Dark Days Ahead [dda]". */
std::string mods_loaded();
/** Generate a game report, including the information returned by all of the other functions. */
std::string game_report();
/** Current world save file version */
std::string save_file_version();
} // namespace game_info

#endif // CATA_SRC_GAME_INFO_H
