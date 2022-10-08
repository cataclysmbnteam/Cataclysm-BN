#pragma once
#ifndef CATA_SRC_WCWIDTH_H
#define CATA_SRC_WCWIDTH_H

#include <cstdint>

/* Get character width in columns.  See wcwidth.cpp for details.
 */
auto mk_wcwidth( uint32_t ucs ) -> int;

#endif // CATA_SRC_WCWIDTH_H
