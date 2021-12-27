#pragma once
#ifndef CATA_SRC_CHARACTER_KEYBINDS_H
#define CATA_SRC_CHARACTER_KEYBINDS_H

#include <map>

#include "input.h"

class character_keybinds
{
    private:
        using keybinds_map = std::map<std::string, action_attributes>;
        keybinds_map keybinds;
    public:
        keybinds_map::const_iterator begin() const {
            return keybinds.begin();
        }
        keybinds_map::const_iterator end() const {
            return keybinds.end();
        }
};

// Global function - might not be the cleanest way to do it
const character_keybinds &get_character_keybinds();

#endif // CATA_SRC_CHARACTER_KEYBINDS_H
