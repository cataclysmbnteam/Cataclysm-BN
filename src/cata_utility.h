#pragma once
#ifndef CATA_SRC_CATA_UTILITY_H
#define CATA_SRC_CATA_UTILITY_H

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>
#include <type_traits>

/**
 * Greater-than comparison operator; required by the sort interface
 */
struct pair_greater_cmp_first {
    template< class T, class U >
    auto operator()( const std::pair<T, U> &a, const std::pair<T, U> &b ) const -> bool {
        return a.first > b.first;
    }

};

/**
 * For use with smart pointers when you don't actually want the deleter to do
 * anything.
 */
struct null_deleter {
    template<typename T>
    void operator()( T * ) const {}
};

/**
 * Round a floating point value down to the nearest integer
 *
 * Optimized floor function, similar to std::floor but faster.
 */
inline auto fast_floor( double v ) -> int
{
    return static_cast<int>( v ) - ( v < static_cast<int>( v ) );
}

/**
 * Round a value up at a given decimal place.
 *
 * @param val Value to be rounded.
 * @param dp Decimal place to round the value at.
 * @return Rounded value.
 */
auto round_up( double val, unsigned int dp ) -> double;

/** Divide @p num by @p den, rounding up
*
* @p num must be non-negative, @p den must be positive, and @c num+den must not overflow.
*/
template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
auto divide_round_up( T num, T den ) -> T
{
    return ( num + den - 1 ) / den;
}

/**
 * Determine whether a value is between two given boundaries.
 *
 * @param test Value to be tested.
 * @param down Lower boundary for value.
 * @param up Upper boundary for value.
 *
 * @return True if test value is greater than lower boundary and less than upper
 *         boundary, otherwise returns false.
 */
auto isBetween( int test, int down, int up ) -> bool;

/**
 * Basic logistic function.
 *
 * Calculates the value at a single point on a standard logistic curve.
 *
 * @param t Point on logistic curve to retrieve value for
 *
 * @return Value of the logistic curve at the given point
 */
auto logarithmic( double t ) -> double;

/**
 * Normalized logistic function
 *
 * Generates a logistic curve on the domain [-6,6], then normalizes such that
 * the value ranges from 1 to 0.  A single point is then calculated on this curve.
 *
 * @param min t-value that should yield an output of 1 on the scaled curve.
 * @param max t-value that should yield an output of 0 on the scaled curve.
 * @param pos t-value to calculate the output for.
 *
 * @return The value of the scaled logistic curve at point pos.
 */
auto logarithmic_range( int min, int max, int pos ) -> double;

/**
 * Cumulative distribution function of a certain normal distribution.
 *
 * @param x point at which the CDF of the distribution is measured
 * @param mean mean of the normal distribution
 * @param stddev standard deviation of the normal distribution
 *
 * @return The probability that a random point from the distribution will be lesser than @param x
 */
auto normal_cdf( double x, double mean, double stddev ) -> double;

/**
 * Clamp the value of a modifier in order to bound the resulting value
 *
 * Ensures that a modifier value will not cause a base value to exceed given
 * bounds when applied.  If necessary, the given modifier value is increased or
 * reduced to meet this constraint.
 *
 * Giving a value of zero for min or max indicates that there is no minimum or
 * maximum boundary, respectively.
 *
 * @param val The base value that the modifier will be applied to
 * @param mod The desired modifier to be added to the base value
 * @param max The desired maximum value of the base value after modification, or zero.
 * @param min The desired minimum value of the base value after modification, or zero.
 *
 * @returns Value of mod, possibly altered to respect the min and max boundaries
 */
auto bound_mod_to_vals( int val, int mod, int max, int min ) -> int;

/**
 * Clamp (number and space wise) value to with,
 * taking into account the specified preferred scale,
 * returning the adjusted (shortened) scale that best fit the width,
 * optionally returning a flag that indicate if the value was truncated to fit the width
 */
/**@{*/
auto clamp_to_width( double value, int width, int &scale ) -> double;
auto clamp_to_width( double value, int width, int &scale, bool *out_truncated ) -> double;
/**@}*/

/**
 * Clamp first argument so that it is no lower than second and no higher than third.
 * Does not check if min is lower than max.
 */
