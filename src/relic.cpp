#include "relic.h"

#include <algorithm>
#include <cmath>

#include "cata_unreachable.h"
#include "creature.h"
#include "character.h"
#include "enum_traits.h"
#include "field.h"
#include "game.h"
#include "generic_factory.h"
#include "json.h"
#include "magic.h"
#include "magic_enchantment.h"
#include "map.h"
#include "map_iterator.h"
#include "messages.h"
#include "rng.h"
#include "translations.h"
#include "trap.h"
#include "type_id.h"
#include "weather.h"

static const efftype_id effect_sleep( "sleep" );

namespace io
{
template<>
std::string enum_to_string<relic_recharge_type>( relic_recharge_type data )
{
    switch( data ) {
        // *INDENT-OFF*
        case relic_recharge_type::time: return "time";
        case relic_recharge_type::solar: return "solar";
        case relic_recharge_type::pain: return "pain";
        case relic_recharge_type::hp: return "hp";
        case relic_recharge_type::fatigue: return "fatigue";
        case relic_recharge_type::field: return "field";
        case relic_recharge_type::trap: return "trap";
        case relic_recharge_type::num:
            break;
        // *INDENT-ON*
    }
    debugmsg( "Invalid relic_recharge_type" );
    abort();
}

template<>
std::string enum_to_string<relic_recharge_req>( relic_recharge_req data )
{
    switch( data ) {
        // *INDENT-OFF*
        case relic_recharge_req::none: return "none";
        case relic_recharge_req::equipped: return "equipped";
        case relic_recharge_req::close_to_skin: return "close_to_skin";
        case relic_recharge_req::sleep: return "sleep";
        case relic_recharge_req::rad: return "rad";
        case relic_recharge_req::wet: return "wet";
        case relic_recharge_req::sky: return "sky";
        case relic_recharge_req::num:
            break;
        // *INDENT-ON*
    }
    debugmsg( "Invalid relic_recharge_req" );
    abort();
}

template<>
std::string enum_to_string<relic_procgen_data::type>( relic_procgen_data::type data )
{
    switch( data ) {
    // *INDENT-OFF*
        case relic_procgen_data::type::active_enchantment: return "active_enchantment";
        case relic_procgen_data::type::hit_me: return "hit_me";
        case relic_procgen_data::type::hit_you: return "hit_you";
        case relic_procgen_data::type::passive_enchantment_add: return "passive_enchantment_add";
        case relic_procgen_data::type::passive_enchantment_mult: return "passive_enchantment_mult";
        case relic_procgen_data::type::last: break;
    // *INDENT-ON*
    }
    debugmsg( "Invalid enchantment::has" );
    abort();
}
} // namespace io

bool relic_recharge::operator==( const relic_recharge &rhs ) const
{
    return type == rhs.type &&
           req == rhs.req &&
           field_type == rhs.field_type &&
           trap_type == rhs.trap_type &&
           interval == rhs.interval &&
           intensity_min == rhs.intensity_min &&
           intensity_max == rhs.intensity_max &&
           rate == rhs.rate &&
           message == rhs.message;
}

void relic_recharge::load( const JsonObject &jo )
{
    try {
        src_loc = jo.get_source_location();
    } catch( const std::exception & ) {
        // Savefiles don't specify source, so ignore error
    }

    jo.read( "type", type );
    jo.read( "req", req );
    jo.read( "field", field_type );
    jo.read( "trap", trap_type );
    jo.read( "interval", interval );
    jo.read( "int_min", intensity_min );
    jo.read( "int_max", intensity_max );
    jo.read( "rate", rate );
    jo.read( "message", message );
}

void relic_recharge::serialize( JsonOut &json ) const
{
    json.start_object();

    json.member( "type", type );
    json.member( "req", req );
    json.member( "field", field_type );
    json.member( "trap", trap_type );
    json.member( "interval", interval );
    json.member( "int_min", intensity_min );
    json.member( "int_max", intensity_max );
    json.member( "rate", rate );
    json.member( "message", message );

    json.end_object();
}

void relic_recharge::deserialize( JsonIn &jsin )
{
    JsonObject data = jsin.get_object();
    load( data );
}

