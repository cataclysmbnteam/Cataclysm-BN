#include <set>

#include "debug.h"
#include "json.h"
#include "make_static.h"
#include "shape.h"
#include "shape_impl.h"
#include "point_float.h"

shape::shape() = default;
shape::shape( const shape & ) = default;
shape::shape( const std::shared_ptr<shape_impl> &impl )
    : impl( impl )
{}

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

// TODO: Find good file
#include "map.h"
#include "map_iterator.h"
std::map<tripoint, double> shape::coverage( const map &here ) const
{
    std::map<tripoint, double> cov;
    inclusive_cuboid<tripoint> bb = bounding_box();
    for( const tripoint &p : here.points_in_rectangle( bb.p_min, bb.p_max ) ) {
        double shape_distance = distance_at( p );
        if( shape_distance > 0.0 ) {
            continue;
        }
        // TODO: Proper origin
        // TODO: Proper range
        // TODO: Proper limiting terrain
        if( /*here.sees( attacker.pos(), p, 60 )*/ true ) {
            cov[p] = -std::max( shape_distance, -1.0 );
        }
    }

    return cov;
}


shape_factory::shape_factory() = default;
shape_factory::shape_factory( const shape_factory & ) = default;
shape_factory::~shape_factory() = default;

void shape_factory::serialize( JsonOut &jsout ) const
{
    // TODO: poly_serialized should handle this with some rewriting
    jsout.start_array();
    if( impl != nullptr ) {
        jsout.write( impl->get_type() );
        jsout.start_object();
        impl->serialize( jsout );
        jsout.end_object();
    } else {
        jsout.write_null();
    }
    jsout.end_array();
}

void shape_factory::deserialize( JsonIn &jsin )
{
    jsin.start_array();
    std::string type_string = jsin.get_string();
    if( type_string == "cone" ) {
        impl = std::make_shared<cone_factory>();
        impl->deserialize( jsin );
    } else {
        debugmsg( "Invalid type" );
        impl.reset();
    }
    jsin.end_array();
}

std::shared_ptr<shape> shape_factory::create( const tripoint &start, const tripoint &end ) const
{
    if( impl == nullptr ) {
        return std::make_shared<shape>( std::make_shared<empty_shape>() );
    }
    return impl->create( start, end );
}
