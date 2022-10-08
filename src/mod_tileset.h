#pragma once
#ifndef CATA_SRC_MOD_TILESET_H
#define CATA_SRC_MOD_TILESET_H

#include <string>
#include <vector>

class mod_tileset;
class JsonObject;

extern std::vector<mod_tileset> all_mod_tilesets;

void load_mod_tileset( const JsonObject &jsobj, const std::string &, const std::string &base_path,
                       const std::string &full_path );
void reset_mod_tileset();

class mod_tileset
{
    public:
        mod_tileset( const std::string &new_base_path, const std::string &new_full_path,
                     int new_num_in_file ) :
            base_path_( new_base_path ),
            full_path_( new_full_path ),
            num_in_file_( new_num_in_file ) { }

        auto get_base_path() const -> const std::string & {
            return base_path_;
        }

        auto get_full_path() const -> const std::string & {
            return full_path_;
        }

        auto num_in_file() const -> int {
            return num_in_file_;
        }

        auto is_compatible( const std::string &tileset_id ) const -> bool;
        void add_compatible_tileset( const std::string &tileset_id );

    private:
        std::string base_path_;
        std::string full_path_;
        int num_in_file_;
        std::vector<std::string> compatibility;
};

#endif // CATA_SRC_MOD_TILESET_H
