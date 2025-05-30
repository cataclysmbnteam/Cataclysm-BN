#include "trap.h"

#include <memory>
#include <set>
#include <vector>

#include "assign.h"
#include "bodypart.h"
#include "character.h"
#include "creature.h"
#include "debug.h"
#include "event.h"
#include "event_bus.h"
#include "generic_factory.h"
#include "int_id.h"
#include "item.h"
#include "json.h"
#include "line.h"
#include "map.h"
#include "map_iterator.h"
#include "point.h"
#include "rng.h"
#include "string_id.h"
#include "translations.h"

static const skill_id skill_traps( "traps" );

static const efftype_id effect_lack_sleep( "lack_sleep" );

static const trait_id trait_PROF_PD_DET( "PROF_PD_DET" );
static const trait_id trait_PROF_POLICE( "PROF_POLICE" );

namespace
{

generic_factory<trap> trap_factory( "trap" );

} // namespace

/** @relates string_id */
template<>
bool int_id<trap>::is_valid() const
{
    return trap_factory.is_valid( *this );
}

/** @relates int_id */
template<>
const trap &int_id<trap>::obj() const
{
    return trap_factory.obj( *this );
}

/** @relates int_id */
template<>
const string_id<trap> &int_id<trap>::id() const
{
    return trap_factory.convert( *this );
}

/** @relates string_id */
template<>
int_id<trap> string_id<trap>::id() const
{
    return trap_factory.convert( *this, tr_null );
}

/** @relates string_id */
template<>
int_id<trap>::int_id( const string_id<trap> &id ) : _id( id.id() )
{
}

/** @relates string_id */
template<>
const trap &string_id<trap>::obj() const
{
    return trap_factory.obj( *this );
}

/** @relates int_id */
template<>
bool string_id<trap>::is_valid() const
{
    return trap_factory.is_valid( *this );
}

static std::vector<const trap *> funnel_traps;

const std::vector<const trap *> &trap::get_funnels()
{
    return funnel_traps;
}

size_t trap::count()
{
    return trap_factory.size();
}

void trap::load_trap( const JsonObject &jo, const std::string &src )
{
    trap_factory.load( jo, src );
}

void trap::load( const JsonObject &jo, const std::string & )
{
    mandatory( jo, was_loaded, "id", id );
    mandatory( jo, was_loaded, "name", name_ );
    if( !assign( jo, "color", color ) ) {
        jo.throw_error( "missing mandatory member \"color\"" );
    }
    mandatory( jo, was_loaded, "symbol", sym, one_char_symbol_reader );
    mandatory( jo, was_loaded, "visibility", visibility );
    mandatory( jo, was_loaded, "avoidance", avoidance );
    mandatory( jo, was_loaded, "difficulty", difficulty );
    optional( jo, was_loaded, "trap_radius", trap_radius, 0 );
    // TODO: Is there a generic_factory version of this?
    act = trap_function_from_string( jo.get_string( "action" ) );

    optional( jo, was_loaded, "map_regen", map_regen, "none" );
    optional( jo, was_loaded, "benign", benign, false );
    optional( jo, was_loaded, "remove_on_trigger", remove_on_trigger, false );
    optional( jo, was_loaded, "always_invisible", always_invisible, false );
    optional( jo, was_loaded, "funnel_radius", funnel_radius_mm, 0 );
    optional( jo, was_loaded, "comfort", comfort, 0 );
    optional( jo, was_loaded, "floor_bedding_warmth", floor_bedding_warmth, 0 );
    optional( jo, was_loaded, "spell_data", spell_data );
    assign( jo, "trigger_weight", trigger_weight );
    if( was_loaded && jo.has_member( "copy-from" ) && looks_like.empty() ) {
        looks_like = jo.get_string( "copy-from" );
    }
    jo.read( "looks_like", looks_like );
    for( const JsonValue entry : jo.get_array( "trigger_items" ) ) {
        itype_id item_type;
        int quantity = 0;
        int charges = 0;
        if( entry.test_object() ) {
            JsonObject jc = entry.get_object();
            jc.read( "item", item_type, true );
            quantity = jc.get_int( "quantity", 1 );
            charges = jc.get_int( "charges", 1 );
        } else {
            entry.read( item_type, true );
            quantity = 1;
            charges = 1;
        }
        if( !item_type.is_empty() && quantity > 0 && charges > 0 ) {
            trigger_components.emplace_back( item_type, quantity, charges );
        }
    }
    for( const JsonValue entry : jo.get_array( "drops" ) ) {
        itype_id item_type;
        int quantity = 0;
        int charges = 0;
        if( entry.test_object() ) {
            JsonObject jc = entry.get_object();
            jc.read( "item", item_type, true );
            quantity = jc.get_int( "quantity", 1 );
            charges = jc.get_int( "charges", 1 );
        } else {
            entry.read( item_type, true );
            quantity = 1;
            charges = 1;
        }
        if( !item_type.is_empty() && quantity > 0 && charges > 0 ) {
            components.emplace_back( item_type, quantity, charges );
        }
    }
    if( jo.has_object( "vehicle_data" ) ) {
        JsonObject jv = jo.get_object( "vehicle_data" );
        vehicle_data.remove_trap = jv.get_bool( "remove_trap", false );
        vehicle_data.do_explosion = jv.get_bool( "do_explosion", false );
        vehicle_data.is_falling = jv.get_bool( "is_falling", false );
        vehicle_data.chance = jv.get_int( "chance", 100 );
        vehicle_data.damage = jv.get_int( "damage", 0 );
        vehicle_data.shrapnel = jv.get_int( "shrapnel", 0 );
        vehicle_data.sound_volume = jv.get_int( "sound_volume", 0 );
        jv.read( "sound", vehicle_data.sound );
        vehicle_data.sound_type = jv.get_string( "sound_type", "" );
        vehicle_data.sound_variant = jv.get_string( "sound_variant", "" );
        vehicle_data.spawn_items.clear();
        if( jv.has_array( "spawn_items" ) ) {
            for( const JsonValue entry : jv.get_array( "spawn_items" ) ) {
                if( entry.test_object() ) {
                    JsonObject joitm = entry.get_object();
                    vehicle_data.spawn_items.emplace_back(
                        itype_id( joitm.get_string( "id" ) ), joitm.get_float( "chance" ) );
                } else {
                    vehicle_data.spawn_items.emplace_back( itype_id( entry.get_string() ), 1.0 );
                }
            }
        }
        vehicle_data.set_trap = trap_str_id::NULL_ID();
        if( jv.read( "set_trap", vehicle_data.set_trap ) ) {
            vehicle_data.remove_trap = false;
        }
    }
}

