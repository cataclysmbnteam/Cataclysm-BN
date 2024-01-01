#include "enums.h"
#include "enum_conversions.h"

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
