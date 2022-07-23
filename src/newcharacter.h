#pragma once
#ifndef CATA_SRC_NEWCHARACTER_H
#define CATA_SRC_NEWCHARACTER_H

#include <string>

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
    int stat_points_left() const;
    int trait_points_left() const;
    int skill_points_left() const;
    bool is_freeform();
    bool is_valid();
    bool has_spare();
    std::string to_string();
};

#endif // CATA_SRC_NEWCHARACTER_H
