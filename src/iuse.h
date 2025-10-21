#pragma once

#include <memory>
#include <string>
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
std::pair<int, units::energy> sewage( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> honeycomb( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> alcohol_weak( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> alcohol_medium( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> alcohol_strong( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> xanax( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> antibiotic( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> eyedrops( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> fungicide( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> antifungal( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> antiparasitic( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> anticonvulsant( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> meth( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> vaccine( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> poison( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> meditate( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> thorazine( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> prozac( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> sleep( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> datura( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> flumed( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> flusleep( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> inhaler( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> blech( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> blech_because_unclean( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> plantblech( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> purifier( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> purify_iv( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> purify_smart( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> marloss( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> marloss_seed( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> marloss_gel( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> mycus( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> petfood( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> antiasthmatic( player *, item *, bool, const tripoint & );
// TOOLS
std::pair<int, units::energy> amputate( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> extinguisher( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> hammer( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> water_purifier( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> directional_antenna( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> radio_off( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> radio_on( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> noise_emitter_off( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> noise_emitter_on( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> note_bionics( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> ma_manual( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> crowbar( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> makemound( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> dig( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> dig_channel( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> fill_pit( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> clear_rubble( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> siphon( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> jackhammer( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> pickaxe( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> burrow( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> geiger( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> teleport( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> can_goo( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> throwable_extinguisher_act( player *, item *, bool,
        const tripoint & );
std::pair<int, units::energy> directional_hologram( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> capture_monster_veh( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> capture_monster_act( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> granade( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> granade_act( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> c4( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> arrow_flammable( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> acidbomb_act( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> grenade_inc_act( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> molotov_lit( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> firecracker_pack_act( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> firecracker_act( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> mininuke( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> pheromone( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> pick_lock( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> portal( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> tazer( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> mp3_on( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> rpgdie( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> dive_tank( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> gasmask( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> portable_game( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> vibe( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> hand_crank( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> vortex( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> dog_whistle( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> call_of_tindalos( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> blood_draw( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> mind_splicer( player *, item *, bool, const tripoint & );
void cut_log_into_planks( player & );
std::pair<int, units::energy> lumber( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> chop_tree( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> chop_logs( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> oxytorch( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> hacksaw( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> boltcutters( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> mop( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> spray_can( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> towel( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> unfold_generic( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> adrenaline_injector( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> jet_injector( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> stimpack( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> contacts( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> talking_doll( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> bell( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> seed( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> oxygen_bottle( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> radio_mod( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> remove_all_mods( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> fishing_rod( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> fish_trap( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> gun_clean( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> gun_repair( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> gunmod_attach( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> toolmod_attach( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> unpack_item( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> pack_cbm( player *p, item *it, bool, const tripoint & );
std::pair<int, units::energy> pack_item( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> radglove( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> robotcontrol( player *, item *, bool, const tripoint & );
// Helper for validating a potential taget of robot control
bool robotcontrol_can_target( player *, const monster & );
std::pair<int, units::energy> einktabletpc( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> camera( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> ehandcuffs( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> foodperson( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> tow_attach( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> cable_attach( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> shavekit( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> hairkit( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> weather_tool( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> ladder( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> solarpack( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> solarpack_off( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> weak_antibiotic( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> strong_antibiotic( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> melatonin_tablet( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> coin_flip( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> play_game( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> magic_8_ball( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> toggle_heats_food( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> toggle_ups_charging( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> report_grid_charge( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> report_grid_connections( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> modify_grid_connections( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> bullet_vibe_on( player *, item *, bool, const tripoint & );

// MACGUFFINS

std::pair<int, units::energy> radiocar( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> radiocaron( player *, item *, bool, const tripoint & );
std::pair<int, units::energy> radiocontrol( player *, item *, bool, const tripoint & );

std::pair<int, units::energy> autoclave( player *, item *, bool, const tripoint & );

std::pair<int, units::energy> remoteveh( player *, item *, bool, const tripoint & );

std::pair<int, units::energy> craft( player *, item *, bool, const tripoint & );

std::pair<int, units::energy> disassemble( player *, item *, bool, const tripoint & );

// ARTIFACTS
/* This function is used when an artifact is activated.
   It examines the item's artifact-specific properties.
   See artifact.h for a list.                        */
std::pair<int, units::energy> artifact( player *, item *, bool, const tripoint & );

// Helper for listening to music, might deserve a better home, but not sure where.
void play_music( player &p, const tripoint &source, int volume, int max_morale );
std::pair<int, units::energy> towel_common( player *, item *, bool );

// Helper for handling pesky wannabe-artists
std::pair<int, units::energy> handle_ground_graffiti( player &p, item *it,
        const std::string &prefix,
        const tripoint &where );

// Helper for wood chopping
int chop_moves( Character &ch, item &tool );

// LEGACY
std::pair<int, units::energy> cauterize_hotplate( player *, item *, bool, const tripoint & );

} // namespace iuse

void remove_radio_mod( item &it, player &p );


using use_function_pointer = std::pair<int, units::energy> ( * )( player *, item *, bool,
                             const tripoint & );

class iuse_actor
{
    protected:
        iuse_actor( const std::string &type, int cost = -1, units::energy e_cost = -1_J ) :
            type( type ), cost( cost ), e_cost( e_cost ) {}

    public:
        /**
         * The type of the action. It's not translated. Different iuse_actor instances may have the
         * same type, but different data.
         */
        const std::string type;

        /** Units of ammo required per invocation (or use value from base item if negative) */
        int cost;
        units::energy e_cost;

        virtual ~iuse_actor() = default;
        virtual void load( const JsonObject &jo ) = 0;
        virtual std::pair<int, units::energy> use( player &, item &, bool,
                const tripoint & ) const = 0;
        virtual ret_val<bool> can_use( const Character &, const item &, bool, const tripoint & ) const;
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
        virtual std::unique_ptr<iuse_actor> clone() const = 0;
        /**
         * Returns whether the actor is valid (exists in the generator).
         */
        virtual bool is_valid() const;
        /**
         * Returns the translated name of the action. It is used for the item action menu.
         */
        virtual std::string get_name() const;
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

        std::pair<int, units::energy> call( player &, item &, bool, const tripoint & ) const;
        ret_val<bool> can_call( const Character &, const item &, bool t, const tripoint &pos ) const;

        iuse_actor *get_actor_ptr() {
            return actor.get();
        }

        const iuse_actor *get_actor_ptr() const {
            return actor.get();
        }

        explicit operator bool() const {
            return actor != nullptr;
        }

        /** @return See @ref iuse_actor::type */
        std::string get_type() const;
        /** @return See @ref iuse_actor::get_name */
        std::string get_name() const;
        /** @return Used by @ref item::info to get description of the actor */
        void dump_info( const item &, std::vector<iteminfo> & ) const;
};


