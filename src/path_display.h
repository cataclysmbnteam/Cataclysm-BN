#ifndef CATA_SRC_PATH_DISPLAY_H
#define CATA_SRC_PATH_DISPLAY_H

#include <string>

auto resolved_game_paths() -> std::string;
auto user_directory() -> std::string;
auto defaults_directory() -> std::string;
auto config_directory() -> std::string;

#endif // CATA_SRC_PATH_DISPLAY_H
