#pragma once
#ifndef CATA_SRC_PANELS_UTILITY_H
#define CATA_SRC_PANELS_UTILITY_H

#include <string>
#include "color.h"

std::string trunc_ellipse( const std::string &input, unsigned int trunc );

auto color_compare_base( int base, int value ) -> nc_color;

auto value_trimmed( int value, int maximum = 100 ) -> std::string;

auto focus_color( int focus ) -> nc_color;

#endif
