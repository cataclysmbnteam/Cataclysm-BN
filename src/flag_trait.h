#pragma once
#ifndef CATA_SRC_FLAG_H
#define CATA_SRC_FLAG_H

#include <set>
#include <string>

#include "translations.h"
#include "type_id.h"
class JsonObject;

extern const trait_flag_str_id flag_NULL;

class json_trait_flag
{
        friend class DynamicDataLoader;
        friend class generic_factory<json_trait_flag>;

    public:
        // used by generic_factory
        trait_flag_str_id id = flag_NULL;
        bool was_loaded = false;

        json_trait_flag() = default;

        /** Fetches flag definition (or null flag if not found) */
        static const json_trait_flag &get( const std::string &id );

        /** Is this a valid (non-null) flag */
        operator bool() const;

        void check() const;

        /** true, if flags were loaded */
        static bool is_ready();

        static const std::vector<json_trait_flag> &get_all();

    private:
        translation info_;
        translation restriction_;
        std::set<std::string> conflicts_;

        /** Load flag definition from JSON */
        void load( const JsonObject &jo, const std::string &src );

        /** Load all flags from JSON */
        static void load_all( const JsonObject &jo, const std::string &src );

        /** finalize */
        static void finalize_all( );

        /** Check consistency of all loaded flags */
        static void check_consistency();

        /** Clear all loaded flags (invalidating any pointers) */
        static void reset();
};

#endif // CATA_SRC_FLAG_H
