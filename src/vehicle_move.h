#pragma once
#ifndef CATA_SRC_VEHICLE_MOVE_H
#define CATA_SRC_VEHICLE_MOVE_H

#include "units_angle.h"
#include "point.h"

class vehicle;

namespace vehicle_movement
{

struct rail_processing_result {
    bool do_turn = false;
    bool do_shift = false;
    units::angle turn_dir;
    tripoint shift_amount;
};

rail_processing_result process_movement_on_rails( const vehicle &veh );

bool is_on_rails( const vehicle &veh );

} // namespace vehicle_movement

#endif // CATA_SRC_VEHICLE_MOVE_H
