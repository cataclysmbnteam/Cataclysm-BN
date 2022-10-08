#pragma once
#ifndef CATA_SRC_PROFESSION_H
#define CATA_SRC_PROFESSION_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "pldata.h"
#include "string_id.h"
#include "translations.h"
#include "type_id.h"

template<typename T>
class generic_factory;

class item;
class JsonObject;
class avatar;
class player;

class profession
{
    public:
        using StartingSkill = std::pair<skill_id, int>;
        using StartingSkillList = std::vector<StartingSkill>;
        struct itypedec {
            itype_id type_id;
            /** Snippet id, @see snippet_library. */
            snippet_id snip_id;
            // compatible with when this was just a std::string
            itypedec( const std::string &t ) : type_id( t ), snip_id( snippet_id::NULL_ID() ) {
            }
            itypedec( const std::string &t, const snippet_id &d ) : type_id( t ), snip_id( d ) {
            }
        };
        using itypedecvec = std::vector<itypedec>;
        friend class string_id<profession>;
        friend class generic_factory<profession>;

    private:
        profession_id id;
        bool was_loaded = false;

        translation _name_male;
        translation _name_female;
        translation _description_male;
        translation _description_female;
        signed int _point_cost = 0;

        // TODO: In professions.json, replace lists of itypes (legacy) with item groups
        itypedecvec legacy_starting_items;
        itypedecvec legacy_starting_items_male;
        itypedecvec legacy_starting_items_female;
        item_group_id _starting_items = item_group_id( "EMPTY_GROUP" );
        item_group_id _starting_items_male = item_group_id( "EMPTY_GROUP" );
        item_group_id _starting_items_female = item_group_id( "EMPTY_GROUP" );
        itype_id no_bonus; // See profession::items and class json_item_substitution in profession.cpp

        std::vector<addiction> _starting_addictions;
        std::vector<bionic_id> _starting_CBMs;
        std::vector<trait_id> _starting_traits;
        std::set<trait_id> _forbidden_traits;
        std::vector<mtype_id> _starting_pets;
        vproto_id _starting_vehicle = vproto_id::NULL_ID();
        // the int is what level the spell starts at
        std::map<spell_id, int> _starting_spells;
        std::set<std::string> flags; // flags for some special properties of the profession
        StartingSkillList  _starting_skills;

        void check_item_definitions( const itypedecvec &items ) const;

        void load( const JsonObject &jo, const std::string &src );

    public:
        //these three aren't meant for external use, but had to be made public regardless
        profession();

        static void load_profession( const JsonObject &jo, const std::string &src );
        static void load_item_substitutions( const JsonObject &jo );

        // these should be the only ways used to get at professions
        static auto generic() -> const profession_id &; // gives id of generic, default profession
        static auto get_all() -> const std::vector<profession> &;

        static auto has_initialized() -> bool;
        // clear profession map, every profession pointer becomes invalid!
        static void reset();
        /** calls @ref check_definition for each profession */
        static void check_definitions();
        /** Check that item/CBM/addiction/skill definitions are valid. */
        void check_definition() const;

        auto ident() const -> const profession_id &;
        auto gender_appropriate_name( bool male ) const -> std::string;
        auto description( bool male ) const -> std::string;
        auto point_cost() const -> signed int;
        auto items( bool male, const std::vector<trait_id> &traits ) const -> std::list<item>;
        auto addictions() const -> std::vector<addiction>;
        auto vehicle() const -> vproto_id;
        auto pets() const -> std::vector<mtype_id>;
        auto CBMs() const -> std::vector<bionic_id>;
        auto skills() const -> StartingSkillList;

        auto spells() const -> std::map<spell_id, int>;

        /**
         * Check if this type of profession has a certain flag set.
         *
         * Current flags: none
         */
        auto has_flag( const std::string &flag ) const -> bool;

        auto is_locked_trait( const trait_id &trait ) const -> bool;
        auto is_forbidden_trait( const trait_id &trait ) const -> bool;
        auto get_locked_traits() const -> std::vector<trait_id>;
        auto get_forbidden_traits() const -> std::set<trait_id>;
};

#endif // CATA_SRC_PROFESSION_H
