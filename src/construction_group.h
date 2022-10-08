#pragma once
#ifndef CATA_SRC_CONSTRUCTION_GROUP_H
#define CATA_SRC_CONSTRUCTION_GROUP_H

#include <string>
#include <vector>

#include "translations.h"
#include "type_id.h"

class JsonObject;

struct construction_group {
        void load( const JsonObject &jo, const std::string &src );

        construction_group_str_id id;
        bool was_loaded = false;

        auto name() const -> std::string;

        static auto count() -> size_t;

    private:
        translation _name;
};

namespace construction_groups
{

void load( const JsonObject &jo, const std::string &src );
void reset();

auto get_all() -> const std::vector<construction_group> &;

} // namespace construction_groups

#endif // CATA_SRC_CONSTRUCTION_GROUP_H
