#pragma once
#ifndef CATA_SRC_RECIPE_H
#define CATA_SRC_RECIPE_H

#include <cstddef>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "optional.h"
#include "requirements.h"
#include "translations.h"
#include "type_id.h"

class JsonObject;
class item;
class time_duration;
class Character;

enum class recipe_filter_flags : int {
    none = 0,
    no_rotten = 1,
};

inline constexpr auto operator&( recipe_filter_flags l, recipe_filter_flags r ) -> recipe_filter_flags
{
    return static_cast<recipe_filter_flags>(
               static_cast<unsigned>( l ) & static_cast<unsigned>( r ) );
}

class recipe
{
        friend class recipe_dictionary;

    private:
        itype_id result_ = itype_id::NULL_ID();

    public:
        recipe();

        operator bool() const {
            return !result_.is_null();
        }

        auto result() const -> const itype_id & {
            return result_;
        }

        bool obsolete = false;

        std::string category;
        std::string subcategory;

        translation description;

        int time = 0; // in movement points (100 per turn)
        int difficulty = 0;

        /** Fetch combined requirement data (inline and via "using" syntax).
         *
         * Use simple_requirements() for player display or when you just want to
         * know the requirements as listed in the json files.  Use
         * deduped_requirements() to calculate actual craftability of a recipe. */
        auto simple_requirements() const -> const requirement_data & {
            return requirements_;
        }

        auto deduped_requirements() const -> const deduped_requirement_data & {
            return deduped_requirements_;
        }

        auto ident() const -> const recipe_id & {
            return ident_;
        }

        auto is_blacklisted() const -> bool {
            return requirements_.is_blacklisted();
        }

        // Slower equivalent of is_blacklisted that needs to be used before
        // recipe finalization happens
        auto will_be_blacklisted() const -> bool;

        auto get_component_filter(
            recipe_filter_flags = recipe_filter_flags::none ) const -> std::function<bool( const item & )>;

        /** Prevent this recipe from ever being added to the player's learned recipies ( used for special NPC crafting ) */
        bool never_learn = false;

        /** If recipe can be used for disassembly fetch the combined requirements */
        auto disassembly_requirements() const -> requirement_data {
            if( reversible ) {
                return simple_requirements().disassembly_requirements();
            } else {
                return {};
            }
        }

        /// @returns The name (@ref item::nname) of the resulting item (@ref result).
        auto result_name() const -> std::string;

        std::map<itype_id, int> byproducts;

        skill_id skill_used;
        std::map<skill_id, int> required_skills;

        std::map<skill_id, int> autolearn_requirements; // Skill levels required to autolearn
        std::map<skill_id, int> learn_by_disassembly; // Skill levels required to learn by disassembly
        std::map<itype_id, int> booksets; // Books containing this recipe, and the skill level required
        std::set<std::string> flags_to_delete; // Flags to delete from the resultant item.

        // Create a string list to describe the skill requirements for this recipe
        // Format: skill_name(level/amount), skill_name(level/amount)
        // Character object (if provided) used to color levels

        // These are primarily used by the crafting menu.
        // Format the primary skill string.
        auto primary_skill_string( const Character *c, bool print_skill_level ) const -> std::string;

        // Format the other skills string.  This is also used for searching within the crafting
        // menu which includes the primary skill.
        auto required_skills_string( const Character *, bool include_primary_skill,
                                            bool print_skill_level ) const -> std::string;

        // This is used by the basecamp bulletin board.
        auto required_all_skills_string() const -> std::string;


        // Create a string to describe the time savings of batch-crafting, if any.
        // Format: "N% at >M units" or "none"
        auto batch_savings_string() const -> std::string;

        // Create an item instance as if the recipe was just finished,
        // Contain charges multiplier
        auto create_result() const -> item;
        auto create_results( int batch = 1 ) const -> std::vector<item>;

