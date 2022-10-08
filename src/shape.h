#pragma once
#ifndef CATA_SRC_SHAPE_H
#define CATA_SRC_SHAPE_H

#include <map>
#include <memory>

#include "cuboid_rectangle.h"
#include "point.h"
#include "point_float.h"
#include "translations.h"

class shape_impl;
class shape_factory_impl;
class JsonIn;
class JsonOut;

/**
 * Class describing shapes in 3D space. The shapes can cover some points partially.
 */
class shape
{
    public:
        auto distance_at( const tripoint &p ) const -> double;
        auto distance_at( const rl_vec3d &p ) const -> double;
        /**
         * Approximation of bounding box, guaranteed to contain all points.
         */
        auto bounding_box() const -> inclusive_cuboid<tripoint>;
        // TODO: Envelope instead of transformed bb
        auto bounding_box_float() const -> inclusive_cuboid<rl_vec3d>;
        auto get_origin() const -> const tripoint &;

        shape( const std::shared_ptr<shape_impl> &, const tripoint &origin );
        shape( const shape & );
    private:
        std::shared_ptr<shape_impl> impl;
        tripoint origin;
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

        auto create( const tripoint &start, const tripoint &end ) const -> std::shared_ptr<shape>;
        auto create( const rl_vec3d &start, const rl_vec3d &end ) const -> std::shared_ptr<shape>;
        auto get_range() const -> double;
        auto get_description() const -> std::string;

        auto operator=( const shape_factory &other ) -> shape_factory &;
    private:
        std::shared_ptr<shape_factory_impl> impl;
};

#endif // CATA_SRC_SHAPE_H
