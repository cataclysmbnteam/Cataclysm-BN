#pragma once
#ifndef CATA_SRC_RECIPE_DICTIONARY_H
#define CATA_SRC_RECIPE_DICTIONARY_H

#include <cstddef>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "recipe.h"
#include "type_id.h"

class JsonIn;
class JsonOut;
class JsonObject;

class recipe_dictionary
{
        friend class Item_factory; // allow removal of blacklisted recipes
        friend recipe_id;

    public:
        /** Returns all recipes that can be automatically learned */
        auto all_autolearn() const -> const std::set<const recipe *> & {
            return autolearn;
        }

        /** Returns all blueprints */
        auto all_blueprints() const -> const std::set<const recipe *> & {
            return blueprints;
        }

        auto size() const -> size_t;
        auto begin() const -> std::map<recipe_id, recipe>::const_iterator;
        auto end() const -> std::map<recipe_id, recipe>::const_iterator;

        auto is_item_on_loop( const itype_id & ) const -> bool;

        /** Returns disassembly recipe (or null recipe if no match) */
        static auto get_uncraft( const itype_id &id ) -> const recipe &;

        static void load_recipe( const JsonObject &jo, const std::string &src );
        static void load_uncraft( const JsonObject &jo, const std::string &src );

        static void finalize();
        static void reset();

    protected:
        /**
         * Remove all recipes matching the predicate
         * @warning must not be called after finalize()
         */
        static void delete_if( const std::function<bool( const recipe & )> &pred );

        static auto load( const JsonObject &jo, const std::string &src,
                             std::map<recipe_id, recipe> &out ) -> recipe &;

    private:
        std::map<recipe_id, recipe> recipes;
        std::map<recipe_id, recipe> uncraft;
        std::set<const recipe *> autolearn;
        std::set<const recipe *> blueprints;
        std::unordered_set<itype_id> items_on_loops;

        static void finalize_internal( std::map<recipe_id, recipe> &obj );
        void find_items_on_loops();
};

extern recipe_dictionary recipe_dict;

using recipe_filter = std::function<bool( const recipe &r )>;

auto recipe_filter_by_component( const itype_id &c ) -> recipe_filter;

class recipe_subset
{
    public:
        recipe_subset() = default;
        recipe_subset( const recipe_subset &src, const std::vector<const recipe *> &recipes );
        /**
         * Include a recipe to the subset.
         * @param r recipe to include
         * @param custom_difficulty If specified, it defines custom difficulty for the recipe
         */
        void include( const recipe *r, int custom_difficulty = -1 );
        void include( const recipe_subset &subset );
        /**
         * Include a recipe to the subset. Based on the condition.
         * @param subset Where to included the recipe
         * @param pred Unary predicate that accepts a @ref recipe.
         */
        template<class Predicate>
        void include_if( const recipe_subset &subset, Predicate pred ) {
            for( const auto &elem : subset ) {
                if( pred( *elem ) ) {
                    include( elem );
                }
            }
        }

        /** Check if the subset contains a recipe with the specified id. */
        auto contains( const recipe &r ) const -> bool {
            return ids.count( r.ident() ) != 0;
        }

        /**
         * Get custom difficulty for the recipe.
         * @return Either custom difficulty if it was specified, or recipe default difficulty.
         */
        auto get_custom_difficulty( const recipe *r ) const -> int;

        /** Check if there is any recipes in given category (optionally restricted to subcategory) */
        auto empty_category(
            const std::string &cat,
            const std::string &subcat = std::string() ) const -> bool;

        /** Get all recipes in given category (optionally restricted to subcategory) */
        auto in_category(
            const std::string &cat,
            const std::string &subcat = std::string() ) const -> std::vector<const recipe *>;

        /** Returns all recipes which could use component */
        auto of_component( const itype_id &id ) const -> const std::set<const recipe *> &;

        enum class search_type {
            name,
            skill,
            primary_skill,
            component,
            tool,
            quality,
            quality_result,
            description_result
        };

        /** Find marked favorite recipes */
        auto favorite() const -> std::vector<const recipe *>;

        /** Find recently used recipes */
        auto recent() const -> std::vector<const recipe *>;

        /** Find hidden recipes */
        auto hidden() const -> std::vector<const recipe *>;

        /** Find recipes matching query (left anchored partial matches are supported) */
        auto search( const std::string &txt,
                                            search_type key = search_type::name ) const -> std::vector<const recipe *>;
        /** Find recipes matching query and return a new recipe_subset */
        auto reduce( const std::string &txt, search_type key = search_type::name ) const -> recipe_subset;
        /** Set intersection between recipe_subsets */
        auto intersection( const recipe_subset &subset ) const -> recipe_subset;
        /** Set difference between recipe_subsets */
        auto difference( const recipe_subset &subset ) const -> recipe_subset;
        /** Find recipes producing the item */
        auto search_result( const itype_id &item ) const -> std::vector<const recipe *>;

        auto size() const -> size_t {
            return recipes.size();
        }

        void clear() {
            component.clear();
            category.clear();
            recipes.clear();
            ids.clear();
        }

        auto begin() const -> std::set<const recipe *>::const_iterator {
            return recipes.begin();
        }

        auto end() const -> std::set<const recipe *>::const_iterator {
            return recipes.end();
        }

    private:
        std::set<const recipe *> recipes;
        std::map<const recipe *, int> difficulties;
        std::map<std::string, std::set<const recipe *>> category;
        std::map<itype_id, std::set<const recipe *>> component;
        std::unordered_set<recipe_id> ids;
};

void serialize( const recipe_subset &value, JsonOut &jsout );
void deserialize( recipe_subset &value, JsonIn &jsin );

#endif // CATA_SRC_RECIPE_DICTIONARY_H
