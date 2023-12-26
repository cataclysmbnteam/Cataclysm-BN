#ifdef LUA
#include "catalua_bindings.h"

#include "activity_type.h"
#include "avatar.h"
#include "bionics.h"
#include "bodypart.h"
#include "catalua_bindings_utils.h"
#include "catalua_impl.h"
#include "catalua_log.h"
#include "catalua_luna_doc.h"
#include "catalua_luna.h"
#include "catalua.h"
#include "character.h"
#include "creature.h"
#include "damage.h"
#include "disease.h"
#include "field_type.h"
#include "field.h"
#include "morale_types.h"
#include "monfaction.h"
#include "monster.h"
#include "mutation.h"
#include "npc.h"
#include "player.h"
#include "pldata.h"
#include "skill.h"

#include "type_id.h"
#include "flag.h"
#include "flag_trait.h"

// Note(AluminumAlman): using these macros makes binding to Lua much less of a chore.
// I'd rather shoot myself in the foot than to torture myself with busywork.
// SET MEMBer
#define SET_MEMB(prop_name) luna::set( ut, #prop_name, &UT_CLASS::prop_name )
// SET_MEMBer with Lua Name
//#define SET_MEMB_LN(prop_name, lua_name_str) luna::set( ut, lua_name_str, &UT_CLASS::prop_name )
// SET_FX
#define SET_FX(func_name) luna::set_fx ( ut, #func_name, &UT_CLASS::func_name)
// SET_FX with Type
#define SET_FX_T(func_name, func_type) luna::set_fx( ut, #func_name, \
        sol::resolve< func_type >( &UT_CLASS::func_name))
// SET_FX with Lua Name
//#define SET_FX_LN(func_name, lua_name_str) luna::set_fx ( ut, lua_name_str, &UT_CLASS::func_name)
// SET_FX with Lua Name and Type
#define SET_FX_LN_T(func_name, lua_name_str, func_type) luna::set_fx( ut, lua_name_str, \
        sol::resolve< func_type >( &UT_CLASS::func_name))

void cata::detail::reg_creature_family( sol::state &lua )
{
    // Specifying base classes here allows us to pass derived classes
    // from Lua to C++ functions that expect base class.
#define UT_CLASS Creature
    {
        sol::usertype<UT_CLASS> ut =
            luna::new_usertype<UT_CLASS>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        // Note(AluminumAlman): I used these commands on class definitions in header files.
        // They're not fool-proof, by the way. I suggest using the ones for npc,
        // since I actively documented those.

        // Gets rid of trailing whitespace in empty lines and blank comments
        // s@\(\s\|//\)\+\(\n\)@\2@g
        // Gets rid of comments
        // s@ *//.*@@g
        // s@ * /\*\_.\{-}\*/@@g
        // Gets rid of non-public members in class header files (not fool-proof)
        // s@\(protected:\|private:\)\_.\{-}\(private:\|protected:\|public:\|};\)@\2@g
        // Gets rid of overriden functions
        // s@.*override *{\_.\{-}}\n@@g
        // s@.*override *;\n@@g
        // Cleanup the header function definitions
        // s@ = 0;@;@g
        // Makes all function definitions be single lines
        // '%s@,\n\s*@, '
        // Gets rid of function param names and values
        // s@ *\([A-z0-9_]\+ *= *\)\=[A-z0-9_.{}]\+\( *,\| *)\)@\2@g
        // Formats class functions into the macro's format
        // s@\(.\{-}\)\([A-z_]*\)\((.\{-}\);@SET_FX_T(\2, \1\3 );

        // Methods
        SET_FX_T( get_name, std::string() const );
        SET_FX_T( disp_name, std::string( bool, bool ) const );
        SET_FX_T( skin_name, std::string() const );
        SET_FX_T( get_grammatical_genders, std::vector<std::string>() const );

        //SET_FX_T( is_player, bool() const );
        SET_FX_T( is_avatar, bool() const );
        SET_FX_T( is_npc, bool() const );
        SET_FX_T( is_monster, bool() const );
        SET_FX_T( as_monster, monster * () );
        SET_FX_T( as_npc, npc * () );
        SET_FX_T( as_character, Character * () );
        //SET_FX_T( as_player, player * () );
        SET_FX_T( as_avatar, avatar * () );

        SET_FX_T( hit_roll, float() const );
        SET_FX_T( dodge_roll, float() );
        SET_FX_T( stability_roll, float() const );

        SET_FX_T( attitude_to, Creature::Attitude( const Creature & ) const );

        SET_FX_T( sees, bool( const Creature & ) const );

        SET_FX_T( sight_range, int( int ) const );

        SET_FX_T( power_rating, float() const );

        SET_FX_T( speed_rating, float() const );

        SET_FX_T( ranged_target_size, double() const );

        SET_FX_T( knock_back_to, void( const tripoint & ) );

        SET_FX_T( size_melee_penalty, int() const );

        //SET_FX_T( deal_damage, dealt_damage_instance( Creature *,
        //bodypart_id, const damage_instance & ) );

        SET_FX_T( digging, bool() const );
        SET_FX_T( is_on_ground, bool() const );
        SET_FX_T( is_underwater, bool() const );
        SET_FX_T( set_underwater, void( bool x ) );
        SET_FX_T( is_warm, bool() const );
        SET_FX_T( in_species, bool( const species_id & ) const );

        SET_FX_T( has_weapon, bool() const );
        SET_FX_T( is_hallucination, bool() const );
        SET_FX_LN_T( is_dead_state, "is_dead", bool() const );

        SET_FX_T( is_elec_immune, bool() const );
        SET_FX_T( is_immune_effect, bool( const efftype_id & ) const );
        SET_FX_T( is_immune_damage, bool( damage_type ) const );

        //SET_FX_T( is_dangerous_fields, bool( const field & ) const );

        //SET_FX_T( is_dangerous_field, bool( const field_entry & ) const );

        //SET_FX_T( is_immune_field, bool( const field_type_id & ) const );

        SET_FX_LN_T( pos, "get_pos_ms", const tripoint & () const );

        SET_FX_LN_T( setpos, "set_pos_ms", void( const tripoint & ) );

        luna::set_fx( ut, "has_effect", []( const Creature & cr, const efftype_id & eff,
        sol::optional<const bodypart_str_id &> bpid ) -> bool {
            if( bpid.has_value() )
            {
                return cr.has_effect( eff, *bpid );
            } else
            {
                return cr.has_effect( eff );
            }
        } );

        luna::set_fx( ut, "has_effect_with_flag", []( const Creature & cr,
        const flag_id & flag, sol::optional<const bodypart_str_id &> bpid ) -> bool {
            body_part bp = bpid ? ( *bpid ) -> token : num_bp;
            return cr.has_effect_with_flag( flag, bp );
        } );

        luna::set_fx( ut, "get_effect_dur", []( const Creature & cr, const efftype_id & eff,
        sol::optional<const bodypart_str_id &> bpid ) -> time_duration {
            body_part bp = bpid ? ( *bpid ) -> token : num_bp;
            return cr.get_effect_dur( eff, bp );
        } );

        luna::set_fx( ut, "get_effect_int", []( const Creature & cr, const efftype_id & eff,
        sol::optional<const bodypart_str_id &> bpid ) -> int {
            body_part bp = bpid ? ( *bpid ) -> token : num_bp;
            return cr.get_effect_int( eff, bp );
        } );

        DOC( "Effect type, duration, bodypart and intensity" );
        luna::set_fx( ut, "add_effect", []( Creature & cr, const efftype_id & eff,
                                            const time_duration & dur,
                                            sol::optional<const bodypart_str_id &> bpid,
                                            sol::optional<int> intensity
        ) {
            int eint = intensity ? *intensity : 0;
            body_part bp = bpid ? ( *bpid ) -> token : num_bp;
            cr.add_effect( eff, dur, bp, eint );
        } );

        luna::set_fx( ut, "remove_effect", []( Creature & cr, const efftype_id & eff,
        sol::optional<const bodypart_str_id &> bpid ) -> bool {
            body_part bp = bpid ? ( *bpid ) -> token : num_bp;
            return cr.remove_effect( eff, bp );
        } );

        SET_FX_T( clear_effects, void() );

        SET_FX_T( set_value, void( const std::string &, const std::string & ) );
        SET_FX_T( remove_value, void( const std::string & ) );
        SET_FX_T( get_value, std::string( const std::string & ) const );

        SET_FX_T( get_weight, units::mass() const );

        SET_FX_T( has_trait, bool( const trait_id & ) const );

        SET_FX_T( mod_pain, void( int ) );
        SET_FX_T( mod_pain_noresist, void( int ) );
        SET_FX_T( set_pain, void( int ) );
        SET_FX_T( get_pain, int() const );
        SET_FX_T( get_perceived_pain, int() const );

        SET_FX_T( get_moves, int() const );
        SET_FX_T( mod_moves, void( int ) );
        SET_FX_T( set_moves, void( int ) );

        SET_FX_T( get_num_blocks, int() const );
        SET_FX_T( get_num_dodges, int() const );

        SET_FX_T( get_env_resist, int( bodypart_id ) const );

        SET_FX_T( get_armor_bash, int( bodypart_id ) const );
        SET_FX_T( get_armor_cut, int( bodypart_id ) const );
        SET_FX_T( get_armor_bullet, int( bodypart_id ) const );
        SET_FX_T( get_armor_bash_base, int( bodypart_id ) const );
        SET_FX_T( get_armor_cut_base, int( bodypart_id ) const );
        SET_FX_T( get_armor_bullet_base, int( bodypart_id ) const );
        SET_FX_T( get_armor_bash_bonus, int() const );
        SET_FX_T( get_armor_cut_bonus, int() const );
        SET_FX_T( get_armor_bullet_bonus, int() const );

        SET_FX_T( get_armor_type, int( damage_type, bodypart_id ) const );

        SET_FX_T( get_dodge, float() const );

        SET_FX_T( get_melee, float() const );
        SET_FX_T( get_hit, float() const );

        SET_FX_T( get_speed, int() const );
        SET_FX_T( get_size, m_size() const );
        //SET_FX_T( get_hp, int( const bodypart_id & ) const );
        //SET_FX_T( get_hp, int() const );
        luna::set_fx( ut, "get_hp", []( const Creature & cr,
        sol::optional<const bodypart_id &> bpid ) -> int {
            if( bpid.has_value() )
            {
                return cr.get_hp( *bpid );
            } else
            {
                return cr.get_hp();
            }
        } );
        //SET_FX_T( get_hp_max, int( const bodypart_id & ) const );
        //SET_FX_T( get_hp_max, int() const );
        luna::set_fx( ut, "get_hp_max", []( const Creature & cr,
        sol::optional<const bodypart_id &> bpid ) -> int {
            if( bpid.has_value() )
            {
                return cr.get_hp_max( *bpid );
            } else
            {
                return cr.get_hp_max();
            }
        } );

        SET_FX_T( hp_percentage, int() const );
        //SET_FX_T( made_of, bool( const material_id & ) const );
        //SET_FX_T( made_of_any, bool( const std::set<material_id> & ) const );
        //SET_FX_T( bloodType, field_type_id() const );
        //SET_FX_T( gibType, field_type_id() const );
        SET_FX_T( has_flag, bool( const m_flag ) const );

        //SET_FX_T( uncanny_dodge, bool() );

        //SET_FX_T( get_anatomy, anatomy_id() const );
        //SET_FX_T( set_anatomy, void( anatomy_id ) );

        //SET_FX_T( get_random_body_part, bodypart_id( bool ) const );

        //SET_FX_T( get_all_body_parts, std::vector<bodypart_id>( bool ) const );

        //SET_FX_T( get_body, (const std::map<bodypart_str_id, bodypart> &() const) );
        //SET_FX_T( set_body, void() );
        //SET_FX_T( get_part, bodypart &( const bodypart_id & ) );
        //SET_FX_T( get_part, const bodypart &( const bodypart_id & ) const );

        SET_FX_T( get_part_hp_cur, int( const bodypart_id & ) const );
        SET_FX_T( get_part_hp_max, int( const bodypart_id & ) const );

        SET_FX_T( get_part_healed_total, int( const bodypart_id & ) const );

        SET_FX_T( set_part_hp_cur, void( const bodypart_id &, int ) );
        SET_FX_T( set_part_hp_max, void( const bodypart_id &, int ) );
        //SET_FX_T( set_part_healed_total, void( const bodypart_id &, int ) );
        SET_FX_T( mod_part_hp_cur, void( const bodypart_id &, int ) );
        SET_FX_T( mod_part_hp_max, void( const bodypart_id &, int ) );
        //SET_FX_T( mod_part_healed_total, void( const bodypart_id &, int ) );


        SET_FX_T( set_all_parts_hp_cur, void( int ) );
        SET_FX_T( set_all_parts_hp_to_max, void() );

        SET_FX_T( get_speed_base, int() const );
        SET_FX_T( get_speed_bonus, int() const );
        SET_FX_T( get_speed_mult, float() const );
        SET_FX_T( get_block_bonus, int() const );

        SET_FX_T( get_dodge_base, float() const );
        SET_FX_T( get_hit_base, float() const );
        SET_FX_T( get_dodge_bonus, float() const );
        SET_FX_T( get_hit_bonus, float() const );

        SET_FX_T( has_grab_break_tec, bool() const );

        //SET_FX_T( mod_stat, void( const std::string &, float ) );

        //SET_FX_T( set_num_blocks_bonus, void( int ) );
        //SET_FX_T( mod_num_dodges_bonus, void( int ) );

        //SET_FX_T( set_armor_bash_bonus, void( int ) );
        //SET_FX_T( set_armor_cut_bonus, void( int ) );
        //SET_FX_T( set_armor_bullet_bonus, void( int ) );

        //SET_FX_T( set_speed_base, void( int ) );
        //SET_FX_T( set_speed_bonus, void( int ) );
        //SET_FX_T( set_speed_mult, void( float ) );
        //SET_FX_T( set_block_bonus, void( int ) );

        //SET_FX_T( mod_speed_bonus, void( int ) );
        //SET_FX_T( mod_speed_mult, void( float ) );
        //SET_FX_T( mod_block_bonus, void( int ) );

        //SET_FX_T( set_dodge_bonus, void( float ) );
        //SET_FX_T( set_hit_bonus, void( float ) );

        //SET_FX_T( mod_dodge_bonus, void( float ) );
        //SET_FX_T( mod_hit_bonus, void( float ) );

        luna::set_fx( ut, "get_weight_capacity", []( Creature & cr ) -> std::int64_t {
            return cr.weight_capacity().value();
        } );

        // Can't use SET_FX_T because they're templates,
        // but I don't think these methods are necessary, anyways.
        //SET_FX_T( add_msg_if_player, void( const std::string & ) const );
        //SET_FX_T( add_msg_if_player,
        //void( const game_message_params &, const std::string & ) const );
        //SET_FX_T( add_msg_if_player, void( const translation & ) const );
        //SET_FX_T( add_msg_if_player,
        //void( const game_message_params &, const translation & ) const );

        //SET_FX_T( add_msg_if_npc, void( const std::string & ) const );
        //SET_FX_T( add_msg_if_npc,
        //void( const game_message_params &, const std::string & ) const );
        //SET_FX_T( add_msg_if_npc, void( const translation & ) const );
        //SET_FX_T( add_msg_if_npc,
        //void( const game_message_params &, const translation & ) const );

        //SET_FX_T( add_msg_player_or_npc,
        //void( const std::string &, const std::string & ) const );
        //SET_FX_T( add_msg_player_or_npc,
        //void( const game_message_params &, const std::string &, const std::string & ) const );
        //SET_FX_T( add_msg_player_or_npc,
        //void( const translation &, const translation & ) const );
        //SET_FX_T( add_msg_player_or_npc,
        //void( const game_message_params &, const translation &, const translation & ) const );

        //SET_FX_T( add_msg_player_or_say,
        //void( const std::string &, const std::string & ) const );
        //SET_FX_T( add_msg_player_or_say,
        //void( const game_message_params &, const std::string &, const std::string & ) const );
        //SET_FX_T( add_msg_player_or_say,
        //void( const translation &, const translation & ) const );
        //SET_FX_T( add_msg_player_or_say,
        //void( const game_message_params &, const translation &, const translation & ) const );

        //SET_FX_T( extended_description, std::string() const );

        //SET_FX_T( get_all_effects, effects_map() const );

        //SET_FX_T( select_body_part, body_part( Creature *, int ) const );
    }
#undef UT_CLASS // #define UT_CLASS Creature

#define UT_CLASS monster
    {
        sol::usertype<UT_CLASS> ut =
            luna::new_usertype<UT_CLASS>(
                lua,
                luna::bases<Creature>(),
                luna::no_constructor
            );

        // Members
        SET_MEMB( friendly );
        SET_MEMB( anger );
        SET_MEMB( morale );
        SET_MEMB( faction );
        SET_MEMB( death_drops );
        SET_MEMB( unique_name );

        // Methods
        SET_FX_T( can_upgrade, bool() const );
        SET_FX_T( hasten_upgrade, void() );
        SET_FX_T( get_upgrade_time, int() const );
        SET_FX_T( try_upgrade, void( bool ) );
        SET_FX_T( try_reproduce, void() );
        SET_FX_T( refill_udders, void() );
        SET_FX_T( spawn, void( const tripoint & ) );

        SET_FX_T( name, std::string( unsigned int ) const );
        SET_FX_T( name_with_armor, std::string() const );

        //SET_FX_T( color_with_effects, nc_color() const );

        SET_FX_T( can_see, bool() const );
        SET_FX_T( can_hear, bool() const );
        SET_FX_T( can_submerge, bool() const );
        SET_FX_T( can_drown, bool() const );
        SET_FX_T( can_climb, bool() const );
        SET_FX_T( can_dig, bool() const );
        SET_FX_T( digs, bool() const );
        SET_FX_T( flies, bool() const );
        SET_FX_T( climbs, bool() const );
        SET_FX_T( swims, bool() const );

        SET_FX_T( attitude, monster_attitude( const Character * ) const );

        SET_FX_T( heal, int( int, bool ) );

        SET_FX_T( set_hp, void( int ) );

        SET_FX_T( make_fungus, bool() );
        SET_FX_T( make_friendly, void() );

        SET_FX_T( make_ally, void( const monster & ) );
    }
#undef UT_CLASS // #define UT_CLASS monster

    // Note(AluminumAlman): character bindings to Lua were moved
    // to a separate function because of how large the code snippet is.
    reg_character( lua );

#define UT_CLASS player
    {
        // Note(AluminumAlman): skipping binding members and methods of this class because
        // most of the methods and members are already binded through Character.
        sol::usertype<UT_CLASS> ut =
            luna::new_usertype<UT_CLASS>(
                lua,
                luna::bases<Character, Creature>(),
                luna::no_constructor
            );
    }
#undef UT_CLASS // #define UT_CLASS player

    // Note(AluminumAlman): same reasoning as for reg_character.
    reg_npc( lua );

#define UT_CLASS avatar
    {
        // Note(AluminumAlman): skipping binding members and methods of this class because
        // most of the methods and members are already binded through Character.
        sol::usertype<UT_CLASS> ut =
            luna::new_usertype<UT_CLASS>(
                lua,
                luna::bases<player, Character, Creature>(),
                luna::no_constructor
            );
    }
#undef UT_CLASS // #define UT_CLASS avatar
}

