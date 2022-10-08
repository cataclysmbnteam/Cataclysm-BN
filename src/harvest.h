#pragma once
#ifndef CATA_SRC_HARVEST_H
#define CATA_SRC_HARVEST_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "translations.h"
#include "type_id.h"

class JsonObject;

// Could be reused for butchery
struct harvest_entry {
    // drop can be either an itype_id or a group id
    std::string drop = "null";
    std::pair<float, float> base_num = { 1.0f, 1.0f };
    // This is multiplied by survival and added to the above
    // TODO: Make it a map: skill->scaling
    std::pair<float, float> scale_num = { 0.0f, 0.0f };

    int max = 1000;
    std::string type = "null";
    float mass_ratio = 0.00f;

    static auto load( const JsonObject &jo, const std::string &src ) -> harvest_entry;

    std::vector<std::string> flags;
    std::vector<fault_id> faults;
};

class harvest_list
{
    public:
        harvest_list();

        auto id() const -> const harvest_id &;

        auto message() const -> std::string;

        auto is_null() const -> bool;

        auto entries() const -> const std::list<harvest_entry> & {
            return entries_;
        }

        auto empty() const -> bool {
            return entries().empty();
        }

        auto has_entry_type( std::string type ) const -> bool;

        /**
         * Returns a set of cached, translated names of the items this harvest entry could produce.
         * Filled in at finalization and not valid before that stage.
         */
        auto names() const -> const std::set<std::string> & {
            return names_;
        }

        auto describe( int at_skill = -1 ) const -> std::string;

        auto begin() const -> std::list<harvest_entry>::const_iterator;
        auto end() const -> std::list<harvest_entry>::const_iterator;
        auto rbegin() const -> std::list<harvest_entry>::const_reverse_iterator;
        auto rend() const -> std::list<harvest_entry>::const_reverse_iterator;

        /** Load harvest data, create relevant global entries, then return the id of the new list */
        static auto load( const JsonObject &jo, const std::string &src,
                                       const std::string &force_id = "" ) -> const harvest_id &;

        /** Get all currently loaded harvest data */
        static auto all() -> const std::map<harvest_id, harvest_list> &;

        /** Fills out the set of cached names. */
        static void finalize_all();

        /** Check consistency of all loaded harvest data */
        static void check_consistency();

        /** Clear all loaded harvest data (invalidating any pointers) */
        static void reset();
    private:
        harvest_id id_;
        std::list<harvest_entry> entries_;
        std::set<std::string> names_;
        translation message_;

        void finalize();
};

#endif // CATA_SRC_HARVEST_H
