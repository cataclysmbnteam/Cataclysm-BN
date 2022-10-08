#pragma once
#ifndef CATA_SRC_INVENTORY_UI_H
#define CATA_SRC_INVENTORY_UI_H

#include <cassert>
#include <climits>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <array>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "color.h"
#include "cursesdef.h"
#include "input.h"
#include "item_handling_util.h"
#include "item_location.h"
#include "memory_fast.h"
#include "pimpl.h"
#include "units.h"

class Character;
class item;
class item_category;
class player;
class string_input_popup;
struct tripoint;
class ui_adaptor;

using excluded_stack = std::pair<const item *, int>;
using excluded_stacks = std::map<const item *, int>;

enum class navigation_mode : int {
    ITEM = 0,
    CATEGORY
};

enum class scroll_direction : int {
    FORWARD = 1,
    BACKWARD = -1
};

struct navigation_mode_data;
struct inventory_input;

class inventory_entry
{
    public:
        std::vector<item_location> locations;

        size_t chosen_count = 0;
        int custom_invlet = INT_MIN;
        std::string cached_name;

        inventory_entry() = default;

        inventory_entry( const item_category *custom_category ) :
            custom_category( custom_category )
        {}

        // Copy with new category.  Used to copy entries into the "selected"
        // category when they are selected.
        inventory_entry( const inventory_entry &entry, const item_category *custom_category ) :
            inventory_entry( entry ) {
            this->custom_category = custom_category;
        }

        inventory_entry( const std::vector<item_location> &locations,
                         const item_category *custom_category = nullptr,
                         bool enabled = true ) :
            locations( locations ),
            custom_category( custom_category ),
            enabled( enabled )
        {}

        auto operator==( const inventory_entry &other ) const -> bool;
        auto operator!=( const inventory_entry &other ) const -> bool {
            return !( *this == other );
        }

        operator bool() const {
            return !is_null();
        }
        /** Whether the entry is null (dummy) */
        auto is_null() const -> bool {
            return get_category_ptr() == nullptr;
        }
        /**
         * Whether the entry is an item.
         */
        auto is_item() const -> bool {
            return !locations.empty();
        }
        /** Whether the entry is a category */
        auto is_category() const -> bool {
            return !is_null() && !is_item();
        }
        /** Whether the entry can be selected */
        auto is_selectable() const -> bool {
            return is_item() && enabled;
        }

        auto any_item() const -> const item_location & {
            assert( !locations.empty() );
            return locations.front();
        }

        /** Pointer to first item in relevant stack on character. */
        auto item_stack_on_character() const -> const item * {
            assert( !locations.empty() );
            return locations.front().get_item();
        }

        auto get_stack_size() const -> size_t {
            return locations.size();
        }

        auto get_total_charges() const -> int;
        auto get_selected_charges() const -> int;

        auto get_available_count() const -> size_t;
        auto get_category_ptr() const -> const item_category *;
        auto get_invlet() const -> int;
        auto get_invlet_color() const -> nc_color;
        void update_cache();

    private:
        const item_category *custom_category = nullptr;
        bool enabled = true;

};

class inventory_selector_preset
{
    public:
        inventory_selector_preset();
        virtual ~inventory_selector_preset() = default;

        /** Does this entry satisfy the basic preset conditions? */
        virtual auto is_shown( const item_location & ) const -> bool {
            return true;
        }

        /**
         * The reason why this entry cannot be selected.
         * @return Either the reason of denial or empty string if it's accepted.
         */
        virtual auto get_denial( const item_location & ) const -> std::string {
            return std::string();
        }
        /** Whether the first item is considered to go before the second. */
        virtual auto sort_compare( const inventory_entry &lhs, const inventory_entry &rhs ) const -> bool;
        /** Color that will be used to display the entry string. */
        virtual auto get_color( const inventory_entry &entry ) const -> nc_color;

        auto get_denial( const inventory_entry &entry ) const -> std::string;
        /** Text in the cell */
        auto get_cell_text( const inventory_entry &entry, size_t cell_index ) const -> std::string;
        /** @return Whether the cell is a stub */
        auto is_stub_cell( const inventory_entry &entry, size_t cell_index ) const -> bool;
        /** Number of cells in the preset. */
        auto get_cells_count() const -> size_t {
            return cells.size();
        }
        /** Whether items should make new stacks if components differ */
        auto get_checking_components() const -> bool {
            return check_components;
        }

