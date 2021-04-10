#pragma once
#ifndef CATA_SRC_UNITS_H
#define CATA_SRC_UNITS_H

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "units_def.h"
#include "units_volume.h"
#include "units_mass.h"
#include "units_energy.h"
#include "units_money.h"
#include "units_temperature.h"

namespace units
{

// Streaming operators for debugging and tests
// (for UI output other functions should be used which render in the user's
// chosen units)
inline std::ostream &operator<<( std::ostream &o, mass_in_milligram_tag )
{
    return o << "mg";
}

inline std::ostream &operator<<( std::ostream &o, volume_in_milliliter_tag )
{
    return o << "ml";
}

inline std::ostream &operator<<( std::ostream &o, energy_in_millijoule_tag )
{
    return o << "mJ";
}

inline std::ostream &operator<<( std::ostream &o, money_in_cent_tag )
{
    return o << "cent";
}

inline std::ostream &operator<<( std::ostream &o, temperature_in_millidegree_celsius_tag )
{
    return o << "mC";
}

template<typename value_type, typename tag_type>
inline std::ostream &operator<<( std::ostream &o, const quantity<value_type, tag_type> &v )
{
    return o << v.value() << tag_type{};
}

std::string display( const units::energy v );

} // namespace units

namespace units
{
static const std::vector<std::pair<std::string, energy>> energy_units = { {
        { "mJ", 1_mJ },
        { "J", 1_J },
        { "kJ", 1_kJ },
    }
};
static const std::vector<std::pair<std::string, mass>> mass_units = { {
        { "mg", 1_milligram },
        { "g", 1_gram },
        { "kg", 1_kilogram },
    }
};
static const std::vector<std::pair<std::string, money>> money_units = { {
        { "cent", 1_cent },
        { "USD", 1_USD },
        { "kUSD", 1_kUSD },
    }
};
static const std::vector<std::pair<std::string, volume>> volume_units = { {
        { "ml", 1_ml },
        { "L", 1_liter }
    }
};
static const std::vector<std::pair<std::string, temperature>> temperature_units = { {
        { "mC", 1_mc },
        { "C", 1_c },
        { "F", 1_f }
    }
};
} // namespace units

#endif // CATA_SRC_UNITS_H
