#pragma once
#ifndef CATA_SRC_MONSTER_ORACLE_H
#define CATA_SRC_MONSTER_ORACLE_H

#include "behavior.h"
#include "behavior_oracle.h"

class monster;

namespace behavior
{

class monster_oracle_t : public oracle_t
{
    public:
        monster_oracle_t( const monster *subject ) {
            this->subject = subject;
        }
        /**
         * Predicates used by AI to determine goals.
         */
        auto has_special() const -> status_t;
        auto not_hallucination() const -> status_t;
        auto items_available() const -> status_t;
    private:
        const monster *subject;
};

} // namespace behavior
#endif // CATA_SRC_MONSTER_ORACLE_H
