#pragma once
#ifndef CATA_SRC_VITAMIN_H
#define CATA_SRC_VITAMIN_H

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "calendar.h"
#include "translations.h"
#include "type_id.h"

class JsonObject;
template <typename T> struct enum_traits;

enum vitamin_type {
    VITAMIN,
    TOXIN,
    DRUG,
    COUNTER,
    num_vitamin_types
};

template<>
struct enum_traits<vitamin_type> {
    static constexpr auto last = vitamin_type::num_vitamin_types;
};

class vitamin
{
    public:
        vitamin() : id_( vitamin_id( "null" ) ), rate_( 1_hours ) {}

        auto id() const -> const vitamin_id & {
            return id_;
        }

        auto type() const -> const vitamin_type & {
            return type_;
        }

        auto is_null() const -> bool {
            return id_ == vitamin_id( "null" );
        }

        auto name() const -> std::string {
            return name_.translated();
        }

        auto has_flag( const std::string &flag ) const -> bool {
            return flags_.count( flag ) > 0;
        }

        /** Disease effect with increasing intensity proportional to vitamin deficiency */
        auto deficiency() const -> const efftype_id & {
            return deficiency_;
        }

        /** Disease effect with increasing intensity proportional to vitamin excess */
        auto excess() const -> const efftype_id & {
            return excess_;
        }

        /** Lower bound for deficiency of this vitamin */
        auto min() const -> int {
            return min_;
        }

        /** Upper bound for any accumulation of this vitamin */
        auto max() const -> int {
            return max_;
        }

        /**
         * Usage rate of vitamin (time to consume unit)
         * Lower bound is zero whereby vitamin is not required (but may still accumulate)
         */
        auto rate() const -> time_duration {
            return rate_;
        }

        /** Get intensity of deficiency or zero if not deficient for specified qty */
        auto severity( int qty ) const -> int;

        /** Load vitamin from JSON definition */
        static void load_vitamin( const JsonObject &jo );

        /** Get all currently loaded vitamins */
        static auto all() -> const std::map<vitamin_id, vitamin> &;

        /** Check consistency of all loaded vitamins */
        static void check_consistency();

        /** Clear all loaded vitamins (invalidating any pointers) */
        static void reset();

    private:
        vitamin_id id_;
        vitamin_type type_ = vitamin_type::num_vitamin_types;
        translation name_;
        efftype_id deficiency_;
        efftype_id excess_;
        int min_ = 0;
        int max_ = 0;
        time_duration rate_ = 0_turns;
        std::vector<std::pair<int, int>> disease_;
        std::vector<std::pair<int, int>> disease_excess_;
        std::set<std::string> flags_;
};

#endif // CATA_SRC_VITAMIN_H
