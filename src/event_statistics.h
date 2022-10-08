#pragma once
#ifndef CATA_SRC_EVENT_STATISTICS_H
#define CATA_SRC_EVENT_STATISTICS_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "clone_ptr.h"
#include "string_id.h"
#include "translations.h"

class cata_variant;
enum class cata_variant_type : int;
class event_multiset;
enum class event_type : int;
class JsonObject;
enum class monotonically : int;
class stats_tracker;
class stats_tracker_state;

using event_fields_type = std::unordered_map<std::string, cata_variant_type>;

// event_tansformations and event_statistics are both functions of events.
// They are intended to be calculated via a stats_tracker object.
// They can be defined in json, and are useful therein for the creation of
// scores and achievements.
// An event_transformation yields an event_multiset, while an event_statistic
// yields a single cata_variant value (usually an int).
// The values can be accessed in two ways:
// - By direct calculation, by calling stats_tracker::get_events or
//   stats_tracker::value_of.
// - On a 'live updating' basis, by calling stats_tracker::add_watcher.
//
// For details on how watching values is implemented, see the comment in
// event_statistics.cpp.

// A transformation from one multiset of events to another
class event_transformation
{
    public:
        auto value( stats_tracker & ) const -> event_multiset;
        auto watch( stats_tracker & ) const -> std::unique_ptr<stats_tracker_state>;

        void load( const JsonObject &, const std::string & );
        void check() const;
        static void load_transformation( const JsonObject &, const std::string & );
        static void check_consistency();
        static void reset();

        string_id<event_transformation> id;
        bool was_loaded = false;

        auto fields() const -> event_fields_type;
        auto monotonicity() const -> monotonically;

        class impl;
    private:
        cata::clone_ptr<impl> impl_;
};

// A value computed from events somehow
class event_statistic
{
    public:
        auto value( stats_tracker & ) const -> cata_variant;
        auto watch( stats_tracker & ) const -> std::unique_ptr<stats_tracker_state>;

        void load( const JsonObject &, const std::string & );
        void check() const;
        static void load_statistic( const JsonObject &, const std::string & );
        static void check_consistency();
        static void reset();

        string_id<event_statistic> id;
        bool was_loaded = false;

        auto description() const -> const translation & {
            return description_;
        }

        auto type() const -> cata_variant_type;
        auto monotonicity() const -> monotonically;

        class impl;
    private:
        translation description_;
        cata::clone_ptr<impl> impl_;
};

class score
{
    public:
        score() = default;
        // Returns translated description including value
        auto description( stats_tracker & ) const -> std::string;
        auto value( stats_tracker & ) const -> cata_variant;

        void load( const JsonObject &, const std::string & );
        void check() const;
        static void load_score( const JsonObject &, const std::string & );
        static void check_consistency();
        static auto get_all() -> const std::vector<score> &;
        static void reset();

        string_id<score> id;
        bool was_loaded = false;
    private:
        translation description_;
        string_id<event_statistic> stat_;
};

#endif // CATA_SRC_EVENT_STATISTICS_H