        // Create byproduct instances as if the recipe was just finished
        auto create_byproducts( int batch = 1 ) const -> std::vector<item>;

        auto has_byproducts() const -> bool;

        auto batch_time( int batch, float multiplier, size_t assistants ) const -> int;
        auto batch_duration( int batch = 1, float multiplier = 1.0,
                                      size_t assistants = 0 ) const -> time_duration;

        auto has_flag( const std::string &flag_name ) const -> bool;

        auto is_reversible() const -> bool {
            return reversible;
        }

        void load( const JsonObject &jo, const std::string &src );
        void finalize();

        /** Returns a non-empty string describing an inconsistency (if any) in the recipe. */
        auto get_consistency_error() const -> std::string;

        auto is_blueprint() const -> bool;
        auto get_blueprint() const -> const std::string &;
        auto blueprint_name() const -> const translation &;
        auto blueprint_resources() const -> const std::vector<itype_id> &;
        auto blueprint_provides() const -> const std::vector<std::pair<std::string, int>> &;
        auto blueprint_requires() const -> const std::vector<std::pair<std::string, int>> &;
        auto blueprint_excludes() const -> const std::vector<std::pair<std::string, int>> &;
        /**
         * Calculate blueprint requirements according to changed terrain and furniture
         * tiles, then check the calculated requirements against blueprint requirements
         * specified in JSON.  If there's any inconsistency, it issues a debug message.
         * This is only used in unit tests so as to speed up data loading in gameplay.
         */
        void check_blueprint_requirements();

        auto hot_result() const -> bool;

        /** Returns the amount or charges recipe will produce. */
        auto makes_amount() const -> int;
        /** Returns number of charges of the item needed for single disassembly. */
        auto disassembly_batch_size() const -> int;

    private:
        void add_requirements( const std::vector<std::pair<requirement_id, int>> &reqs );

    private:
        recipe_id ident_ = recipe_id::NULL_ID();

        /** Abstract recipes can be inherited from but are themselves disposed of at finalization */
        bool abstract = false;

        /** set learning requirements equal to required skills at finalization? */
        bool autolearn = false;

        /** Does the item spawn contained in container? */
        bool contained = false;

        /** Can recipe be used for disassembly of @ref result via @ref disassembly_requirements */
        bool reversible = false;

        /** What does the item spawn contained in? Unset ("null") means default container. */
        itype_id container = itype_id::NULL_ID();

        /** External requirements (via "using" syntax) where second field is multiplier */
        std::vector<std::pair<requirement_id, int>> reqs_external;

        /** Requires specified inline with the recipe (and replaced upon inheritance) */
        std::vector<std::pair<requirement_id, int>> reqs_internal;

        /** Combined requirements cached when recipe finalized */
        requirement_data requirements_;

        /** Deduped version constructed from the above requirements_ */
        deduped_requirement_data deduped_requirements_;

        std::set<std::string> flags;

        /** If set (zero or positive) set charges of output result for items counted by charges */
        cata::optional<int> charges;

        // maximum achievable time reduction, as percentage of the original time.
        // if zero then the recipe has no batch crafting time reduction.
        double batch_rscale = 0.0;
        int batch_rsize = 0; // minimum batch size to needed to reach batch_rscale
        int result_mult = 1; // used by certain batch recipes that create more than one stack of the result
        std::string blueprint;
        translation bp_name;
        std::vector<itype_id> bp_resources;
        std::vector<std::pair<std::string, int>> bp_provides;
        std::vector<std::pair<std::string, int>> bp_requires;
        std::vector<std::pair<std::string, int>> bp_excludes;

        /** Blueprint requirements to be checked in unit test */
        bool has_blueprint_needs = false;
        bool check_blueprint_needs = false;
        int time_blueprint = 0;
        std::map<skill_id, int> skills_blueprint;
        std::vector<std::pair<requirement_id, int>> reqs_blueprint;
};

#endif // CATA_SRC_RECIPE_H
