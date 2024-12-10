#ifdef LUA
#include "catalua_bindings.h"

#include "catalua.h"
// Thx Almantuxas
#include "catalua_bindings_utils.h"
#include "catalua_impl.h"
#include "catalua_log.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

#include "creature.h"
#include "magic.h"

// IN WAITING: fake_spell, enchantment_id, enchantments in general
void cata::detail::reg_magic( sol::state &lua ) {
    reg_spell_type( lua );
    reg_spell_fake( lua );
    reg_spell( lua );
}

void cata::detail::reg_spell_type( sol::state &lua )
{
#define UT_CLASS spell_type
    {
        /* NOTE: These changes are applied to the "SpellTypeRaw" Lua obj.
         * Because spell_type is bound as an ID, the actual object is
         * shoved into a 'Raw' binding.
         */
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::no_constructor
        );

        // The string conversion function references this object's str_id.
        luna::set_fx( ut, sol::meta_function::to_string,
        []( const UT_CLASS & id ) -> std::string {
            return string_format( "%s[%s]", luna::detail::luna_traits<UT_CLASS>::name, id.id.c_str() );
        } );


        SET_MEMB_RO( id );
        SET_MEMB_RO( effect_name );
        SET_MEMB_RO( effect_str );

        // Need to see how the code handles this...
        //SET_MEMB_RO( field );
        SET_MEMB_RO( field_chance );
        SET_MEMB_RO( min_field_intensity );
        SET_MEMB_RO( field_intensity_increment );
        SET_MEMB_RO( max_field_intensity );
        SET_MEMB_RO( field_intensity_variance );

        SET_MEMB_RO( min_damage );
        SET_MEMB_RO( damage_increment );
        SET_MEMB_RO( max_damage );

        SET_MEMB_RO( min_range );
        SET_MEMB_RO( range_increment );
        SET_MEMB_RO( max_range );

        SET_MEMB_RO( min_aoe );
        SET_MEMB_RO( aoe_increment );
        SET_MEMB_RO( max_aoe );

        SET_MEMB_RO( min_dot );
        SET_MEMB_RO( dot_increment );
        SET_MEMB_RO( max_dot );

        SET_MEMB_RO( min_duration );
        SET_MEMB_RO( duration_increment );
        SET_MEMB_RO( max_duration );

        // Ignoring pierce damage for now, amongst other things.

        SET_MEMB_RO( base_energy_cost );
        SET_MEMB_RO( energy_increment );
        SET_MEMB_RO( final_energy_cost );

        //SET_MEMB_RO( spell_class );
        SET_MEMB_RO( difficulty );
        SET_MEMB_RO( max_level );
        SET_MEMB_RO( base_casting_time );
        SET_MEMB_RO( casting_time_increment );
        SET_MEMB_RO( final_casting_time );

        DOC( "Other spells cast by this spell." );
        luna::set_fx( ut, "additional_spells", []( const UT_CLASS & spid ) -> std::vector<fake_spell> {
            std::vector<fake_spell> rv = spid.additional_spells; return rv;
        } );

        DOC( "Returns a (long) list of every spell in the game." );
        SET_FX_T( get_all, const std::vector<spell_type> &() );

    }
#undef UT_CLASS // #define UT_CLASS spell_type
}

void cata::detail::reg_spell_fake( sol::state &lua )
{
#define UT_CLASS fake_spell
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors<
            UT_CLASS( spell_id sp, bool hit_self ),
            UT_CLASS( spell_id sp, bool hit_self, int max_level )
            > ()
        );

        luna::set_fx( ut, sol::meta_function::to_string,
        []( const UT_CLASS & id ) -> std::string {
            return string_format( "%s[%s]", luna::detail::luna_traits<UT_CLASS>::name, id.id.c_str() );
        } );

        SET_MEMB_RO( id );
        // TODO: Look into binding std::optional<int> max_level
        // Also consider making this SET_MEMB for modification.
        SET_MEMB_RO( level );
        SET_MEMB_N_RO( self, "target_is_source" );

        // TODO: Support min_level_override
        luna::set_fx( ut, "cast",
         []( UT_CLASS & sp, 
                    Creature & source,
                    const tripoint & target,
                    std::optional<int> min_level_override )
                {
                    int mlo = min_level_override ? *min_level_override : 0;
                    sp.get_spell( mlo ).cast_all_effects( source, target );
                });
    }
#undef UT_CLASS // #define UT_CLASS fake_spell
}

void cata::detail::reg_spell( sol::state &lua )
{
#define UT_CLASS spell
    {
        /* NOTE: This is the actual 'spell type', which is fully-featured and
         * intended for use with players. As such, it tracks things like xp
         * and level, which you may not care about.
         * Make sure to look at SpellFake (i.e., fake_spell) too.
         */
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::constructors<
            UT_CLASS( spell_id, int )
            > ()
        );

        // Lets us grab the ID from the object.
        // *NOTE* May not work (it's marked private).
        SET_MEMB_N_RO( type, "id" );
        // ↑ I have no clue why that works, even after 'type' is made private
        // again. I do not presently have the patience to investigate further.
        //
        //DOC( "Testing..." );
        //SET_FX_T( id, spell_id() const );
        //SET_FX( id );
        
        SET_FX_T( xp, int() const );
        SET_FX_T( gain_exp, void( int ) );
        SET_FX_T( set_exp, void( int ) );
        SET_FX_T( gain_levels, void( int ) );
        SET_FX_T( set_level, void( int ) );
        SET_FX_T( get_level, int() const );

        SET_FX_T( name, std::string() const );
        SET_FX_N_T( description, "desc", std::string() const );

        // Present goal is basic functionality.

        SET_FX_N_T( cast_all_effects, "cast", void( Creature & source, const tripoint & target ) const );
        SET_FX_N_T( cast_spell_effect, "cast_single_effect", void( Creature & source, const tripoint & target ) const );
    }
#undef UT_CLASS // #define UT_CLASS spell
}

#endif // #ifdef LUA
