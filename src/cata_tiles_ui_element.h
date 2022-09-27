#pragma once
#ifndef CATA_SRC_CATA_TILES_UI_ELEMENT_H
#define CATA_SRC_CATA_TILES_UI_ELEMENT_H

#include <string>
#include <vector>

#include "string_id.h"
#include "int_id.h"

class JsonObject;

class tiles_ui_element;

using ui_element_str_id = string_id<tiles_ui_element>;
using ui_element_int_id = int_id<tiles_ui_element>;

class tiles_ui_element
{
    public:
        static void reset();
        static void load_ui_element( const JsonObject &jo, const std::string &src );
        static void finalize();
        static const std::vector<tiles_ui_element> &get_all();
        void load( const JsonObject &jo, const std::string & );

        bool was_loaded = false;
        ui_element_str_id id;
        ui_element_int_id id_int;
};

#endif // CATA_SRC_CATA_TILES_UI_ELEMENT_H
