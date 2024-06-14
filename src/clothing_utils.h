#ifndef CATA_SRC_CLOTHING_UTILS_H
#define CATA_SRC_CLOTHING_UTILS_H

class Character;
class item;

/// Determines whether an item is compact enough to wear multiple of the same type
/// without encumbrance penalty.
auto is_compact( const item &it, const Character &c ) -> bool;

#endif // CATA_SRC_CLOTHING_UTILS_H
