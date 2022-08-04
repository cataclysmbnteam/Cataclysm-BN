#include "map_functions.h"
#include "map.h"

namespace map_funcs
{

int climbing_cost( const map &m, const tripoint &from, const tripoint &to )
{
    // TODO: All sorts of mutations, equipment weight etc. for characters
    if( !m.valid_move( from, to, false, true ) ) {
        return 0;
    }
    const int diff = m.climb_difficulty( from );
    if( diff > 5 ) {
        return 0;
    }
    return 50 + diff * 100;
}

} // namespace map_funcs