void relic_recharge::check() const
{
    if( type == relic_recharge_type::field && !field_type.is_valid() ) {
        show_warning_at_json_loc( src_loc,
                                  string_format( "Relic specifies invalid field type '%s' to recharge from.", field_type )
                                );
    }
    if( type == relic_recharge_type::trap && !trap_type.is_valid() ) {
        show_warning_at_json_loc( src_loc,
                                  string_format( "Relic specifies invalid trap type '%s' to recharge from.", trap_type )
                                );
    }
    if( to_seconds<int>( interval ) <= 0 ) {
        show_warning_at_json_loc( src_loc, "Relic specifies invalid recharge interval." );
    }
    if( intensity_min < 0 || intensity_max < 0 ) {
        show_warning_at_json_loc( src_loc, "Relic specifies invalid recharge intensity." );
    }
}

template<>
const relic_procgen_data &string_id<relic_procgen_data>::obj() const
{
    return relic_procgen_data_factory.obj( *this );
}

template<>
bool string_id<relic_procgen_data>::is_valid() const
{
    return relic_procgen_data_factory.is_valid( *this );
}

void relic_procgen_data::load_relic_procgen_data( const JsonObject &jo, const std::string &src )
{
    relic_procgen_data_factory.load( jo, src );
}

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

void relic::add_recharge_scheme( const relic_recharge &r )
{
    recharge_scheme.emplace_back( r );
}


template<typename T>
void relic_procgen_data::enchantment_value_passive<T>::load( const JsonObject &jo )
{
    mandatory( jo, was_loaded, "type", type );
    optional( jo, was_loaded, "power_per_increment", power_per_increment, 1 );
    optional( jo, was_loaded, "increment", increment, 1 );
    optional( jo, was_loaded, "min_value", min_value, 0 );
    optional( jo, was_loaded, "max_value", max_value, 0 );
}

template<typename T>
void relic_procgen_data::enchantment_value_passive<T>::deserialize( JsonIn &jsin )
{
    JsonObject jobj = jsin.get_object();
    load( jobj );
}

void relic_procgen_data::enchantment_active::load( const JsonObject &jo )
{
    mandatory( jo, was_loaded, "spell_id", activated_spell );
    optional( jo, was_loaded, "base_power", base_power, 0 );
    optional( jo, was_loaded, "power_per_increment", power_per_increment, 1 );
    optional( jo, was_loaded, "increment", increment, 1 );
    optional( jo, was_loaded, "min_level", min_level, 0 );
    optional( jo, was_loaded, "max_level", max_level, 0 );
}

void relic_procgen_data::enchantment_active::deserialize( JsonIn &jsin )
{
    JsonObject jobj = jsin.get_object();
    load( jobj );
}

void relic_procgen_data::load( const JsonObject &jo, const std::string & )
{
    for( const JsonObject &jo_inner : jo.get_array( "passive_add_procgen_values" ) ) {
        int weight = 0;
        mandatory( jo_inner, was_loaded, "weight", weight );
        relic_procgen_data::enchantment_value_passive<int> val;
        val.load( jo_inner );

        passive_add_procgen_values.add( val, weight );
    }

    for( const JsonObject &jo_inner : jo.get_array( "passive_mult_procgen_values" ) ) {
        int weight = 0;
        mandatory( jo_inner, was_loaded, "weight", weight );
        relic_procgen_data::enchantment_value_passive<float> val;
        val.load( jo_inner );

        passive_mult_procgen_values.add( val, weight );
    }

    for( const JsonObject &jo_inner : jo.get_array( "type_weights" ) ) {
        int weight = 0;
        mandatory( jo_inner, was_loaded, "weight", weight );
        relic_procgen_data::type val;
        mandatory( jo_inner, was_loaded, "value", val );

        type_weights.add( val, weight );
    }

    for( const JsonObject &jo_inner : jo.get_array( "items" ) ) {
        int weight = 0;
        mandatory( jo_inner, was_loaded, "weight", weight );
        itype_id it;
        mandatory( jo_inner, was_loaded, "item", it );

        item_weights.add( it, weight );
    }
}

