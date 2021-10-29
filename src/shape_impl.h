#pragma once
#ifndef CATA_SRC_SHAPE_IMPL_H
#define CATA_SRC_SHAPE_IMPL_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include "cata_utility.h"
#include "cuboid_rectangle.h"
#include "line.h"
#include "shape.h"
#include "point.h"
#include "map_iterator.h"

class shape_impl
{
    public:
        virtual ~shape_impl() {}
        virtual double signed_distance( const rl_vec3d &p ) const = 0;

        virtual inclusive_cuboid<rl_vec3d> bounding_box() const = 0;
};

inclusive_cuboid<tripoint> shape::bounding_box() const
{
    const inclusive_cuboid<rl_vec3d> &bb_float = bounding_box_float();
    const tripoint min = tripoint( std::floor( bb_float.p_min.x ),
                                   std::floor( bb_float.p_min.y ),
                                   std::floor( bb_float.p_min.z ) );
    const tripoint max = tripoint( std::ceil( bb_float.p_max.x ),
                                   std::ceil( bb_float.p_max.y ),
                                   std::ceil( bb_float.p_max.z ) );
    return inclusive_cuboid<tripoint>( min, max );
}
inclusive_cuboid<rl_vec3d> shape::bounding_box_float() const
{
    return impl->bounding_box();
}

double shape::distance_at( const tripoint &p ) const
{
    return distance_at( rl_vec3d( p.x, p.y, p.z ) );
}
double shape::distance_at( const rl_vec3d &p ) const
{
    return impl->signed_distance( p );
}

template<typename T>
constexpr T sign( T x )
{
    return x > 0 ? 1 : ( x < 0 ? -1 : 0 );
}

/**
 * Cone centered along y axis.
 * Tip is at zero, base at (0, -h, 0).
 * A 90 degree half angle will cause zero division.
 */
class cone : public shape_impl
{
    private:
        double h;
        double sin_angle;
        double cos_angle;

    public:
        cone( double half_angle, double h )
            : h( h )
            , sin_angle( std::sin( half_angle ) )
            , cos_angle( std::cos( half_angle ) )
        {}

        double signed_distance( const rl_vec3d &p ) const override {
            // Based on https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
            rl_vec2d q( h * sin_angle / cos_angle, -h );
            rl_vec2d w( std::sqrt( p.x * p.x + p.z * p.z ), p.y );
            rl_vec2d a = w - q * clamp( w.dot_product( q ) / q.dot_product( q ), 0.0f, 1.0f );
            rl_vec2d b = w - rl_vec2d( q.x * clamp( w.x / q.x, 0.0f, 1.0f ), q.y );
            float k = q.y >= 0.0f ? 1.0f : -1.0f;
            float d = std::min( a.dot_product( a ), b.dot_product( b ) );
            float s = std::max( k * ( w.x * q.y - w.y * q.x ), k * ( w.y - q.y ) );
            return std::sqrt( d ) * sign( s );
        }

        inclusive_cuboid<rl_vec3d> bounding_box() const override {
            return inclusive_cuboid<rl_vec3d>( rl_vec3d( -sin_angle * h, -h, 0 ),
                                               rl_vec3d( sin_angle * h, 0, 0 ) );
        }
};

class empty_shape : public shape_impl
{
    public:
        empty_shape() = default;

        double signed_distance( const rl_vec3d & ) const override {
            return 0.0;
        }

        inclusive_cuboid<rl_vec3d> bounding_box() const override {
            return inclusive_cuboid<rl_vec3d>( rl_vec3d(), rl_vec3d() );
        }
};

/**
 * Offsets a shape by a point.
 * Actually offsets the point by minus point.
 */
class offset_shape : public shape_impl
{
    public:
        offset_shape( const std::shared_ptr<shape_impl> &shape, const rl_vec3d &offset )
            : shape( shape ), offset( offset )
        {}

        double signed_distance( const rl_vec3d &p ) const override {
            return shape->signed_distance( p - offset );
        }

        inclusive_cuboid<rl_vec3d> bounding_box() const override {
            const inclusive_cuboid<rl_vec3d> &bb = shape->bounding_box();
            return inclusive_cuboid<rl_vec3d>( bb.p_min - offset, bb.p_max - offset );
        }

    private:
        std::shared_ptr<shape_impl> shape;
        rl_vec3d offset;
};

/**
 * Rotates a shape around z axis by a given angle.
 * Actually rotates input point around z axis by minus angle.
 */
class rotate_z_shape : public shape_impl
{
    public:
        rotate_z_shape( const std::shared_ptr<shape_impl> &shape, double angle_z )
            : shape( shape ), angle_z( -angle_z )
        {}

        double signed_distance( const rl_vec3d &p ) const override {
            return shape->signed_distance( p.rotated( angle_z ) );
        }

        inclusive_cuboid<rl_vec3d> bounding_box() const override {
            const inclusive_cuboid<rl_vec3d> &bb = shape->bounding_box();
            const std::set<rl_vec3d> pts = {
                bb.p_min,
                rl_vec3d( bb.p_max.x, bb.p_min.y, bb.p_min.z ),
                rl_vec3d( bb.p_min.x, bb.p_max.y, bb.p_min.z ),
                rl_vec3d( bb.p_max.x, bb.p_max.y, bb.p_min.z ),
                rl_vec3d( bb.p_min.x, bb.p_min.y, bb.p_max.z ),
                rl_vec3d( bb.p_max.x, bb.p_min.y, bb.p_max.z ),
                rl_vec3d( bb.p_min.x, bb.p_max.y, bb.p_max.z ),
                bb.p_max
            };
            rl_vec3d min;
            rl_vec3d max;
            for( const rl_vec3d &v : pts ) {
                min.x = std::min( min.x, v.x );
                min.y = std::min( min.y, v.y );
                min.z = std::min( min.z, v.z );
                max.x = std::max( max.x, v.x );
                max.y = std::max( max.y, v.y );
                max.z = std::max( max.z, v.z );
            }
            return inclusive_cuboid<rl_vec3d>( min, max );
        }

    private:
        std::shared_ptr<shape_impl> shape;
        double angle_z;
};

#endif // CATA_SRC_SHAPE_IMPL_H
