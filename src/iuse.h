#pragma once
#ifndef CATA_SRC_IUSE_H
#define CATA_SRC_IUSE_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "clone_ptr.h"
#include "type_id.h"
#include "units.h"

class Character;
class JsonObject;
class item;
class monster;
class player;
struct iteminfo;
template<typename T> class ret_val;
struct tripoint;

// iuse methods returning a bool indicating whether to consume a charge of the item being used.
namespace iuse
{
// FOOD AND DRUGS (ADMINISTRATION)
auto sewage( player *, item *, bool, const tripoint & ) -> int;
auto honeycomb( player *, item *, bool, const tripoint & ) -> int;
auto alcohol_weak( player *, item *, bool, const tripoint & ) -> int;
auto alcohol_medium( player *, item *, bool, const tripoint & ) -> int;
auto alcohol_strong( player *, item *, bool, const tripoint & ) -> int;
auto xanax( player *, item *, bool, const tripoint & ) -> int;
auto smoking( player *, item *, bool, const tripoint & ) -> int;
auto ecig( player *, item *, bool, const tripoint & ) -> int;
auto antibiotic( player *, item *, bool, const tripoint & ) -> int;
auto eyedrops( player *, item *, bool, const tripoint & ) -> int;
auto fungicide( player *, item *, bool, const tripoint & ) -> int;
auto antifungal( player *, item *, bool, const tripoint & ) -> int;
auto antiparasitic( player *, item *, bool, const tripoint & ) -> int;
auto anticonvulsant( player *, item *, bool, const tripoint & ) -> int;
auto weed_cake( player *, item *, bool, const tripoint & ) -> int;
auto meth( player *, item *, bool, const tripoint & ) -> int;
auto vaccine( player *, item *, bool, const tripoint & ) -> int;
auto poison( player *, item *, bool, const tripoint & ) -> int;
auto meditate( player *, item *, bool, const tripoint & ) -> int;
auto thorazine( player *, item *, bool, const tripoint & ) -> int;
auto prozac( player *, item *, bool, const tripoint & ) -> int;
auto sleep( player *, item *, bool, const tripoint & ) -> int;
auto datura( player *, item *, bool, const tripoint & ) -> int;
auto flumed( player *, item *, bool, const tripoint & ) -> int;
auto flusleep( player *, item *, bool, const tripoint & ) -> int;
auto inhaler( player *, item *, bool, const tripoint & ) -> int;
auto blech( player *, item *, bool, const tripoint & ) -> int;
auto blech_because_unclean( player *, item *, bool, const tripoint & ) -> int;
auto plantblech( player *, item *, bool, const tripoint & ) -> int;
auto chew( player *, item *, bool, const tripoint & ) -> int;
auto purifier( player *, item *, bool, const tripoint & ) -> int;
auto purify_iv( player *, item *, bool, const tripoint & ) -> int;
auto purify_smart( player *, item *, bool, const tripoint & ) -> int;
auto marloss( player *, item *, bool, const tripoint & ) -> int;
auto marloss_seed( player *, item *, bool, const tripoint & ) -> int;
auto marloss_gel( player *, item *, bool, const tripoint & ) -> int;
auto mycus( player *, item *, bool, const tripoint & ) -> int;
auto dogfood( player *, item *, bool, const tripoint & ) -> int;
auto catfood( player *, item *, bool, const tripoint & ) -> int;
auto feedcattle( player *, item *, bool, const tripoint & ) -> int;
auto feedbird( player *, item *, bool, const tripoint & ) -> int;
auto antiasthmatic( player *, item *, bool, const tripoint & ) -> int;
// TOOLS
auto extinguisher( player *, item *, bool, const tripoint & ) -> int;
auto hammer( player *, item *, bool, const tripoint & ) -> int;
auto water_purifier( player *, item *, bool, const tripoint & ) -> int;
auto directional_antenna( player *, item *, bool, const tripoint & ) -> int;
auto radio_off( player *, item *, bool, const tripoint & ) -> int;
auto radio_on( player *, item *, bool, const tripoint & ) -> int;
auto noise_emitter_off( player *, item *, bool, const tripoint & ) -> int;
auto noise_emitter_on( player *, item *, bool, const tripoint & ) -> int;
auto note_bionics( player *, item *, bool, const tripoint & ) -> int;
auto ma_manual( player *, item *, bool, const tripoint & ) -> int;
auto crowbar( player *, item *, bool, const tripoint & ) -> int;
auto makemound( player *, item *, bool, const tripoint & ) -> int;
auto dig( player *, item *, bool, const tripoint & ) -> int;
auto dig_channel( player *, item *, bool, const tripoint & ) -> int;
auto fill_pit( player *, item *, bool, const tripoint & ) -> int;
auto clear_rubble( player *, item *, bool, const tripoint & ) -> int;
auto siphon( player *, item *, bool, const tripoint & ) -> int;
auto chainsaw_off( player *, item *, bool, const tripoint & ) -> int;
auto chainsaw_on( player *, item *, bool, const tripoint & ) -> int;
auto elec_chainsaw_off( player *, item *, bool, const tripoint & ) -> int;
auto elec_chainsaw_on( player *, item *, bool, const tripoint & ) -> int;
auto cs_lajatang_off( player *, item *, bool, const tripoint & ) -> int;
auto cs_lajatang_on( player *, item *, bool, const tripoint & ) -> int;
auto ecs_lajatang_off( player *, item *, bool, const tripoint & ) -> int;
auto ecs_lajatang_on( player *, item *, bool, const tripoint & ) -> int;
auto carver_off( player *, item *, bool, const tripoint & ) -> int;
auto carver_on( player *, item *, bool, const tripoint & ) -> int;
auto trimmer_off( player *, item *, bool, const tripoint & ) -> int;
auto trimmer_on( player *, item *, bool, const tripoint & ) -> int;
auto circsaw_on( player *, item *, bool, const tripoint & ) -> int;
auto combatsaw_off( player *, item *, bool, const tripoint & ) -> int;
auto combatsaw_on( player *, item *, bool, const tripoint & ) -> int;
auto e_combatsaw_off( player *, item *, bool, const tripoint & ) -> int;
auto e_combatsaw_on( player *, item *, bool, const tripoint & ) -> int;
auto jackhammer( player *, item *, bool, const tripoint & ) -> int;
auto pickaxe( player *, item *, bool, const tripoint & ) -> int;
auto burrow( player *, item *, bool, const tripoint & ) -> int;
auto geiger( player *, item *, bool, const tripoint & ) -> int;
auto teleport( player *, item *, bool, const tripoint & ) -> int;
auto can_goo( player *, item *, bool, const tripoint & ) -> int;
auto throwable_extinguisher_act( player *, item *, bool, const tripoint & ) -> int;
auto directional_hologram( player *, item *, bool, const tripoint & ) -> int;
auto capture_monster_veh( player *, item *, bool, const tripoint & ) -> int;
auto capture_monster_act( player *, item *, bool, const tripoint & ) -> int;
auto granade( player *, item *, bool, const tripoint & ) -> int;
auto granade_act( player *, item *, bool, const tripoint & ) -> int;
auto c4( player *, item *, bool, const tripoint & ) -> int;
auto arrow_flammable( player *, item *, bool, const tripoint & ) -> int;
auto acidbomb_act( player *, item *, bool, const tripoint & ) -> int;
auto grenade_inc_act( player *, item *, bool, const tripoint & ) -> int;
auto molotov_lit( player *, item *, bool, const tripoint & ) -> int;
auto firecracker_pack( player *, item *, bool, const tripoint & ) -> int;
auto firecracker_pack_act( player *, item *, bool, const tripoint & ) -> int;
auto firecracker( player *, item *, bool, const tripoint & ) -> int;
auto firecracker_act( player *, item *, bool, const tripoint & ) -> int;
auto mininuke( player *, item *, bool, const tripoint & ) -> int;
auto pheromone( player *, item *, bool, const tripoint & ) -> int;
auto portal( player *, item *, bool, const tripoint & ) -> int;
auto tazer( player *, item *, bool, const tripoint & ) -> int;
auto tazer2( player *, item *, bool, const tripoint & ) -> int;
auto shocktonfa_off( player *, item *, bool, const tripoint & ) -> int;
auto shocktonfa_on( player *, item *, bool, const tripoint & ) -> int;
auto mp3( player *, item *, bool, const tripoint & ) -> int;
auto mp3_on( player *, item *, bool, const tripoint & ) -> int;
auto rpgdie( player *, item *, bool, const tripoint & ) -> int;
auto dive_tank( player *, item *, bool, const tripoint & ) -> int;
auto gasmask( player *, item *, bool, const tripoint & ) -> int;
auto portable_game( player *, item *, bool, const tripoint & ) -> int;
auto fitness_check( player *p, item *it, bool, const tripoint & ) -> int;
auto vibe( player *, item *, bool, const tripoint & ) -> int;
auto hand_crank( player *, item *, bool, const tripoint & ) -> int;
auto vortex( player *, item *, bool, const tripoint & ) -> int;
auto dog_whistle( player *, item *, bool, const tripoint & ) -> int;
auto call_of_tindalos( player *, item *, bool, const tripoint & ) -> int;
auto blood_draw( player *, item *, bool, const tripoint & ) -> int;
auto mind_splicer( player *, item *, bool, const tripoint & ) -> int;
void cut_log_into_planks( player & );
auto lumber( player *, item *, bool, const tripoint & ) -> int;
auto chop_tree( player *, item *, bool, const tripoint & ) -> int;
auto chop_logs( player *, item *, bool, const tripoint & ) -> int;
auto oxytorch( player *, item *, bool, const tripoint & ) -> int;
auto hacksaw( player *, item *, bool, const tripoint & ) -> int;
auto boltcutters( player *, item *, bool, const tripoint & ) -> int;
auto mop( player *, item *, bool, const tripoint & ) -> int;
auto spray_can( player *, item *, bool, const tripoint & ) -> int;
auto towel( player *, item *, bool, const tripoint & ) -> int;
auto unfold_generic( player *, item *, bool, const tripoint & ) -> int;
auto adrenaline_injector( player *, item *, bool, const tripoint & ) -> int;
auto jet_injector( player *, item *, bool, const tripoint & ) -> int;
auto stimpack( player *, item *, bool, const tripoint & ) -> int;
auto contacts( player *, item *, bool, const tripoint & ) -> int;
auto talking_doll( player *, item *, bool, const tripoint & ) -> int;
auto bell( player *, item *, bool, const tripoint & ) -> int;
auto seed( player *, item *, bool, const tripoint & ) -> int;
auto oxygen_bottle( player *, item *, bool, const tripoint & ) -> int;
auto radio_mod( player *, item *, bool, const tripoint & ) -> int;
auto remove_all_mods( player *, item *, bool, const tripoint & ) -> int;
auto fishing_rod( player *, item *, bool, const tripoint & ) -> int;
auto fish_trap( player *, item *, bool, const tripoint & ) -> int;
auto gun_repair( player *, item *, bool, const tripoint & ) -> int;
auto gunmod_attach( player *, item *, bool, const tripoint & ) -> int;
auto toolmod_attach( player *, item *, bool, const tripoint & ) -> int;
auto rm13armor_off( player *, item *, bool, const tripoint & ) -> int;
auto rm13armor_on( player *, item *, bool, const tripoint & ) -> int;
auto unpack_item( player *, item *, bool, const tripoint & ) -> int;
auto pack_cbm( player *p, item *it, bool, const tripoint & ) -> int;
auto pack_item( player *, item *, bool, const tripoint & ) -> int;
auto radglove( player *, item *, bool, const tripoint & ) -> int;
auto robotcontrol( player *, item *, bool, const tripoint & ) -> int;
// Helper for validating a potential taget of robot control
auto robotcontrol_can_target( player *, const monster & ) -> bool;
auto einktabletpc( player *, item *, bool, const tripoint & ) -> int;
auto camera( player *, item *, bool, const tripoint & ) -> int;
auto ehandcuffs( player *, item *, bool, const tripoint & ) -> int;
auto foodperson( player *, item *, bool, const tripoint & ) -> int;
auto tow_attach( player *, item *, bool, const tripoint & ) -> int;
auto cable_attach( player *, item *, bool, const tripoint & ) -> int;
auto shavekit( player *, item *, bool, const tripoint & ) -> int;
auto hairkit( player *, item *, bool, const tripoint & ) -> int;
auto weather_tool( player *, item *, bool, const tripoint & ) -> int;
auto ladder( player *, item *, bool, const tripoint & ) -> int;
auto wash_soft_items( player *, item *, bool, const tripoint & ) -> int;
auto wash_hard_items( player *, item *, bool, const tripoint & ) -> int;
auto wash_all_items( player *, item *, bool, const tripoint & ) -> int;
auto solarpack( player *, item *, bool, const tripoint & ) -> int;
auto solarpack_off( player *, item *, bool, const tripoint & ) -> int;
auto weak_antibiotic( player *, item *, bool, const tripoint & ) -> int;
auto strong_antibiotic( player *, item *, bool, const tripoint & ) -> int;
auto melatonin_tablet( player *, item *, bool, const tripoint & ) -> int;
auto coin_flip( player *, item *, bool, const tripoint & ) -> int;
auto play_game( player *, item *, bool, const tripoint & ) -> int;
auto magic_8_ball( player *, item *, bool, const tripoint & ) -> int;
auto toggle_heats_food( player *, item *, bool, const tripoint & ) -> int;
auto report_grid_charge( player *, item *, bool, const tripoint & ) -> int;
auto report_grid_connections( player *, item *, bool, const tripoint & ) -> int;

// MACGUFFINS

auto radiocar( player *, item *, bool, const tripoint & ) -> int;
auto radiocaron( player *, item *, bool, const tripoint & ) -> int;
auto radiocontrol( player *, item *, bool, const tripoint & ) -> int;

auto autoclave( player *, item *, bool, const tripoint & ) -> int;

auto multicooker( player *, item *, bool, const tripoint & ) -> int;

auto remoteveh( player *, item *, bool, const tripoint & ) -> int;

auto craft( player *, item *, bool, const tripoint & ) -> int;

auto disassemble( player *, item *, bool, const tripoint & ) -> int;

// ARTIFACTS
/* This function is used when an artifact is activated.
   It examines the item's artifact-specific properties.
   See artifact.h for a list.                        */
auto artifact( player *, item *, bool, const tripoint & ) -> int;

// Helper for listening to music, might deserve a better home, but not sure where.
void play_music( player &p, const tripoint &source, int volume, int max_morale );
auto towel_common( player *, item *, bool ) -> int;

// Helper for handling pesky wannabe-artists
auto handle_ground_graffiti( player &p, item *it, const std::string &prefix,
                            const tripoint &where ) -> int;

// Helper for wood chopping
auto chop_moves( Character &ch, item &tool ) -> int;

// LEGACY
auto cauterize_hotplate( player *, item *, bool, const tripoint & ) -> int;

} // namespace iuse

