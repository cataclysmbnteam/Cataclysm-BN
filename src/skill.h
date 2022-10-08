#pragma once
#ifndef CATA_SRC_SKILL_H
#define CATA_SRC_SKILL_H

#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "calendar.h"
#include "translations.h"
#include "type_id.h"

class JsonIn;
class JsonObject;
class JsonOut;
class item;
class recipe;
template <typename T> class string_id;

struct time_info_t {
    // Absolute floor on the time taken to attack.
    int min_time = 50;
    // The base or max time taken to attack.
    int base_time = 220;
    // The reduction in time given per skill level.
    int time_reduction_per_level = 25;
};

class Skill
{
        friend class string_id<Skill>;
        skill_id _ident;

        translation _name;
        translation _description;
        std::set<std::string> _tags;
        time_info_t _time_to_attack;
        skill_displayType_id _display_type;
        std::unordered_map<std::string, int> _companion_skill_practice;
        // these are not real skills, they depend on context
        static std::map<skill_id, Skill> contextual_skills;
        int _companion_combat_rank_factor = 0;
        int _companion_survival_rank_factor = 0;
        int _companion_industry_rank_factor = 0;
    public:
        static std::vector<Skill> skills;
        static void load_skill( const JsonObject &jsobj );
        // For loading old saves that still have integer-based ids.
        static auto from_legacy_int( int legacy_id ) -> skill_id;
        static auto random_skill() -> skill_id;

        // clear skill vector, every skill pointer becomes invalid!
        static void reset();

        static auto get_skills_sorted_by(
            std::function<bool ( const Skill &, const Skill & )> pred ) -> std::vector<const Skill *>;

        Skill();
        Skill( const skill_id &ident, const translation &name, const translation &description,
               const std::set<std::string> &tags, skill_displayType_id display_type );

        auto ident() const -> const skill_id & {
            return _ident;
        }
        auto name() const -> std::string {
            return _name.translated();
        }
        auto description() const -> std::string {
            return _description.translated();
        }
        auto get_companion_skill_practice( const std::string &companion_skill ) const -> int {
            return _companion_skill_practice.find( companion_skill ) == _companion_skill_practice.end() ? 0 :
                   _companion_skill_practice.at( companion_skill );
        }
        auto display_category() const -> skill_displayType_id {
            return _display_type;
        }
        auto time_to_attack() const -> time_info_t {
            return _time_to_attack;
        }
        auto companion_combat_rank_factor() const -> int {
            return _companion_combat_rank_factor;
        }
        auto companion_survival_rank_factor() const -> int {
            return _companion_survival_rank_factor;
        }
        auto companion_industry_rank_factor() const -> int {
            return _companion_industry_rank_factor;
        }

        auto operator==( const Skill &b ) const -> bool {
            return this->_ident == b._ident;
        }

        auto operator!=( const Skill &b ) const -> bool {
            return !( *this == b );
        }

        auto is_combat_skill() const -> bool;
        auto is_contextual_skill() const -> bool;
};

class SkillLevel
{
        int _level = 0;
        int _exercise = 0;
        time_point _lastPracticed = calendar::turn;
        bool _isTraining = true;
        int _highestLevel = 0;

    public:
        SkillLevel() = default;

        auto isTraining() const -> bool {
            return _isTraining;
        }
        auto toggleTraining() -> bool {
            _isTraining = !_isTraining;
            return _isTraining;
        }

        auto level() const -> int {
            return _level;
        }
        auto level( int plevel ) -> int {
            _level = plevel;
            if( _level > _highestLevel ) {
                _highestLevel = _level;
            }
            return plevel;
        }

        auto highestLevel() const -> int {
            return _highestLevel;
        }

        auto exercise( bool raw = false ) const -> int {
            return raw ? _exercise : _exercise / ( ( _level + 1 ) * ( _level + 1 ) );
        }

        auto exercised_level() const -> int {
            return level() * level() * 100 + exercise();
        }

        void train( int amount, bool skip_scaling = false );
        auto isRusting() const -> bool;
        auto rust( bool charged_bio_mem, int character_rate ) -> bool;
        void practice();
        auto can_train() const -> bool;

        void readBook( int minimumGain, int maximumGain, int maximumLevel = -1 );

        auto operator==( const SkillLevel &b ) const -> bool {
            return this->_level == b._level && this->_exercise == b._exercise;
        }
        auto operator< ( const SkillLevel &b ) const -> bool {
            return this->_level < b._level || ( this->_level == b._level && this->_exercise < b._exercise );
        }
        auto operator> ( const SkillLevel &b ) const -> bool {
            return this->_level > b._level || ( this->_level == b._level && this->_exercise > b._exercise );
        }

        auto operator==( const int &b ) const -> bool {
            return this->_level == b;
        }
        auto operator< ( const int &b ) const -> bool {
            return this->_level < b;
        }
        auto operator> ( const int &b ) const -> bool {
            return this->_level > b;
        }

        auto operator!=( const SkillLevel &b ) const -> bool {
            return !( *this == b );
        }
        auto operator<=( const SkillLevel &b ) const -> bool {
            return !( *this > b );
        }
        auto operator>=( const SkillLevel &b ) const -> bool {
            return !( *this < b );
        }

        auto operator!=( const int &b ) const -> bool {
            return !( *this == b );
        }
        auto operator<=( const int &b ) const -> bool {
            return !( *this > b );
        }
        auto operator>=( const int &b ) const -> bool {
            return !( *this < b );
        }

        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );
};

class SkillLevelMap : public std::map<skill_id, SkillLevel>
{
    public:
        auto get_skill_level_object( const skill_id &ident ) const -> const SkillLevel &;
        auto get_skill_level_object( const skill_id &ident ) -> SkillLevel &;
        void mod_skill_level( const skill_id &ident, int delta );
        auto get_skill_level( const skill_id &ident ) const -> int;
        auto get_skill_level( const skill_id &ident, const item &context ) const -> int;

        auto meets_skill_requirements( const std::map<skill_id, int> &req ) const -> bool;
        auto meets_skill_requirements( const std::map<skill_id, int> &req,
                                       const item &context ) const -> bool;
        /** Calculates skill difference
         * @param req Required skills to be compared with.
         * @param context An item to provide context for contextual skills. Can be null.
         * @return Difference in skills. Positive numbers - exceeds; negative - lacks; empty map - no difference.
         */
        auto compare_skill_requirements(
            const std::map<skill_id, int> &req, const item &context ) const -> std::map<skill_id, int>;
        auto compare_skill_requirements(
            const std::map<skill_id, int> &req ) const -> std::map<skill_id, int>;
        auto exceeds_recipe_requirements( const recipe &rec ) const -> int;
        auto has_recipe_requirements( const recipe &rec ) const -> bool;
};

class SkillDisplayType
{
        friend class string_id<SkillDisplayType>;
        skill_displayType_id _ident;
        translation _display_string;
    public:
        static std::vector<SkillDisplayType> skillTypes;
        static void load( const JsonObject &jsobj );
        static void reset();

        static auto get_skill_type( const skill_displayType_id & ) -> const SkillDisplayType &;

        SkillDisplayType();
        SkillDisplayType( const skill_displayType_id &ident, const translation &display_string );

        auto ident() const -> const skill_displayType_id & {
            return _ident;
        }
        auto display_string() const -> std::string {
            return _display_string.translated();
        }
};

auto price_adjustment( int ) -> double;

#endif // CATA_SRC_SKILL_H
