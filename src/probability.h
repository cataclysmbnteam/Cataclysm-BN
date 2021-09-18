#pragma once
#ifndef CATA_SRC_PROBABILITY_H
#define CATA_SRC_PROBABILITY_H

/**
 * Approximates the inverse of @ref std::erf on range (0, 1).
 */
double erfinv_approx( double x );

/**
 * Probit is an inverse of cumulative distribution function for N(0, 1)
 * Intuitively, probit of x is the number such that a number
 * picked from N(0, 1) is x*100% likely to be less than x.
 */
namespace probit
{

/**
 * Approximates probit with error ~0.01.
 */
double approx( double x );
/**
 * As @ref probit::approx, but rescales the results to (0, 1) range.
 */
double rescaled_to_zero_to_one( double x );


} // namespace probit

#endif
