#pragma once
#ifndef CATA_SRC_ADDICTION_H
#define CATA_SRC_ADDICTION_H

#include <string>

#include "pldata.h"
#include "type_id.h"

class Character;

// Minimum intensity before effects are seen
constexpr int MIN_ADDICTION_LEVEL = 3;
constexpr int MAX_ADDICTION_LEVEL = 20;

// cancel_activity is called when the addiction effect wants to interrupt the player
// with an optional pre-translated message.
void addict_effect( Character &u, addiction &add );

auto addiction_type_name( add_type cur ) -> std::string;

auto addiction_name( const addiction &cur ) -> std::string;

auto addiction_craving( add_type cur ) -> morale_type;

auto addiction_type( const std::string &name ) -> add_type;

auto addiction_text( const addiction &cur ) -> std::string;

#endif // CATA_SRC_ADDICTION_H
