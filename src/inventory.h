#pragma once
#ifndef CATA_SRC_INVENTORY_H
#define CATA_SRC_INVENTORY_H

#include <array>
#include <bitset>
#include <cstddef>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "item.h"
#include "units.h"
#include "visitable.h"

class Character;
class JsonIn;
class JsonOut;
class item_stack;
class map;
class npc;
class player;
struct tripoint;

using invstack = std::list<std::list<item> >;
using invslice = std::vector<std::list<item> *>;
using const_invslice = std::vector<const std::list<item> *>;
using indexed_invslice = std::vector< std::pair<std::list<item>*, int> >;
using itype_bin = std::unordered_map< itype_id, std::list<const item *> >;
using invlets_bitset = std::bitset<std::numeric_limits<char>::max()>;

/** First element is pointer to item stack (first item), second is amount. */
using excluded_stacks = std::map<const item *, int>;

/**
 * Wrapper to handled a set of valid "inventory" letters. "inventory" can be any set of
 * objects that the player can access via a single character (e.g. bionics).
 * The class is (currently) derived from std::string for compatibility and because it's
 * simpler. But it may be changed to derive from `std::set<int>` or similar to get the full
 * range of possible characters.
 */
class invlet_wrapper : private std::string
{
    public:
        invlet_wrapper( const char *chars ) : std::string( chars ) { }

        auto valid( int invlet ) const -> bool;
        auto get_allowed_chars() const -> std::string {
            return *this;
        }

        using std::string::begin;
        using std::string::end;
        using std::string::rbegin;
        using std::string::rend;
        using std::string::size;
        using std::string::length;
};

const extern invlet_wrapper inv_chars;

// For each item id, store a set of "favorite" inventory letters.
// This class maintains a bidirectional mapping between invlet letters and item ids.
// Each invlet has at most one id and each id has any number of invlets.
class invlet_favorites
{
    public:
        invlet_favorites() = default;
        invlet_favorites( const std::unordered_map<itype_id, std::string> & );

        void set( char invlet, const itype_id & );
        void erase( char invlet );
        auto contains( char invlet, const itype_id & ) const -> bool;
        auto invlets_for( const itype_id & ) const -> std::string;

        // For serialization only
        auto get_invlets_by_id() const -> const std::unordered_map<itype_id, std::string> &;
    private:
        std::unordered_map<itype_id, std::string> invlets_by_id;
        std::array<itype_id, 256> ids_by_invlet;
};

class inventory : public visitable<inventory>
{
    public:
        friend visitable<inventory>;

        auto slice() -> invslice;
        auto const_slice() const -> const_invslice;
        auto const_stack( int i ) const -> const std::list<item> &;
        auto size() const -> size_t;

        std::map<char, itype_id> assigned_invlet;

        inventory();
        inventory( inventory && ) = default;
        inventory( const inventory & ) = default;
        auto operator=( inventory && ) -> inventory & = default;
        auto operator=( const inventory & ) -> inventory & = default;

        auto operator+= ( const inventory &rhs ) -> inventory &;
        auto operator+= ( const item &rhs ) -> inventory &;
        auto operator+= ( const std::list<item> &rhs ) -> inventory &;
        auto operator+= ( const std::vector<item> &rhs ) -> inventory &;
        auto operator+= ( const item_stack &rhs ) -> inventory &;
        auto  operator+ ( const inventory &rhs ) -> inventory;
        auto  operator+ ( const item &rhs ) -> inventory;
        auto  operator+ ( const std::list<item> &rhs ) -> inventory;

        void unsort(); // flags the inventory as unsorted
        void clear();
        void push_back( const std::list<item> &newits );
        // returns a reference to the added item
        auto add_item( item newit, bool keep_invlet = false, bool assign_invlet = true,
                        bool should_stack = true ) -> item &;
        // use item type cache to speed up, remember to run build_items_type_cache() before using it
        auto add_item_by_items_type_cache( item newit, bool keep_invlet = false, bool assign_invlet = true,
                                            bool should_stack = true ) -> item &;
        void add_item_keep_invlet( item newit );
        void push_back( item newit );

        /* Check all items for proper stacking, rearranging as needed
         * game pointer is not necessary, but if supplied, will ensure no overlap with
         * the player's worn items / weapon
         */
        void restack( player &p );
        void form_from_zone( map &m, std::unordered_set<tripoint> &zone_pts, const Character *pl = nullptr,
                             bool assign_invlet = true );
        void form_from_map( const tripoint &origin, int range, const Character *pl = nullptr,
                            bool assign_invlet = true,
                            bool clear_path = true );
        void form_from_map( map &m, const tripoint &origin, int range, const Character *pl = nullptr,
                            bool assign_invlet = true,
                            bool clear_path = true );
        void form_from_map( map &m, std::vector<tripoint> pts, const Character *pl,
                            bool assign_invlet = true );
        /**
         * Remove a specific item from the inventory. The item is compared
         * by pointer. Contents of the item are removed as well.
         * @param it A pointer to the item to be removed. The item *must* exists
         * in this inventory.
         * @return A copy of the removed item.
         */
        auto remove_item( const item *it ) -> item;
        auto remove_item( int position ) -> item;
        /**
         * Randomly select items until the volume quota is filled.
         */
        auto remove_randomly_by_volume( const units::volume &volume ) -> std::list<item>;
        auto reduce_stack( int position, int quantity ) -> std::list<item>;