        virtual auto get_filter( const std::string &filter )
        const -> std::function<bool( const inventory_entry & )>;

    protected:
        /** Text of the first column (default: item name) */
        virtual auto get_caption( const inventory_entry &entry ) const -> std::string;
        /**
         * Append a new cell to the preset.
         * @param func The function that returns text for the cell.
         * @param title Title of the cell.
         * @param stub The cell won't be "revealed" if it contains only this value
         */
        void append_cell( const std::function<std::string( const item_location & )> &func,
                          const std::string &title = std::string(),
                          const std::string &stub = std::string() );
        void append_cell( const std::function<std::string( const inventory_entry & )> &func,
                          const std::string &title = std::string(),
                          const std::string &stub = std::string() );
        bool check_components = false;

    private:
        class cell_t
        {
            public:
                cell_t( const std::function<std::string( const inventory_entry & )> &func,
                        const std::string &title, const std::string &stub ) :
                    title( title ),
                    stub( stub ),
                    func( func ) {}

                auto get_text( const inventory_entry &entry ) const -> std::string;

                std::string title;
                std::string stub;

            private:
                std::function<std::string( const inventory_entry & )> func;
        };

        std::vector<cell_t> cells;
};

const inventory_selector_preset default_preset;

class inventory_column
{
    public:
        inventory_column( const inventory_selector_preset &preset = default_preset ) : preset( preset ) {
            cells.resize( preset.get_cells_count() );
        }

        virtual ~inventory_column() = default;

        auto empty() const -> bool {
            return entries.empty();
        }
        /**
         * Can this column be activated?
         * @return Whether the column contains selectable entries.
         * Note: independent from 'allows_selecting'
         */
        virtual auto activatable() const -> bool;
        /** Is this column visible? */
        auto visible() const -> bool {
            return !empty() && visibility && preset.get_cells_count() > 0;
        }
        /**
         * Does this column allow selecting?
         * "Cosmetic" columns (list of selected items) can explicitly prohibit selecting.
         * Note: independent from 'activatable'
         */
        virtual auto allows_selecting() const -> bool {
            return true;
        }

        auto page_index() const -> size_t {
            return page_of( page_offset );
        }

        auto pages_count() const -> size_t {
            return page_of( entries.size() + entries_per_page - 1 );
        }

        auto has_available_choices() const -> bool;
        auto is_selected( const inventory_entry &entry ) const -> bool;

        /**
         * Does this entry belong to the selected category?
         * When @ref navigation_mode::ITEM is used it's equivalent to @ref is_selected().
         */
        auto is_selected_by_category( const inventory_entry &entry ) const -> bool;

        auto get_selected() const -> const inventory_entry &;
        auto get_all_selected() const -> std::vector<inventory_entry *>;
        auto get_entries(
            const std::function<bool( const inventory_entry &entry )> &filter_func ) const -> std::vector<inventory_entry *>;

        auto find_by_invlet( int invlet ) const -> inventory_entry *;

        void draw( const catacurses::window &win, point pos ) const;

        void add_entry( const inventory_entry &entry );
        void move_entries_to( inventory_column &dest );
        void clear();
        void set_stack_favorite( const item_location &location, bool favorite );

        /** Selects the specified location. */
        auto select( const item_location &loc ) -> bool;

        /**
         * Change the selection.
         * @param new_index Index of the entry to select.
         * @param dir If the entry is not selectable, move in the specified direction
         */
        void select( size_t new_index, scroll_direction dir );

        auto get_selected_index() -> size_t {
            return selected_index;
        }

        void set_multiselect( bool multiselect ) {
            this->multiselect = multiselect;
        }

        void set_visibility( bool visibility ) {
            this->visibility = visibility;
        }

