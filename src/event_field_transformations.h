#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "cata_variant.h"

struct event_field_transformation {
    using function_type = std::vector<cata_variant>( * )( const cata_variant & );
    function_type function;
    cata_variant_type return_type;
    std::vector<cata_variant_type> argument_types;
};

extern const std::unordered_map<std::string, event_field_transformation>
event_field_transformations;


