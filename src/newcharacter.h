#pragma once
#ifndef CATA_SRC_NEWCHARACTER_H
#define CATA_SRC_NEWCHARACTER_H

#include <string>

#include "type_id.h"

class Character;

struct points_left {
    int stat_points = 0;
    int trait_points = 0;
    int skill_points = 0;

    enum point_limit : int {
        FREEFORM = 0,
        ONE_POOL,
        MULTI_POOL,
        TRANSFER,
    };
    point_limit limit = point_limit::FREEFORM;

    points_left();
    void init_from_options();
    // Highest amount of points to spend on stats without points going invalid
    auto stat_points_left() const -> int;
    auto trait_points_left() const -> int;
    auto skill_points_left() const -> int;
    auto is_freeform() -> bool;
    auto is_valid() -> bool;
    auto has_spare() -> bool;
    auto to_string() -> std::string;
};

namespace newcharacter
{
/** Returns the id of a random starting trait that costs >= 0 points */
auto random_good_trait() -> trait_id;
/** Returns the id of a random starting trait that costs < 0 points */
auto random_bad_trait() -> trait_id;
/**
 * Adds mandatory scenario and profession traits unless character already has them.
 * And if they do, refunds the points.
 */
void add_traits( Character &ch );
void add_traits( Character &ch, points_left &points );

/** Returns true if character has a conflicting trait to the entered trait. */
auto has_conflicting_trait( const Character &ch, const trait_id &t ) -> bool;
/** Returns true if charcater has a trait which upgrades into the entered trait. */
auto has_lower_trait( const Character &ch, const trait_id &t ) -> bool;
/** Returns true if character has a trait which is an upgrade of the entered trait. */
auto has_higher_trait( const Character &ch, const trait_id &t ) -> bool;
/** Returns true if character has a trait that shares a type with the entered trait. */
auto has_same_type_trait( const Character &ch, const trait_id &t ) -> bool;
} // namespace newcharacter

#endif // CATA_SRC_NEWCHARACTER_H
