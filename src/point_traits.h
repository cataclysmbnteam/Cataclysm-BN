#ifndef CATA_SRC_POINT_TRAITS_H
#define CATA_SRC_POINT_TRAITS_H

#include <type_traits>

struct point;
struct tripoint;
struct rl_vec2d;
struct rl_vec3d;

template<typename Point, typename = void>
struct point_traits {
    static int &x( Point &p ) {
        return p.x();
    }
    static int x( const Point &p ) {
        return p.x();
    }
    static int &y( Point &p ) {
        return p.y();
    }
    static int y( const Point &p ) {
        return p.y();
    }
    static int &z( Point &p ) {
        return p.z();
    }
    static int z( const Point &p ) {
        return p.z();
    }
};

template<typename Point>
struct point_traits <
    Point,
    std::enable_if_t < std::is_same<Point, point>::value || std::is_same<Point, tripoint>::value >
    >  {
    static int &x( Point &p ) {
        return p.x;
    }
    static const int &x( const Point &p ) {
        return p.x;
    }
    static int &y( Point &p ) {
        return p.y;
    }
    static const int &y( const Point &p ) {
        return p.y;
    }
    static int &z( Point &p ) {
        return p.z;
    }
    static const int &z( const Point &p ) {
        return p.z;
    }
};

template<typename Point>
struct point_traits <
    Point,
    std::enable_if_t < std::is_same<Point, rl_vec2d>::value || std::is_same<Point, rl_vec3d>::value >
    >  {
    static float &x( Point &p ) {
        return p.x;
    }
    static const float &x( const Point &p ) {
        return p.x;
    }
    static float &y( Point &p ) {
        return p.y;
    }
    static const float &y( const Point &p ) {
        return p.y;
    }
    static float &z( Point &p ) {
        return p.z;
    }
    static const float &z( const Point &p ) {
        return p.z;
    }
};

#endif // CATA_SRC_POINT_TRAITS_H
