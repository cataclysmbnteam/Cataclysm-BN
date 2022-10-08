#pragma once
#ifndef CATA_SRC_FILTER_UTILS_H
#define CATA_SRC_FILTER_UTILS_H

/** Used as a default filter in various functions */
template<typename T>
auto return_true( const T & ) -> bool
{
    return true;
}

#endif // CATA_SRC_FILTER_UTILS_H