std::string trap::name() const
{
    return _( name_ );
}

std::string trap::map_regen_target() const
{
    return map_regen;
}

void trap::reset()
{
    funnel_traps.clear();
    trap_factory.reset();
}

bool trap::detect_trap( const tripoint &pos, const Character &p ) const
{
    // Some decisions are based around:
    // * Starting, and thus average perception, is 8.
    // * Buried landmines, the silent killer, has a visibility of 10.
    // * There will always be a distance malus of 1 unless you're on top of the trap.
    // * ...and an average character should at least have a minor chance of
    //   noticing a buried landmine if standing right next to it.
    // Effective Perception...
    ///\EFFECT_PER increases chance of detecting a trap
    return p.per_cur - p.encumb( body_part_eyes ) / 10 +
           // ...small bonus from stimulants...
           ( p.get_stim() > 10 ? rng( 1, 2 ) : 0 ) +
           // ...bonus from trap skill...
           ///\EFFECT_TRAPS increases chance of detecting a trap
           p.get_skill_level( skill_traps ) * 2 +
           // ...luck, might be good, might be bad...
           rng( -4, 4 ) -
           // ...malus if we are tired...
           ( p.has_effect( effect_lack_sleep ) ? rng( 1, 5 ) : 0 ) -
           // ...malus farther we are from trap...
           rl_dist( p.pos(), pos ) +
           // Police are trained to notice Something Wrong.
           ( p.has_trait( trait_PROF_POLICE ) ? 1 : 0 ) +
           ( p.has_trait( trait_PROF_PD_DET ) ? 2 : 0 ) >
           // ...must all be greater than the trap visibility.
           visibility;
}

// Whether or not, in the current state, the player can see the trap.
bool trap::can_see( const tripoint &pos, const Character &p ) const
{
    if( is_null() ) {
        // There is no trap at all, so logically one can not see it.
        return false;
    }
    return visibility < 0 || p.knows_trap( pos );
}

void trap::trigger( const tripoint &pos, Creature *creature, item *item ) const
{
    const bool is_real_creature = creature != nullptr && !creature->is_hallucination();
    if( is_real_creature || item != nullptr ) {
        bool triggered = act( pos, creature, item );
        if( triggered && is_real_creature ) {
            if( Character *ch = creature->as_character() ) {
                get_event_bus().send<event_type::character_triggers_trap>( ch->getID(), id );
            }
        }
    }
}

bool trap::is_null() const
{
    return loadid == tr_null;
}

bool trap::triggered_by_item( const item &itm ) const
{
    return !is_null() && itm.weight() >= trigger_weight;
}

bool trap::is_funnel() const
{
    return !is_null() && funnel_radius_mm > 0;
}


void trap::trigger_aftermath( map &m, const tripoint &p ) const
{
    for( auto &i : m.tr_at( p ).trigger_components ) {
        const itype_id &item_type = std::get<0>( i );
        const int quantity = std::get<1>( i );
        const int charges = std::get<2>( i );
        m.spawn_item( p.xy(), item_type, quantity, charges );
    }
    if( m.tr_at( p ).remove_trap_when_triggered() ) {
        m.remove_trap( p );
    }
}

