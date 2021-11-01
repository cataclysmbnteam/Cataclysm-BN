#pragma once
#ifndef CATA_SRC_SHAPE_H
#define CATA_SRC_SHAPE_H

#include <map>
#include <memory>

#include "cuboid_rectangle.h"
#include "point.h"
#include "point_float.h"

class shape_impl;
class shape_factory_impl;
class JsonIn;
class JsonOut;
class map;

/**
 * Class describing shapes in 3D space. The shapes can cover some points partially.
 */
class shape
{
    public:
        double distance_at( const tripoint &p ) const;
        double distance_at( const rl_vec3d &p ) const;
        inclusive_cuboid<tripoint> bounding_box() const;
        inclusive_cuboid<rl_vec3d> bounding_box_float() const;

        shape();
        shape( const std::shared_ptr<shape_impl> & );
        shape( const shape & );
    private:
        std::shared_ptr<shape_impl> impl;
};

/**
 * A class that produces shapes from two points: origin and destination.
 */
class shape_factory
{
    public:
        shape_factory();
        shape_factory( const shape_factory & );
        ~shape_factory();

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        std::shared_ptr<shape> create( const tripoint &start, const tripoint &end ) const;
    private:
        std::shared_ptr<shape_factory_impl> impl;
};

#endif // CATA_SRC_SHAPE_H
