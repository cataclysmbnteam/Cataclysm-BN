#pragma once
#ifndef CATA_SRC_HELP_H
#define CATA_SRC_HELP_H

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "cursesdef.h"
#include "input.h"

class JsonIn;

class help
{
    public:
        void load();
        void display_help();

    private:
        void deserialize( JsonIn &jsin );
        void draw_menu( const catacurses::window &win );
        auto get_note_colors() -> std::string;
        auto get_dir_grid() -> std::string;

        std::map<int, std::pair<std::string, std::vector<std::string> > > help_texts;
        std::vector< std::vector<std::string> > hotkeys;

        input_context ctxt;
};

auto get_help() -> help &;

auto get_hint() -> std::string;

#endif // CATA_SRC_HELP_H