void trap::on_disarmed( map &m, const tripoint &p ) const
{
    for( auto &i : components ) {
        const itype_id &item_type = std::get<0>( i );
        const int quantity = std::get<1>( i );
        const int charges = std::get<2>( i );
        m.spawn_item( p.xy(), item_type, quantity, charges );
    }
    for( const tripoint &dest : m.points_in_radius( p, trap_radius ) ) {
        m.remove_trap( dest );
    }
}

//////////////////////////
// convenient int-lookup names for hard-coded functions
trap_id
tr_null,
tr_bubblewrap,
tr_glass,
tr_cot,
tr_funnel,
tr_metal_funnel,
tr_makeshift_funnel,
tr_leather_funnel,
tr_rollmat,
tr_fur_rollmat,
tr_beartrap,
tr_beartrap_buried,
tr_nailboard,
tr_caltrops,
tr_caltrops_glass,
tr_tripwire,
tr_crossbow,
tr_shotgun_2,
tr_shotgun_2_1,
tr_shotgun_1,
tr_engine,
tr_blade,
tr_landmine,
tr_landmine_buried,
tr_telepad,
tr_goo,
tr_dissector,
tr_sinkhole,
tr_pit,
tr_spike_pit,
tr_lava,
tr_portal,
tr_ledge,
tr_boobytrap,
tr_temple_flood,
tr_temple_toggle,
tr_glow,
tr_hum,
tr_shadow,
tr_drain,
tr_snake,
tr_glass_pit;

void trap::check_consistency()
{
    for( const auto &t : trap_factory.get_all() ) {
        for( auto &i : t.components ) {
            const itype_id &item_type = std::get<0>( i );
            if( !item_type.is_valid() ) {
                debugmsg( "trap %s has unknown item as component %s", t.id.str(), item_type.str() );
            }
        }
    }
}

void trap::finalize()
{
    for( const trap &t_const : trap_factory.get_all() ) {
        trap &t = const_cast<trap &>( t_const );
        // We need to set int ids manually now
        t.loadid = t.id.id();
        if( t.is_funnel() ) {
            funnel_traps.push_back( &t );
        }
    }
    const auto trapfind = []( const char *id ) {
        return trap_str_id( id ).id();
    };
    tr_null = trap_str_id::NULL_ID().id();
    tr_bubblewrap = trapfind( "tr_bubblewrap" );
    tr_glass = trapfind( "tr_glass" );
    tr_cot = trapfind( "tr_cot" );
    tr_funnel = trapfind( "tr_funnel" );
    tr_metal_funnel = trapfind( "tr_metal_funnel" );
    tr_makeshift_funnel = trapfind( "tr_makeshift_funnel" );
    tr_leather_funnel = trapfind( "tr_leather_funnel" );
    tr_rollmat = trapfind( "tr_rollmat" );
    tr_fur_rollmat = trapfind( "tr_fur_rollmat" );
    tr_beartrap = trapfind( "tr_beartrap" );
    tr_beartrap_buried = trapfind( "tr_beartrap_buried" );
    tr_nailboard = trapfind( "tr_nailboard" );
    tr_caltrops = trapfind( "tr_caltrops" );
    tr_caltrops_glass = trapfind( "tr_caltrops_glass" );
    tr_tripwire = trapfind( "tr_tripwire" );
    tr_crossbow = trapfind( "tr_crossbow" );
    tr_shotgun_2 = trapfind( "tr_shotgun_2" );
    tr_shotgun_2_1 = trapfind( "tr_shotgun_2_1" );
    tr_shotgun_1 = trapfind( "tr_shotgun_1" );
    tr_engine = trapfind( "tr_engine" );
    tr_blade = trapfind( "tr_blade" );
    tr_landmine = trapfind( "tr_landmine" );
    tr_landmine_buried = trapfind( "tr_landmine_buried" );
    tr_telepad = trapfind( "tr_telepad" );
    tr_goo = trapfind( "tr_goo" );
    tr_dissector = trapfind( "tr_dissector" );
    tr_sinkhole = trapfind( "tr_sinkhole" );
    tr_pit = trapfind( "tr_pit" );
    tr_spike_pit = trapfind( "tr_spike_pit" );
    tr_lava = trapfind( "tr_lava" );
    tr_portal = trapfind( "tr_portal" );
    tr_ledge = trapfind( "tr_ledge" );
    tr_boobytrap = trapfind( "tr_boobytrap" );
    tr_temple_flood = trapfind( "tr_temple_flood" );
    tr_temple_toggle = trapfind( "tr_temple_toggle" );
    tr_glow = trapfind( "tr_glow" );
    tr_hum = trapfind( "tr_hum" );
    tr_shadow = trapfind( "tr_shadow" );
    tr_drain = trapfind( "tr_drain" );
    tr_snake = trapfind( "tr_snake" );
    tr_glass_pit = trapfind( "tr_glass_pit" );
}
