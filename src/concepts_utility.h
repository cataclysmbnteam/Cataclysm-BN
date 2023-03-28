#pragma once
#ifndef CATA_SRC_CONCEPTS_UTILITY_DEF_H
#define CATA_SRC_CONCEPTS_UTILITY_DEF_H
#include <cmath>

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

#endif // CATA_SRC_CONCEPTS_UTILITY_DEF_H
