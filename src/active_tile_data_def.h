#pragma once
#ifndef CATA_SRC_ACTIVE_TILE_DATA_DEF_H
#define CATA_SRC_ACTIVE_TILE_DATA_DEF_H

#include "active_tile_data.h"
#include "point.h"

class battery_tile : public active_tile_data
{
    public:
        /* In J */
        int stored;
        int max_stored;

        void update_internal( time_point to, const tripoint &p, distribution_grid &grid ) override;
        active_tile_data *clone() const override;
        const std::string &get_type() const override;
        void store( JsonOut &jsout ) const override;
        void load( JsonObject &jo ) override;

        int get_resource() const;
        int mod_resource( int amt );
};

class charger_tile : public active_tile_data
{
    public:
        /* In W */
        int power;

        void update_internal( time_point to, const tripoint &p, distribution_grid &grid ) override;
        active_tile_data *clone() const override;
        const std::string &get_type() const override;
        void store( JsonOut &jsout ) const override;
        void load( JsonObject &jo ) override;
};

class solar_tile : public active_tile_data
{
    public:
        /* In W */
        int power;

        void update_internal( time_point to, const tripoint &p, distribution_grid &grid ) override;
        active_tile_data *clone() const override;
        const std::string &get_type() const override;
        void store( JsonOut &jsout ) const override;
        void load( JsonObject &jo ) override;
};

class vehicle_connector_tile : public active_tile_data
{
    public:
        /* In absolute map square coordinates */
        std::vector<tripoint> connected_vehicles;

        void update_internal( time_point to, const tripoint &p, distribution_grid &grid ) override;
        active_tile_data *clone() const override;
        const std::string &get_type() const override;
        void store( JsonOut &jsout ) const override;
        void load( JsonObject &jo ) override;
};

#endif // CATA_SRC_ACTIVE_TILE_DATA_DEF_H
