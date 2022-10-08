#pragma once
#ifndef CATA_SRC_CHARACTER_ORACLE_H
#define CATA_SRC_CHARACTER_ORACLE_H

#include "behavior.h"
#include "behavior_oracle.h"

class Character;

namespace behavior
{

class character_oracle_t : public oracle_t
{
    public:
        character_oracle_t( const Character *subject ) {
            this->subject = subject;
        }
        /**
         * Predicates used by AI to determine goals.
         */
        auto needs_warmth_badly() const -> status_t;
        auto needs_water_badly() const -> status_t;
        auto needs_food_badly() const -> status_t;
        auto can_wear_warmer_clothes() const -> status_t;
        auto can_make_fire() const -> status_t;
        auto can_take_shelter() const -> status_t;
        auto has_water() const -> status_t;
        auto has_food() const -> status_t;
    private:
        const Character *subject;
};

} //namespace behavior
#endif // CATA_SRC_CHARACTER_ORACLE_H
