#pragma once
#ifndef CATA_SRC_SHAPE_IMPL_H
#define CATA_SRC_SHAPE_IMPL_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <set>
#include <vector>
#include "cata_utility.h"
#include "cuboid_rectangle.h"
#include "line.h"
#include "shape.h"
#include "point.h"
#include "point_float.h"
#include "map_iterator.h"
#include "json.h"

template<typename T>
constexpr T sign( T x )
{
    return x > 0 ? 1 : ( x < 0 ? -1 : 0 );
}

class shape_impl
{
    public:
        virtual ~shape_impl() {}
        virtual double signed_distance( const rl_vec3d &p ) const = 0;

        virtual inclusive_cuboid<rl_vec3d> bounding_box() const = 0;
};

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

class shape_factory_impl
{
    public:
        virtual std::shared_ptr<shape> create( const tripoint &start, const tripoint &end ) const = 0;
        virtual const std::string &get_type() const = 0;
        virtual ~shape_factory_impl() = default;

        virtual void serialize( JsonOut & ) const {};
        virtual void deserialize( JsonIn & ) {};
};

#include "make_static.h"
class cone_factory : public shape_factory_impl
{
    private:
        /** TODO: Angles deserve a proper unit type */
        double half_angle;
        double length;
    public:
        cone_factory() = default;
        cone_factory( const cone_factory & ) = default;
        cone_factory( double half_angle, double length )
            : half_angle( half_angle )
            , length( length )
        {}

        std::shared_ptr<shape> create( const tripoint &start, const tripoint &end ) const override {
            auto c = std::make_shared<cone>( half_angle, length );
            tripoint diff = end - start;
            // Plus 90 deg because @ref cone extends in -y direction
            double rotation_angle = atan2( diff.y, diff.x ) + deg2rad( 90 );
            std::shared_ptr<rotate_z_shape> r = std::make_shared<rotate_z_shape>( c, rotation_angle );
            std::shared_ptr<offset_shape> o = std::make_shared<offset_shape>( r,
                                              rl_vec3d( start.x, start.y, start.z ) );
            return std::make_shared<shape>( o );
        }

        const std::string &get_type() const override {
            return STATIC( "cone_factory" );
        }

        void serialize( JsonOut &jsout ) const override {
            jsout.start_object();
            jsout.member( "half_angle", static_cast<int>( std::round( rad2deg( half_angle ) ) ) );
            jsout.member( "length", length );
        }
        void deserialize( JsonIn &jsin ) override {
            JsonObject jo = jsin.get_object();
            half_angle = deg2rad( jo.get_int( "half_angle" ) );
            jo.read( "length", length );
        }
};

#endif // CATA_SRC_SHAPE_IMPL_H
