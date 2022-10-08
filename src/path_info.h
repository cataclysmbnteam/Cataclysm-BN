#pragma once
#ifndef CATA_SRC_PATH_INFO_H
#define CATA_SRC_PATH_INFO_H

#include <string>

enum class holiday : int;

namespace PATH_INFO
{
void init_base_path( std::string path );
void init_user_dir( std::string dir );
void set_standard_filenames();

auto autopickup() -> std::string;
auto base_colors() -> std::string;
auto base_path() -> std::string;
auto colors() -> std::string;
auto color_templates() -> std::string;
auto config_dir() -> std::string;
auto custom_colors() -> std::string;
auto datadir() -> std::string;
auto debug() -> std::string;
auto defaultsounddir() -> std::string;
auto defaulttilejson() -> std::string;
auto defaulttilepng() -> std::string;
auto fontconfig() -> std::string;
auto user_fontconfig() -> std::string;
auto fontdir() -> std::string;
auto user_fontdir() -> std::string;
auto language_defs_file() -> std::string;
auto graveyarddir() -> std::string;
auto help() -> std::string;
auto keybindings() -> std::string;
auto keybindings_vehicle() -> std::string;
auto keybindings_edit_creature() -> std::string;
auto lastworld() -> std::string;
auto memorialdir() -> std::string;
auto jsondir() -> std::string;
auto moddir() -> std::string;
auto options() -> std::string;
auto panel_options() -> std::string;
auto safemode() -> std::string;
auto savedir() -> std::string;
auto sokoban() -> std::string;
auto templatedir() -> std::string;
auto user_dir() -> std::string;
auto user_keybindings() -> std::string;
auto user_moddir() -> std::string;
auto worldoptions() -> std::string;
auto crash() -> std::string;
auto tileset_conf() -> std::string;
auto gfxdir() -> std::string;
auto user_gfx() -> std::string;
auto data_sound() -> std::string;
auto user_sound() -> std::string;
auto mods_replacements() -> std::string;
auto mods_dev_default() -> std::string;
auto mods_user_default() -> std::string;
auto soundpack_conf() -> std::string;

auto credits() -> std::string;
auto motd() -> std::string;
auto title( holiday current_holiday ) -> std::string;
auto names() -> std::string;

void set_datadir( const std::string &datadir );
void set_config_dir( const std::string &config_dir );
void set_savedir( const std::string &savedir );
void set_memorialdir( const std::string &memorialdir );
void set_options( const std::string &options );
void set_autopickup( const std::string &autopickup );
void set_motd( const std::string &motd );

} // namespace PATH_INFO

#endif // CATA_SRC_PATH_INFO_H