void remove_radio_mod( item &it, player &p );

// Helper for clothes washing
struct washing_requirements {
    int water;
    int cleanser;
    int time;
};
auto washing_requirements_for_volume( const units::volume & ) -> washing_requirements;

using use_function_pointer = int ( * )( player *, item *, bool, const tripoint & );

class iuse_actor
{
    protected:
        iuse_actor( const std::string &type, int cost = -1 ) : type( type ), cost( cost ) {}

    public:
        /**
         * The type of the action. It's not translated. Different iuse_actor instances may have the
         * same type, but different data.
         */
        const std::string type;

        /** Units of ammo required per invocation (or use value from base item if negative) */
        int cost;

        virtual ~iuse_actor() = default;
        virtual void load( const JsonObject &jo ) = 0;
        virtual auto use( player &, item &, bool, const tripoint & ) const -> int = 0;
        virtual auto can_use( const Character &, const item &, bool, const tripoint & ) const -> ret_val<bool>;
        virtual void info( const item &, std::vector<iteminfo> & ) const {}
        /**
         * Returns a deep copy of this object. Example implementation:
         * \code
         * class my_iuse_actor {
         *     std::unique_ptr<iuse_actor> clone() const override {
         *         return std::make_unique<my_iuse_actor>( *this );
         *     }
         * };
         * \endcode
         * The returned value should behave like the original item and must have the same type.
         */
        virtual auto clone() const -> std::unique_ptr<iuse_actor> = 0;
        /**
         * Returns whether the actor is valid (exists in the generator).
         */
        virtual auto is_valid() const -> bool;
        /**
         * Returns the translated name of the action. It is used for the item action menu.
         */
        virtual auto get_name() const -> std::string;
        /**
         * Finalizes the actor. Must be called after all items are loaded.
         */
        virtual void finalize( const itype_id &/*my_item_type*/ ) { }
};

struct use_function {
    protected:
        cata::clone_ptr<iuse_actor> actor;

    public:
        use_function() = default;
        use_function( const std::string &type, use_function_pointer f );
        use_function( std::unique_ptr<iuse_actor> f ) : actor( std::move( f ) ) {}

        auto call( player &, item &, bool, const tripoint & ) const -> int;
        auto can_call( const Character &, const item &, bool t, const tripoint &pos ) const -> ret_val<bool>;

        auto get_actor_ptr() -> iuse_actor * {
            return actor.get();
        }

        auto get_actor_ptr() const -> const iuse_actor * {
            return actor.get();
        }

        explicit operator bool() const {
            return actor != nullptr;
        }

        /** @return See @ref iuse_actor::type */
        auto get_type() const -> std::string;
        /** @return See @ref iuse_actor::get_name */
        auto get_name() const -> std::string;
        /** @return Used by @ref item::info to get description of the actor */
        void dump_info( const item &, std::vector<iteminfo> & ) const;
};

#endif // CATA_SRC_IUSE_H