void cata::detail::reg_character( sol::state &lua )
{
#define UT_CLASS Character
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::bases<Creature>(),
            luna::no_constructor
        );
        // These vim commands may not be perfect, but it beats doing everything manually.
        //// Get rid of comments
        //%s@\s*//.*@@g
        //%s@\s*/\*\_.\{-}\*/@@g
        //
        //// Throw all arguments into a single line
        //%s@,\n\s*@, @g
        //
        //// Get rid of large empty lines
        //%s@\n\{3,}@\r\r@g
        //
        //// Get rid of defined overrides
        //%s@.*override {\_.\{-}\n}@@g
        //
        //// Get rid of declared overrides
        //%s@.*override.*@@
        //
        //// Get rid of struct argument defaults
        //%s@ *= *{}@@g
        //
        //// Get rid of defined functions
        //%s@ {\_.\{-}\n}@;@g
        //
        //// Get rid of other newlines in function args
        //%s@\n \+@ @g
        //
        //// Find multi-line function declarations
        //.\+\(;\)\@<!\n
        //
        //// Count functions
        //%s@(@@gn
        //
        //// Move function to end of file
        //%s@\(.*(.\{-}).*;\n*\)\n\(\_.*\)@\2\1
        //
        //// Get rid of default method args
        //%s@\s*=\s*.\{-}\( )\|,\)@\1@g
        //
        //// Get rid of member default values
        //%s@\s*=\s*.\{-};@;@g
        //
        //// Put the members into a macro
        //%s@.\{-}\([^ ]*\);@SET_MEMB( \1 );
        //
        //// Get rid of virtual keyword
        //%s@virtual @@g
        //
        //// Put the functions into a macro
        //%s@\(.\{-}\)\([A-z_]*\)\((\)\{1}\(.*\);@SET_FX_T( \2, \1\3\4 );

        // Members
        //SET_MEMB( death_drops );
        //
        //SET_MEMB( controlling_vehicle );
        //
        //SET_MEMB( str_max );
        //SET_MEMB( dex_max );
        //SET_MEMB( int_max );
        //SET_MEMB( per_max );
        //
        //SET_MEMB( str_cur );
        //SET_MEMB( dex_cur );
        //SET_MEMB( int_cur );
        //SET_MEMB( per_cur );
        //
        //SET_MEMB( blocks_left );
        //SET_MEMB( dodges_left );
        //
        //SET_MEMB( recoil );

        //SET_MEMB( prof );
        //SET_MEMB( custom_profession );

        //SET_MEMB( reach_attacking );
        //
        //SET_MEMB( fleshy );
        //
        //SET_MEMB( magic );

        SET_MEMB( name );
        SET_MEMB( male );

        //SET_MEMB( worn );
        //SET_MEMB( damage_disinfected );
        //SET_MEMB( nv_cached );

        //SET_MEMB( in_vehicle );
        //SET_MEMB( hauling );

        //SET_MEMB( stashed_outbounds_activity );
        //SET_MEMB( stashed_outbounds_backlog );
        //SET_MEMB( activity );
        //SET_MEMB( backlog );
        //SET_MEMB( destination_point );
        //SET_MEMB( inv );
        //SET_MEMB( last_item );

        //SET_MEMB( scent );
        //SET_MEMB( my_bionics );
        //SET_MEMB( martial_arts_data );

        //SET_MEMB( stomach );
        //SET_MEMB( consumption_history );

        //SET_MEMB( oxygen );
        //SET_MEMB( tank_plut );
        //SET_MEMB( reactor_plut );
        //SET_MEMB( slow_rad );

        SET_MEMB( focus_pool );
        SET_MEMB( cash );
        SET_MEMB( follower_ids );

        //SET_MEMB( ammo_location );
        //SET_MEMB( camps );

        //SET_MEMB( cached_time );

        //SET_MEMB( addictions );

        //SET_MEMB( mounted_creature );

        //SET_MEMB( mounted_creature_id );

        //SET_MEMB( activity_vehicle_part_index );

        //SET_MEMB( healed_total );

        SET_MEMB( mutation_category_level );

        //SET_MEMB( omt_path );

        //SET_MEMB( my_mutations );
        //SET_MEMB( last_sleep_check );
        //SET_MEMB( bio_soporific_powered_at_last_sleep_check );

        //SET_MEMB( temp_conv );
        //SET_MEMB( body_wetness );
        //SET_MEMB( drench_capacity );

        //SET_MEMB( next_climate_control_check );
        //SET_MEMB( last_climate_control_ret );

        // Methods
        SET_FX_T( getID, character_id() const );
        //luna::set_fx( ut, "getID", []( const UT_CLASS &charac ) -> int {
        //    return charac.getID().get_value();
        //} );

        SET_FX_T( setID, void( character_id, bool ) );
        //luna::set_fx( ut, "setID", []( UT_CLASS &charac, const int id,
        //const std::optional<bool> force) {
        //    charac.setID( character_id( id ), force ? *force : false );
        //} );

        SET_FX_T( get_str, int() const );
        SET_FX_T( get_dex, int() const );
        SET_FX_T( get_per, int() const );
        SET_FX_T( get_int, int() const );

        SET_FX_T( get_str_base, int() const );
        SET_FX_T( get_dex_base, int() const );
        SET_FX_T( get_per_base, int() const );
        SET_FX_T( get_int_base, int() const );

        SET_FX_T( get_str_bonus, int() const );
        SET_FX_T( get_dex_bonus, int() const );
        SET_FX_T( get_per_bonus, int() const );
        SET_FX_T( get_int_bonus, int() const );

        //SET_FX_T( ranged_dex_mod, int() const );
        //SET_FX_T( ranged_per_mod, int() const );

        SET_FX_T( set_str_bonus, void( int ) );
        SET_FX_T( set_dex_bonus, void( int ) );
        SET_FX_T( set_per_bonus, void( int ) );
        SET_FX_T( set_int_bonus, void( int ) );
        SET_FX_T( mod_str_bonus, void( int ) );
        SET_FX_T( mod_dex_bonus, void( int ) );
        SET_FX_T( mod_per_bonus, void( int ) );
        SET_FX_T( mod_int_bonus, void( int ) );

        //SET_FX_T( print_health, void() const );

        SET_FX_T( get_healthy, int() const );
        SET_FX_T( get_healthy_mod, int() const );

        SET_FX_T( mod_healthy, void( int ) );
        SET_FX_T( mod_healthy_mod, void( int, int ) );

        SET_FX_T( set_healthy, void( int ) );
        SET_FX_T( set_healthy_mod, void( int ) );

        SET_FX_T( get_stored_kcal, int() const );

        SET_FX_T( max_stored_kcal, int() const );
        SET_FX_T( get_kcal_percent, float() const );
        SET_FX_T( get_thirst, int() const );
        //SET_FX_T( get_thirst_description,( std::pair<std::string, nc_color>() const ) );
        //SET_FX_T( get_hunger_description,( std::pair<std::string, nc_color>() const ) );
        //SET_FX_T( get_fatigue_description,( std::pair<std::string, nc_color>() const ) );
        SET_FX_T( get_fatigue, int() const );
        SET_FX_T( get_sleep_deprivation, int() const );

        SET_FX_T( mod_stored_kcal, void( int ) );
        //SET_FX_T( mod_stored_nutr, void( int ) );
        SET_FX_T( mod_thirst, void( int ) );
        SET_FX_T( mod_fatigue, void( int ) );
        SET_FX_T( mod_sleep_deprivation, void( int ) );

        SET_FX_T( set_stored_kcal, void( int ) );
        SET_FX_T( set_thirst, void( int ) );
        SET_FX_T( set_fatigue, void( int ) );
        SET_FX_T( set_sleep_deprivation, void( int ) );

        //SET_FX_T( recalculate_size, void() );

        //SET_FX_T( recalc_speed_bonus, void() );

        //SET_FX_T( get_faction, faction *() const );
        luna::set_fx( ut, "get_faction_id", []( const UT_CLASS & charac ) -> faction_id {
            faction *fac = charac.get_faction();
            return fac == nullptr ? faction_id::NULL_ID() : fac->id;
        } );
        //SET_FX_LN_T( set_fac_id, "set_faction_id", void( const std::string & ) );
        luna::set_fx( ut, "set_faction_id", []( UT_CLASS & charac, faction_id id )
        {
            charac.set_fac_id( id.str() );
        } );

        //SET_FX_T( unimpaired_range, int() const );

        //SET_FX_T( overmap_los, bool( const tripoint_abs_omt &, int ) );

        //SET_FX_LN_T( overmap_sight_range, int( int ) const );

        //SET_FX_T( clairvoyance, int() const );

        SET_FX_T( sight_impaired, bool() const );

        SET_FX_T( has_alarm_clock, bool() const );

        SET_FX_T( has_watch, bool() const );

        //SET_FX_T( action_taken, void() );

        //SET_FX_T( swim_speed, int() const );
        //
        //SET_FX_T( add_miss_reason, void( const std::string &reason, unsigned int weight ) );
        //
        //SET_FX_T( clear_miss_reasons, void() );
        //
        //SET_FX_T( get_miss_reason, std::string() );

        //SET_FX_T( regen, void( int rate_multiplier ) );
        //
        //SET_FX_T( enforce_minimum_healing, void() );
        //
        //SET_FX_T( best_quality_item, item *( const quality_id &qual ) );

        //SET_FX_T( update_health, void( int external_modifiers ) );
        //
        //SET_FX_T( update_body, void() );
        //
        //SET_FX_T( update_body, void( const time_point &from, const time_point &to ) );
        //
        //SET_FX_T( update_stomach, void( const time_point &from, const time_point &to ) );
        //
        //SET_FX_T( update_needs, void( int rate_multiplier ) );
        //SET_FX_T( calc_needs_rates, needs_rates() const );
        //
        //SET_FX_T( check_needs_extremes, void() );

        //SET_FX_T( is_hibernating, bool() const );

        //SET_FX_T( update_bodytemp, void( const map &m, const weather_manager &weather ) );

        //SET_FX_T( temp_equalizer, void( const bodypart_id &bp1, const bodypart_id &bp2 ) );

        SET_FX_T( blood_loss, int( const bodypart_id & bp ) const );

        //SET_FX_T( environmental_revert_effect, void() );

        //SET_FX_T( reset_encumbrance, void() );

        SET_FX_LN_T( encumb, "get_part_encumbrance", int( body_part bp ) const );

        //SET_FX_T( get_encumbrance, char_encumbrance_data() const );
        //
        //SET_FX_T( get_encumbrance, char_encumbrance_data( const item &new_item ) const );
        //
        //SET_FX_T( extraEncumbrance, int( layer_level level, int bp ) const );

        SET_FX_T( is_wearing_power_armor, bool( bool * ) const );

        SET_FX_T( is_wearing_active_power_armor, bool() const );

        SET_FX_T( is_wearing_active_optcloak, bool() const );

        SET_FX_T( in_climate_control, bool() );

        SET_FX_T( is_blind, bool() const );

        SET_FX_T( is_invisible, bool() const );

        //SET_FX_T( visibility, int( bool check_color, int stillness ) const );

        //SET_FX_T( active_light, float() const );

        //SET_FX_T( sees_with_specials, bool( const Creature &critter ) const );

        //SET_FX_T( exclusive_flag_coverage, body_part_set( const std::string &flag ) const );

        //SET_FX_T( wait_effects, void() );

        //SET_FX_T( movement_mode_is, bool( character_movemode mode ) const );
        SET_FX_T( get_movement_mode, character_movemode() const );

        SET_FX_T( set_movement_mode, void( character_movemode ) );

        SET_FX_T( expose_to_disease, void( diseasetype_id ) );

        //SET_FX_T( hardcoded_effects, void( effect &it ) );

        //SET_FX_T( process_items, void() );

        //SET_FX_T( recalc_hp, void() );

        //SET_FX_T( calc_all_parts_hp, void( float hp_mod, float hp_adjust, int str_max ) );

        //SET_FX_T( recalc_sight_limits, void() );

        //SET_FX_T( get_vision_threshold, float( float light_level ) const );

        //SET_FX_T( flag_encumbrance, void() );

        //SET_FX_T( check_item_encumbrance_flag, void() );

        //SET_FX_T( natural_attack_restricted_on, bool( const bodypart_id &bp ) const );

        //SET_FX_T( can_use_grab_break_tec, bool( const item &weap ) const );

        //SET_FX_T( can_miss_recovery, bool( const item &weap ) const );

        SET_FX_T( is_quiet, bool() const );

        SET_FX_T( is_stealthy, bool() const );

        //SET_FX_T( best_shield, item &() );

        //SET_FX_T( handle_melee_wear, bool( item &shield, float wear_multiplier ) );

        //SET_FX_T( pick_technique,
        //matec_id( Creature &t, const item &weap, bool crit,
        //bool dodge_counter, bool block_counter ) );
        //SET_FX_T( perform_technique,
        //void( const ma_technique &technique, Creature &t,
        //damage_instance &di, int &move_cost ) );

        //SET_FX_T( melee_attack,
        //void( Creature &t, bool allow_special, const matec_id *force_technique,
        //bool allow_unarmed ) );

        //SET_FX_T( melee_special_effects,
        //std::string( Creature &t, damage_instance &d, item &weap ) );

        //SET_FX_T( perform_special_attacks,
        //void( Creature &t, dealt_damage_instance &dealt_dam ) );

        //SET_FX_T( reach_attack, void( const tripoint &p ) );

        //SET_FX_T( mutation_attacks, std::vector<special_attack>( Creature &t ) const );

        //SET_FX_T( bonus_damage, float( bool random ) const );

        //SET_FX_T( get_melee_hit_base, float() const );

        //SET_FX_T( crit_chance,
        //double( float roll_hit, float target_dodge, const item &weap ) const );

        //SET_FX_T( scored_crit, bool( float target_dodge, const item &weap ) const );

        //SET_FX_T( attack_cost, int( const item &weap ) const );

        //SET_FX_T( get_hit_weapon, float( const item &weap ) const );
        //
        //SET_FX_T( roll_all_damage,
        //void( bool crit, damage_instance &di, bool average, const item &weap ) const );
        //
        //SET_FX_T( roll_bash_damage,
        //void( bool crit, damage_instance &di, bool average, const item &weap ) const );
        //
        //SET_FX_T( roll_cut_damage,
        //void( bool crit, damage_instance &di, bool average, const item &weap ) const );
        //
        //SET_FX_T( roll_stab_damage,
        //void( bool crit, damage_instance &di, bool average, const item &weap ) const );

        //SET_FX_T( did_hit, void( Creature &target ) );

        //SET_FX_T( reduce_healing_effect,
        //int( const efftype_id &eff_id, int remove_med, const bodypart_id &hurt ) );

        SET_FX_T( cough, void( bool harmful, int loudness ) );

        //SET_FX_T( passive_absorb_hit, void( const bodypart_id &bp, damage_unit &du ) const );

        //SET_FX_T( armor_absorb, bool( damage_unit &du, item &armor ) );

        SET_FX_T( bionic_armor_bonus, float( const bodypart_id &, damage_type ) const );

        SET_FX_T( mabuff_armor_bonus, int( damage_type ) const );

        //SET_FX_T( get_armor_fire,
        //std::map<bodypart_id, int>( const std::map<bodypart_id, std::vector<const item *>> & ) const );

        SET_FX_T( has_base_trait, bool( const trait_id & b ) const );

        SET_FX_T( has_trait_flag, bool( const trait_flag_str_id & b ) const );

        SET_FX_T( has_opposite_trait, bool( const trait_id & flag ) const );

        //SET_FX_T( toggle_trait, void( const trait_id & ) );

        SET_FX_T( set_mutation, void( const trait_id & ) );
        SET_FX_T( unset_mutation, void( const trait_id & ) );

        //SET_FX_T( switch_mutations,
        //void( const trait_id &switched, const trait_id &target, bool start_powered ) );

        //SET_FX_T( activate_mutation, void( const trait_id &mutation ) );
        //SET_FX_T( deactivate_mutation, void( const trait_id &mut ) );

        //SET_FX_T( mutation_spend_resources, void( const trait_id &mut ) );

        //SET_FX_T( bp_to_hp, static hp_part( body_part bp ) );

        //SET_FX_T( hp_to_bp, static body_part( hp_part hpart ) );

        SET_FX_T( can_mount, bool( const monster & critter ) const );
        SET_FX_T( mount_creature, void( monster & z ) );
        SET_FX_T( is_mounted, bool() const );
        SET_FX_T( check_mount_will_move, bool( const tripoint & dest_loc ) );
        SET_FX_T( check_mount_is_spooked, bool() );
        SET_FX_T( dismount, void() );
        SET_FX_T( forced_dismount, void() );

        SET_FX_T( is_deaf, bool() const );

        SET_FX_T( has_two_arms, bool() const );

        SET_FX_T( get_working_arm_count, int() const );

        SET_FX_T( get_working_leg_count, int() const );

        SET_FX_T( is_limb_disabled, bool( const bodypart_id & ) const );

        // This function is declared, but it isn't defined. It compiles, but it fails to link with no error.
        // Lovely.
        //SET_FX_T( is_limb_hindered, bool( hp_part limb ) const );

        SET_FX_T( is_limb_broken, bool( const bodypart_id & ) const );

        SET_FX_T( can_run, bool() );

        SET_FX_T( hurtall, void( int dam, Creature * source, bool ) );

        SET_FX_T( hitall, int( int dam, int vary, Creature * source ) );

        //SET_FX_T( on_hurt, void( Creature *source, bool disturb ) );

        SET_FX_T( heal, void( const bodypart_id &, int ) );

        SET_FX_T( healall, void( int dam ) );

        //SET_FX_T( body_window,
        //hp_part( const std::string &menu_header, bool show_all, bool precise,
        //int normal_bonus, int head_bonus, int torso_bonus,
        //float bleed, float bite, float infect,
        //float bandage_power, float disinfectant_power ) const );

        //SET_FX_T( limb_color,
        //nc_color( const bodypart_id &bp, bool bleed, bool bite, bool infect ) const );

        //SET_FX_T( setx, inline void( int x ) );
        //SET_FX_T( sety, inline void( int y ) );
        //SET_FX_T( setz, inline void( int z ) );

        SET_FX_T( global_square_location, tripoint() const );

        SET_FX_T( global_sm_location, tripoint() const );

        SET_FX_T( global_omt_location, tripoint_abs_omt() const );

        //SET_FX_T( recalculate_enchantment_cache, void() );
        //SET_FX_T( rebuild_mutation_cache, void() );

        //SET_FX_T( bonus_from_enchantments,
        //double( double base, enchant_vals::mod value, bool round ) const );

        SET_FX_T( has_mabuff, bool( const mabuff_id & ) const );

        SET_FX_T( mabuff_tohit_bonus, float() const );

        SET_FX_T( mabuff_dodge_bonus, float() const );

        SET_FX_T( mabuff_block_bonus, int() const );

        SET_FX_T( mabuff_speed_bonus, int() const );

        SET_FX_T( mabuff_arpen_bonus, int( damage_type ) const );

        SET_FX_T( mabuff_damage_mult, float( damage_type ) const );

        SET_FX_T( mabuff_damage_bonus, int( damage_type ) const );

        SET_FX_T( mabuff_attack_cost_penalty, int() const );

        SET_FX_T( mabuff_attack_cost_mult, float() const );

        SET_FX_T( mutation_effect, void( const trait_id & ) );

        SET_FX_T( mutation_loss_effect, void( const trait_id & ) );

        SET_FX_T( has_active_mutation, bool( const trait_id & ) const );

        SET_FX_T( mutate, void() );

        SET_FX_T( mutation_ok, bool( const trait_id &, bool, bool ) const );

        SET_FX_T( mutate_category, void( const mutation_category_id & ) );

        SET_FX_T( mutate_towards, bool( std::vector<trait_id>, int ) );

        SET_FX_T( mutate_towards, bool( const trait_id & ) );

        SET_FX_T( remove_mutation, void( const trait_id &, bool ) );

        //SET_FX_T( mutation_chances,( std::map<trait_id, float>() const ) );

        SET_FX_T( has_child_flag, bool( const trait_id & flag ) const );

        SET_FX_T( remove_child_flag, void( const trait_id & flag ) );

        //SET_FX_T( set_highest_cat_level, void() );

        SET_FX_T( get_highest_category, mutation_category_id() const );

        //SET_FX_T( drench_mut_calc, void() );

        //SET_FX_T( build_mut_dependency_map,
        //void( const trait_id &mut, std::unordered_map<trait_id, int> &dependency_map,
        //int distance ) );

        //SET_FX_T( is_category_allowed, bool( const std::vector<std::string> &category ) const );
        //SET_FX_T( is_category_allowed, bool( const std::string &category ) const );

        SET_FX_T( is_weak_to_water, bool() const );

        //SET_FX_T( can_use_heal_item, bool( const item &med ) const );

        //SET_FX_T( can_install_cbm_on_bp, bool( const std::vector<bodypart_id> &bps ) const );

        //SET_FX_T( mutation_armor, resistances( bodypart_id bp ) const );
        SET_FX_T( mutation_armor, float( bodypart_id, damage_type ) const );
        //SET_FX_T( mutation_armor, float( bodypart_id bp, const damage_unit &du ) const );

        //SET_FX_T( activate_bionic, bool( bionic &bio, bool eff_only ) );
        SET_FX_T( get_bionics, std::vector<bionic_id>() const );

        //SET_FX_T( get_bionic_state, bionic &( const bionic_id &id ) );

        //SET_FX_T( amount_of_storage_bionics, std::pair<int, int>() const );

        SET_FX_T( has_bionic, bool( const bionic_id & b ) const );

        SET_FX_T( has_active_bionic, bool( const bionic_id & b ) const );

        SET_FX_T( has_any_bionic, bool() const );

        //SET_FX_T( can_fuel_bionic_with, bool( const item &it ) const );

        //SET_FX_T( get_bionic_fueled_with, std::vector<bionic_id>( const item &it ) const );
        //
        //SET_FX_T( get_fueled_bionics, std::vector<bionic_id>() const );
        //
        //SET_FX_T( get_remote_fueled_bionic, bionic_id() const );
        //
        //SET_FX_T( get_most_efficient_bionic,
        //bionic_id( const std::vector<bionic_id> &bids ) const );
        //
        //SET_FX_T( get_fuel_available, std::vector<itype_id>( const bionic_id &bio ) const );
        //
        //SET_FX_T( get_fuel_type_available, int( const itype_id &fuel ) const );
        //
        //SET_FX_T( get_fuel_capacity, int( const itype_id &fuel ) const );
        //
        //SET_FX_T( get_total_fuel_capacity, int( const itype_id &fuel ) const );
        //
        //SET_FX_T( update_fuel_storage, void( const itype_id &fuel ) );

        //SET_FX_T( get_mod_stat_from_bionic, int( const character_stat &Stat ) const );

        //SET_FX_T( process_bionic, void( bionic &bio ) );

        //SET_FX_T( deactivate_bionic, bool( bionic &bio, bool eff_only ) );

        SET_FX_T( has_bionics, bool() const );

        SET_FX_T( clear_bionics, void() );
        SET_FX_T( get_used_bionics_slots, int( const bodypart_id & ) const );
        SET_FX_T( get_total_bionics_slots, int( const bodypart_id & ) const );
        SET_FX_T( get_free_bionics_slots, int( const bodypart_id & ) const );

        //SET_FX_T( has_enough_anesth, bool( const itype *cbm, player &patient ) );

        //SET_FX_T( introduce_into_anesthesia,
        //void( const time_duration &duration, player &installer, bool needs_anesthesia ) );

        SET_FX_T( remove_bionic, void( const bionic_id & ) );

        SET_FX_T( add_bionic, void( const bionic_id & ) );

        //SET_FX_T( env_surgery_bonus, float( int radius ) );

        //SET_FX_T( bionics_adjusted_skill,
        //float( const skill_id &most_important_skill, const skill_id &important_skill,
        //const skill_id &least_important_skill, int skill_level ) );

        //SET_FX_T( bionics_pl_skill,
        //int( const skill_id &most_important_skill, const skill_id &important_skill,
        //const skill_id &least_important_skill, int skill_level ) );

        //SET_FX_T( can_install_bionics,
        //bool( const itype &type, player &installer, bool autodoc, int skill_level ) );
        //SET_FX_T( bionic_installation_issues,
        //std::map<bodypart_id, int>( const bionic_id &bioid ) const );
        //
        //SET_FX_T( install_bionics,
        //bool( const itype &type, player &installer, bool autodoc, int skill_level ) );
        //
        //SET_FX_T( perform_install,
        //void( bionic_id bid, bionic_id upbid, int difficulty, int success, int pl_skill,
        //const std::string &installer_name, const std::vector<trait_id> &trait_to_rem ) );
        //SET_FX_T( do_damage_for_bionic_failure, void( int min_damage, int max_damage ) );
        //SET_FX_T( bionics_install_failure,
        //void( const std::string &installer, int difficulty, int success,
        //float adjusted_skill ) );
        //
        //SET_FX_T( can_uninstall_bionic,
        //bool( const bionic_id &b_id, player &installer, bool autodoc, int skill_level ) );
        //
        //SET_FX_T( uninstall_bionic,
        //bool( const bionic_id &b_id, player &installer, bool autodoc, int skill_level ) );
        //
        //SET_FX_T( perform_uninstall,
        //void( bionic_id bid, int difficulty, int success,
        //const units::energy &power_lvl, int pl_skill ) );
        //
        //SET_FX_T( bionics_uninstall_failure,
        //void( int difficulty, int success, float adjusted_skill ) );
        //
        //SET_FX_T( uninstall_bionic,
        //bool( const bionic &target_cbm, monster &installer,
        //player &patient, float adjusted_skill ) );
        //
        //SET_FX_T( bionics_uninstall_failure,
        //void( monster &installer, player &patient, int difficulty, int success,
        //float adjusted_skill ) );
        //
        //SET_FX_T( burn_fuel, bool( bionic &bio, bool start ) );
        //
        //SET_FX_T( passive_power_gen, void( bionic &bio ) );
        //
        //SET_FX_T( find_remote_fuel, itype_id( bool look_only ) );
        //
        //SET_FX_T( consume_remote_fuel, int( int amount ) );
        //SET_FX_T( reset_remote_fuel, void() );

        //SET_FX_T( heat_emission, void( bionic &bio, int fuel_energy ) );

        //SET_FX_T( get_effective_efficiency, float( bionic &bio, float fuel_efficiency ) );

        SET_FX_T( get_power_level, units::energy() const );
        SET_FX_T( get_max_power_level, units::energy() const );
        SET_FX_T( mod_power_level, void( const units::energy & ) );
        SET_FX_T( mod_max_power_level, void( const units::energy & ) );
        SET_FX_T( set_power_level, void( const units::energy & ) );
        SET_FX_T( set_max_power_level, void( const units::energy & ) );
        SET_FX_T( is_max_power, bool() const );
        SET_FX_T( has_power, bool() const );
        SET_FX_T( has_max_power, bool() const );
        //SET_FX_T( enough_power_for, bool( const bionic_id &bid ) const );
        //SET_FX_T( conduct_blood_analysis, void() const );

        //SET_FX_T( worn_position_to_index, static int( int position ) );

        SET_FX_T( is_worn, bool( const item & ) const );

        //SET_FX_T( invoke_item, bool( item *, const tripoint &pt ) );
        //
        //SET_FX_T( invoke_item, bool( item *, const std::string &, const tripoint &pt ) );
        //
        //SET_FX_T( invoke_item, bool( item * ) );
        //SET_FX_T( invoke_item, bool( item *, const std::string & ) );
        //
        //SET_FX_T( dispose_item, bool( item_location &&obj, const std::string &prompt ) );
        //
        //SET_FX_T( has_enough_charges, bool( const item &it, bool show_msg ) const );
        //
        //SET_FX_T( consume_charges, bool( item &used, int qty ) );
        //
        //SET_FX_T( item_handling_cost,
        //int( const item &it, bool penalties, int base_cost ) const );
        //
        //SET_FX_T( item_store_cost,
        //int( const item &it, const item &container, bool penalties, int base_cost ) const );
        //
        //SET_FX_T( item_wear_cost, int( const item &it ) const );
        //
        //SET_FX_T( amount_worn, int( const itype_id &id ) const );

        //SET_FX_T( nearby,
        //std::vector<item_location>( const std::function<bool( const item *, const item * )> &func,
        //int radius ) const );

        //SET_FX_T( remove_worn_items_with,
        //std::list<item>( std::function<bool( item & )> filter ) );

        //SET_FX_T( invlet_to_item, item *( int invlet ) );

        //SET_FX_T( i_at, item &( int position ) );
        //SET_FX_T( i_at, const item &( int position ) const );

        //SET_FX_T( get_item_position, int( const item *it ) const );
        //
        //SET_FX_T( wielded_items, std::vector<item *>() );
        //SET_FX_T( wielded_items, std::vector<const item *>() const );
        //
        //SET_FX_T( used_weapon, item &() );
        //SET_FX_T( used_weapon, const item &() const );
        //
        //SET_FX_T( primary_weapon, item &() );
        //SET_FX_T( primary_weapon, const item &() const );
        //
        //SET_FX_T( set_primary_weapon, void( const item &new_weapon ) );

        //SET_FX_T( i_add_to_container, int( const item &it, bool unloading ) );
        //SET_FX_T( i_add, item &( item it, bool should_stack ) );

        //SET_FX_T( pour_into, bool( item &container, item &liquid ) );
        //SET_FX_T( pour_into, bool( vehicle &veh, item &liquid ) );
        //
        //SET_FX_T( i_rem, item( int pos ) );
        //
        //SET_FX_T( i_rem, item( const item *it ) );
        //SET_FX_T( i_rem_keep_contents, void( int idx ) );
        //
        //SET_FX_T( i_add_or_drop, bool( item &it, int qty ) );

        //SET_FX_T( max, std::bitset<std::numeric_limits<char>::()> allocated_invlets() const );

        //SET_FX_T( has_active_item, bool( const itype_id &id ) const );
        //SET_FX_T( remove_weapon, item() );
        //SET_FX_T( has_mission_item, bool( int mission_id ) const );
        //SET_FX_T( remove_mission_items, void( int mission_id ) );

        //SET_FX_T( throw_range, int( const item & ) const );

        //SET_FX_T( unarmed_attack, bool() const );

        //SET_FX_T( best_nearby_lifting_assist, int() const );

        //SET_FX_T( best_nearby_lifting_assist, int( const tripoint &world_pos ) const );

        //SET_FX_T( inv_dump, std::vector<item *>() );

        SET_FX_T( weight_carried, units::mass() const );
        SET_FX_T( volume_carried, units::volume() const );

        //SET_FX_T( weight_carried_reduced_by,
        //units::mass( const excluded_stacks &without ) const );
        //SET_FX_T( volume_carried_reduced_by,
        //units::volume( const excluded_stacks &without ) const );

        SET_FX_T( volume_capacity, units::volume() const );
        //SET_FX_T( volume_capacity_reduced_by,
        //units::volume( const units::volume &mod, const excluded_stacks &without ) const );


        //SET_FX_T( can_pick_volume, bool( const item &it ) const );
        SET_FX_T( can_pick_volume, bool( units::volume ) const );
        //SET_FX_T( can_pick_weight, bool( const item &it, bool safe ) const );
        SET_FX_T( can_pick_weight, bool( units::mass, bool ) const );

        //SET_FX_T( can_use, bool( const item &it, const item &context ) const );

        //SET_FX_T( can_wear, ret_val<bool>( const item &it, bool with_equip_change ) const );

        //SET_FX_T( wear_possessed,
        //std::optional<std::list<item>::iterator>( item &to_wear, bool interactive ) );

        //SET_FX_T( wear_item,
        //std::optional<std::list<item>::iterator>( const item &to_wear, bool interactive ) );

        //SET_FX_T( can_takeoff,
        //ret_val<bool>( const item &it, const std::list<item> *res ) const );


        //SET_FX_T( takeoff, bool( item &it, std::list<item> *res ) );

        SET_FX_T( is_armed, bool() const );

        // TODO: wrap me
        //SET_FX_T( can_wield, ret_val<bool>( const item & ) const );

        SET_FX_T( wield, bool( item & target ) );

        // TODO: wrap me
        //SET_FX_T( can_unwield, ret_val<bool>( const item & ) const );

        SET_FX_T( unwield, bool() );

        //SET_FX_T( can_swap, ret_val<bool>( const item &it ) const );

        //SET_FX_T( drop_invalid_inventory, void() );

        //SET_FX_T( get_dependent_worn_items, std::list<item *>( const item &it ) const );

        //SET_FX_T( drop, void( item_location loc, const tripoint &where ) );
        //SET_FX_T( drop, void( const drop_locations &what, const tripoint &target, bool stash ) );

        //SET_FX_T( has_artifact_with, bool( art_effect_passive effect ) const );

        SET_FX_T( is_wielding, bool( const item & ) const );

        //SET_FX_T( covered_with_flag,
        //bool( const std::string &flag, const body_part_set &parts ) const );
        //SET_FX_T( is_waterproof, bool( const body_part_set &parts ) const );

        //SET_FX_T( leak_level, int( const std::string &flag ) const );

        //SET_FX_T( can_reload, bool( const item &it, const itype_id &ammo ) const );

        //SET_FX_T( item_reload_cost, int( const item &it, const item &ammo, int qty ) const );

        SET_FX_T( is_wearing, bool( const item & ) const );

        //SET_FX_T( is_wearing, bool( const itype_id & ) const );

        SET_FX_T( is_wearing_on_bp, bool( const itype_id &, const bodypart_id & ) const );

        SET_FX_T( worn_with_flag, bool( const flag_id &, const bodypart_id & ) const );

        SET_FX_T( item_worn_with_flag,
                  const item * ( const flag_id &, const bodypart_id & ) const );

        //SET_FX_T( get_overlay_ids, std::vector<std::string>() const );

        SET_FX_T( get_skill_level, int( const skill_id & ) const );
        //SET_FX_T( get_skill_level, int( const skill_id &, const item &context ) const );

        // TODO: bind SkillLevelMap and uncomment
        //SET_FX_T( get_all_skills, const SkillLevelMap &() const );
        //SET_FX_T( get_skill_level_object, SkillLevel &( const skill_id &ident ) );

        //SET_FX_T( get_skill_level_object, const SkillLevel &( const skill_id &ident ) const );

        SET_FX_T( set_skill_level, void( const skill_id &, int ) );
        SET_FX_T( mod_skill_level, void( const skill_id &, int ) );

        //SET_FX_T( meets_skill_requirements,
        //bool( const std::map<skill_id, int> &req, const item &context ) const );
        //
        //SET_FX_T( meets_skill_requirements, bool( const construction &con ) const );
        //
        //SET_FX_T( meets_stat_requirements, bool( const item &it ) const );
        //
        //SET_FX_T( meets_requirements, bool( const item &it, const item &context ) const );
        //
        //SET_FX_T( enumerate_unmet_requirements,
        //std::string( const item &it, const item &context ) const );

        SET_FX_T( rust_rate, int() const );

        SET_FX_T( practice, void( const skill_id &, int, int, bool ) );

        SET_FX_T( read_speed, int( bool ) const );

        SET_FX_T( get_time_died, time_point() const );

        //SET_FX_T( set_time_died, void( const time_point &time ) );

        // TODO: Maybe uncomment this? Though isn't there already a query?
        //SET_FX_T( query_yn,( bool( const std::string & ) const ) );

        SET_FX_T( is_rad_immune, bool() const );

        SET_FX_T( is_throw_immune, bool() const );

        SET_FX_T( rest_quality, float() const );

        SET_FX_T( healing_rate, float( float ) const );

        SET_FX_T( healing_rate_medicine, float( float, const bodypart_id & ) const );

        SET_FX_T( mutation_value, float( const std::string & ) const );

        //SET_FX_T( get_mutation_social_mods, social_modifiers() const );

        //SET_FX_T( pick_name, void( bool bUseDefault ) );

        SET_FX_T( get_base_traits, std::vector<trait_id>() const );

        SET_FX_T( get_mutations, std::vector<trait_id>( bool ) const );

        //SET_FX_T( get_vision_modes, const std::bitset<NUM_VISION_MODES> &() const );

        SET_FX_T( clear_skills, void() );

        SET_FX_T( clear_mutations, void() );

        SET_FX_T( crossed_threshold, bool() const );

        SET_FX_T( add_addiction, void( add_type, int ) );
        SET_FX_T( rem_addiction, void( add_type ) );
        SET_FX_T( has_addiction, bool( add_type ) const );
        SET_FX_T( addiction_level, int( add_type ) const );

        //SET_FX_T( start_hauling, void() );
        //SET_FX_T( stop_hauling, void() );
        SET_FX_T( is_hauling, bool() const );

        SET_FX_T( has_item_with_flag, bool( const flag_id & flag, bool need_charges ) const );
        SET_FX_T( all_items_with_flag,
                  std::vector<item *>( const flag_id & flag ) const );

        //SET_FX_T( has_charges,
        //bool( const itype_id &it, int quantity,
        //const std::function<bool( const item & )> &filter ) const );
        //
        //SET_FX_T( use_amount,
        //std::list<item>( itype_id it, int quantity,
        //const std::function<bool( const item & )> &filter ) );/
        //
        //SET_FX_T( use_charges_if_avail, bool( const itype_id &it, int quantity ) );
        //
        //SET_FX_T( use_charges,
        //std::list<item>( const itype_id &what, int qty,
        //const std::function<bool( const item & )> &filter ) );

        //SET_FX_T( has_fire, bool( int quantity ) const );
        //SET_FX_T( use_fire, void( int quantity ) );
        //SET_FX_T( assign_stashed_activity, void() );
        //SET_FX_T( check_outbounds_activity, bool( const player_activity &, bool ) );

        SET_FX_T( assign_activity,
                  void( const activity_id &, int, int, int, const std::string & ) );
        //SET_FX_T( assign_activity, void( const player_activity &act, bool allow_resume ) );

        SET_FX_T( has_activity, bool( const activity_id & type ) const );
        //SET_FX_T( has_activity, bool( const std::vector<activity_id> &types ) const );

        //SET_FX_T( resume_backlog_activity, void() );
        SET_FX_T( cancel_activity, void() );
        //SET_FX_T( cancel_stashed_activity, void() );
        //SET_FX_T( get_stashed_activity, player_activity() const );
        //SET_FX_T( set_stashed_activity,
        //void( const player_activity &act, const player_activity &act_back ) );
        //SET_FX_T( has_stashed_activity, bool() const );
        //SET_FX_T( initialize_stomach_contents, void() );

        //SET_FX_T( metabolic_rate_base, float() const );

        SET_FX_T( metabolic_rate, float() const );

        //SET_FX_T( get_weight_string, std::string() const );

        //SET_FX_T( get_max_healthy, int() const );

        //SET_FX_T( bmi, float() const );

        //SET_FX_T( bmr, int() const );

        //SET_FX_T( reset_chargen_attributes, void() );

        SET_FX_T( base_age, int() const );
        SET_FX_T( set_base_age, void( int ) );
        SET_FX_T( mod_base_age, void( int ) );

        SET_FX_T( age, int() const );
        //SET_FX_T( age_string, std::string() const );

        SET_FX_T( base_height, int() const );
        SET_FX_T( set_base_height, void( int ) );
        SET_FX_T( mod_base_height, void( int ) );
        //SET_FX_T( height_string, std::string() const );

        SET_FX_T( height, int() const );

        SET_FX_T( bodyweight, units::mass() const );

        SET_FX_T( bionics_weight, units::mass() const );

        SET_FX_T( get_armor_acid, int( bodypart_id bp ) const );

        //SET_FX_T( get_all_armor_type,
        //std::map<bodypart_id, int>( damage_type dt, const std::map<bodypart_id,
        //std::vector<const item *>> &clothing_map ) const );

        SET_FX_T( get_stim, int() const );
        SET_FX_T( set_stim, void( int ) );
        SET_FX_T( mod_stim, void( int ) );

        SET_FX_T( get_rad, int() const );
        SET_FX_T( set_rad, void( int ) );
        SET_FX_T( mod_rad, void( int ) );

        SET_FX_T( get_stamina, int() const );
        SET_FX_T( get_stamina_max, int() const );
        SET_FX_T( set_stamina, void( int ) );
        SET_FX_T( mod_stamina, void( int ) );
        //SET_FX_T( burn_move_stamina, void( int moves ) );
        //SET_FX_T( stamina_move_cost_modifier, float() const );

        //SET_FX_T( update_stamina, void( int turns ) );

        //SET_FX_T( on_item_wear, void( const item &it ) );
        //
        //SET_FX_T( on_item_takeoff, void( const item &it ) );
        //
        //SET_FX_T( on_worn_item_washed, void( const item &it ) );
        //
        //SET_FX_T( on_mutation_gain, void( const trait_id &mid ) );
        //
        //SET_FX_T( on_mutation_loss, void( const trait_id &mid ) );
        //
        //SET_FX_T( on_worn_item_transform, void( const item &old_it, const item &new_it ) );

        SET_FX_T( wake_up, void() );

        SET_FX_T( get_shout_volume, int() const );

        SET_FX_T( shout, void( std::string, bool ) );

        SET_FX_T( vomit, void() );

        //SET_FX_T( healed_bp, void( int bp, int amount ) );

        //SET_FX_T( adjust_for_focus, int( int amount ) const );
        //SET_FX_T( update_type_of_scent, void( bool init ) );
        //SET_FX_T( update_type_of_scent, void( const trait_id &mut, bool gain ) );
        //SET_FX_T( set_type_of_scent, void( const scenttype_id &id ) );
        //SET_FX_T( get_type_of_scent, scenttype_id() const );

        SET_FX_T( restore_scent, void() );

        SET_FX_T( mod_painkiller, void( int ) );

        SET_FX_T( set_painkiller, void( int ) );

        SET_FX_T( get_painkiller, int() const );
        //SET_FX_T( react_to_felt_pain, void( int ) );

        SET_FX_T( spores, void() );
        SET_FX_T( blossoms, void() );

        //SET_FX_T( rooted_message, void() const );
        SET_FX_T( rooted, void() );

        SET_FX_T( fall_asleep, void() );
        SET_FX_T( fall_asleep, void( const time_duration & duration ) );

        //SET_FX_T( is_snuggling, std::string() const );

        //SET_FX_T( item_with_best_of_quality, item &( const quality_id &qid ) );

        //SET_FX_T( sees_with_infrared, bool( const Creature & ) const );

        //SET_FX_T( place_corpse, void() );

        //SET_FX_T( place_corpse, void( const tripoint_abs_omt &om_target ) );

        //SET_FX_T( run_cost, int( int base_cost, bool diag ) const );

        SET_FX_T( get_hostile_creatures, std::vector<Creature *>( int ) const );

        SET_FX_T( get_visible_creatures, std::vector<Creature *>( int ) const );

        //SET_FX_T( visible_mutations, std::string( int visibility_cap ) const );
        //SET_FX_T( get_destination_activity, player_activity() const );
        //SET_FX_T( set_destination_activity,
        //void( const player_activity &new_destination_activity ) );
        //SET_FX_T( clear_destination_activity, void() );
        //
        //SET_FX_T( warmth,
        //std::map<bodypart_id, int>( const std::map<bodypart_id, std::vector<const item *>> & ) const );
        //
        //SET_FX_T( can_use_floor_warmth, bool() const );
        //
        //SET_FX_T( floor_bedding_warmth, static int( const tripoint &pos ) );
        //
        //SET_FX_T( floor_item_warmth, static int( const tripoint &pos ) );
        //
        //SET_FX_T( floor_warmth, int( const tripoint &pos ) const );
        //
        //SET_FX_T( bodytemp_modifier_traits, int( bool overheated ) const );
        //
        //SET_FX_T( bodytemp_modifier_traits_floor, int() const );
        //
        //SET_FX_T( temp_corrected_by_climate_control, int( int temperature ) const );
        //
        //SET_FX_T( update_vitamins, void( const vitamin_id &vit ) );
        //
        //SET_FX_T( vitamin_get, int( const vitamin_id &vit ) const );
        //
        //SET_FX_T( vitamin_set, bool( const vitamin_id &vit, int qty ) );
        //
        //SET_FX_T( vitamin_mod, int( const vitamin_id &vit, int qty, bool capped ) );
        //SET_FX_T( vitamins_mod, void( const std::map<vitamin_id, int> &, bool capped ) );
        //
        //SET_FX_T( vitamin_rate, time_duration( const vitamin_id &vit ) const );

        //SET_FX_T( nutrition_for, int( const item &comest ) const );
        //
        //SET_FX_T( can_eat, ret_val<edible_rating>( const item &food ) const );
        //
        //SET_FX_T( will_eat, ret_val<edible_rating>( const item &food, bool interactive ) const );
        //
        //SET_FX_T( can_feed_furnace_with, bool( const item &it ) const );
        //SET_FX_T( get_cbm_rechargeable_with, rechargeable_cbm( const item &it ) const );
        //SET_FX_T( get_acquirable_energy, int( const item &it, rechargeable_cbm cbm ) const );
        //SET_FX_T( get_acquirable_energy, int( const item &it ) const );
        //
        //SET_FX_T( feed_furnace_with, bool( item &it ) );
        //SET_FX_T( fuel_bionic_with, bool( item &it ) );
        //
        //SET_FX_T( modify_stimulation, void( const islot_comestible &comest ) );
        //
        //SET_FX_T( modify_fatigue, void( const islot_comestible &comest ) );
        //
        //SET_FX_T( modify_radiation, void( const islot_comestible &comest ) );
        //
        //SET_FX_T( modify_addiction, void( const islot_comestible &comest ) );
        //
        //SET_FX_T( modify_health, void( const islot_comestible &comest ) );
        //
        //SET_FX_T( consume_effects, bool( item &food ) );
        //
        //SET_FX_T( can_consume, bool( const item &it ) const );
        //
        //SET_FX_T( can_estimate_rot, bool() const );
        //
        //SET_FX_T( can_consume_as_is, bool( const item &it ) const );
        //SET_FX_T( can_consume_for_bionic, bool( const item &it ) const );
        //
        //SET_FX_T( get_consumable_from, item &( item &it ) const );
        //
        //SET_FX_T( consume, void( item_location loc ) );
        //
        //SET_FX_T( consume_item, bool( item &target ) );
        //
        //SET_FX_T( consume_med, bool( item &target ) );
        //
        //SET_FX_T( eat, bool( item &food, bool force ) );
        //
        //SET_FX_T( compute_nutrient_range,
        //std::pair<nutrients, nutrients>( const item &, const recipe_id &,
        //const cata::flat_set<std::string> &extra_flags ) const );
        //
        //SET_FX_T( compute_nutrient_range,
        //std::pair<nutrients, nutrients>( const itype_id &,
        //const cata::flat_set<std::string> &extra_flags ) const );
        //
        //SET_FX_T( allergy_type, morale_type( const item &food ) const );
        //SET_FX_T( compute_effective_nutrients, nutrients( const item & ) const );

        SET_FX_T( wearing_something_on, bool( const bodypart_id & ) const );

        SET_FX_T( is_wearing_helmet, bool() const );

        //SET_FX_T( head_cloth_encumbrance, int() const );

        //SET_FX_T( armwear_factor, double() const );

        //SET_FX_T( shoe_type_count, int( const itype_id &it ) const );

        //SET_FX_T( footwear_factor, double() const );

        //SET_FX_T( is_wearing_shoes, bool( const side &which_side ) const );

        //SET_FX_T( change_side, bool( item &it, bool interactive ) );
        //SET_FX_T( change_side, bool( item_location &loc, bool interactive ) );

        //SET_FX_T( get_check_encumbrance, bool() );
        //SET_FX_T( set_check_encumbrance, void( bool new_check ) );

        //SET_FX_T( update_morale, void() );

        //SET_FX_T( apply_persistent_morale, void() );

        //SET_FX_T( modify_morale, void( item &food, int nutr ) );

        SET_FX_T( get_morale_level, int() const );
        SET_FX_T( add_morale,
                  void( const morale_type &, int, int, const time_duration &, const time_duration &,
                        bool, const itype * ) );
        SET_FX_T( has_morale, bool( const morale_type & ) const );
        SET_FX_T( get_morale, int( const morale_type & ) const );
        SET_FX_T( rem_morale, void( const morale_type & ) );
        SET_FX_T( clear_morale, void() );
        SET_FX_T( has_morale_to_read, bool() const );
        SET_FX_T( has_morale_to_craft, bool() const );
        //SET_FX_T( crafting_inventory, const inventory &( bool clear_path ) );
        //SET_FX_T( crafting_inventory,
        //const inventory &( const tripoint &src_pos, int radius, bool clear_path ) );
        //SET_FX_T( invalidate_crafting_inventory, void() );

        // TODO: uncomment and fix me
        //SET_FX_T( get_learned_recipes, const recipe_subset &() const );
        //SET_FX_T( knows_recipe, bool( const recipe *rec ) const );
        //SET_FX_T( learn_recipe, void( const recipe *rec ) );
        //SET_FX_T( can_learn_by_disassembly, bool( const recipe &rec ) const );

        //SET_FX_T( check_and_recover_morale, bool() );

        //SET_FX_T( fun_for, std::pair<int, int>( const item &comest ) const );

        SET_FX_T( suffer, void() );

        SET_FX_T( irradiate, bool( float rads, bool bypass ) );

        //SET_FX_T( sound_hallu, void() );

        SET_FX_T( drench, void( int saturation, const body_part_set & flags, bool ignore_waterproof ) );

        //SET_FX_T( apply_wetness_morale, void( int temperature ) );
        //SET_FX_T( short_description_parts, std::vector<std::string>() const );
        //SET_FX_T( short_description, std::string() const );

        SET_FX_T( can_hear, bool( const tripoint & source, int volume ) const );

        SET_FX_T( hearing_ability, float() const );

        //SET_FX_T( knows_trap, bool( const tripoint &pos ) const );
        //SET_FX_T( add_known_trap, void( const tripoint &pos, const trap &t ) );

        //SET_FX_T( bodytemp_color, nc_color( int bp ) const );

        SET_FX_T( get_lowest_hp, int() const );

        //SET_FX_T( shift_destination, void( point shift ) );
        //
        //SET_FX_T( set_destination,
        //void( const std::vector<tripoint> &route, const player_activity &new_destination_activity ) );
        //SET_FX_T( clear_destination, void() );
        //SET_FX_T( has_distant_destination, bool() const );

        //SET_FX_T( is_auto_moving, bool() const );

        //SET_FX_T( has_destination, bool() const );

        //SET_FX_T( has_destination_activity, bool() const );

        //SET_FX_T( start_destination_activity, void() );
        //SET_FX_T( get_auto_move_route, std::vector<tripoint> &() );
        //SET_FX_T( get_next_auto_move_direction, action_id() );
        //SET_FX_T( defer_move, bool( const tripoint &next ) );

        // Note(AluminumAlman): I'm too stupid to deal with templates in macros; using SET_FX
        //SET_FX_LN_T(( bodypart_exposure<bodypart_id, float> ), "bodypart_exposure",
        //( std::map<bodypart_id, float>() ) );
        SET_FX( bodypart_exposure );

        //SET_FX_T( clear_npc_ai_info_cache, void( npc_ai_info key ) const );
        //SET_FX_T( set_npc_ai_info_cache, void( npc_ai_info key, double val ) const );
        //SET_FX_T( get_npc_ai_info_cache, std::optional<double>( npc_ai_info key ) const );
    }
