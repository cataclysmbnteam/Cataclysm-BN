#pragma once
#ifndef CATA_SRC_RNG_H
#define CATA_SRC_RNG_H

#include <array>
#include <functional>
#include <iosfwd>
#include <random>
#include <type_traits>

#include "optional.h"
#include "units_angle.h"
#include "units_probability.h"

class map;
class time_duration;
template<typename Tripoint>
class tripoint_range;
struct tripoint;

// All PRNG functions use an engine, see the C++11 <random> header
// By default, that engine is seeded by time on first call to such a function.
// If this function is called with a non-zero seed then the engine will be
// seeded (or re-seeded) with the given seed.
void rng_set_engine_seed( unsigned int seed );

using cata_default_random_engine = std::minstd_rand0;
auto rng_get_engine() -> cata_default_random_engine &;
auto rng_bits() -> unsigned int;

auto rng( int lo, int hi ) -> int;
auto rng_float( double lo, double hi ) -> double;

template<typename U>
auto rng_float( units::quantity<double, U> lo,
                                      units::quantity<double, U> hi ) -> units::quantity<double, U>
{
    return { rng_float( lo.value(), hi.value() ), U{} };
}

auto random_direction() -> units::angle;

auto one_in( int chance ) -> bool;
auto one_turn_in( const time_duration &duration ) -> bool;
auto x_in_y( double x, double y ) -> bool;
auto check( units::probability p ) -> bool;
auto dice( int number, int sides ) -> int;

// Returns x + x_in_y( x-int(x), 1 )
auto roll_remainder( double value ) -> int;
inline auto roll_remainder( float value ) -> int
{
    return roll_remainder( static_cast<double>( value ) );
}

auto djb2_hash( const unsigned char *input ) -> int;

auto rng_normal( double lo, double hi ) -> double;

inline auto rng_normal( double hi ) -> double
{
    return rng_normal( 0.0, hi );
}

auto normal_roll( double mean, double stddev ) -> double;

auto rng_exponential( double min, double mean ) -> double;

inline auto rng_exponential( double mean ) -> double
{
    return rng_exponential( 0.0, mean );
}

auto exponential_roll( double lambda ) -> double;

/**
 * Returns a random entry in the container.
 * The container must have a `size()` function and must support iterators as usual.
 * For empty containers it returns the given default object.
 * `C` is the container type,
 * `D` is the type of the default value (which may differ from the return type)
 * `V` is the type of the elements in the container.
 * Note that this function does not return a reference because the default value could be
 * a temporary object that is not valid after this function has left:
 * \code random_entry( vect, std::string("default") ); \endcode
 */
template<typename C, typename D, typename V = typename C::value_type>
inline auto random_entry( const C &container, D default_value ) -> V
{
    if( container.empty() ) {
        return default_value;
    }
    auto iter = container.begin();
    std::advance( iter, rng( 0, container.size() - 1 ) );
    return *iter;
}
/**
 * Same as above but returns a reference to the chosen entry (if the container
 * is not empty) or an empty `optional` object (if the container is empty).
 * This function handles empty containers without requiring an instance of the
 * contained type when container is empty.
 */
template<typename C>
inline auto random_entry_opt( C &container ) ->
cata::optional<decltype( std::ref( *container.begin() ) )>
{
    if( container.empty() ) {
        return cata::nullopt;
    }
    auto iter = container.begin();
    std::advance( iter, rng( 0, container.size() - 1 ) );
    return std::ref( *iter );
}
/**
 * Same as above, but returns a default constructed value if the container
 * is empty.
 */
template<typename C, typename V = typename C::value_type>
inline auto random_entry( const C &container ) -> V
{
    if( container.empty() ) {
        return V();
    }
    auto iter = container.begin();
    std::advance( iter, rng( 0, container.size() - 1 ) );
    return *iter;
}

template<typename ...T>
class is_std_array_helper : public std::false_type
{
};
template<typename T, std::size_t N>
class is_std_array_helper<std::array<T, N>> : public std::true_type
{
};
template<typename T>
class is_std_array : public is_std_array_helper<typename std::decay<T>::type>
{
};

/**
 * Same as above, but with a statically allocated default value (using the default
 * constructor). This allows to return a reference, either into the given container
 * or to the default value.
 */
template<typename C, typename V = typename C::value_type>
inline auto random_entry_ref( const C &container ) -> typename std::enable_if < !is_std_array<C>::value,
       const V & >::type
{
    if( container.empty() ) {
        static const V default_value = V();
        return default_value;
    }
    auto iter = container.begin();
    std::advance( iter, rng( 0, container.size() - 1 ) );
    return *iter;
}
template<typename V, std::size_t N>
inline auto random_entry_ref( const std::array<V, N> &container ) -> const V &
{
    static_assert( N > 0, "Need a non-empty array to get a random value from it" );
    return container[rng( 0, N - 1 )];
}
/**
 * Returns a random entry in the container and removes it from the container.
 * The container must not be empty!
 */
template<typename C, typename V = typename C::value_type>
inline auto random_entry_removed( C &container ) -> V
{
    auto iter = container.begin();
    std::advance( iter, rng( 0, container.size() - 1 ) );
    const V result = std::move( *iter ); // Copy because the original is removed and thereby destroyed
    container.erase( iter );
    return result;
}

/// Returns a range enclosing all valid points of the map.
auto points_in_range( const map &m ) -> tripoint_range<tripoint>;
/// Returns a random point in the given range that satisfies the given predicate ( if any ).
auto random_point( const tripoint_range<tripoint> &range,
                                       const std::function<bool( const tripoint & )> &predicate ) -> cata::optional<tripoint>;
/// Same as other random_point with a range enclosing all valid points of the map.
auto random_point( const map &m,
                                       const std::function<bool( const tripoint & )> &predicate ) -> cata::optional<tripoint>;

#endif // CATA_SRC_RNG_H