        void set_width( size_t new_width, const std::vector<inventory_column *> &all_columns );
        void set_height( size_t new_height );
        auto get_width() const -> size_t;
        auto get_height() const -> size_t;
        /** Expands the column to fit the new entry. */
        void expand_to_fit( const inventory_entry &entry );
        /** Resets width to original (unchanged). */
        virtual void reset_width( const std::vector<inventory_column *> &all_columns );
        /** Returns next custom inventory letter. */
        auto reassign_custom_invlets( const player &p, int min_invlet, int max_invlet ) -> int;
        /** Reorder entries, repopulate titles, adjust to the new height. */
        virtual void prepare_paging( const std::string &filter = "" );
        /**
         * Event handlers
         */
        virtual void on_input( const inventory_input &input );
        /** The entry has been changed. */
        virtual void on_change( const inventory_entry & ) {}
        /** The column has been activated. */
        virtual void on_activate() {
            active = true;
        }
        /** The column has been deactivated. */
        virtual void on_deactivate() {
            active = false;
        }
        /** Selection mode has been changed. */
        virtual void on_mode_change( navigation_mode mode ) {
            this->mode = mode;
        }

        void set_filter( const std::string &filter );

    protected:
        struct entry_cell_cache_t {
            bool assigned = false;
            nc_color color = c_unset;
            std::string denial;
            std::vector<std::string> text;
        };

        /**
         * Move the selection.
         */
        void move_selection( scroll_direction dir );
        void move_selection_page( scroll_direction dir );

        auto next_selectable_index( size_t index, scroll_direction dir ) const -> size_t;

        auto page_of( size_t index ) const -> size_t;
        auto page_of( const inventory_entry &entry ) const -> size_t;
        /**
         * Indentation of the entry.
         * @param entry The entry to check
         * @returns Either left indent when it's zero, or a gap between cells.
         */
        auto get_entry_indent( const inventory_entry &entry ) const -> size_t;
        /**
         *  Overall cell width.
         *  If corresponding cell is not empty (its width is greater than zero),
         *  then a value returned by  inventory_column::get_entry_indent() is added to the result.
         */
        auto get_entry_cell_width( size_t index, size_t cell_index ) const -> size_t;
        auto get_entry_cell_width( const inventory_entry &entry, size_t cell_index ) const -> size_t;
        /** Sum of the cell widths */
        auto get_cells_width() const -> size_t;

        auto make_entry_cell_cache( const inventory_entry &entry ) const -> entry_cell_cache_t;
        auto get_entry_cell_cache( size_t index ) const -> const entry_cell_cache_t &;

        const inventory_selector_preset &preset;

        std::vector<inventory_entry> entries;
        std::vector<inventory_entry> entries_unfiltered;
        navigation_mode mode = navigation_mode::ITEM;
        bool active = false;
        bool multiselect = false;
        bool paging_is_valid = false;
        bool visibility = true;

        size_t selected_index = 0;
        size_t page_offset = 0;
        size_t entries_per_page = std::numeric_limits<size_t>::max();
        size_t height = std::numeric_limits<size_t>::max();
        size_t reserved_width = 0;

    private:
        struct cell_t {
            size_t current_width = 0;   /// Current cell widths (can be affected by set_width())
            size_t real_width = 0;      /// Minimal cell widths (to embrace all the entries nicely)

            auto visible() const -> bool {
                return current_width > 0;
            }
            /** @return Gap before the cell. Negative value means the cell is shrunk */
            auto gap() const -> int {
                return current_width - real_width;
            }
        };

        std::vector<cell_t> cells;
        mutable std::vector<entry_cell_cache_t> entries_cell_cache;

        /** @return Number of visible cells */
        auto visible_cells() const -> size_t;
};

class selection_column : public inventory_column
{
    public:
        selection_column( const std::string &id, const std::string &name );
        ~selection_column() override;

        auto activatable() const -> bool override {
            return inventory_column::activatable() && pages_count() > 1;
        }

        auto allows_selecting() const -> bool override {
            return false;
        }

        void reset_width( const std::vector<inventory_column *> &all_columns ) override;

        void prepare_paging( const std::string &filter = "" ) override;

        void on_change( const inventory_entry &entry ) override;
        void on_mode_change( navigation_mode ) override {
            // Intentionally ignore mode change.
        }

    private:
        const pimpl<item_category> selected_cat;
        inventory_entry last_changed;
};

