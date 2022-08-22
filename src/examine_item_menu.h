#pragma once
#ifndef CATA_SRC_EXAMINE_ITEM_MENU_H
#define CATA_SRC_EXAMINE_ITEM_MENU_H

#include <functional>

#include "item_location.h"

namespace examine_item_menu
{

enum class menu_pos_t {
    left,
    right
};

bool run(
    item_location loc,
    const std::function<int()> &func_pos_x,
    const std::function<int()> &func_width,
    menu_pos_t menu_pos
);

} // namespace examine_item_menu

#endif // CATA_SRC_EXAMINE_ITEM_MENU_H
