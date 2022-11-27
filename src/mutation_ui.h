#pragma once
#ifndef CATA_SRC_MUTATION_UI_H
#define CATA_SRC_MUTATION_UI_H

#include "type_id.h"

class Character;

namespace detail
{
enum class mutations_ui_cmd {
    exit,
    activate,
    deactivate,
};

struct mutations_ui_result {
    mutations_ui_cmd cmd;
    trait_id mut;
};

mutations_ui_result show_mutations_ui_internal( Character &who );

} // namespace detail

void show_mutations_ui( Character &who );

#endif // CATA_SRC_MUTATION_UI_H
