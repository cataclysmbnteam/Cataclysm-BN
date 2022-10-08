#pragma once
#ifndef CATA_SRC_NAME_H
#define CATA_SRC_NAME_H

#include <string>

enum nameFlags {
    nameIsMaleName   = 1 << 0,
    nameIsFemaleName = 1 << 1,
    nameIsUnisexName = nameIsMaleName | nameIsFemaleName,
    nameIsGivenName  = 1 << 2,
    nameIsFamilyName = 1 << 3,
    nameIsNickName   = 1 << 4,
    nameIsTownName   = 1 << 5,
    nameIsFullName   = 1 << 6,
    nameIsWorldName  = 1 << 7
};

namespace Name
{
/// Load names from given json file to use for generation
void load_from_file( const std::string &filename );

/// Return a random name given search flags
auto get( nameFlags searchFlags ) -> std::string;

/// Return a random full name given gender
auto generate( bool is_male ) -> std::string;

/// Clear names used for generation
void clear();
} // namespace Name

inline auto operator|( nameFlags l, nameFlags r ) -> nameFlags
{
    return static_cast<nameFlags>( static_cast<unsigned>( l ) | static_cast<unsigned>( r ) );
}

inline auto operator&( nameFlags l, nameFlags r ) -> nameFlags
{
    return static_cast<nameFlags>( static_cast<unsigned>( l ) & static_cast<unsigned>( r ) );
}

#endif // CATA_SRC_NAME_H
