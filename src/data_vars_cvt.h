#pragma once

#include <sstream>
#include <iomanip>

#include "point.h"
#include "json.h"
#include "cata_utility.h"

namespace data_vars
{

template<typename T>
struct json_converter {
    using value_type = T;

    bool operator()( const T &in_val, std::string &out_val ) const {
        std::ostringstream os;
        JsonOut jsout{os};
        jsout.write( in_val );
        out_val = os.str();
        return true;
    }

    bool operator()( const std::string &in_val, T &out_val ) const {
        std::istringstream is{in_val};
        JsonIn jsin{is};
        return jsin.read( out_val, false );
    }
};

template<typename T>
struct type_converter {
    using type = json_converter<T>;
};

} // namespace data_vars