void relic_procgen_data::deserialize( JsonIn &jsin )
{
    JsonObject jobj = jsin.get_object();
    load( jobj );
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
    if( jo.has_array( "recharge_scheme" ) ) {
        for( JsonObject jobj : jo.get_array( "recharge_scheme" ) ) {
            relic_recharge recharge;
            recharge.load( jobj );
            add_recharge_scheme( recharge );
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

    if( !recharge_scheme.empty() ) {
        jsout.member( "recharge_scheme" );
        jsout.start_array();
        for( const relic_recharge &sp : recharge_scheme ) {
            sp.serialize( jsout );
        }
        jsout.end_array();
    }

    jsout.end_object();
}

int relic::activate( Creature &caster, const tripoint &target ) const
{
    caster.moves -= moves;
    for( const fake_spell &sp : active_effects ) {
        sp.get_spell( sp.level ).cast_all_effects( caster, target );
    }
    return charges_per_activation;
}

bool relic::operator==( const relic &rhs ) const
{
    return charges_per_activation == rhs.charges_per_activation &&
           moves == rhs.moves &&
           item_name_override == rhs.item_name_override &&
           active_effects == rhs.active_effects &&
           passive_effects == rhs.passive_effects &&
           recharge_scheme == rhs.recharge_scheme;
}

std::string relic::name() const
{
    return item_name_override.translated();
}

void relic::check() const
{
    for( const enchantment &ench : passive_effects ) {
        ench.check();
    }
    for( const relic_recharge &rech : recharge_scheme ) {
        rech.check();
    }
}

namespace relic_funcs
{

bool check_recharge_reqs( const item &itm, const relic_recharge &rech, const Character &carrier )
{
    switch( rech.req ) {
        case relic_recharge_req::none: {
            return true;
        }
        case relic_recharge_req::equipped: {
            if( itm.is_armor() ) {
                return carrier.is_wearing( itm );
            } else {
                return carrier.is_wielding( itm );
            }
        }
        case relic_recharge_req::close_to_skin: {
            if( itm.is_armor() ) {
                if( !carrier.is_wearing( itm ) ) {
                    return false;
                }
                for( const bodypart_id &bp : carrier.get_all_body_parts() ) {
                    if( !itm.covers( bp ) ) {
                        continue;
                    }
                    bool this_bp_good = true;
                    for( const item *wi : carrier.worn ) {
                        if( wi->get_coverage( bp ) == 0 ) {
                            continue;
                        }
                        if( wi == &itm ) {
                            break;
                        } else if( wi->covers( bp ) ) {
                            this_bp_good = false;
                            break;
                        }
                    }
                    if( this_bp_good ) {
                        return true;
                    }
                }
                return false;
            } else {
                if( !carrier.is_wielding( itm ) ) {
                    return false;
                }
                bool hand_l_ok = true;
                bool hand_r_ok = true;
                for( const item *wi : carrier.worn ) {
                    if( wi->get_coverage( body_part_hand_l ) == 0 && wi->get_coverage( body_part_hand_r ) == 0 ) {
                        continue;
                    }
                    if( wi->covers( body_part_hand_l ) ) {
                        hand_l_ok = false;
                    }
                    if( wi->covers( body_part_hand_r ) ) {
                        hand_r_ok = false;
                    }
                }
                return hand_l_ok || hand_r_ok;
            }
        }
        case relic_recharge_req::sleep: {
            return carrier.has_effect( effect_sleep );
        }
        case relic_recharge_req::rad: {
            return get_map().get_radiation( carrier.pos() ) > 0 || carrier.get_rad() > 0;
        }
        case relic_recharge_req::wet: {
            bool has_wet = std::any_of( carrier.body_wetness.begin(), carrier.body_wetness.end(),
            []( const int w ) {
                return w != 0;
            } );
            if( has_wet ) {
                return true;
            } else {
                const weather_type &wt = get_weather().weather_id.obj();
                if( !wt.rains || wt.acidic || wt.precip == precip_class::none ) {
                    return false;
                }
                return get_map().is_outside( carrier.pos() );
            }
        }
        case relic_recharge_req::sky: {
            return carrier.posz() > 0;
        }
        default: {
            std::abort();
        }
    }
    cata::unreachable();
}

bool process_recharge_entry( item &itm, const relic_recharge &rech, Character &carrier )
{
    if( !calendar::once_every( rech.interval ) ) {
        return false;
    }
    if( !check_recharge_reqs( itm, rech, carrier ) ) {
        return false;
    }
    map &here = get_map();
    switch( rech.type ) {
        case relic_recharge_type::time: {
            break;
        }
        case relic_recharge_type::solar: {
            if( !g->is_in_sunlight( carrier.pos() ) ) {
                return false;
            }
            break;
        }
        case relic_recharge_type::pain: {
            carrier.add_msg_if_player( m_bad, _( "You suddenly feel sharp pain for no reason." ) );
            carrier.mod_pain_noresist( rng( rech.intensity_min, rech.intensity_max ) );
            break;
        }
        case relic_recharge_type::hp: {
            carrier.add_msg_if_player( m_bad, _( "You feel your body decaying." ) );
            carrier.hurtall( rng( rech.intensity_min, rech.intensity_max ), nullptr );
            break;
        }
        case relic_recharge_type::fatigue: {
            carrier.add_msg_if_player( m_bad, _( "You feel fatigue seeping into your body." ) );
            carrier.mod_fatigue( rng( rech.intensity_min, rech.intensity_max ) );
            carrier.mod_stamina( -100 * rng( rech.intensity_min, rech.intensity_max ) );
            break;
        }
        case relic_recharge_type::field: {
            bool consumed = false;
            for( const tripoint &dest : here.points_in_radius( carrier.pos(), 1 ) ) {
                field_entry *field_at = here.field_at( dest ).find_field( rech.field_type );
                if( !field_at ) {
                    continue;
                }
                int int_here = field_at->get_field_intensity();
                if( int_here >= rech.intensity_min && int_here <= rech.intensity_max ) {
                    here.remove_field( dest, rech.field_type );
                    consumed = true;
                    break;
                }
            }
            if( !consumed ) {
                return false;
            }
            break;
        }
        case relic_recharge_type::trap: {
            bool consumed = false;
            for( const tripoint &dest : here.points_in_radius( carrier.pos(), 1 ) ) {
                if( here.tr_at( dest ).id == rech.trap_type ) {
                    here.remove_trap( dest );
                    consumed = true;
                }
            }
            if( !consumed ) {
                return false;
            }
            break;
        }
        default: {
            std::abort();
        }
    }
    // If relic has a valid ammo type, make sure the first charge loaded isn't a "none"
    bool was_zero = itm.charges == 0;
    itm.charges = clamp( itm.charges + rech.rate, 0, itm.ammo_capacity() );
    if( was_zero && !itm.ammo_types().empty() ) {
        itm.ammo_set( itm.ammo_default(), itm.charges );
    }
    if( rech.message ) {
        carrier.add_msg_if_player( _( *rech.message ) );
    }
    return true;
}

void process_recharge( item &itm, Character &carrier )
{
    if( !itm.is_tool() ) {
        return;
    }
    if( itm.ammo_remaining() >= itm.ammo_capacity() ) {
        return;
    }
    for( const relic_recharge &rech : itm.get_relic_recharge_scheme() ) {
        process_recharge_entry( itm, rech, carrier );
    }
}

generic_factory<relic_procgen_data> relic_procgen_data_factory( "relic_procgen_data" );
} // namespace relic_funcs

int relic::power_level( const relic_procgen_id &ruleset ) const
{
    int total_power_level = 0;
    for( const enchantment &ench : passive_effects ) {
        total_power_level += ruleset->power_level( ench );
    }
    for( const fake_spell &sp : active_effects ) {
        total_power_level += ruleset->power_level( sp );
    }
    return total_power_level;
}

int relic_procgen_data::power_level( const enchantment &ench ) const
{
    int power = 0;

    for( const weighted_object<int, relic_procgen_data::enchantment_value_passive<int>>
         &add_val_passive : passive_add_procgen_values ) {
        int val = ench.get_value_add( add_val_passive.obj.type );
        if( val != 0 ) {
            power += static_cast<float>( add_val_passive.obj.power_per_increment ) /
                     static_cast<float>( add_val_passive.obj.increment ) * val;
        }
    }

    for( const weighted_object<int, relic_procgen_data::enchantment_value_passive<float>>
         &mult_val_passive : passive_mult_procgen_values ) {
        float val = ench.get_value_multiply( mult_val_passive.obj.type );
        if( val != 0.0f ) {
            power += mult_val_passive.obj.power_per_increment / mult_val_passive.obj.increment * val;
        }
    }

    return power;
}

int relic_procgen_data::power_level( const fake_spell &sp ) const
{
    for( const weighted_object<int, relic_procgen_data::enchantment_active> &vals :
         active_procgen_values ) {
        if( vals.obj.activated_spell == sp.id ) {
            return vals.obj.calc_power( sp.level );
        }
    }
    return 0;
}

item relic_procgen_data::create_item( const relic_procgen_data::generation_rules &rules ) const
{
    const itype_id *it_id = item_weights.pick();
    if( it_id == nullptr ) {
        debugmsg( "ERROR: %s procgen data does not have items", id.c_str() );
        return null_item_reference();
    }

    item it( *it_id, calendar::turn );

    it.overwrite_relic( generate( rules, *it_id ) );

    return it;
}

relic relic_procgen_data::generate( const relic_procgen_data::generation_rules &rules,
                                    const itype_id &it_id ) const
{
    relic ret;
    int num_attributes = 0;
    int negative_attribute_power = 0;
    const bool is_armor = item( it_id ).is_armor();

    while( rules.max_attributes > num_attributes && rules.power_level > ret.power_level( id ) ) {
        switch( *type_weights.pick() ) {
            case relic_procgen_data::type::active_enchantment: {
                const relic_procgen_data::enchantment_active *active = active_procgen_values.pick();
                if( active != nullptr ) {
                    fake_spell active_sp;
                    active_sp.id = active->activated_spell;
                    active_sp.level = rng( active->min_level, active->max_level );
                    num_attributes++;
                    int power = power_level( active_sp );
                    if( power < 0 ) {
                        if( rules.max_negative_power > negative_attribute_power ) {
                            break;
                        }
                        negative_attribute_power += power;
                    }
                    ret.add_active_effect( active_sp );
                }
                break;
            }
            case relic_procgen_data::type::passive_enchantment_add: {
                const relic_procgen_data::enchantment_value_passive<int> *add = passive_add_procgen_values.pick();
                if( add != nullptr ) {
                    enchantment ench;
                    int value = rng( add->min_value, add->max_value );
                    if( value == 0 ) {
                        break;
                    }
                    ench.add_value_add( add->type, value );
                    num_attributes++;
                    int negative_ench_attribute = power_level( ench );
                    if( negative_ench_attribute < 0 ) {
                        if( rules.max_negative_power > negative_attribute_power ) {
                            break;
                        }
                        negative_attribute_power += negative_ench_attribute;
                    }
                    if( is_armor ) {
                        ench.set_has( enchantment::has::WORN );
                    } else {
                        ench.set_has( enchantment::has::WIELD );
                    }
                    ret.add_passive_effect( ench );
                }
                break;
            }
            case relic_procgen_data::type::passive_enchantment_mult: {
                const relic_procgen_data::enchantment_value_passive<float> *mult =
                    passive_mult_procgen_values.pick();
                if( mult != nullptr ) {
                    enchantment ench;
                    float value = rng( mult->min_value, mult->max_value );
                    ench.add_value_mult( mult->type, value );
                    num_attributes++;
                    int negative_ench_attribute = power_level( ench );
                    if( negative_ench_attribute < 0 ) {
                        if( rules.max_negative_power > negative_attribute_power ) {
                            break;
                        }
                        negative_attribute_power += negative_ench_attribute;
                    }
                    if( is_armor ) {
                        ench.set_has( enchantment::has::WORN );
                    } else {
                        ench.set_has( enchantment::has::WIELD );
                    }
                    ret.add_passive_effect( ench );
                }
                break;
            }
            case relic_procgen_data::type::hit_me: {
                const relic_procgen_data::enchantment_active *active = passive_hit_me.pick();
                if( active != nullptr ) {
                    fake_spell active_sp;
                    active_sp.id = active->activated_spell;
                    active_sp.level = rng( active->min_level, active->max_level );
                    num_attributes++;
                    enchantment ench;
                    ench.add_hit_me( active_sp );
                    int power = power_level( ench );
                    if( power < 0 ) {
                        if( rules.max_negative_power > negative_attribute_power ) {
                            break;
                        }
                        negative_attribute_power += power;
                    }
                    if( is_armor ) {
                        ench.set_has( enchantment::has::WORN );
                    } else {
                        ench.set_has( enchantment::has::WIELD );
                    }
                    ret.add_passive_effect( ench );
                }
                break;
            }
            case relic_procgen_data::type::hit_you: {
                const relic_procgen_data::enchantment_active *active = passive_hit_you.pick();
                if( active != nullptr ) {
                    fake_spell active_sp;
                    active_sp.id = active->activated_spell;
                    active_sp.level = rng( active->min_level, active->max_level );
                    num_attributes++;
                    enchantment ench;
                    ench.add_hit_you( active_sp );
                    int power = power_level( ench );
                    if( power < 0 ) {
                        if( rules.max_negative_power > negative_attribute_power ) {
                            break;
                        }
                        negative_attribute_power += power;
                    }
                    if( is_armor ) {
                        ench.set_has( enchantment::has::WORN );
                    } else {
                        ench.set_has( enchantment::has::WIELD );
                    }
                    ret.add_passive_effect( ench );
                }
                break;
            }
            case relic_procgen_data::type::last: {
                debugmsg( "ERROR: invalid relic procgen type" );
                break;
            }
        }
    }

    return ret;
}