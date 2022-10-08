#include "relic.h"

#include <algorithm>
#include <cmath>

#include "creature.h"
#include "json.h"
#include "magic.h"
#include "magic_enchantment.h"
#include "translations.h"
#include "type_id.h"

void relic::add_active_effect( const fake_spell &sp )
{
    active_effects.emplace_back( sp );
}

void relic::add_passive_effect( const enchantment &nench )
{
    for( enchantment &ench : passive_effects ) {
        if( ench.add( nench ) ) {
            return;
        }
    }
    passive_effects.emplace_back( nench );
}

void relic::load( const JsonObject &jo )
{
    if( jo.has_array( "active_effects" ) ) {
        for( JsonObject jobj : jo.get_array( "active_effects" ) ) {
            fake_spell sp;
            sp.load( jobj );
            add_active_effect( sp );
        }
    }
    if( jo.has_array( "passive_effects" ) ) {
        for( JsonObject jobj : jo.get_array( "passive_effects" ) ) {
            enchantment ench;
            ench.load( jobj );
            if( !ench.id.is_empty() ) {
                ench = ench.id.obj();
            }
            add_passive_effect( ench );
        }
    }
    jo.read( "name", item_name_override );
    charges_per_activation = jo.get_int( "charges_per_activation", 1 );
    moves = jo.get_int( "moves", 100 );
}

void relic::deserialize( JsonIn &jsin )
{
    JsonObject jobj = jsin.get_object();
    load( jobj );
}

void relic::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "moves", moves );
    jsout.member( "charges_per_activation", charges_per_activation );
    // item_name_override is not saved, in case the original json text changes:
    // in such case names read back from a save wouold no longer be properly translated.

    if( !passive_effects.empty() ) {
        jsout.member( "passive_effects" );
        jsout.start_array();
        for( const enchantment &ench : passive_effects ) {
            ench.serialize( jsout );
        }
        jsout.end_array();
    }

    if( !active_effects.empty() ) {
        jsout.member( "active_effects" );
        jsout.start_array();
        for( const fake_spell &sp : active_effects ) {
            sp.serialize( jsout );
        }
        jsout.end_array();
    }

    jsout.end_object();
}

auto relic::activate( Creature &caster, const tripoint &target ) const -> int
{
    caster.moves -= moves;
    for( const fake_spell &sp : active_effects ) {
        sp.get_spell( sp.level ).cast_all_effects( caster, target );
    }
    return charges_per_activation;
}

auto relic::operator==( const relic &rhs ) const -> bool
{
    return charges_per_activation == rhs.charges_per_activation &&
           moves == rhs.moves &&
           item_name_override == rhs.item_name_override &&
           active_effects == rhs.active_effects &&
           passive_effects == rhs.passive_effects;
}

auto relic::name() const -> std::string
{
    return item_name_override.translated();
}

auto relic::get_enchantments() const -> std::vector<enchantment>
{
    return passive_effects;
}

void relic::check() const
{
    for( const enchantment &ench : passive_effects ) {
        ench.check();
    }
}