class inventory_selector
{
    public:
        inventory_selector( player &u, const inventory_selector_preset &preset = default_preset );
        virtual ~inventory_selector();
        /** These functions add items from map / vehicles. */
        void add_character_items( Character &character );
        void add_map_items( const tripoint &target );
        void add_vehicle_items( const tripoint &target );
        void add_nearby_items( int radius = 1 );
        /** Remove all items */
        void clear_items();
        /** Assigns a title that will be shown on top of the menu. */
        void set_title( const std::string &title ) {
            this->title = title;
        }
        /** Assigns a hint. */
        void set_hint( const std::string &hint ) {
            this->hint = hint;
        }
        /** Specify whether the header should show stats (weight and volume). */
        void set_display_stats( bool display_stats ) {
            this->display_stats = display_stats;
        }
        /** @return true when the selector is empty. */
        auto empty() const -> bool;
        /** @return true when there are enabled entries to select. */
        auto has_available_choices() const -> bool;

        /** Apply filter string to all columns */
        void set_filter( const std::string &str );
        /** Get last filter string set by set_filter or entered by player */
        auto get_filter() const -> std::string;

        // An array of cells for the stat lines. Example: ["Weight (kg)", "10", "/", "20"].
        using stat = std::array<std::string, 4>;
        using stats = std::array<stat, 2>;

        bool keep_open = false;

    protected:
        player &u;
        const inventory_selector_preset &preset;

        /**
         * The input context for navigation, already contains some actions for movement.
         */
        input_context ctxt;

        auto naturalize_category( const item_category &category,
                const tripoint &pos ) -> const item_category *;

        void add_entry( inventory_column &target_column,
                        std::vector<item_location> &&locations,
                        const item_category *custom_category = nullptr );

        void add_item( inventory_column &target_column,
                       item_location &&location,
                       const item_category *custom_category = nullptr );

        void add_items( inventory_column &target_column,
                        const std::function<item_location( item * )> &locator,
                        const std::vector<std::list<item *>> &stacks,
                        const item_category *custom_category = nullptr );

        auto get_input() -> inventory_input;

        /** Given an action from the input_context, try to act according to it. */
        void on_input( const inventory_input &input );
        /** Entry has been changed */
        void on_change( const inventory_entry &entry );

        auto create_or_get_ui_adaptor() -> shared_ptr_fast<ui_adaptor>;

        auto get_layout_width() const -> size_t;
        auto get_layout_height() const -> size_t;

        void set_filter();

        /** Tackles screen overflow */
        virtual void rearrange_columns( size_t client_width );

        static auto get_weight_and_volume_stats(
            units::mass weight_carried, units::mass weight_capacity,
            const units::volume &volume_carried, const units::volume &volume_capacity ) -> stats;

        /** Get stats to display in top right.
         *
         * By default, computes volume/weight numbers for @c u */
        virtual auto get_raw_stats() const -> stats;

        auto get_stats() const -> std::vector<std::string>;
        auto get_footer( navigation_mode m ) const -> std::pair<std::string, nc_color>;

        auto get_header_height() const -> size_t;
        auto get_header_min_width() const -> size_t;
        auto get_footer_min_width() const -> size_t;

        /** @return an entry from all entries by its invlet */
        auto find_entry_by_invlet( int invlet ) const -> inventory_entry *;

        auto get_all_columns() const -> const std::vector<inventory_column *> & {
            return columns;
        }
        auto get_visible_columns() const -> std::vector<inventory_column *>;

    private:
        // These functions are called from resizing/redraw callbacks of ui_adaptor
        // and should not be made protected or public.
        void prepare_layout( size_t client_width, size_t client_height );
        void prepare_layout();

        void resize_window( int width, int height );
        void refresh_window() const;

        void draw_header( const catacurses::window &w ) const;
        void draw_footer( const catacurses::window &w ) const;
        void draw_columns( const catacurses::window &w ) const;
        void draw_frame( const catacurses::window &w ) const;

    public:
        /**
         * Select a location
         * @param loc Location to select
         * @return true on success.
         */
        auto select( const item_location &loc ) -> bool;

        auto get_selected() -> const inventory_entry & {
            return get_active_column().get_selected();
        }

        void select_position( std::pair<size_t, size_t> position ) {
            prepare_layout();
            set_active_column( position.first );
            get_active_column().select( position.second, scroll_direction::BACKWARD );
        }

        auto get_selection_position() -> std::pair<size_t, size_t> {
            std::pair<size_t, size_t> position;
            position.first = active_column_index;
            position.second = get_active_column().get_selected_index();
            return position;
        }

