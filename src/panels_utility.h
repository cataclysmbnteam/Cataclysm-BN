#pragma once
#ifndef CATA_SRC_PANELS_UTILITY_H
#define CATA_SRC_PANELS_UTILITY_H

#include <string>
#include "color.h"

std::string trunc_ellipse( const std::string &input, unsigned int trunc );

nc_color color_compare_base( int base, int value );

std::string value_trimmed( int value, int maximum = 100 );

nc_color focus_color( int focus );

#endif
