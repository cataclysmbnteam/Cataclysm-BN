#pragma once
#ifndef CATA_SRC_VEHICLE_MOVE_H
#define CATA_SRC_VEHICLE_MOVE_H

#include "units_angle.h"
#include "point.h"

class map;
class vehicle;

namespace vehicle_movement
{

struct rail_processing_result {
    bool do_turn = false;
    bool do_shift = false;
    units::angle turn_dir;
    tripoint shift_amount;
};

/**
 * Decides how the vehicle should move in order to follow rails, or get on rails.
 */
auto process_movement_on_rails( const map &m, const vehicle &veh ) -> rail_processing_result;

/**
 * Returns whether the vehicle is currently on rails.
 */
auto is_on_rails( const map &m, const vehicle &veh ) -> bool;

} // namespace vehicle_movement

#endif // CATA_SRC_VEHICLE_MOVE_H
