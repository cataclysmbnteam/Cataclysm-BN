#pragma once

#include <climits>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "bodypart.h"
#include "calendar.h"
#include "character.h"
#include "character_id.h"
#include "color.h"
#include "creature.h"
#include "cursesdef.h"
#include "enums.h"
#include "game_constants.h"
#include "item.h"
#include "pimpl.h"
#include "point.h"
#include "ret_val.h"
#include "safe_reference.h"
#include "string_id.h"
#include "type_id.h"

class craft_command;
class effect;
class faction;
class inventory;
class map;
class npc;
class recipe;
struct damage_unit;
struct requirement_data;

enum class recipe_filter_flags : int;
struct itype;

static const std::string
DEFAULT_HOTKEYS( "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" );

class recipe_subset;

enum action_id : int;
class JsonIn;
class JsonObject;
class JsonOut;
class dispersion_sources;
struct bionic;
struct dealt_projectile_attack;
class profession;
struct trap;

nc_color encumb_color( int level );
enum game_message_type : int;
class vehicle;
struct item_comp;
struct tool_comp;
struct w_point;

using recipe_filter = std::function<bool( const recipe &r )>;

/** @relates ret_val */
template<>
struct ret_val<edible_rating>::default_success : public
    std::integral_constant<edible_rating, edible_rating::edible> {};

/** @relates ret_val */
template<>
struct ret_val<edible_rating>::default_failure : public
    std::integral_constant<edible_rating, edible_rating::inedible> {};

class player : public Character
{
    public:
        player();
        player( const player & ) = delete;
        player( player && ) noexcept ;
        ~player() override;
        player &operator=( const player & ) = delete;
        player &operator=( player && ) noexcept ;

        bool is_player() const override {
            return true;
        }
        player *as_player() override {
            return this;
        }
        const player *as_player() const override {
            return this;
        }

        bool is_npc() const override {
            return false;    // Overloaded for NPCs in npc.h
        }

        // populate variables, inventory items, and misc from json object
        virtual void deserialize( JsonIn &jsin ) = 0;

        // by default save all contained info
        virtual void serialize( JsonOut &jsout ) const = 0;

        /**
         * Remove charges from a specific item (given by its item position).
         * The item must exist and it must be counted by charges.
         * @param position Item position of the item.
         * @param quantity The number of charges to remove, must not be larger than
         * the current charges of the item.
         * @return An item that contains the removed charges, it's effectively a
         * copy of the item with the proper charges.
         */
        detached_ptr<item> reduce_charges( int position, int quantity );
        /**
         * Remove charges from a specific item (given by a pointer to it).
         * Otherwise identical to @ref reduce_charges(int,int)
         * @param it A pointer to the item, it *must* exist.
         * @param quantity How many charges to remove
         * @return An item that contains the removed charges, it's effectively a
         * copy of the item with the proper charges.
         */
        detached_ptr<item> reduce_charges( item *it, int quantity );


        /** consume components and create an active, in progress craft containing them */
        item *start_craft( craft_command &command, const tripoint &loc );
        /**
         * Start various types of crafts
         * @param loc the location of the workbench. tripoint_zero indicates crafting from inventory.
         */
        void craft( const tripoint &loc = tripoint_zero );
        void recraft( const tripoint &loc = tripoint_zero );
        void long_craft( const tripoint &loc = tripoint_zero );
        void make_craft( const recipe_id &id, int batch_size, const tripoint &loc = tripoint_zero );
        void make_all_craft( const recipe_id &id, int batch_size, const tripoint &loc = tripoint_zero );

        // ---------------VALUES-----------------
        tripoint view_offset;
        // Relative direction of a grab, add to posx, posy to get the coordinates of the grabbed thing.
        tripoint grab_point;
        int volume = 0;

        bool random_start_location = false;
        start_location_id start_location;

        // Save favorite ammo location
        //TODO!: check this
        safe_reference<item> ammo_location;
        int movecounter = 0;

        bool manual_examine = false;
        vproto_id starting_vehicle = vproto_id::NULL_ID();
        std::vector<mtype_id> starting_pets;

        void make_craft_with_command( const recipe_id &id_to_make, int batch_size, bool is_long = false,
                                      const tripoint &loc = tripoint_zero );
        pimpl<craft_command> last_craft;

        recipe_id lastrecipe;
        int last_batch = 0;
        itype_id lastconsumed;        //used in crafting.cpp and construction.cpp

        std::set<character_id> follower_ids;

        //message related stuff
        using Character::add_msg_if_player;
        void add_msg_if_player( const std::string &msg ) const override;
        void add_msg_if_player( const game_message_params &params, const std::string &msg ) const override;
        using Character::add_msg_player_or_npc;
        void add_msg_player_or_npc( const std::string &player_msg,
                                    const std::string &npc_str ) const override;
        void add_msg_player_or_npc( const game_message_params &params, const std::string &player_msg,
                                    const std::string &npc_msg ) const override;
        using Character::add_msg_player_or_say;
        void add_msg_player_or_say( const std::string &player_msg,
                                    const std::string &npc_speech ) const override;
        void add_msg_player_or_say( const game_message_params &params, const std::string &player_msg,
                                    const std::string &npc_speech ) const override;

        using Character::query_yn;
        bool query_yn( const std::string &mes ) const override;

    protected:

        void store( JsonOut &json ) const;
        void load( const JsonObject &data );
};


