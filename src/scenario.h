#pragma once
#ifndef CATA_SRC_SCENARIO_H
#define CATA_SRC_SCENARIO_H

#include <set>
#include <string>
#include <vector>

#include "string_id.h"
#include "translations.h"
#include "type_id.h"

class JsonObject;
class profession;
template<typename T>
class generic_factory;

class scenario
{
    private:
        friend class string_id<scenario>;
        friend class generic_factory<scenario>;
        string_id<scenario> id;
        bool was_loaded = false;
        translation _name_male;
        translation _name_female;
        translation _description_male;
        translation _description_female;
        translation _start_name;

        bool blacklist = false; // If true, professions is a blacklist.
        bool extra_professions = false; // If true, professions add to default professions.
        std::vector<profession_id> professions; // as specified in JSON, verbatim

        /**
         * @ref permitted_professions populates this vector on the first call, which takes
         * a bit of work. On subsequent calls, this vector is returned.
        */
        mutable std::vector<profession_id> cached_permitted_professions;

        std::set<trait_id> _allowed_traits;
        std::set<trait_id> _forced_traits;
        std::set<trait_id> _forbidden_traits;
        std::vector<start_location_id> _allowed_locs;
        int _point_cost = 0;
        std::set<std::string> flags; // flags for some special properties of the scenario
        std::string _map_extra;
        std::vector<mission_type_id> _missions;

        vproto_id _starting_vehicle = vproto_id::NULL_ID();

        void load( const JsonObject &jo, const std::string &src );
        auto scenario_traits_conflict_with_profession_traits( const profession &p ) const -> bool;

    public:
        //these three aren't meant for external use, but had to be made public regardless
        scenario();
        static void load_scenario( const JsonObject &jo, const std::string &src );

        // these should be the only ways used to get at scenario
        static auto generic() -> const scenario *; // points to the generic, default profession
        // return a random scenario, weighted for use w/ random character creation
        static auto weighted_random() -> const scenario *;
        static auto get_all() -> const std::vector<scenario> &;

        // clear scenario map, every scenario pointer becomes invalid!
        static void reset();
        /** calls @ref check_definition for each scenario */
        static void check_definitions();
        /** Check that item definitions are valid */
        void check_definition() const;

        auto ident() const -> const string_id<scenario> &;
        auto gender_appropriate_name( bool male ) const -> std::string;
        auto description( bool male ) const -> std::string;
        auto start_location() const -> start_location_id;
        auto random_start_location() const -> start_location_id;
        auto start_name() const -> std::string;
        auto start_location_count() const -> int;
        auto start_location_targets_count() const -> int;

        auto vehicle() const -> vproto_id;

        auto weighted_random_profession() const -> const profession_id &;
        auto permitted_professions() const -> std::vector<profession_id>;

        auto traitquery( const trait_id &trait ) const -> bool;
        auto get_locked_traits() const -> std::set<trait_id>;
        auto is_locked_trait( const trait_id &trait ) const -> bool;
        auto is_forbidden_trait( const trait_id &trait ) const -> bool;

        auto allowed_start( const start_location_id &loc ) const -> bool;
        auto point_cost() const -> signed int;
        auto has_map_extra() const -> bool;
        auto get_map_extra() const -> const std::string &;

        /**
         * Returns "All", "Limited", or "Almost all" (translated)
         * This is used by newcharacter.cpp
        */
        auto prof_count_str() const -> std::string;

        // Is this scenario blacklisted?
        auto scen_is_blacklisted() const -> bool;

        /** Such as a seasonal start, fiery start, surrounded start, etc. */
        auto has_flag( const std::string &flag ) const -> bool;

        /**
         *
         */
        auto can_pick( const scenario &current_scenario, int points ) const -> bool;

        auto missions() const -> const std::vector<mission_type_id> &;

};

struct scen_blacklist {
    std::set<string_id<scenario>> scenarios;
    bool whitelist = false;

    static void load_scen_blacklist( const JsonObject &jo, const std::string &src );
    void load( const JsonObject &jo, const std::string & );
    void finalize();
};

void reset_scenarios_blacklist();

#endif // CATA_SRC_SCENARIO_H
