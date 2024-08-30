#pragma once
#ifndef CATA_SRC_DEMANGLE_H
#define CATA_SRC_DEMANGLE_H

#include <string>

/**
 * Demangles a C++ symbol
 */
auto demangle( const char *symbol ) -> std::string;

#endif // CATA_SRC_DEMANGLE_H
