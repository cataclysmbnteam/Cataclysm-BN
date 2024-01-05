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

template<>
std::string io::enum_to_string<m_size>( m_size data )
{
    switch( data ) {
        case m_size::MS_TINY:
            return "TINY";
        case m_size::MS_SMALL:
            return "SMALL";
        case m_size::MS_MEDIUM:
            return "MEDIUM";
        case m_size::MS_LARGE:
            return "LARGE";
        case m_size::MS_HUGE:
            return "HUGE";
        case m_size::num_m_size:
            break;
    }
    debugmsg( "Invalid body_size value: %d", static_cast<int>( data ) );
    abort();
}