        auto find_item( int position ) const -> const item &;
        auto find_item( int position ) -> item &;

        /**
         * Returns the item position of the stack that contains the given item (compared by
         * pointers). Returns INT_MIN if the item is not found.
         * Note that this may lose some information, for example the returned position is the
         * same when the given item points to the container and when it points to the item inside
         * the container. All items that are part of the same stack have the same item position.
         */
        auto position_by_item( const item *it ) const -> int;
        auto position_by_type( const itype_id &type ) const -> int;

        /** Return the item position of the item with given invlet, return INT_MIN if
         * the inventory does not have such an item with that invlet. Don't use this on npcs inventory. */
        auto invlet_to_position( char invlet ) const -> int;

        // Below, "amount" refers to quantity
        //        "charges" refers to charges
        auto use_amount( itype_id it, int quantity,
                                    const std::function<bool( const item & )> &filter = return_true<item> ) -> std::list<item>;

        auto has_tools( const itype_id &it, int quantity,
                        const std::function<bool( const item & )> &filter = return_true<item> ) const -> bool;
        auto has_components( const itype_id &it, int quantity,
                             const std::function<bool( const item & )> &filter = return_true<item> ) const -> bool;
        auto has_charges( const itype_id &it, int quantity,
                          const std::function<bool( const item & )> &filter = return_true<item> ) const -> bool;

        auto leak_level( const std::string &flag ) const -> int; // level of leaked bad stuff from items

        // NPC/AI functions
        auto worst_item_value( npc *p ) const -> int;
        auto has_enough_painkiller( int pain ) const -> bool;
        auto most_appropriate_painkiller( int pain ) -> item *;

        void rust_iron_items();

        auto weight() const -> units::mass;
        auto weight_without( const excluded_stacks &without ) const -> units::mass;
        auto volume() const -> units::volume;
        auto volume_without( const excluded_stacks &without ) const -> units::volume;

        // dumps contents into dest (does not delete contents)
        void dump( std::vector<item *> &dest );

        // vector rather than list because it's NOT an item stack
        // returns all items that need processing
        auto active_items() -> std::vector<item *>;

        void json_load_invcache( JsonIn &jsin );
        void json_load_items( JsonIn &jsin );

        void json_save_invcache( JsonOut &json ) const;
        void json_save_items( JsonOut &json ) const;

        // Assigns an invlet if any remain.  If none do, will assign ` if force is
        // true, empty (invlet = 0) otherwise.
        void assign_empty_invlet( item &it, const Character &p, bool force = false );
        // Assigns the item with the given invlet, and updates the favorite invlet cache. Does not check for uniqueness
        void reassign_item( item &it, char invlet, bool remove_old = true );
        // Removes invalid invlets, and assigns new ones if assign_invlet is true. Does not update the invlet cache.
        void update_invlet( item &it, bool assign_invlet = true );

        void set_stack_favorite( int position, bool favorite );

        auto allocated_invlets() const -> invlets_bitset;

        /**
         * Returns visitable items binned by their itype.
         * May not contain items that wouldn't be visited by @ref visitable methods.
         */
        auto get_binned_items() const -> const itype_bin &;

        void update_cache_with_item( item &newit );

        void copy_invlet_of( const inventory &other );

        // gets a singular enchantment that is an amalgamation of all items that have active enchantments
        auto get_active_enchantment_cache( const Character &owner ) const -> enchantment;

        auto count_item( const itype_id &item_type ) const -> int;

        void update_quality_cache();
        auto get_quality_cache() const -> const std::map<quality_id, std::map<int, int>> &;

        void build_items_type_cache();

    private:
        invlet_favorites invlet_cache;
        auto find_usable_cached_invlet( const itype_id &item_type ) -> char;

        invstack items;
        std::map<itype_id, std::list<std::list<item>*>> items_type_cache;
        std::map<quality_id, std::map<int, int>> quality_cache;

        bool items_type_cached = false;
        mutable bool binned = false;
        /**
         * Items binned by their type.
         * That is, item_bin["carrot"] is a list of pointers to all carrots in inventory.
         * `mutable` because this is a pure cache that doesn't affect the contained items.
         */
        mutable itype_bin binned_items;
};

#endif // CATA_SRC_INVENTORY_H
