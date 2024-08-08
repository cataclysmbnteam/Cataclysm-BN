///
/// A place for enum conversions which don't fit anywhere else
///
#include "enum_conversions.h"

#include "enums.h"

template<>
std::string io::enum_to_string<Attitude>( Attitude att )
{
    switch( att ) {
        case Attitude::A_HOSTILE:
            return "Hostile";
        case Attitude::A_NEUTRAL:
            return "Neutral";
        case Attitude::A_FRIENDLY:
            return "Friendly";
        case Attitude::A_ANY:
            return "Any";
        case Attitude::NUM_A:
            break;
    }
    debugmsg( "Invalid Attitude" );
    abort();
}

template<>
std::string io::enum_to_string<creature_size>( creature_size data )
{
    switch( data ) {
        case creature_size::tiny:
            return "TINY";
        case creature_size::small:
            return "SMALL";
        case creature_size::medium:
            return "MEDIUM";
        case creature_size::large:
            return "LARGE";
        case creature_size::huge:
            return "HUGE";
        case creature_size::num_creature_size:
            break;
    }
    debugmsg( "Invalid body_size value: %d", static_cast<int>( data ) );
    abort();
}

