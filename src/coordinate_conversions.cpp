#include "coordinate_conversions.h"

namespace
{

auto divide( int v, int m ) -> int
{
    if( v >= 0 ) {
        return v / m;
    }
    return ( v - m + 1 ) / m;
}

auto divide( int v, int m, int &r ) -> int
{
    const int result = divide( v, m );
    r = v - result * m;
    return result;
}

} // namespace


auto omt_to_om_copy( point p ) -> point
{
    return point( divide( p.x, OMAPX ), divide( p.y, OMAPY ) );
}

auto omt_to_om_copy( const tripoint &p ) -> tripoint
{
    return tripoint( divide( p.x, OMAPX ), divide( p.y, OMAPY ), p.z );
}

void omt_to_om( int &x, int &y )
{
    x = divide( x, OMAPX );
    y = divide( y, OMAPY );
}

auto omt_to_om_remain( int &x, int &y ) -> point
{
    return point( divide( x, OMAPX, x ), divide( y, OMAPY, y ) );
}

auto om_to_omt_copy( point p ) -> point
{
    return point( p.x * OMAPX, p.y * OMAPY );
}

auto sm_to_omt_copy( point p ) -> point
{
    return point( divide( p.x, 2 ), divide( p.y, 2 ) );
}

auto sm_to_omt_copy( const tripoint &p ) -> tripoint
{
    return tripoint( divide( p.x, 2 ), divide( p.y, 2 ), p.z );
}

void sm_to_omt( int &x, int &y )
{
    x = divide( x, 2 );
    y = divide( y, 2 );
}

auto sm_to_omt_remain( int &x, int &y ) -> point
{
    return point( divide( x, 2, x ), divide( y, 2, y ) );
}

auto sm_to_om_copy( point p ) -> point
{
    return point( divide( p.x, 2 * OMAPX ), divide( p.y, 2 * OMAPY ) );
}

auto sm_to_om_copy( const tripoint &p ) -> tripoint
{
    return tripoint( divide( p.x, 2 * OMAPX ), divide( p.y, 2 * OMAPY ), p.z );
}

void sm_to_om( int &x, int &y )
{
    x = divide( x, 2 * OMAPX );
    y = divide( y, 2 * OMAPY );
}

auto sm_to_om_remain( int &x, int &y ) -> point
{
    return point( divide( x, 2 * OMAPX, x ), divide( y, 2 * OMAPY, y ) );
}

auto omt_to_ms_copy( point p ) -> point
{
    return point( p.x * 2 * SEEX, p.y * 2 * SEEY );
}

auto omt_to_sm_copy( point p ) -> point
{
    return point( p.x * 2, p.y * 2 );
}

auto omt_to_sm_copy( const tripoint &p ) -> tripoint
{
    return tripoint( p.x * 2, p.y * 2, p.z );
}

void omt_to_sm( int &x, int &y )
{
    x *= 2;
    y *= 2;
}

auto om_to_sm_copy( point p ) -> point
{
    return point( p.x * 2 * OMAPX, p.y * 2 * OMAPX );
}

auto om_to_sm_copy( const tripoint &p ) -> tripoint
{
    return tripoint( p.x * 2 * OMAPX, p.y * 2 * OMAPX, p.z );
}

void om_to_sm( int &x, int &y )
{
    x *= 2 * OMAPX;
    y *= 2 * OMAPY;
}

auto ms_to_sm_copy( point p ) -> point
{
    return point( divide( p.x, SEEX ), divide( p.y, SEEY ) );
}

auto ms_to_sm_copy( const tripoint &p ) -> tripoint
{
    return tripoint( divide( p.x, SEEX ), divide( p.y, SEEY ), p.z );
}

void ms_to_sm( int &x, int &y )
{
    x = divide( x, SEEX );
    y = divide( y, SEEY );
}

auto ms_to_sm_remain( int &x, int &y ) -> point
{
    return point( divide( x, SEEX, x ), divide( y, SEEY, y ) );
}


void sm_to_ms( int &x, int &y )
{
    x *= SEEX;
    y *= SEEY;
}

auto ms_to_omt_copy( point p ) -> point
{
    return point( divide( p.x, SEEX * 2 ), divide( p.y, SEEY * 2 ) );
}

auto ms_to_omt_copy( const tripoint &p ) -> tripoint
{
    return tripoint( divide( p.x, SEEX * 2 ), divide( p.y, SEEY * 2 ), p.z );
}

void ms_to_omt( int &x, int &y )
{
    x = divide( x, SEEX * 2 );
    y = divide( y, SEEY * 2 );
}

auto ms_to_omt_remain( int &x, int &y ) -> point
{
    return point( divide( x, SEEX * 2, x ), divide( y, SEEY * 2, y ) );
}

auto omt_to_seg_copy( const tripoint &p ) -> tripoint
{
    return tripoint( divide( p.x, SEG_SIZE ), divide( p.y, SEG_SIZE ), p.z );
}

auto sm_to_mmr_remain( int &x, int &y ) -> point
{
    return point( divide( x, MM_REG_SIZE, x ), divide( y, MM_REG_SIZE, y ) );
}

auto mmr_to_sm_copy( const tripoint &p ) -> tripoint
{
    return tripoint( p.x * MM_REG_SIZE, p.y * MM_REG_SIZE, p.z );
}