        auto get_column( size_t index ) const -> inventory_column &;
        auto get_active_column() const -> inventory_column & {
            return get_column( active_column_index );
        }

        void set_active_column( size_t index );

    protected:
        auto get_columns_width( const std::vector<inventory_column *> &columns ) const -> size_t;
        /** @return Percentage of the window occupied by columns */
        auto get_columns_occupancy_ratio( size_t client_width ) const -> double;
        /** @return Do the visible columns need to be center-aligned */
        auto are_columns_centered( size_t client_width ) const -> bool;
        /** @return Are visible columns wider than available width */
        auto is_overflown( size_t client_width ) const -> bool;

        auto is_active_column( const inventory_column &column ) const -> bool {
            return &column == &get_active_column();
        }

        void append_column( inventory_column &column );

        /**
         * Activates either previous or next column.
         * @param dir Forward - next column, backward - previous.
         */
        void toggle_active_column( scroll_direction dir );

        void refresh_active_column() {
            if( !get_active_column().activatable() ) {
                toggle_active_column( scroll_direction::FORWARD );
            }
        }
        void toggle_navigation_mode();

        auto get_navigation_data( navigation_mode m ) const -> const navigation_mode_data &;

    private:
        catacurses::window w_inv;

        weak_ptr_fast<ui_adaptor> ui;

        std::unique_ptr<string_input_popup> spopup;

        std::vector<inventory_column *> columns;

        std::string title;
        std::string hint;
        size_t active_column_index;
        std::list<item_category> categories;
        navigation_mode mode;

        inventory_column own_inv_column;     // Column for own inventory items
        inventory_column own_gear_column;    // Column for own gear (weapon, armor) items
        inventory_column map_column;         // Column for map and vehicle items

        const int border = 1;                // Width of the window border
        std::string filter;

        bool is_empty = true;
        bool display_stats = true;

    public:
        auto action_bound_to_key( char key ) const -> std::string;
        /** Returns all keys in the current context which are bound to an action. Warning: may contain duplicates. */
        auto all_bound_keys( ) const -> std::vector<char>;
};

auto display_stat( const std::string &caption, int cur_value, int max_value,
                                       const std::function<std::string( int )> &disp_func ) -> inventory_selector::stat;

class inventory_pick_selector : public inventory_selector
{
    public:
        inventory_pick_selector( player &p,
                                 const inventory_selector_preset &preset = default_preset ) :
            inventory_selector( p, preset ) {}

        auto execute() -> item_location;
};

class inventory_multiselector : public inventory_selector
{
    public:
        inventory_multiselector( player &p, const inventory_selector_preset &preset = default_preset,
                                 const std::string &selection_column_title = "" );
    protected:
        void rearrange_columns( size_t client_width ) override;

    private:
        std::unique_ptr<inventory_column> selection_col;
};

class inventory_compare_selector : public inventory_multiselector
{
    public:
        inventory_compare_selector( player &p );
        auto execute() -> std::pair<const item *, const item *>;

    protected:
        std::vector<const item *> compared;

        void toggle_entry( inventory_entry *entry );
};

// This and inventory_drop_selectors should probably both inherit from a higher-abstraction "action selector".
// Should accept a function to calculate dummy values.

class inventory_iuse_selector : public inventory_multiselector
{
    public:
        using GetStats = std::function<stats( const std::map<const item *, int> & )>;
        inventory_iuse_selector( player &p,
                                 const std::string &selector_title,
                                 const inventory_selector_preset &preset = default_preset,
                                 const GetStats & = {} );
        auto execute() -> std::list<iuse_location>;

    protected:
        auto get_raw_stats() const -> stats override;
        void set_chosen_count( inventory_entry &entry, size_t count );

    private:
        GetStats get_stats;
        std::map<const item *, std::vector<iuse_location>> to_use;
};

class inventory_drop_selector : public inventory_multiselector
{
    public:
        inventory_drop_selector( player &p,
                                 const inventory_selector_preset &preset = default_preset );
        auto execute() -> drop_locations;

    protected:
        auto get_raw_stats() const -> stats override;
        /** Toggle item dropping */
        void set_chosen_count( inventory_entry &entry, size_t count );
        void process_selected( int &count, const std::vector<inventory_entry *> &selected );

    private:
        excluded_stacks dropping;
};

#endif // CATA_SRC_INVENTORY_UI_H