#undef UT_CLASS // #define UT_CLASS Character

#define UT_CLASS character_id
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors <
            UT_CLASS(),
            UT_CLASS( int )
            > ()
        );

        SET_FX( is_valid );
        SET_FX( get_value );
    }
#undef UT_CLASS // #define UT_CLASS character_id
}

void cata::detail::reg_npc( sol::state &lua )
{
#define UT_CLASS npc
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::bases<player, Character, Creature>(),
            luna::no_constructor
        );
        //Delete any private/protected methods:
        //%s@\(private:\|protected:\)\_.\{-}\(public:\|};\)@\2
        //Delete the public labels:
        //%s@ *public:\n@
        //
        //Delete comments:
        //%s@ *\/\*\_.\{-}\*\/@
        //%s@ *\/\/\_.\{-}\n@\r
        //
        //Delete the enveloping 'class ... { ... },
        //indent backwards until there's no unnecessary indentation.
        //
        //Turn method definitions into declarations:
        //%s@ *{\(}\|\_.\{-}\n^}\)@;
        //
        //Push most method declarations into a single line
        //%s@\((\|,\)\n *@\1
        //
        //Remove default values.
        //%s@ *= *\_.\{-}\( )\|;\|,\)@\1@g
        //
        //Remove overriden/static methods/members
        //%s@.*\(override\|static\).*\n@
        //
        //Remove usings
        //%s@.*using.*\n@
        //
        //Remove templates
        //%s@template<.*\n@
        //
        //Remove virtual tag
        //%s@virtual *@
        //
        //Count how many functions there are
        //%s@\(.*(\_.\{-}).*\n\)@@nc
        //
        //Push first found function to end of file
        //%s@\(.*(\_.\{-}).*\n\+\)\(\_.*\)@\2\1
        //
        //In neovim, input '[COUNT]@:' to repeat the above command for [COUNT] times,
        //where [COUNT] is matches of functions - 1
        //
        //Clean up new lines
        //%s@\n\{3,}@\r\r
        //
        //Search for non-semicolon ended lines
        //[^;]\n
        //
        //Wrap methods into a macro:
        //%s@\(.*\) \+\([^ ]\+\)\((.*\);@SET_FX_T( \2, \1\3 );
        //
        //Wrap members into a macro; BE SURE TO SELECT WHERE TO WRAP!
        //s@.\{-}\([^ ]\+\);@SET_MEMB( \1 );

        // Members
        //SET_MEMB( chosen_mount );
        //
        //SET_MEMB( myclass );
        //
        //SET_MEMB( idz );
        //
        //SET_MEMB( miss_ids );
        //SET_MEMB( assigned_camp );
        //
        //SET_MEMB( last_player_seen_pos );
        //
        //SET_MEMB( goto_to_this_pos );
        //SET_MEMB( last_seen_player_turn );
        //SET_MEMB( wanted_item_pos );
        //SET_MEMB( guard_pos );
        //SET_MEMB( chair_pos );
        //SET_MEMB( base_location );
        //
        //SET_MEMB( goal );
        //SET_MEMB( wander_pos );
        //SET_MEMB( wander_time );
        //
        //SET_MEMB( pulp_location );
        //SET_MEMB( restock );
        //SET_MEMB( fetching_item );
        //SET_MEMB( has_new_items );
        //SET_MEMB( worst_item_value );
        //
        //SET_MEMB( path );
        SET_MEMB( current_activity_id );
        //
        //SET_MEMB( companion_mission_role_id );
        //
        //SET_MEMB( companion_mission_points );
        //SET_MEMB( companion_mission_time );
        //SET_MEMB( companion_mission_time_ret );
        //SET_MEMB( companion_mission_inv );
        //SET_MEMB( mission );
        //SET_MEMB( previous_mission );
        SET_MEMB( personality );
        SET_MEMB( op_of_u );
        //SET_MEMB( chatbin );
        SET_MEMB( patience );
        //SET_MEMB( rules );
        SET_MEMB( marked_for_death );
        SET_MEMB( hit_by_player );
        //SET_MEMB( hallucination );
        SET_MEMB( needs );

        //SET_MEMB( confident_range_cache );

        //SET_MEMB( job );
        //SET_MEMB( last_updated );

        // Methods
        //SET_FX_T( load_npc_template, void( const string_id<npc_template> &ident ) );
        //SET_FX_T( npc_dismount, void() );
        //SET_FX_T( randomize, void( const npc_class_id &type ) );
        //SET_FX_T( randomize_from_faction, void( faction *fac ) );
        //SET_FX_T( apply_ownership_to_inv, void() );
        //
        //SET_FX_T( get_faction_ver, int() const );
        //SET_FX_T( set_faction_ver, void( int new_version ) );
        //SET_FX_T( has_faction_relationship,
        //bool( const player &p, npc_factions::relationship flag ) const );

        //SET_FX_T( set_fac, void( const faction_id &id ) );
        SET_FX_LN_T( set_fac, "set_faction_id", void( const faction_id & id ) );

        // Already handled in Character bindings
        //SET_FX_T( get_fac_id, faction_id() const );

        //SET_FX_T( spawn_at_sm, void( const tripoint &p ) );
        //
        //SET_FX_T( spawn_at_precise, void( point submap_offset, const tripoint &square ) );
        //
        //SET_FX_T( place_on_map, void() );
        //
        //SET_FX_T( add_new_mission, void( mission *miss ) );
        //SET_FX_T( best_skill, skill_id() const );
        //SET_FX_T( best_skill_level, int() const );
        //SET_FX_T( starting_weapon, void( const npc_class_id &type ) );

        //SET_FX_T( opinion_text, std::string() const );
        //SET_FX_T( faction_display, int( const catacurses::window &fac_w, int width ) const );
        //
        //SET_FX_T( form_opinion, void( const player &u ) );
        //SET_FX_T( pick_talk_topic, std::string( const player &u ) );
        //SET_FX_T( character_danger, float( const Character &u ) const );
        //SET_FX_T( vehicle_danger, float( int radius ) const );
        //SET_FX_T( pretend_fire, void( npc *source, int shots, item &gun ) );

        SET_FX_T( turned_hostile, bool() const );

        SET_FX_T( hostile_anger_level, int() const );

        SET_FX_T( make_angry, void() );

        //SET_FX_T( on_attacked, void( const Creature &attacker ) );
        //SET_FX_T( assigned_missions_value, int() );

        //SET_FX_T( skills_offered_to, std::vector<skill_id>( const player &p ) const );
        //
        //SET_FX_T( styles_offered_to, std::vector<matype_id>( const player &p ) const );

        SET_FX_T( is_enemy, bool() const );

        SET_FX_T( is_following, bool() const );
        SET_FX_T( is_obeying, bool( const Character & p ) const );

        SET_FX_T( is_friendly, bool( const Character & p ) const );

        SET_FX_T( is_leader, bool() const );

        SET_FX_T( is_walking_with, bool() const );

        SET_FX_T( is_ally, bool( const Character & p ) const );

        SET_FX_T( is_player_ally, bool() const );

        SET_FX_T( is_stationary, bool( bool include_guards ) const );

        SET_FX_T( is_guarding, bool() const );

        SET_FX_T( is_patrolling, bool() const );
        SET_FX_T( within_boundaries_of_camp, bool() const );

        SET_FX_T( has_player_activity, bool() const );
        SET_FX_T( is_travelling, bool() const );

        SET_FX_T( is_minion, bool() const );

        SET_FX_T( guaranteed_hostile, bool() const );

        SET_FX_T( mutiny, void() );

        SET_FX_T( get_monster_faction, mfaction_id() const );

        SET_FX_T( follow_distance, int() const );

        //SET_FX_T( talk_to_u, void( bool radio_contact ) );
        //
        //SET_FX_T( shop_restock, void() );
        //
        //SET_FX_T( minimum_item_value, int() const );
        //
        //SET_FX_T( update_worst_item_value, void() );
        //SET_FX_T( value, int( const item &it ) const );
        //SET_FX_T( value, int( const item &it, int market_price ) const );
        //SET_FX_T( wear_if_wanted, bool( const item &it, std::string &reason ) );
        //SET_FX_T( start_read, void( item_location loc, player *pl ) );
        //SET_FX_T( finish_read, void( item_location loc ) );
        //SET_FX_T( can_read, bool( const item &book, std::vector<std::string> &fail_reasons ) );
        //SET_FX_T( time_to_read, int( const item &book, const player &reader ) const );
        //SET_FX_T( do_npc_read, void() );
        //SET_FX_T( stow_item, void( item &it ) );
        //
        //SET_FX_T( adjust_worn, bool() );
        //SET_FX_T( has_healing_item, bool( healing_options try_to_fix ) );
        //SET_FX_T( patient_assessment, healing_options( const Character &c ) );
        //SET_FX_T( has_healing_options, healing_options() );
        //SET_FX_T( has_healing_options, healing_options( healing_options try_to_fix ) );
        //SET_FX_T( get_healing_item, item &( healing_options try_to_fix, bool first_best ) );
        //SET_FX_T( has_painkiller, bool() );
        //SET_FX_T( took_painkiller, bool() const );
        //SET_FX_T( use_painkiller, void() );
        //SET_FX_T( activate_item, void( int item_index ) );
        //
        //SET_FX_T( will_accept_from_player, bool( const item &it ) const );
        //
        //SET_FX_T( wants_to_sell, bool( const item &it ) const );
        //SET_FX_T( wants_to_sell, bool( const item &, int at_price, int market_price ) const );
        //SET_FX_T( wants_to_buy, bool( const item &it ) const );
        //SET_FX_T( wants_to_buy, bool( const item &, int at_price, int ) const );
        //
        //SET_FX_T( will_exchange_items_freely, bool() const );
        //SET_FX_T( max_credit_extended, int() const );
        //SET_FX_T( max_willing_to_owe, int() const );
        //
        //SET_FX_T( regen_ai_cache, void() );
        //SET_FX_T( current_target, const Creature *() const );
        SET_FX_T( current_target, Creature * () );
        //SET_FX_T( current_ally, const Creature *() const );
        SET_FX_T( current_ally, Creature * () );
        //SET_FX_T( good_escape_direction, tripoint( bool include_pos ) );

        SET_FX_T( danger_assessment, float() );

        //SET_FX_T( average_damage_dealt, float() );
        //SET_FX_T( bravery_check, bool( int diff ) );
        //SET_FX_T( emergency, bool() const );
        //SET_FX_T( emergency, bool( float danger ) const );
        //SET_FX_T( is_active, bool() const );
        luna::set_fx( ut, "say", &UT_CLASS::say<> );
        //SET_FX_T( say, void( const char *const line, Args &&... args ) const );
        //SET_FX_T( say, void( const std::string &line, sounds::sound_t spriority ) const );
        //SET_FX_T( decide_needs, void() );
        //SET_FX_T( reboot, void() );

        // Already handled in Creature bindings
        //SET_FX_T( is_dead, bool() const );

        SET_FX_T( smash_ability, int() const );

        //SET_FX_T( adjust_power_cbms, void() );
        //SET_FX_T( activate_combat_cbms, void() );
        //SET_FX_T( deactivate_combat_cbms, void() );
        //
        //SET_FX_T( recharge_cbm, bool() );
        //
        //SET_FX_T( wants_to_recharge_cbm, bool() );
        //
        //SET_FX_T( can_use_offensive_cbm, bool() const );
        //
        //SET_FX_T( use_bionic_by_id, bool( const bionic_id &cbm_id, bool eff_only ) );
        //
        //SET_FX_T( activate_bionic_by_id, bool( const bionic_id &cbm_id, bool eff_only ) );
        //SET_FX_T( deactivate_bionic_by_id, bool( const bionic_id &cbm_id, bool eff_only ) );
        //
        //SET_FX_T( discharge_cbm_weapon, void() );
        //
        //SET_FX_T( check_or_use_weapon_cbm, void() );
        //
        //SET_FX_T( check_toggle_cbm, const std::map<item, bionic_id>() );

        //SET_FX_T( complain_about,
        //bool( const std::string &, const time_duration &, const std::string &,
        //bool, sounds::sound_t ) );
        luna::set_fx( ut, "complain_about", []( UT_CLASS & npchar, const std::string & issue,
        const time_duration & dur, const std::string & speech, std::optional<bool> force ) -> bool {
            return npchar.complain_about( issue, dur, speech, force ? *force : false );
        } );

        SET_FX_T( warn_about,
                  void( const std::string & type, const time_duration &, const std::string &,
                        int, const tripoint & ) );

        SET_FX_T( complain, bool() );

        //SET_FX_T( calc_spell_training_cost, int( bool knows, int difficulty, int level ) );
        //
        //SET_FX_T( handle_sound,
        //void( sounds::sound_t priority, const std::string &description,
        //int heard_volume, const tripoint &spos ) );
        //
        //SET_FX_T( shift, void( point s ) );

        //SET_FX_T( move, void() );
        //SET_FX_T( execute_action, void( npc_action action ) );

        SET_FX_T( evaluate_enemy, float( const Creature & ) const );

        SET_FX_T( assess_danger, void() );

        //SET_FX_T( method_of_fleeing, npc_action() );
        //SET_FX_T( method_of_attack, npc_action() );
        //
        //
        //SET_FX_T( address_needs, npc_action() );
        //SET_FX_T( address_needs, npc_action( float danger ) );
        //SET_FX_T( address_player, npc_action() );
        //SET_FX_T( long_term_goal_action, npc_action() );
        //
        //SET_FX_T( scan_new_items, void() );
        //
        //SET_FX_T( wield_better_weapon, bool() );
        //
        //SET_FX_T( confidence_mult, double() const );
        //SET_FX_T( confident_shoot_range, int( const item &it, int at_recoil ) const );
        //SET_FX_T( confident_gun_mode_range, int( const gun_mode &gun, int at_recoil ) const );
        //SET_FX_T( confident_throw_range, int( const item &, Creature * ) const );
        //SET_FX_T( invalidate_range_cache, void() );
        //SET_FX_T( wont_hit_friend, bool( const tripoint &tar, const item &it, bool throwing ) const );
        //SET_FX_T( enough_time_to_reload, bool( const item &gun ) const );
        //
        //SET_FX_T( can_reload_current, bool() );
        //
        //SET_FX_T( find_reloadable, const item &() const );
        //SET_FX_T( find_reloadable, item &() );
        //
        //SET_FX_T( check_or_reload_cbm, void() );
        //
        //SET_FX_T( find_usable_ammo, item_location( const item &weap ) );
        //SET_FX_T( find_usable_ammo, item_location( const item &weap ) const );
        //
        //SET_FX_T( aim, void() );
        //SET_FX_T( do_reload, void( const item &it ) );
        //
        //SET_FX_T( update_path, bool( const tripoint &p, bool no_bashing, bool force ) );
        SET_FX_T( can_open_door, bool( const tripoint &, bool ) const );
        SET_FX_T( can_move_to, bool( const tripoint &, bool ) const );

        //SET_FX_T( move_to, void( const tripoint &p, bool no_bashing, std::set<tripoint> *nomove ) );
        //
        //SET_FX_T( move_to_next, void() );
        //
        //SET_FX_T( avoid_friendly_fire, void() );
        //SET_FX_T( escape_explosion, void() );
        //
        //SET_FX_T( move_away_from,
        //void( const tripoint &p, bool no_bash_atk, std::set<tripoint> *nomove ) );
        //SET_FX_T( move_away_from, void( const std::vector<sphere> &spheres, bool no_bashing ) );
        //
        //SET_FX_T( worker_downtime, void() );
        //SET_FX_T( find_job_to_perform, bool() );
        //
        //SET_FX_T( move_pause, void() );
        //
        //SET_FX_T( get_pathfinding_settings, const pathfinding_settings &( bool no_bashing ) const );
        //
        //SET_FX_T( see_item_say_smth, void( const itype_id &object, const std::string &smth ) );
        //
        //SET_FX_T( find_item, void() );
        //
        //SET_FX_T( pick_up_item, void() );
        //
        //SET_FX_T( drop_items,
        //void( units::mass drop_weight, units::volume drop_volume, int min_val ) );
        //
        //SET_FX_T( pick_up_item_map, std::list<item>( const tripoint &where ) );
        //SET_FX_T( pick_up_item_vehicle, std::list<item>( vehicle &veh, int part_index ) );
        //
        //SET_FX_T( has_item_whitelist, bool() const );
        //SET_FX_T( item_name_whitelisted, bool( const std::string &to_match ) );
        //SET_FX_T( item_whitelisted, bool( const item &it ) );
        //
        //SET_FX_T( find_corpse_to_pulp, bool() );
        //
        //SET_FX_T( do_pulp, bool() );
        //
        //SET_FX_T( do_player_activity, bool() );
        //
        //SET_FX_T( alt_attack, bool() );
        //SET_FX_T( heal_player, void( player &patient ) );
        //SET_FX_T( heal_self, void() );
        //SET_FX_T( pretend_heal, void( player &patient, item used ) );
        //SET_FX_T( mug_player, void( Character &mark ) );
        //SET_FX_T( look_for_player, void( const Character &sought ) );

        SET_FX_T( saw_player_recently, bool() const );

        //SET_FX_T( consume_food, bool() );
        //SET_FX_T( consume_food_from_camp, bool() );

        SET_FX_T( has_omt_destination, bool() const );

        //SET_FX_T( set_omt_destination, void() );
        //
        //SET_FX_T( go_to_omt_destination, void() );
        //
        //SET_FX_T( reach_omt_destination, void() );
        //
        //SET_FX_T( guard_current_pos, void() );

        SET_FX_T( get_attitude, npc_attitude() const );
        SET_FX_T( set_attitude, void( npc_attitude new_attitude ) );
        SET_FX_T( has_activity, bool() const );
        SET_FX_T( has_job, bool() const );

        //SET_FX_T( get_previous_attitude, npc_attitude() );
        //SET_FX_T( get_previous_mission, npc_mission() );
        //SET_FX_T( revert_after_activity, void() );

        //SET_FX_T( on_unload, void() );

        //SET_FX_T( on_load, void() );
        //
        //SET_FX_T( npc_update_body, void() );
        //
        //SET_FX_T( get_known_to_u, bool() );
        //
        //SET_FX_T( set_known_to_u, void( bool known ) );
        //
        //SET_FX_T( set_companion_mission, void( npc &p, const std::string &mission_id ) );
        //SET_FX_T( set_companion_mission,
        //void( const tripoint_abs_omt &omt_pos, const std::string &role_id,
        //const std::string &mission_id ) );
        //SET_FX_T( set_companion_mission,
        //void( const tripoint_abs_omt &omt_pos, const std::string &role_id,
        //const std::string &mission_id, const tripoint_abs_omt &destination ) );
        //
        //SET_FX_T( reset_companion_mission, void() );
        //SET_FX_T( get_mission_destination, std::optional<tripoint_abs_omt>() const );
        //SET_FX_T( has_companion_mission, bool() const );
        //SET_FX_T( get_companion_mission, npc_companion_mission() const );
        //SET_FX_T( get_attitude_group, attitude_group( npc_attitude att ) const );
    }
#undef UT_CLASS // #define UT_CLASS npc

#define UT_CLASS npc_personality
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors <
            UT_CLASS()
            > ()
        );
        SET_MEMB( aggression );
        SET_MEMB( bravery );
        SET_MEMB( collector );
        SET_MEMB( altruism );
    }
#undef UT_CLASS // #define UT_CLASS npc_personality

#define UT_CLASS npc_opinion
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors <
            UT_CLASS(),
            UT_CLASS( int, int, int, int, int )
            > ()
        );
        SET_MEMB( trust );
        SET_MEMB( fear );
        SET_MEMB( value );
        SET_MEMB( anger );
        SET_MEMB( owed );
    }
#undef UT_CLASS // #define UT_CLASS npc_opinion
}

#undef SET_FX_LN_T
#undef SET_FX_LN
#undef SET_FX_T
#undef SET_FX
#undef SET_MEMB_LN
#undef SET_MEMB

#endif // #ifdef LUA
