#pragma once
#ifndef CATA_SRC_STOMACH_H
#define CATA_SRC_STOMACH_H

#include <map>

#include "calendar.h"
#include "type_id.h"

class JsonIn;
class JsonOut;
struct needs_rates;

// Separate struct for nutrients so that we can easily perform arithmetic on
// them
struct nutrients {
    /** amount of kcal this food has */
    int kcal = 0;

    /** vitamins potentially provided by this comestible (if any) */
    std::map<vitamin_id, int> vitamins;

    /** Replace the values here with the minimum (or maximum) of themselves and the corresponding
     * values taken from r. */
    void min_in_place( const nutrients &r );
    void max_in_place( const nutrients &r );

    auto get_vitamin( const vitamin_id & ) const -> int;

    auto operator==( const nutrients &r ) const -> bool;
    auto operator!=( const nutrients &r ) const -> bool {
        return !( *this == r );
    }

    auto operator+=( const nutrients &r ) -> nutrients &;
    auto operator-=( const nutrients &r ) -> nutrients &;
    auto operator*=( int r ) -> nutrients &;
    auto operator/=( int r ) -> nutrients &;

    friend auto operator*( nutrients l, int r ) -> nutrients {
        l *= r;
        return l;
    }

    friend auto operator/( nutrients l, int r ) -> nutrients {
        l /= r;
        return l;
    }
};

// Contains all information that can pass out of (or into) a stomach
struct food_summary {
    nutrients nutr;
};

// how much a stomach_contents can digest
// based on 30 minute increments
struct stomach_digest_rates {
    float percent_kcal;
    int min_kcal;
    float percent_vitamin;
    int min_vitamin;
};

// an abstract of food that has been eaten.
class stomach_contents
{
    public:
        stomach_contents();

        /**
         * @brief Directly adds food to stomach contents.
         * Will still add contents if past maximum volume. Also updates last_ate to current turn.
         * @param ingested The food to be ingested
         */
        void ingest( const food_summary &ingested );

        /**
         * @brief Processes food and outputs nutrients that are finished processing
         * Metabolic rates are required because they determine the rate of absorption of
         * nutrients into the body.
         * @param metabolic_rates The metabolic rates of the owner of this stomach
         * @param five_mins Five-minute intervals passed since this method was last called
         */
        auto digest( const needs_rates &metabolic_rates, int five_mins ) -> food_summary;

        // Empties the stomach of all contents.
        void empty();

        auto get_calories() const -> int;

        // changes calorie amount
        void mod_calories( int calories );

        // how long has it been since i ate?
        // only really relevant for player::stomach
        auto time_since_ate() const -> time_duration;
        // update last_ate to calendar::turn
        void ate();

        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &json );

    private:

        // nutrients (calories and vitamins)
        nutrients nutr;
        // when did this stomach_contents call stomach_contents::ingest()
        time_point last_ate;

        // Gets the rates at which this stomach will digest things.
        auto get_digest_rates( const needs_rates &metabolic_rates ) -> stomach_digest_rates;

};

#endif // CATA_SRC_STOMACH_H
