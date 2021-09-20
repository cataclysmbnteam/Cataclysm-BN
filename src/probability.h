#pragma once
#ifndef CATA_SRC_PROBABILITY_H
#define CATA_SRC_PROBABILITY_H

#include <stddef.h>
#include <array>

/**
 * Probit is an inverse of cumulative distribution function for N(0, 1)
 * Intuitively, probit of x is the number such that a number
 * picked from N(0, 1) is x*100% likely to be less than x.
 * Here we use truncated normal distribution:
 *   N(0.5, 0.25) for 0 <= x <= 1
 *   0 for x < 0
 *   1 for x > 1
 */
namespace truncated_probit
{

constexpr size_t n = 4096;
extern const std::array<double, n> lookup_table;

/**
 * Uses lookup table above.
 */
double at( double x );

/**
 * Rescaled variant of @ref at.
 */
double rescaled( double x, double mean, double stddev, double min, double max );


} // namespace probit

#endif
