#pragma once
#ifndef CATA_SRC_CONCEPTS_UTILITY_H
#define CATA_SRC_CONCEPTS_UTILITY_H

#include <concepts>

template <typename T>
concept Arithmatic = std::is_arithmetic_v<T>;

#endif // CATA_SRC_CONCEPTS_UTILITY_H
