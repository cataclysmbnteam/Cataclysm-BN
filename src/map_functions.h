#pragma once
#ifndef CATA_SRC_MAP_FUNCTIONS_H
#define CATA_SRC_MAP_FUNCTIONS_H

struct tripoint;
class map;

namespace map_funcs
{

/**
 * Checks both the neighborhoods of from and to for climbable surfaces,
 * returns move cost of climbing from `from` to `to`.
 * 0 means climbing is not possible.
 * Return value can depend on the orientation of the terrain.
 */
int climbing_cost( const map &m, const tripoint &from, const tripoint &to );

} // namespace map_funcs

#endif // CATA_SRC_MAP_FUNCTIONS_H
