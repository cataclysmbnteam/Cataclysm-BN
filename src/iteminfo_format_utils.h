#pragma once
#ifndef CATA_SRC_ITEMINFO_FORMAT_UTILS_H
#define CATA_SRC_ITEMINFO_FORMAT_UTILS_H

#include "item.h"

/**
 * Inserts a separation line into the given iteminfo vector if the last entry is not a separation.
 */
void insert_separation_line( std::vector<iteminfo> &info );

#endif // CATA_SRC_ITEMINFO_FORMAT_UTILS_H

