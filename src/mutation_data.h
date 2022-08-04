#pragma once
#ifndef CATA_SRC_MUTATION_DATA_H
#define CATA_SRC_MUTATION_DATA_H

#include <string>
#include <vector>

#include "translations.h"

class JsonObject;

struct dream {
    std::vector<translation> messages; // The messages that the dream will give
    std::string category; // The category that will trigger the dream
    int strength = 0; // The category strength required for the dream
};

namespace dreams
{
void load( const JsonObject &jo );
void clear();

/** Returns a random dream description that matches given category and strength. */
std::string get_random_for_category( const std::string &cat, int strength );

} // namespace dreams

#endif // CATA_SRC_MUTATION_DATA_H
