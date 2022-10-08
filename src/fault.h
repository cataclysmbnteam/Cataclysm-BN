#pragma once
#ifndef CATA_SRC_FAULT_H
#define CATA_SRC_FAULT_H

#include <map>
#include <set>
#include <string>

#include "calendar.h"
#include "optional.h"
#include "translations.h"
#include "type_id.h"

class JsonObject;

struct mending_method {
    std::string id;
    translation name;
    translation description;
    translation success_msg;
    time_duration time;
    std::map<skill_id, int> skills;
    requirement_id requirements;
    cata::optional<fault_id> turns_into;
    cata::optional<fault_id> also_mends;
};

class fault
{
    public:
        fault() : id_( fault_id( "null" ) ) {}

        auto id() const -> const fault_id & {
            return id_;
        }

        auto is_null() const -> bool {
            return id_ == fault_id( "null" );
        }

        auto name() const -> std::string {
            return name_.translated();
        }

        auto description() const -> std::string {
            return description_.translated();
        }

        auto mending_methods() const -> const std::map<std::string, mending_method> & {
            return mending_methods_;
        }

        auto find_mending_method( const std::string &id ) const -> const mending_method * {
            if( mending_methods_.find( id ) != mending_methods_.end() ) {
                return &mending_methods_.at( id );
            } else {
                return nullptr;
            }
        }

        auto has_flag( const std::string &flag ) const -> bool {
            return flags.count( flag );
        }

        /** Load fault from JSON definition */
        static void load_fault( const JsonObject &jo );

        /** Get all currently loaded faults */
        static auto all() -> const std::map<fault_id, fault> &;

        /** Clear all loaded faults (invalidating any pointers) */
        static void reset();

        /** Checks all loaded from JSON are valid */
        static void check_consistency();

    private:
        fault_id id_;
        translation name_;
        translation description_;
        std::map<std::string, mending_method> mending_methods_;
        std::set<std::string> flags;
};

#endif // CATA_SRC_FAULT_H