template<typename T>
constexpr auto clamp( const T &val, const T &min, const T &max ) -> T
{
    return std::max( min, std::min( max, val ) );
}

/**
 * Linear interpolation: returns first argument if t is 0, second if t is 1, otherwise proportional to t.
 * Does not clamp t, meaning it can return values lower than min (if t<0) or higher than max (if t>1).
 */
template<typename T>
constexpr auto lerp( const T &min, const T &max, float t ) -> T
{
    return ( 1.0f - t ) * min + t * max;
}

/** Linear interpolation with t clamped to [0, 1] */
template<typename T>
constexpr auto lerp_clamped( const T &min, const T &max, float t ) -> T
{
    return lerp( min, max, clamp( t, 0.0f, 1.0f ) );
}

/**
 * From `points`, finds p1 and p2 such that p1.first < x < p2.first
 * Then linearly interpolates between p1.second and p2.second and returns the result.
 * `points` should be sorted by first elements of the pairs.
 * If x is outside range, returns second value of the first (if x < points[0].first) or last point.
 */
auto multi_lerp( const std::vector<std::pair<float, float>> &points, float x ) -> float;

/**
 * @brief Class used to access a list as if it were circular.
 *
 * Some times you just want to have a list loop around on itself.
 * This wrapper class allows you to do that. It requires the list to exist
 * separately, but that also means any changes to the list get propagated (both ways).
 */
template<typename T>
class list_circularizer
{
    private:
        unsigned int _index = 0;
        std::vector<T> *_list;
    public:
        /** Construct list_circularizer from an existing std::vector. */
        list_circularizer( std::vector<T> &_list ) : _list( &_list ) {
        }

        /** Advance list to next item, wrapping back to 0 at end of list */
        void next() {
            _index = ( _index == _list->size() - 1 ? 0 : _index + 1 );
        }

        /** Advance list to previous item, wrapping back to end at zero */
        void prev() {
            _index = ( _index == 0 ? _list->size() - 1 : _index - 1 );
        }

        /** Return list element at the current location */
        auto cur() const -> T & {
            // list could be null, but it would be a design time mistake and really, the callers fault.
            return ( *_list )[_index];
        }
};

/** Apply fuzzy effect to a string like:
 * Hello, world! --> H##lo, wurl#!
 *
 * @param str the original string to be processed
 * @param f the function that guides how to mess the message
 * f() will be called for each character (lingual, not byte):
 * [-] f() == -1 : nothing will be done
 * [-] f() == 0  : the said character will be replace by a random character
 * [-] f() == ch : the said character will be replace by ch
 *
 * @return The processed string
 *
 */

auto obscure_message( const std::string &str, std::function<char()> f ) -> std::string;

/**
 * Erases elements from a set that match given predicate function.
 * Will work on vector, albeit not optimally performance-wise.
 * @return true if set was changed
 */
//bool erase_if( const std::function<bool( const value_type & )> &predicate ) {
template<typename Col, class Pred>
auto erase_if( Col &set, Pred predicate ) -> bool
{
    bool ret = false;
    auto iter = set.begin();
    for( ; iter != set.end(); ) {
        if( predicate( *iter ) ) {
            iter = set.erase( iter );
            ret = true;
        } else {
            ++iter;
        }
    }
    return ret;
}

auto modulo( int v, int m ) -> int;

class on_out_of_scope
{
    private:
        std::function<void()> func;
    public:
        on_out_of_scope( const std::function<void()> &func ) : func( func ) {
        }

        ~on_out_of_scope() {
            if( func ) {
                func();
            }
        }

        void cancel() {
            func = nullptr;
        }
};

template<typename T>
class restore_on_out_of_scope
{
    private:
        T &t;
        T orig_t;
        on_out_of_scope impl;
    public:
        // *INDENT-OFF*
        restore_on_out_of_scope( T &t_in ) : t( t_in ), orig_t( t_in ),
            impl( [this]() { t = std::move( orig_t ); } ) {
        }

        restore_on_out_of_scope( T &&t_in ) : t( t_in ), orig_t( std::move( t_in ) ),
            impl( [this]() { t = std::move( orig_t ); } ) {
        }
        // *INDENT-ON*
};

#endif // CATA_SRC_CATA_UTILITY_H
