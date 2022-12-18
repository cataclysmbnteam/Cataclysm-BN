#include "player.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "action.h"
#include "activity_handlers.h"
#include "ammo.h"
#include "avatar.h"
#include "avatar_action.h"
#include "bionics.h"
#include "cata_utility.h"
#include "catacharset.h"
#include "character_functions.h"
#include "character_effects.h"
#include "character_martial_arts.h"
#include "character_turn.h"
#include "clzones.h"
#include "craft_command.h"
#include "damage.h"
#include "debug.h"
#include "effect.h"
#include "enums.h"
#include "faction.h"
#include "fault.h"
#include "flag.h"
#include "field_type.h"
#include "game.h"
#include "game_inventory.h"
#include "gun_mode.h"
#include "handle_liquid.h"
#include "input.h"
#include "int_id.h"
#include "inventory.h"
#include "item.h"
#include "item_contents.h"
#include "item_location.h"
#include "itype.h"
#include "lightmap.h"
#include "line.h"
#include "magic_enchantment.h"
#include "make_static.h"
#include "map.h"
#include "map_iterator.h"
#include "mapdata.h"
#include "martialarts.h"
#include "messages.h"
#include "monster.h"
#include "morale.h"
#include "mtype.h"
#include "mutation.h"
#include "npc.h"
#include "options.h"
#include "output.h"
#include "overmap_types.h"
#include "overmapbuffer.h"
#include "pickup.h"
#include "player_activity.h"
#include "pldata.h"
#include "profession.h"
#include "recipe.h"
#include "recipe_dictionary.h"
#include "requirements.h"
#include "rng.h"
#include "skill.h"
#include "stomach.h"
#include "string_formatter.h"
#include "string_id.h"
#include "translations.h"
#include "trap.h"
#include "ui.h"
#include "uistate.h"
#include "units.h"
#include "value_ptr.h"
#include "veh_type.h"
#include "vehicle.h"
#include "visitable.h"
#include "vitamin.h"
#include "vpart_position.h"
#include "weather.h"
#include "weather_gen.h"

static const efftype_id effect_blind( "blind" );
static const efftype_id effect_downed( "downed" );
static const efftype_id effect_stunned( "stunned" );

static const itype_id itype_battery( "battery" );
static const itype_id itype_brass_catcher( "brass_catcher" );
static const itype_id itype_large_repairkit( "large_repairkit" );
static const itype_id itype_plut_cell( "plut_cell" );
static const itype_id itype_small_repairkit( "small_repairkit" );
static const itype_id itype_syringe( "syringe" );

static const trait_id trait_DEBUG_NODMG( "DEBUG_NODMG" );

static const trait_id trait_CF_HAIR( "CF_HAIR" );
static const trait_id trait_DEBUG_HS( "DEBUG_HS" );
static const trait_id trait_DEFT( "DEFT" );
static const trait_id trait_PACIFIST( "PACIFIST" );
static const trait_id trait_PARKOUR( "PARKOUR" );
static const trait_id trait_PROF_SKATER( "PROF_SKATER" );
static const trait_id trait_QUILLS( "QUILLS" );
static const trait_id trait_SAVANT( "SAVANT" );
static const trait_id trait_SPINES( "SPINES" );
static const trait_id trait_THORNS( "THORNS" );
static const trait_id trait_WATERSLEEP( "WATERSLEEP" );

static const std::string flag_SPLINT( "SPLINT" );
static const std::string flag_RESTRICT_HAND( "RESTRICT_HANDS" );

static const skill_id skill_dodge( "dodge" );
static const skill_id skill_gun( "gun" );
static const skill_id skill_weapon( "weapon" );

static const bionic_id bio_cqb( "bio_cqb" );
static const bionic_id bio_syringe( "bio_syringe" );
static const bionic_id bio_uncanny_dodge( "bio_uncanny_dodge" );

player::player()
{
    str_cur = 8;
    str_max = 8;
    dex_cur = 8;
    dex_max = 8;
    int_cur = 8;
    int_max = 8;
    per_cur = 8;
    per_max = 8;
    dodges_left = 1;
    blocks_left = 1;
    set_power_level( 0_kJ );
    set_max_power_level( 0_kJ );
    cash = 0;
    scent = 500;
    male = true;

    start_location = start_location_id( "sloc_shelter" );
    moves = 100;
    movecounter = 0;
    oxygen = 0;
    in_vehicle = false;
    controlling_vehicle = false;
    grab_point = tripoint_zero;
    hauling = false;
    focus_pool = 100;
    last_item = itype_id( "null" );
    sight_max = 9999;
    last_batch = 0;
    lastconsumed = itype_id( "null" );
    death_drops = true;

    nv_cached = false;
    volume = 0;

    set_value( "THIEF_MODE", "THIEF_ASK" );

    for( const auto &v : vitamin::all() ) {
        vitamin_levels[ v.first ] = 0;
    }

    if( g != nullptr && json_flag::is_ready() ) {
        recalc_sight_limits();
        reset_encumbrance();
    }
}

player::~player() = default;
player::player( player && ) = default;
player &player::operator=( player && ) = default;

void player::mod_stat( const std::string &stat, float modifier )
{
    if( stat == "thirst" ) {
        mod_thirst( modifier );
    } else if( stat == "fatigue" ) {
        mod_fatigue( modifier );
    } else if( stat == "oxygen" ) {
        oxygen += modifier;
    } else if( stat == "stamina" ) {
        mod_stamina( modifier );
    } else {
        // Fall through to the creature method.
        Character::mod_stat( stat, modifier );
    }
}

void player::on_dodge( Creature *source, float difficulty )
{
    static const matec_id tec_none( "tec_none" );

    // Each avoided hit consumes an available dodge
    // When no more available we are likely to fail player::dodge_roll
    dodges_left--;

    // dodging throws of our aim unless we are either skilled at dodging or using a small weapon
    if( is_armed() && weapon.is_gun() ) {
        recoil += std::max( weapon.volume() / 250_ml - get_skill_level( skill_dodge ), 0 ) * rng( 0, 100 );
        recoil = std::min( MAX_RECOIL, recoil );
    }

    // Even if we are not to train still call practice to prevent skill rust
    difficulty = std::max( difficulty, 0.0f );
    practice( skill_dodge, difficulty * 2, difficulty );

    martial_arts_data->ma_ondodge_effects( *this );

    // For adjacent attackers check for techniques usable upon successful dodge
    if( source && square_dist( pos(), source->pos() ) == 1 ) {
        matec_id tec = pick_technique( *source, used_weapon(), false, true, false );

        if( tec != tec_none && !is_dead_state() ) {
            if( get_stamina() < get_stamina_max() / 3 ) {
                add_msg( m_bad, _( "You try to counterattack but you are too exhausted!" ) );
            } else {
                melee_attack( *source, false, &tec );
            }
        }
    }
}

void player::on_hit( Creature *source, bodypart_id bp_hit,
                     float /*difficulty*/, dealt_projectile_attack const *const proj )
{
    check_dead_state();
    if( source == nullptr || proj != nullptr ) {
        return;
    }

    bool u_see = g->u.sees( *this );
    if( has_active_bionic( bionic_id( "bio_ods" ) ) && get_power_level() > 5_kJ ) {
        if( is_player() ) {
            add_msg( m_good, _( "Your offensive defense system shocks %s in mid-attack!" ),
                     source->disp_name() );
        } else if( u_see ) {
            add_msg( _( "%1$s's offensive defense system shocks %2$s in mid-attack!" ),
                     disp_name(),
                     source->disp_name() );
        }
        int shock = rng( 1, 4 );
        mod_power_level( units::from_kilojoule( -shock ) );
        damage_instance ods_shock_damage;
        ods_shock_damage.add_damage( DT_ELECTRIC, shock * 5 );
        // Should hit body part used for attack
        source->deal_damage( this, bodypart_id( "torso" ), ods_shock_damage );
    }
    if( !wearing_something_on( bp_hit ) &&
        ( has_trait( trait_SPINES ) || has_trait( trait_QUILLS ) ) ) {
        int spine = rng( 1, has_trait( trait_QUILLS ) ? 20 : 8 );
        if( !is_player() ) {
            if( u_see ) {
                add_msg( _( "%1$s's %2$s puncture %3$s in mid-attack!" ), name,
                         ( has_trait( trait_QUILLS ) ? _( "quills" ) : _( "spines" ) ),
                         source->disp_name() );
            }
        } else {
            add_msg( m_good, _( "Your %1$s puncture %2$s in mid-attack!" ),
                     ( has_trait( trait_QUILLS ) ? _( "quills" ) : _( "spines" ) ),
                     source->disp_name() );
        }
        damage_instance spine_damage;
        spine_damage.add_damage( DT_STAB, spine );
        source->deal_damage( this, bodypart_id( "torso" ), spine_damage );
    }
    if( ( !( wearing_something_on( bp_hit ) ) ) && ( has_trait( trait_THORNS ) ) &&
        ( !( source->has_weapon() ) ) ) {
        if( !is_player() ) {
            if( u_see ) {
                add_msg( _( "%1$s's %2$s scrape %3$s in mid-attack!" ), name,
                         _( "thorns" ), source->disp_name() );
            }
        } else {
            add_msg( m_good, _( "Your thorns scrape %s in mid-attack!" ), source->disp_name() );
        }
        int thorn = rng( 1, 4 );
        damage_instance thorn_damage;
        thorn_damage.add_damage( DT_CUT, thorn );
        // In general, critters don't have separate limbs
        // so safer to target the torso
        source->deal_damage( this, bodypart_id( "torso" ), thorn_damage );
    }
    if( ( !( wearing_something_on( bp_hit ) ) ) && ( has_trait( trait_CF_HAIR ) ) ) {
        if( !is_player() ) {
            if( u_see ) {
                add_msg( _( "%1$s gets a load of %2$s's %3$s stuck in!" ), source->disp_name(),
                         name, ( _( "hair" ) ) );
            }
        } else {
            add_msg( m_good, _( "Your hairs detach into %s!" ), source->disp_name() );
        }
        source->add_effect( effect_stunned, 2_turns );
        if( one_in( 3 ) ) { // In the eyes!
            source->add_effect( effect_blind, 2_turns );
        }
    }
    if( worn_with_flag( "REQUIRES_BALANCE" ) && !has_effect( effect_downed ) ) {
        int rolls = 4;
        if( worn_with_flag( "ROLLER_ONE" ) ) {
            rolls += 2;
        }
        if( has_trait( trait_PROF_SKATER ) ) {
            rolls--;
        }
        if( has_trait( trait_DEFT ) ) {
            rolls--;
        }

        if( stability_roll() < dice( rolls, 10 ) ) {
            if( !is_player() ) {
                if( u_see ) {
                    add_msg( _( "%1$s loses their balance while being hit!" ), name );
                }
            } else {
                add_msg( m_bad, _( "You lose your balance while being hit!" ) );
            }
            // This kind of downing is not subject to immunity.
            add_effect( effect_downed, 2_turns, num_bp, 0, true );
        }
    }
    Character::on_hit( source, bp_hit, 0.0f, proj );
}

int player::get_lift_assist() const
{
    int result = 0;
    const std::vector<npc *> helpers = get_crafting_helpers();
    for( const npc *np : helpers ) {
        result += np->get_str();
    }
    return result;
}

bool player::immune_to( body_part bp, damage_unit dam ) const
{
    if( has_trait( trait_DEBUG_NODMG ) || is_immune_damage( dam.type ) ) {
        return true;
    }

    passive_absorb_hit( convert_bp( bp ).id(), dam );

    for( const item &cloth : worn ) {
        if( cloth.get_coverage() == 100 && cloth.covers( bp ) ) {
            cloth.mitigate_damage( dam );
        }
    }

    return dam.amount <= 0;
}

float player::fall_damage_mod() const
{
    if( has_effect_with_flag( "EFFECT_FEATHER_FALL" ) ) {
        return 0.0f;
    }
    float ret = 1.0f;

    // Ability to land properly is 2x as important as dexterity itself
    /** @EFFECT_DEX decreases damage from falling */

    /** @EFFECT_DODGE decreases damage from falling */
    float dex_dodge = dex_cur / 2.0 + get_skill_level( skill_dodge );
    // Penalize for wearing heavy stuff
    const float average_leg_encumb = ( encumb( bp_leg_l ) + encumb( bp_leg_r ) ) / 2.0;
    dex_dodge -= ( average_leg_encumb + encumb( bp_torso ) ) / 10;
    // But prevent it from increasing damage
    dex_dodge = std::max( 0.0f, dex_dodge );
    // 100% damage at 0, 75% at 10, 50% at 20 and so on
    ret *= ( 100.0f - ( dex_dodge * 4.0f ) ) / 100.0f;

    if( has_trait( trait_PARKOUR ) ) {
        ret *= 2.0f / 3.0f;
    }

    // TODO: Bonus for Judo, mutations. Penalty for heavy weight (including mutations)
    return std::max( 0.0f, ret );
}

// force is maximum damage to hp before scaling
int player::impact( const int force, const tripoint &p )
{
    // Falls over ~30m are fatal more often than not
    // But that would be quite a lot considering 21 z-levels in game
    // so let's assume 1 z-level is comparable to 30 force

    if( force <= 0 ) {
        return force;
    }

    // Damage modifier (post armor)
    float mod = 1.0f;
    int effective_force = force;
    int cut = 0;
    // Percentage armor penetration - armor won't help much here
    // TODO: Make cushioned items like bike helmets help more
    float armor_eff = 1.0f;
    // Shock Absorber CBM heavily reduces damage
    const bool shock_absorbers = has_active_bionic( bionic_id( "bio_shock_absorber" ) );

    // Being slammed against things rather than landing means we can't
    // control the impact as well
    const bool slam = p != pos();
    std::string target_name = "a swarm of bugs";
    Creature *critter = g->critter_at( p );
    if( critter != this && critter != nullptr ) {
        target_name = critter->disp_name();
        // Slamming into creatures and NPCs
        // TODO: Handle spikes/horns and hard materials
        armor_eff = 0.5f; // 2x as much as with the ground
        // TODO: Modify based on something?
        mod = 1.0f;
        effective_force = force;
    } else if( const optional_vpart_position vp = g->m.veh_at( p ) ) {
        // Slamming into vehicles
        // TODO: Integrate it with vehicle collision function somehow
        target_name = vp->vehicle().disp_name();
        if( vp.part_with_feature( "SHARP", true ) ) {
            // Now we're actually getting impaled
            cut = force; // Lots of fun
        }

        mod = slam ? 1.0f : fall_damage_mod();
        armor_eff = 0.25f; // Not much
        if( !slam && vp->part_with_feature( "ROOF", true ) ) {
            // Roof offers better landing than frame or pavement
            // TODO: Make this not happen with heavy duty/plated roof
            effective_force /= 2;
        }
    } else {
        // Slamming into terrain/furniture
        target_name = g->m.disp_name( p );
        int hard_ground = g->m.has_flag( TFLAG_DIGGABLE, p ) ? 0 : 3;
        armor_eff = 0.25f; // Not much
        // Get cut by stuff
        // This isn't impalement on metal wreckage, more like flying through a closed window
        cut = g->m.has_flag( TFLAG_SHARP, p ) ? 5 : 0;
        effective_force = force + hard_ground;
        mod = slam ? 1.0f : fall_damage_mod();
        if( g->m.has_furn( p ) ) {
            // TODO: Make furniture matter
        } else if( g->m.has_flag( TFLAG_SWIMMABLE, p ) ) {
            // TODO: Some formula of swimming
            effective_force /= 4;
        }
    }

    // Rescale for huge force
    // At >30 force, proper landing is impossible and armor helps way less
    if( effective_force > 30 ) {
        // Armor simply helps way less
        armor_eff *= 30.0f / effective_force;
        if( mod < 1.0f ) {
            // Everything past 30 damage gets a worse modifier
            const float scaled_mod = std::pow( mod, 30.0f / effective_force );
            const float scaled_damage = ( 30.0f * mod ) + scaled_mod * ( effective_force - 30.0f );
            mod = scaled_damage / effective_force;
        }
    }

    if( !slam && mod < 1.0f && mod * force < 5 ) {
        // Perfect landing, no damage (regardless of armor)
        add_msg_if_player( m_warning, _( "You land on %s." ), target_name );
        return 0;
    }

    // Shock absorbers kick in only when they need to, so if our other protections fail, fall back on them
    if( shock_absorbers ) {
        effective_force -= 15; // Provide a flat reduction to force
        if( mod > 0.25f ) {
            mod = 0.25f; // And provide a 75% reduction against that force if we don't have it already
        }
        if( effective_force < 0 ) {
            effective_force = 0;
        }
    }

    int total_dealt = 0;
    if( mod * effective_force >= 5 ) {
        for( int i = 0; i < num_hp_parts; i++ ) {
            const bodypart_id bp = convert_bp( hp_to_bp( static_cast<hp_part>( i ) ) ).id();
            const int bash = effective_force * rng( 60, 100 ) / 100;
            damage_instance di;
            di.add_damage( DT_BASH, bash, 0, armor_eff, mod );
            // No good way to land on sharp stuff, so here modifier == 1.0f
            di.add_damage( DT_CUT, cut, 0, armor_eff, 1.0f );
            total_dealt += deal_damage( nullptr, bp, di ).total_damage();
        }
    }

    if( total_dealt > 0 && is_player() ) {
        // "You slam against the dirt" is fine
        add_msg( m_bad, _( "You are slammed against %1$s for %2$d damage." ),
                 target_name, total_dealt );
    } else if( is_player() && shock_absorbers ) {
        add_msg( m_bad, _( "You are slammed against %s!" ),
                 target_name, total_dealt );
        add_msg( m_good, _( "…but your shock absorbers negate the damage!" ) );
    } else if( slam ) {
        // Only print this line if it is a slam and not a landing
        // Non-players should only get this one: player doesn't know how much damage was dealt
        // and landing messages for each slammed creature would be too much
        add_msg_player_or_npc( m_bad,
                               _( "You are slammed against %s." ),
                               _( "<npcname> is slammed against %s." ),
                               target_name );
    } else {
        // No landing message for NPCs
        add_msg_if_player( m_warning, _( "You land on %s." ), target_name );
    }

    if( x_in_y( mod, 1.0f ) ) {
        add_effect( effect_downed, rng( 1_turns, 1_turns + mod * 3_turns ) );
    }

    return total_dealt;
}

void player::knock_back_to( const tripoint &to )
{
    if( to == pos() ) {
        return;
    }

    if( rl_dist( pos(), to ) < 2 && get_map().obstructed_by_vehicle_rotation( pos(), to ) ) {
        tripoint intervening = to;
        if( one_in( 2 ) ) {
            intervening.x = pos().x;
        } else {
            intervening.y = pos().y;
        }

        apply_damage( nullptr, bodypart_id( "torso" ), 3 );
        add_effect( effect_stunned, 2_turns );
        add_msg_player_or_npc( _( "You bounce off a %s!" ), _( "<npcname> bounces off a %s!" ),
                               g->m.obstacle_name( intervening ) );
        return;
    }

    // First, see if we hit a monster
    if( monster *const critter = g->critter_at<monster>( to ) ) {
        deal_damage( critter, bodypart_id( "torso" ), damage_instance( DT_BASH, critter->type->size ) );
        add_effect( effect_stunned, 1_turns );
        /** @EFFECT_STR_MAX allows knocked back player to knock back, damage, stun some monsters */
        if( ( str_max - 6 ) / 4 > critter->type->size ) {
            critter->knock_back_from( pos() ); // Chain reaction!
            critter->apply_damage( this, bodypart_id( "torso" ), ( str_max - 6 ) / 4 );
            critter->add_effect( effect_stunned, 1_turns );
        } else if( ( str_max - 6 ) / 4 == critter->type->size ) {
            critter->apply_damage( this, bodypart_id( "torso" ), ( str_max - 6 ) / 4 );
            critter->add_effect( effect_stunned, 1_turns );
        }
        critter->check_dead_state();

        add_msg_player_or_npc( _( "You bounce off a %s!" ), _( "<npcname> bounces off a %s!" ),
                               critter->name() );
        return;
    }

    if( npc *const np = g->critter_at<npc>( to ) ) {
        deal_damage( np, bodypart_id( "torso" ), damage_instance( DT_BASH, np->get_size() + 1 ) );
        add_effect( effect_stunned, 1_turns );
        np->deal_damage( this, bodypart_id( "torso" ), damage_instance( DT_BASH, 3 ) );
        add_msg_player_or_npc( _( "You bounce off %s!" ), _( "<npcname> bounces off %s!" ),
                               np->name );
        np->check_dead_state();
        return;
    }

    // If we're still in the function at this point, we're actually moving a tile!
    if( g->m.has_flag( "LIQUID", to ) && g->m.has_flag( TFLAG_DEEP_WATER, to ) ) {
        if( !is_npc() ) {
            avatar_action::swim( g->m, g->u, to );
        }
        // TODO: NPCs can't swim!
    } else if( g->m.impassable( to ) ) { // Wait, it's a wall

        // It's some kind of wall.
        // TODO: who knocked us back? Maybe that creature should be the source of the damage?
        apply_damage( nullptr, bodypart_id( "torso" ), 3 );
        add_effect( effect_stunned, 2_turns );
        add_msg_player_or_npc( _( "You bounce off a %s!" ), _( "<npcname> bounces off a %s!" ),
                               g->m.obstacle_name( to ) );

    } else { // It's no wall
        setpos( to );
    }
}

int player::hp_percentage() const
{
    const bodypart_id head_id = bodypart_id( "head" );
    const bodypart_id torso_id = bodypart_id( "torso" );
    int total_cur = 0;
    int total_max = 0;
    // Head and torso HP are weighted 3x and 2x, respectively
    total_cur = get_part_hp_cur( head_id ) * 3 + get_part_hp_cur( torso_id ) * 2;
    total_max = get_part_hp_max( head_id ) * 3 + get_part_hp_max( torso_id ) * 2;
    for( const std::pair< const bodypart_str_id, bodypart> &elem : get_body() ) {
        total_cur += elem.second.get_hp_cur();
        total_max += elem.second.get_hp_max();
    }

    return ( 100 * total_cur ) / total_max;
}

item player::reduce_charges( int position, int quantity )
{
    item &it = i_at( position );
    if( it.is_null() ) {
        debugmsg( "invalid item position %d for reduce_charges", position );
        return item();
    }
    if( it.charges <= quantity ) {
        return i_rem( position );
    }
    it.mod_charges( -quantity );
    item tmp( it );
    tmp.charges = quantity;
    return tmp;
}

item player::reduce_charges( item *it, int quantity )
{
    if( !has_item( *it ) ) {
        debugmsg( "invalid item (name %s) for reduce_charges", it->tname() );
        return item();
    }
    if( it->charges <= quantity ) {
        return i_rem( it );
    }
    it->mod_charges( -quantity );
    item result( *it );
    result.charges = quantity;
    return result;
}

//Returns the amount of charges that were consumed by the player
int player::drink_from_hands( item &water )
{
    int charges_consumed = 0;
    if( query_yn( _( "Drink %s from your hands?" ),
                  colorize( water.type_name(), water.color_in_inventory() ) ) ) {
        // Create a dose of water no greater than the amount of water remaining.
        item water_temp( water );
        // If player is slaked water might not get consumed.
        consume_item( water_temp );
        charges_consumed = water.charges - water_temp.charges;
        if( charges_consumed > 0 ) {
            moves -= 350;
        }
    }

    return charges_consumed;
}

// TODO: Properly split medications and food instead of hacking around
bool player::consume_med( item &target )
{
    if( !target.is_medication() ) {
        return false;
    }

    const itype_id tool_type = target.get_comestible()->tool;
    const itype *req_tool = &*tool_type;
    bool tool_override = false;
    if( tool_type == itype_syringe && has_bionic( bio_syringe ) ) {
        tool_override = true;
    }
    if( req_tool->tool ) {
        if( !( has_amount( tool_type, 1 ) && has_charges( tool_type, req_tool->tool->charges_per_use ) ) &&
            !tool_override ) {
            add_msg_if_player( m_info, _( "You need a %s to consume that!" ), req_tool->nname( 1 ) );
            return false;
        }
        use_charges( tool_type, req_tool->tool->charges_per_use );
    }

    int amount_used = 1;
    if( target.type->has_use() ) {
        amount_used = target.type->invoke( *this, target, pos() );
        if( amount_used <= 0 ) {
            return false;
        }
    }

    // TODO: Get the target it was used on
    // Otherwise injecting someone will give us addictions etc.
    if( target.has_flag( "NO_INGEST" ) ) {
        const auto &comest = *target.get_comestible();
        // Assume that parenteral meds don't spoil, so don't apply rot
        modify_health( comest );
        modify_stimulation( comest );
        modify_fatigue( comest );
        modify_radiation( comest );
        modify_addiction( comest );
        modify_morale( target );
    } else {
        // Take by mouth
        consume_effects( target );
    }

    mod_moves( -250 );
    target.charges -= amount_used;
    return target.charges <= 0;
}

static bool query_consume_ownership( item &target, player &p )
{
    if( !target.is_owned_by( p, true ) ) {
        bool choice = true;
        if( p.get_value( "THIEF_MODE" ) == "THIEF_ASK" ) {
            choice = pickup::query_thief();
        }
        if( p.get_value( "THIEF_MODE" ) == "THIEF_HONEST" || !choice ) {
            return false;
        }
        std::vector<npc *> witnesses;
        for( npc &elem : g->all_npcs() ) {
            if( rl_dist( elem.pos(), p.pos() ) < MAX_VIEW_DISTANCE && elem.sees( p.pos() ) ) {
                witnesses.push_back( &elem );
            }
        }
        for( npc *elem : witnesses ) {
            elem->say( "<witnessed_thievery>", 7 );
        }
        if( !witnesses.empty() && target.is_owned_by( p, true ) ) {
            if( p.add_faction_warning( target.get_owner() ) ) {
                for( npc *elem : witnesses ) {
                    elem->make_angry();
                }
            }
        }
    }
    return true;
}

bool player::consume_item( item &target )
{
    if( target.is_null() ) {
        add_msg_if_player( m_info, _( "You do not have that item." ) );
        return false;
    }
    if( is_underwater() && !has_trait( trait_WATERSLEEP ) ) {
        add_msg_if_player( m_info, _( "You can't do that while underwater." ) );
        return false;
    }

    item &comest = get_consumable_from( target );

    if( comest.is_null() || target.is_craft() ) {
        add_msg_if_player( m_info, _( "You can't eat your %s." ), target.tname() );
        if( is_npc() ) {
            debugmsg( "%s tried to eat a %s", name, target.tname() );
        }
        return false;
    }
    if( is_player() && !query_consume_ownership( target, *this ) ) {
        return false;
    }
    if( consume_med( comest ) ||
        eat( comest ) || feed_reactor_with( comest ) || feed_furnace_with( comest ) ||
        fuel_bionic_with( comest ) ) {

        if( target.is_container() ) {
            target.on_contents_changed();
        }

        return comest.charges <= 0;
    }

    return false;
}

bool player::consume( item_location loc )
{
    item &target = *loc;
    const bool wielding = is_wielding( target );
    const bool worn = is_worn( target );
    const bool inv_item = !( wielding || worn );

    if( consume_item( target ) ) {

        const bool was_in_container = !can_consume_as_is( target );

        if( was_in_container ) {
            i_rem( &target.contents.front() );
        } else {
            i_rem( &target );
        }

        //Restack and sort so that we don't lie about target's invlet
        if( inv_item ) {
            inv.restack( *this );
        }

        if( was_in_container && wielding ) {
            add_msg_if_player( _( "You are now wielding an empty %s." ), weapon.tname() );
        } else if( was_in_container && worn ) {
            add_msg_if_player( _( "You are now wearing an empty %s." ), target.tname() );
        } else if( was_in_container && !is_npc() ) {
            bool drop_it = false;
            if( get_option<std::string>( "DROP_EMPTY" ) == "no" ) {
                drop_it = false;
            } else if( get_option<std::string>( "DROP_EMPTY" ) == "watertight" ) {
                drop_it = !target.is_watertight_container();
            } else if( get_option<std::string>( "DROP_EMPTY" ) == "all" ) {
                drop_it = true;
            }
            if( drop_it ) {
                add_msg( _( "You drop the empty %s." ), target.tname() );
                put_into_vehicle_or_drop( *this, item_drop_reason::deliberate, { inv.remove_item( &target ) } );
            } else {
                int quantity = inv.const_stack( inv.position_by_item( &target ) ).size();
                char letter = target.invlet ? target.invlet : ' ';
                add_msg( m_info, _( "%c - %d empty %s" ), letter, quantity, target.tname( quantity ) );
            }
        }
    } else if( inv_item ) {
        if( pickup::handle_spillable_contents( *this, target, g->m ) ) {
            i_rem( &target );
        }
        inv.restack( *this );
        inv.unsort();
    }

    return true;
}

bool player::add_faction_warning( const faction_id &id )
{
    const auto it = warning_record.find( id );
    if( it != warning_record.end() ) {
        it->second.first += 1;
        if( it->second.second - calendar::turn > 5_minutes ) {
            it->second.first -= 1;
        }
        it->second.second = calendar::turn;
        if( it->second.first > 3 ) {
            return true;
        }
    } else {
        warning_record[id] = std::make_pair( 1, calendar::turn );
    }
    faction *fac = g->faction_manager_ptr->get( id );
    if( fac != nullptr && is_player() && fac->id != faction_id( "no_faction" ) ) {
        fac->likes_u -= 1;
        fac->respects_u -= 1;
    }
    return false;
}

int player::current_warnings_fac( const faction_id &id )
{
    const auto it = warning_record.find( id );
    if( it != warning_record.end() ) {
        if( it->second.second - calendar::turn > 5_minutes ) {
            it->second.first = std::max( 0,
                                         it->second.first - 1 );
        }
        return it->second.first;
    }
    return 0;
}

bool player::beyond_final_warning( const faction_id &id )
{
    const auto it = warning_record.find( id );
    if( it != warning_record.end() ) {
        if( it->second.second - calendar::turn > 5_minutes ) {
            it->second.first = std::max( 0,
                                         it->second.first - 1 );
        }
        return it->second.first > 3;
    }
    return false;
}

item::reload_option player::select_ammo( const item &base,
        std::vector<item::reload_option> opts ) const
{
    if( opts.empty() ) {
        add_msg_if_player( m_info, _( "Never mind." ) );
        return item::reload_option();
    }

    uilist menu;
    menu.text = string_format( base.is_watertight_container() ? _( "Refill %s" ) :
                               base.has_flag( "RELOAD_AND_SHOOT" ) ? _( "Select ammo for %s" ) : _( "Reload %s" ),
                               base.tname() );

    // Construct item names
    std::vector<std::string> names;
    std::transform( opts.begin(), opts.end(),
    std::back_inserter( names ), [&]( const item::reload_option & e ) {
        if( e.ammo->is_magazine() && e.ammo->ammo_data() ) {
            if( e.ammo->ammo_current() == itype_battery ) {
                // This battery ammo is not a real object that can be recovered but pseudo-object that represents charge
                //~ battery storage (charges)
                return string_format( pgettext( "magazine", "%1$s (%2$d)" ), e.ammo->type_name(),
                                      e.ammo->ammo_remaining() );
            } else {
                //~ magazine with ammo (count)
                return string_format( pgettext( "magazine", "%1$s with %2$s (%3$d)" ), e.ammo->type_name(),
                                      e.ammo->ammo_data()->nname( e.ammo->ammo_remaining() ), e.ammo->ammo_remaining() );
            }
        } else if( e.ammo->is_watertight_container() ||
                   ( e.ammo->is_ammo_container() && is_worn( *e.ammo ) ) ) {
            // worn ammo containers should be named by their contents with their location also updated below
            return e.ammo->contents.front().display_name();

        } else {
            const_cast<item_location &>( ammo_location ).make_dirty();
            return ( ammo_location && ammo_location == e.ammo ? "* " : "" ) + e.ammo->display_name();
        }
    } );

    // Get location descriptions
    std::vector<std::string> where;
    std::transform( opts.begin(), opts.end(),
    std::back_inserter( where ), [this]( const item::reload_option & e ) {
        bool is_ammo_container = e.ammo->is_ammo_container();
        if( is_ammo_container || e.ammo->is_container() ) {
            if( is_ammo_container && is_worn( *e.ammo ) ) {
                return e.ammo->type_name();
            }
            return string_format( _( "%s, %s" ), e.ammo->type_name(), e.ammo.describe( &g->u ) );
        }
        return e.ammo.describe( &g->u );
    } );

    // Pads elements to match longest member and return length
    auto pad = []( std::vector<std::string> &vec, int n, int t ) -> int {
        for( const auto &e : vec )
        {
            n = std::max( n, utf8_width( e, true ) + t );
        }
        for( auto &e : vec )
        {
            e += std::string( n - utf8_width( e, true ), ' ' );
        }
        return n;
    };

    // Pad the first column including 4 trailing spaces
    int w = pad( names, utf8_width( menu.text, true ), 6 );
    menu.text.insert( 0, 2, ' ' ); // add space for UI hotkeys
    menu.text += std::string( w + 2 - utf8_width( menu.text, true ), ' ' );

    // Pad the location similarly (excludes leading "| " and trailing " ")
    w = pad( where, utf8_width( _( "| Location " ) ) - 3, 6 );
    menu.text += _( "| Location " );
    menu.text += std::string( w + 3 - utf8_width( _( "| Location " ) ), ' ' );

    menu.text += _( "| Amount  " );
    menu.text += _( "| Moves   " );

    // We only show ammo statistics for guns and magazines
    if( base.is_gun() || base.is_magazine() ) {
        menu.text += _( "| Damage   | Pierce   " );
    }

    auto draw_row = [&]( int idx ) {
        const auto &sel = opts[ idx ];
        std::string row = string_format( "%s| %s |", names[ idx ], where[ idx ] );
        row += string_format( ( sel.ammo->is_ammo() ||
                                sel.ammo->is_ammo_container() ) ? " %-7d |" : "         |", sel.qty() );
        row += string_format( " %-7d ", sel.moves() );

        if( base.is_gun() || base.is_magazine() ) {
            const itype *ammo = sel.ammo->is_ammo_container() ? sel.ammo->contents.front().ammo_data() :
                                sel.ammo->ammo_data();
            if( ammo ) {
                const damage_instance &dam = ammo->ammo->damage;
                const damage_unit &du = dam.damage_units.front();
                if( du.damage_multiplier != 1.0f ) {
                    float dam_amt = du.amount;
                    row += string_format( "| %-3d*%3d%% ", static_cast<int>( dam_amt ),
                                          clamp( static_cast<int>( du.damage_multiplier * 100 ), 0, 999 ) );
                } else {
                    float dam_amt = dam.total_damage();
                    row += string_format( "| %-8d ", static_cast<int>( dam_amt ) );
                }
                if( du.res_mult != 1.0f ) {
                    row += string_format( "| %-3d/%3d%%",
                                          static_cast<int>( du.res_pen ), static_cast<int>( 100 * du.res_mult ) );
                } else {
                    row += string_format( "| %-8d", static_cast<int>( du.res_pen ) );
                }
            } else {
                row += "|          |          ";
            }
        }
        return row;
    };

    const ammotype base_ammotype( base.ammo_default().str() );
    itype_id last = uistate.lastreload[ base_ammotype ];
    // We keep the last key so that pressing the key twice (for example, r-r for reload)
    // will always pick the first option on the list.
    int last_key = inp_mngr.get_previously_pressed_key();
    bool last_key_bound = false;
    // This is the entry that has out default
    int default_to = 0;

    // If last_key is RETURN, don't use that to override hotkey
    if( last_key == '\n' ) {
        last_key_bound = true;
        default_to = -1;
    }

    for( auto i = 0; i < static_cast<int>( opts.size() ); ++i ) {
        const item &ammo = opts[ i ].ammo->is_ammo_container() ? opts[ i ].ammo->contents.front() :
                           *opts[ i ].ammo;

        char hotkey = -1;
        if( has_item( ammo ) ) {
            // if ammo in player possession and either it or any container has a valid invlet use this
            if( ammo.invlet ) {
                hotkey = ammo.invlet;
            } else {
                for( const auto obj : parents( ammo ) ) {
                    if( obj->invlet ) {
                        hotkey = obj->invlet;
                        break;
                    }
                }
            }
        }
        if( last == ammo.typeId() ) {
            if( !last_key_bound && hotkey == -1 ) {
                // If this is the first occurrence of the most recently used type of ammo and the hotkey
                // was not already set above then set it to the keypress that opened this prompt
                hotkey = last_key;
                last_key_bound = true;
            }
            if( !last_key_bound ) {
                // Pressing the last key defaults to the first entry of compatible type
                default_to = i;
                last_key_bound = true;
            }
        }
        if( hotkey == last_key ) {
            last_key_bound = true;
            // Prevent the default from being used: key is bound to something already
            default_to = -1;
        }

        menu.addentry( i, true, hotkey, draw_row( i ) );
    }

    struct reload_callback : public uilist_callback {
        public:
            std::vector<item::reload_option> &opts;
            const std::function<std::string( int )> draw_row;
            int last_key;
            const int default_to;
            const bool can_partial_reload;

            reload_callback( std::vector<item::reload_option> &_opts,
                             std::function<std::string( int )> _draw_row,
                             int _last_key, int _default_to, bool _can_partial_reload ) :
                opts( _opts ), draw_row( _draw_row ),
                last_key( _last_key ), default_to( _default_to ),
                can_partial_reload( _can_partial_reload )
            {}

            bool key( const input_context &, const input_event &event, int idx, uilist *menu ) override {
                auto cur_key = event.get_first_input();
                if( default_to != -1 && cur_key == last_key ) {
                    // Select the first entry on the list
                    menu->ret = default_to;
                    return true;
                }
                if( idx < 0 || idx >= static_cast<int>( opts.size() ) ) {
                    return false;
                }
                auto &sel = opts[ idx ];
                switch( cur_key ) {
                    case KEY_LEFT:
                        if( can_partial_reload ) {
                            sel.qty( sel.qty() - 1 );
                            menu->entries[ idx ].txt = draw_row( idx );
                        }
                        return true;

                    case KEY_RIGHT:
                        if( can_partial_reload ) {
                            sel.qty( sel.qty() + 1 );
                            menu->entries[ idx ].txt = draw_row( idx );
                        }
                        return true;
                }
                return false;
            }
    } cb( opts, draw_row, last_key, default_to, !base.has_flag( "RELOAD_ONE" ) );
    menu.callback = &cb;

    menu.query();
    if( menu.ret < 0 || static_cast<size_t>( menu.ret ) >= opts.size() ) {
        add_msg_if_player( m_info, _( "Never mind." ) );
        return item::reload_option();
    }

    const item_location &sel = opts[ menu.ret ].ammo;
    uistate.lastreload[ ammotype( base.ammo_default().str() ) ] = sel->is_ammo_container() ?
            sel->contents.front().typeId() :
            sel->typeId();
    return opts[ menu.ret ];
}

bool player::list_ammo( const item &base, std::vector<item::reload_option> &ammo_list,
                        bool include_empty_mags, bool include_potential ) const
{
    auto opts = base.gunmods();
    opts.push_back( &base );

    if( base.magazine_current() ) {
        opts.push_back( base.magazine_current() );
    }

    for( const auto mod : base.gunmods() ) {
        if( mod->magazine_current() ) {
            opts.push_back( mod->magazine_current() );
        }
    }

    bool ammo_match_found = false;
    int ammo_search_range = is_mounted() ? -1 : 1;
    for( const auto e : opts ) {
        for( const item_location &ammo : find_ammo( *e, include_empty_mags, ammo_search_range ) ) {
            // don't try to unload frozen liquids
            if( ammo->is_watertight_container() && ammo->contents_made_of( SOLID ) ) {
                continue;
            }
            auto id = ( ammo->is_ammo_container() || ammo->is_container() )
                      ? ammo->contents.front().typeId()
                      : ammo->typeId();
            const bool can_reload_with = e->can_reload_with( id );
            if( can_reload_with ) {
                // Speedloaders require an empty target.
                if( include_potential || !ammo->has_flag( "SPEEDLOADER" ) || e->ammo_remaining() < 1 ) {
                    ammo_match_found = true;
                }
            }
            if( ( include_potential && can_reload_with )
                || can_reload( *e, id ) || e->has_flag( "RELOAD_AND_SHOOT" ) ) {
                ammo_list.emplace_back( this, e, &base, ammo );
            }
        }
    }
    return ammo_match_found;
}

item::reload_option player::select_ammo( const item &base, bool prompt,
        bool include_empty_mags, bool include_potential ) const
{
    std::vector<item::reload_option> ammo_list;
    const bool ammo_match_found = list_ammo( base, ammo_list, include_empty_mags, include_potential );

    if( ammo_list.empty() ) {
        if( !is_npc() ) {
            if( !base.is_magazine() && !base.magazine_integral() && !base.magazine_current() ) {
                add_msg_if_player( m_info, _( "You need a compatible magazine to reload the %s!" ),
                                   base.tname() );

            } else if( ammo_match_found ) {
                add_msg_if_player( m_info, _( "Nothing to reload!" ) );
            } else {
                std::string name;
                if( base.ammo_data() ) {
                    name = base.ammo_data()->nname( 1 );
                } else if( base.is_watertight_container() ) {
                    name = base.is_container_empty() ? "liquid" : base.contents.front().tname();
                } else {
                    name = enumerate_as_string( base.ammo_types().begin(),
                    base.ammo_types().end(), []( const ammotype & at ) {
                        return at->name();
                    }, enumeration_conjunction::none );
                }
                add_msg_if_player( m_info, _( "You don't have any %s to reload your %s!" ),
                                   name, base.tname() );
            }
        }
        return item::reload_option();
    }

    // sort in order of move cost (ascending), then remaining ammo (descending) with empty magazines always last
    std::stable_sort( ammo_list.begin(), ammo_list.end(), []( const item::reload_option & lhs,
    const item::reload_option & rhs ) {
        return lhs.ammo->ammo_remaining() > rhs.ammo->ammo_remaining();
    } );
    std::stable_sort( ammo_list.begin(), ammo_list.end(), []( const item::reload_option & lhs,
    const item::reload_option & rhs ) {
        return lhs.moves() < rhs.moves();
    } );
    std::stable_sort( ammo_list.begin(), ammo_list.end(), []( const item::reload_option & lhs,
    const item::reload_option & rhs ) {
        return ( lhs.ammo->ammo_remaining() != 0 ) > ( rhs.ammo->ammo_remaining() != 0 );
    } );

    if( is_npc() ) {
        return ammo_list[ 0 ];
    }

    if( !prompt && ammo_list.size() == 1 ) {
        // unconditionally suppress the prompt if there's only one option
        return ammo_list[ 0 ];
    }

    return select_ammo( base, std::move( ammo_list ) );
}

ret_val<bool> player::can_wield( const item &it ) const
{
    if( it.made_of( LIQUID ) ) {
        return ret_val<bool>::make_failure( _( "Can't wield spilt liquids." ) );
    }

    if( get_working_arm_count() <= 0 ) {
        return ret_val<bool>::make_failure(
                   _( "You need at least one arm to even consider wielding something." ) );
    }

    if( is_armed() && weapon.has_flag( "NO_UNWIELD" ) ) {
        return ret_val<bool>::make_failure( _( "The %s is preventing you from wielding the %s." ),
                                            character_funcs::fmt_wielded_weapon( *this ), it.tname() );
    }

    monster *mount = mounted_creature.get();
    if( it.is_two_handed( *this ) && ( !has_two_arms() || worn_with_flag( flag_RESTRICT_HAND ) ) &&
        !( is_mounted() && mount->has_flag( MF_RIDEABLE_MECH ) &&
           mount->type->mech_weapon && it.typeId() == mount->type->mech_weapon ) ) {
        if( worn_with_flag( flag_RESTRICT_HAND ) ) {
            return ret_val<bool>::make_failure(
                       _( "Something you are wearing hinders the use of both hands." ) );
        } else if( it.has_flag( "ALWAYS_TWOHAND" ) ) {
            return ret_val<bool>::make_failure( _( "The %s can't be wielded with only one arm." ),
                                                it.tname() );
        } else {
            return ret_val<bool>::make_failure( _( "You are too weak to wield %s with only one arm." ),
                                                it.tname() );
        }
    }
    if( is_mounted() && mount->has_flag( MF_RIDEABLE_MECH ) &&
        mount->type->mech_weapon && it.typeId() != mount->type->mech_weapon ) {
        return ret_val<bool>::make_failure( _( "You cannot wield anything while piloting a mech." ) );
    }

    return ret_val<bool>::make_success();
}

bool player::unwield()
{
    if( weapon.is_null() ) {
        return true;
    }

    if( !can_unwield( weapon ).success() ) {
        return false;
    }

    const std::string query = string_format( _( "Stop wielding %s?" ), weapon.tname() );

    if( !dispose_item( item_location( *this, &weapon ), query ) ) {
        return false;
    }

    inv.unsort();

    return true;
}

// ids of martial art styles that are available with the bio_cqb bionic.
static const std::vector<matype_id> bio_cqb_styles{ {
        matype_id{ "style_aikido" },
        matype_id{ "style_biojutsu" },
        matype_id{ "style_boxing" },
        matype_id{ "style_capoeira" },
        matype_id{ "style_crane" },
        matype_id{ "style_dragon" },
        matype_id{ "style_judo" },
        matype_id{ "style_karate" },
        matype_id{ "style_krav_maga" },
        matype_id{ "style_leopard" },
        matype_id{ "style_muay_thai" },
        matype_id{ "style_ninjutsu" },
        matype_id{ "style_pankration" },
        matype_id{ "style_snake" },
        matype_id{ "style_taekwondo" },
        matype_id{ "style_tai_chi" },
        matype_id{ "style_tiger" },
        matype_id{ "style_wingchun" },
        matype_id{ "style_zui_quan" }
    }};

bool character_martial_arts::pick_style( const avatar &you )    // Style selection menu
{
    enum style_selection {
        KEEP_HANDS_FREE = 0,
        STYLE_OFFSET
    };

    // If there are style already, cursor starts there
    // if no selected styles, cursor starts from no-style

    // Any other keys quit the menu
    const std::vector<matype_id> &selectable_styles = you.has_active_bionic(
                bio_cqb ) ? bio_cqb_styles :
            ma_styles;

    input_context ctxt( "MELEE_STYLE_PICKER" );
    ctxt.register_action( "SHOW_DESCRIPTION" );

    uilist kmenu;
    kmenu.text = string_format( _( "Select a style.\n"
                                   "\n"
                                   "STR: <color_white>%d</color>, DEX: <color_white>%d</color>, "
                                   "PER: <color_white>%d</color>, INT: <color_white>%d</color>\n"
                                   "Press [<color_yellow>%s</color>] for more info.\n" ),
                                you.get_str(), you.get_dex(), you.get_per(), you.get_int(),
                                ctxt.get_desc( "SHOW_DESCRIPTION" ) );
    ma_style_callback callback( static_cast<size_t>( STYLE_OFFSET ), selectable_styles );
    kmenu.callback = &callback;
    kmenu.input_category = "MELEE_STYLE_PICKER";
    kmenu.additional_actions.emplace_back( "SHOW_DESCRIPTION", translation() );
    kmenu.desc_enabled = true;
    kmenu.addentry_desc( KEEP_HANDS_FREE, true, 'h',
                         keep_hands_free ? _( "Keep hands free (on)" ) : _( "Keep hands free (off)" ),
                         _( "When this is enabled, player won't wield things unless explicitly told to." ) );

    kmenu.selected = STYLE_OFFSET;

    for( size_t i = 0; i < selectable_styles.size(); i++ ) {
        auto &style = selectable_styles[i].obj();
        //Check if this style is currently selected
        const bool selected = selectable_styles[i] == style_selected;
        std::string entry_text = style.name.translated();
        if( selected ) {
            kmenu.selected = i + STYLE_OFFSET;
            entry_text = colorize( entry_text, c_pink );
        }
        kmenu.addentry_desc( i + STYLE_OFFSET, true, -1, entry_text, style.description.translated() );
    }

    kmenu.query();
    int selection = kmenu.ret;

    if( selection >= STYLE_OFFSET ) {
        style_selected = selectable_styles[selection - STYLE_OFFSET];
        martialart_use_message( you );
    } else if( selection == KEEP_HANDS_FREE ) {
        keep_hands_free = !keep_hands_free;
    } else {
        return false;
    }

    return true;
}

bool player::can_reload( const item &it, const itype_id &ammo ) const
{
    if( !it.is_reloadable_with( ammo ) ) {
        return false;
    }

    if( it.is_ammo_belt() ) {
        const auto &linkage = it.type->magazine->linkage;
        if( linkage && !has_charges( *linkage, 1 ) ) {
            return false;
        }
    }

    return true;
}

void player::mend_item( item_location &&obj, bool interactive )
{
    if( has_trait( trait_DEBUG_HS ) ) {
        uilist menu;
        menu.text = _( "Toggle which fault?" );
        std::vector<std::pair<fault_id, bool>> opts;
        for( const auto &f : obj->faults_potential() ) {
            opts.emplace_back( f, !!obj->faults.count( f ) );
            menu.addentry( -1, true, -1, string_format(
                               opts.back().second ? pgettext( "fault", "Mend: %s" ) : pgettext( "fault", "Set: %s" ),
                               f.obj().name() ) );
        }
        if( opts.empty() ) {
            add_msg( m_info, _( "The %s doesn't have any faults to toggle." ), obj->tname() );
            return;
        }
        menu.query();
        if( menu.ret >= 0 ) {
            if( opts[ menu.ret ].second ) {
                obj->faults.erase( opts[ menu.ret ].first );
            } else {
                obj->faults.insert( opts[ menu.ret ].first );
            }
        }
        return;
    }

    auto inv = crafting_inventory();

    struct mending_option {
        fault_id fault;
        std::reference_wrapper<const mending_method> method;
        bool doable;
    };

    std::vector<mending_option> mending_options;
    for( const fault_id &f : obj->faults ) {
        for( const auto &m : f->mending_methods() ) {
            mending_option opt { f, m.second, true };
            for( const auto &sk : m.second.skills ) {
                if( get_skill_level( sk.first ) < sk.second ) {
                    opt.doable = false;
                    break;
                }
            }
            opt.doable = opt.doable &&
                         m.second.requirements->can_make_with_inventory( inv, is_crafting_component );
            mending_options.emplace_back( opt );
        }
    }

    if( mending_options.empty() ) {
        if( interactive ) {
            add_msg( m_info, _( "The %s doesn't have any faults to mend." ), obj->tname() );
            if( obj->damage() > 0 ) {
                const std::set<itype_id> &rep = obj->repaired_with();
                if( !rep.empty() ) {
                    const std::string repair_options =
                    enumerate_as_string( rep.begin(), rep.end(), []( const itype_id & e ) {
                        return item::nname( e );
                    }, enumeration_conjunction::or_ );

                    add_msg( m_info, _( "It is damaged, and could be repaired with %s.  "
                                        "%s to use one of those items." ),
                             repair_options, press_x( ACTION_USE ) );
                }
            }
        }
        return;
    }

    int sel = 0;
    if( interactive ) {
        uilist menu;
        menu.text = _( "Mend which fault?" );
        menu.desc_enabled = true;
        menu.desc_lines_hint = 0; // Let uilist handle description height

        constexpr int fold_width = 80;

        for( const mending_option &opt : mending_options ) {
            const mending_method &method = opt.method;
            const nc_color col = opt.doable ? c_white : c_light_gray;

            auto reqs = method.requirements.obj();
            auto tools = reqs.get_folded_tools_list( fold_width, col, inv );
            auto comps = reqs.get_folded_components_list( fold_width, col, inv, is_crafting_component );

            std::string descr;
            if( method.turns_into ) {
                descr += string_format( _( "Turns into: <color_cyan>%s</color>\n" ),
                                        method.turns_into->obj().name() );
            }
            if( method.also_mends ) {
                descr += string_format( _( "Also mends: <color_cyan>%s</color>\n" ),
                                        method.also_mends->obj().name() );
            }
            descr += string_format( _( "Time required: <color_cyan>%s</color>\n" ),
                                    to_string_approx( method.time ) );
            if( method.skills.empty() ) {
                descr += string_format( _( "Skills: <color_cyan>none</color>\n" ) );
            } else {
                descr += string_format( _( "Skills: %s\n" ),
                                        enumerate_as_string( method.skills.begin(), method.skills.end(),
                [this]( const std::pair<skill_id, int> &sk ) -> std::string {
                    if( get_skill_level( sk.first ) >= sk.second )
                    {
                        return string_format( pgettext( "skill requirement",
                                                        //~ %1$s: skill name, %2$s: current skill level, %3$s: required skill level
                                                        "<color_cyan>%1$s</color> <color_green>(%2$d/%3$d)</color>" ),
                                              sk.first->name(), get_skill_level( sk.first ), sk.second );
                    } else
                    {
                        return string_format( pgettext( "skill requirement",
                                                        //~ %1$s: skill name, %2$s: current skill level, %3$s: required skill level
                                                        "<color_cyan>%1$s</color> <color_yellow>(%2$d/%3$d)</color>" ),
                                              sk.first->name(), get_skill_level( sk.first ), sk.second );
                    }
                } ) );
            }

            for( const std::string &line : tools ) {
                descr += line + "\n";
            }
            for( const std::string &line : comps ) {
                descr += line + "\n";
            }

            const std::string desc = method.description + "\n\n" + colorize( descr, col );
            menu.addentry_desc( -1, true, -1, method.name.translated(), desc );
        }
        menu.query();
        if( menu.ret < 0 ) {
            add_msg( _( "Never mind." ) );
            return;
        }
        sel = menu.ret;
    }

    if( sel >= 0 ) {
        const mending_option &opt = mending_options[sel];
        if( !opt.doable ) {
            if( interactive ) {
                add_msg( m_info, _( "You are currently unable to mend the %s this way." ), obj->tname() );
            }
            return;
        }

        const mending_method &method = opt.method;
        assign_activity( activity_id( "ACT_MEND_ITEM" ), to_moves<int>( method.time ) );
        activity.name = opt.fault.str();
        activity.str_values.emplace_back( method.id );
        activity.targets.push_back( std::move( obj ) );
    }
}

int player::item_reload_cost( const item &it, const item &ammo, int qty ) const
{
    if( ammo.is_ammo() ) {
        qty = std::max( std::min( ammo.charges, qty ), 1 );
    } else if( ammo.is_ammo_container() || ammo.is_container() ) {
        qty = clamp( qty, ammo.contents.front().charges, 1 );
    } else if( ammo.is_magazine() ) {
        qty = 1;
    } else {
        debugmsg( "cannot determine reload cost as %s is neither ammo or magazine", ammo.tname() );
        return 0;
    }

    // If necessary create duplicate with appropriate number of charges
    item obj = ammo;
    obj = obj.split( qty );
    if( obj.is_null() ) {
        obj = ammo;
    }
    // No base cost for handling ammo - that's already included in obtain cost
    // We have the ammo in our hands right now
    int mv = item_handling_cost( obj, true, 0 );

    if( ammo.has_flag( "MAG_BULKY" ) ) {
        mv *= 1.5; // bulky magazines take longer to insert
    }

    if( !it.is_gun() && !it.is_magazine() ) {
        return mv + 100; // reload a tool or sealable container
    }

    /** @EFFECT_GUN decreases the time taken to reload a magazine */
    /** @EFFECT_PISTOL decreases time taken to reload a pistol */
    /** @EFFECT_SMG decreases time taken to reload an SMG */
    /** @EFFECT_RIFLE decreases time taken to reload a rifle */
    /** @EFFECT_SHOTGUN decreases time taken to reload a shotgun */
    /** @EFFECT_LAUNCHER decreases time taken to reload a launcher */

    int cost = ( it.is_gun() ? it.get_reload_time() : it.type->magazine->reload_time ) * qty;

    skill_id sk = it.is_gun() ? it.type->gun->skill_used : skill_gun;
    mv += cost / ( 1.0f + std::min( get_skill_level( sk ) * 0.1f, 1.0f ) );

    if( it.has_flag( "STR_RELOAD" ) ) {
        /** @EFFECT_STR reduces reload time of some weapons */
        mv -= get_str() * 20;
    }

    return std::max( mv, 25 );
}

cata::optional<std::list<item>::iterator>
player::wear( int pos, bool interactive )
{
    return wear( i_at( pos ), interactive );
}

cata::optional<std::list<item>::iterator>
player::wear( item &to_wear, bool interactive )
{
    if( is_worn( to_wear ) ) {
        if( interactive ) {
            add_msg_player_or_npc( m_info,
                                   _( "You are already wearing that." ),
                                   _( "<npcname> is already wearing that." )
                                 );
        }
        return cata::nullopt;
    }
    if( to_wear.is_null() ) {
        if( interactive ) {
            add_msg_player_or_npc( m_info,
                                   _( "You don't have that item." ),
                                   _( "<npcname> doesn't have that item." ) );
        }
        return cata::nullopt;
    }

    bool was_weapon;
    item to_wear_copy( to_wear );
    if( &to_wear == &weapon ) {
        weapon = item();
        was_weapon = true;
    } else {
        inv.remove_item( &to_wear );
        inv.restack( *this );
        was_weapon = false;
    }

    auto result = wear_item( to_wear_copy, interactive );
    if( !result ) {
        if( was_weapon ) {
            weapon = to_wear_copy;
        } else {
            inv.add_item( to_wear_copy, true );
        }
        return cata::nullopt;
    }

    return result;
}

bool player::can_lift( int lift_strength_required ) const
{
    // avoid comparing by weight as different objects use differing scales (grams vs kilograms etc)
    int str = get_str();
    if( mounted_creature ) {
        auto mons = mounted_creature.get();
        str = mons->mech_str_addition() == 0 ? str : mons->mech_str_addition();
    }
    const int npc_str = get_lift_assist();
    if( has_trait( trait_id( "STRONGBACK" ) ) ) {
        str *= 1.35;
    } else if( has_trait( trait_id( "BADBACK" ) ) ) {
        str /= 1.35;
    }
    return str + npc_str >= lift_strength_required;
}

ret_val<bool> player::can_takeoff( const item &it, const std::list<item> *res ) const
{
    auto iter = std::find_if( worn.begin(), worn.end(), [ &it ]( const item & wit ) {
        return &it == &wit;
    } );

    if( iter == worn.end() ) {
        return ret_val<bool>::make_failure( !is_npc() ? _( "You are not wearing that item." ) :
                                            _( "<npcname> is not wearing that item." ) );
    }

    if( res == nullptr && !get_dependent_worn_items( it ).empty() ) {
        return ret_val<bool>::make_failure( !is_npc() ?
                                            _( "You can't take off power armor while wearing other power armor components." ) :
                                            _( "<npcname> can't take off power armor while wearing other power armor components." ) );
    }
    if( it.has_flag( "NO_TAKEOFF" ) ) {
        return ret_val<bool>::make_failure( !is_npc() ?
                                            _( "You can't take that item off." ) :
                                            _( "<npcname> can't take that item off." ) );
    }
    return ret_val<bool>::make_success();
}

bool player::takeoff( item &it, std::list<item> *res )
{
    const auto ret = can_takeoff( it, res );
    if( !ret.success() ) {
        add_msg( m_info, "%s", ret.c_str() );
        return false;
    }

    auto iter = std::find_if( worn.begin(), worn.end(), [ &it ]( const item & wit ) {
        return &it == &wit;
    } );

    if( res == nullptr ) {
        if( volume_carried() + it.volume() > volume_capacity_reduced_by( it.get_storage() ) ) {
            if( is_npc() || query_yn( _( "No room in inventory for your %s.  Drop it?" ),
                                      colorize( it.tname(), it.color_in_inventory() ) ) ) {
                item_location loc( *this, &it );
                drop( loc, pos() );
                return true; // the drop activity ends up taking off the item anyway so shouldn't try to do it again here
            } else {
                return false;
            }
        }
        iter->on_takeoff( *this );
        inv.add_item_keep_invlet( it );
    } else {
        iter->on_takeoff( *this );
        res->push_back( it );
    }

    add_msg_player_or_npc( _( "You take off your %s." ),
                           _( "<npcname> takes off their %s." ),
                           it.tname() );

    // TODO: Make this variable
    mod_moves( -250 );
    worn.erase( iter );

    recalc_sight_limits();
    reset_encumbrance();

    return true;
}

bool player::takeoff( int pos )
{
    return takeoff( i_at( pos ) );
}

bool player::add_or_drop_with_msg( item &it, const bool unloading )
{
    if( it.made_of( LIQUID ) ) {
        liquid_handler::consume_liquid( it, 1 );
        return it.charges <= 0;
    }
    it.charges = this->i_add_to_container( it, unloading );
    if( it.is_ammo() && it.charges == 0 ) {
        return true;
    } else if( !this->can_pick_volume( it ) ) {
        put_into_vehicle_or_drop( *this, item_drop_reason::too_large, { it } );
    } else if( !this->can_pick_weight( it, !get_option<bool>( "DANGEROUS_PICKUPS" ) ) ) {
        put_into_vehicle_or_drop( *this, item_drop_reason::too_heavy, { it } );
    } else {
        auto &ni = this->i_add( it );
        add_msg( _( "You put the %s in your inventory." ), ni.tname() );
        add_msg( m_info, "%c - %s", ni.invlet == 0 ? ' ' : ni.invlet, ni.tname() );
    }
    return true;
}

bool player::unload( item_location loc )
{
    item &it = *loc.get_item();
    // Unload a container consuming moves per item successfully removed
    if( it.is_container() || it.is_bandolier() ) {
        if( it.contents.empty() ) {
            add_msg( m_info, _( "The %s is already empty!" ), it.tname() );
            return false;
        }
        if( !it.can_unload_liquid() ) {
            add_msg( m_info, _( "The liquid can't be unloaded in its current state!" ) );
            return false;
        }

        bool changed = false;
        for( item *contained : it.contents.all_items_top() ) {
            int old_charges = contained->charges;
            const bool consumed = this->add_or_drop_with_msg( *contained, true );
            changed = changed || consumed || contained->charges != old_charges;
            if( consumed ) {
                this->mod_moves( -this->item_handling_cost( *contained ) );
                it.remove_item( *contained );
            }
        }

        if( changed ) {
            it.on_contents_changed();
        }
        return true;
    }

    // If item can be unloaded more than once (currently only guns) prompt user to choose
    std::vector<std::string> msgs( 1, it.tname() );
    std::vector<item *> opts( 1, &it );

    for( auto e : it.gunmods() ) {
        if( e->is_gun() && !e->has_flag( "NO_UNLOAD" ) &&
            ( e->magazine_current() || e->ammo_remaining() > 0 || e->casings_count() > 0 ) ) {
            msgs.emplace_back( e->tname() );
            opts.emplace_back( e );
        }
    }

    item *target = nullptr;
    if( opts.size() > 1 ) {
        const int ret = uilist( _( "Unload what?" ), msgs );
        if( ret >= 0 ) {
            target = opts[ret];
        }
    } else {
        target = &it;
    }

    if( target == nullptr ) {
        return false;
    }

    // Next check for any reasons why the item cannot be unloaded
    if( target->ammo_types().empty() || target->ammo_capacity() <= 0 ) {
        add_msg( m_info, _( "You can't unload a %s!" ), target->tname() );
        return false;
    }

    if( target->has_flag( "NO_UNLOAD" ) ) {
        if( target->has_flag( "RECHARGE" ) || target->has_flag( "USE_UPS" ) ) {
            add_msg( m_info, _( "You can't unload a rechargeable %s!" ), target->tname() );
        } else {
            add_msg( m_info, _( "You can't unload a %s!" ), target->tname() );
        }
        return false;
    }

    if( !target->magazine_current() && target->ammo_remaining() <= 0 && target->casings_count() <= 0 ) {
        if( target->is_tool() ) {
            add_msg( m_info, _( "Your %s isn't charged." ), target->tname() );
        } else {
            add_msg( m_info, _( "Your %s isn't loaded." ), target->tname() );
        }
        return false;
    }

    target->casings_handle( [&]( item & e ) {
        return this->i_add_or_drop( e );
    } );

    if( target->is_magazine() ) {
        // Calculate the time to remove the contained ammo (consuming half as much time as required to load the magazine)
        int mv = 0;
        int qty = 0;
        std::vector<item *> remove_contained;
        for( item *contained : it.contents.all_items_top() ) {
            mv += this->item_reload_cost( it, *contained, contained->charges ) / 2;
            if( add_or_drop_with_msg( *contained, true ) ) {
                qty += contained->charges;
                remove_contained.push_back( contained );
            }
        }
        // remove the ammo leads in the belt
        for( item *remove : remove_contained ) {
            it.remove_item( *remove );
        }

        // remove the belt linkage
        if( it.is_ammo_belt() ) {
            if( it.type->magazine->linkage ) {
                item link( *it.type->magazine->linkage, calendar::turn, qty );
                add_or_drop_with_msg( link, true );
            }
            add_msg( _( "You disassemble your %s." ), it.tname() );
        } else {
            add_msg( _( "You unload your %s." ), it.tname() );
        }

        mod_moves( -std::min( 200, mv ) );
        if( loc->has_flag( "MAG_DESTROY" ) && loc->ammo_remaining() == 0 ) {
            loc.remove_item();
        }
        return true;

    } else if( target->magazine_current() ) {
        if( !this->add_or_drop_with_msg( *target->magazine_current(), true ) ) {
            return false;
        }
        // Eject magazine consuming half as much time as required to insert it
        this->moves -= this->item_reload_cost( *target, *target->magazine_current(), -1 ) / 2;

        target->remove_items_with( [&target]( const item & e ) {
            return target->magazine_current() == &e;
        } );

    } else if( target->ammo_remaining() ) {
        int qty = target->ammo_remaining();

        if( target->ammo_current() == itype_plut_cell ) {
            qty = target->ammo_remaining() / PLUTONIUM_CHARGES;
            if( qty > 0 ) {
                add_msg( _( "You recover %i unused plutonium." ), qty );
            } else {
                add_msg( m_info, _( "You can't remove partially depleted plutonium!" ) );
                return false;
            }
        }

        // Construct a new ammo item and try to drop it
        item ammo( target->ammo_current(), calendar::turn, qty );
        if( target->is_filthy() ) {
            ammo.set_flag( "FILTHY" );
        }

        if( ammo.made_of( LIQUID ) ) {
            if( !this->add_or_drop_with_msg( ammo ) ) {
                qty -= ammo.charges; // only handled part (or none) of the liquid
            }
            if( qty <= 0 ) {
                return false; // no liquid was moved
            }

        } else if( !this->add_or_drop_with_msg( ammo, qty > 1 ) ) {
            return false;
        }

        // If successful remove appropriate qty of ammo consuming half as much time as required to load it
        this->moves -= this->item_reload_cost( *target, ammo, qty ) / 2;

        if( target->ammo_current() == itype_plut_cell ) {
            qty *= PLUTONIUM_CHARGES;
        }

        target->ammo_set( target->ammo_current(), target->ammo_remaining() - qty );
    }

    // Turn off any active tools
    if( target->is_tool() && target->active && target->ammo_remaining() == 0 ) {
        target->type->invoke( *this, *target, this->pos() );
    }

    add_msg( _( "You unload your %s." ), target->tname() );
    return true;

}


void player::use_wielded()
{
    use( -1 );
}

void player::use( int inventory_position )
{
    item &used = i_at( inventory_position );
    auto loc = item_location( *this, &used );

    use( loc );
}

static bool is_pet_food( const item &itm )
{
    return itm.type->can_use( "DOGFOOD" ) ||
           itm.type->can_use( "CATFOOD" ) ||
           itm.type->can_use( "BIRDFOOD" ) ||
           itm.type->can_use( "CATTLEFODDER" );
}

void player::use( item_location loc )
{
    item &used = *loc.get_item();

    if( used.is_null() ) {
        add_msg( m_info, _( "You do not have that item." ) );
        return;
    }

    last_item = used.typeId();

    if( used.is_tool() ) {
        if( !used.type->has_use() ) {
            add_msg_if_player( _( "You can't do anything interesting with your %s." ), used.tname() );
            return;
        }
        invoke_item( &used, loc.position() );

    } else if( is_pet_food( used ) ) {
        invoke_item( &used, loc.position() );

    } else if( !used.is_container_empty() && is_pet_food( used.get_contained() ) ) {
        unload( loc );

    } else if( !used.is_craft() && ( used.is_medication() || ( !used.type->has_use() &&
                                     ( used.is_food() ||
                                       used.get_contained().is_food() ||
                                       used.get_contained().is_medication() ) ) ) ) {
        consume( loc );

    } else if( used.is_book() ) {
        // TODO: Handle this with dynamic dispatch.
        if( avatar *u = as_avatar() ) {
            u->read( loc );
        }
    } else if( used.type->has_use() ) {
        invoke_item( &used, loc.position() );
    } else if( used.has_flag( flag_SPLINT ) ) {
        ret_val<bool> need_splint = can_wear( used );
        if( need_splint.success() ) {
            wear_item( used );
            loc.remove_item();
        } else {
            add_msg( m_info, need_splint.str() );
        }
    } else {
        add_msg( m_info, _( "You can't do anything interesting with your %s." ),
                 used.tname() );
    }
    recalculate_enchantment_cache();
}

void player::reassign_item( item &it, int invlet )
{
    bool remove_old = true;
    if( invlet ) {
        item *prev = invlet_to_item( invlet );
        if( prev != nullptr ) {
            remove_old = it.typeId() != prev->typeId();
            inv.reassign_item( *prev, it.invlet, remove_old );
        }
    }

    if( !invlet || inv_chars.valid( invlet ) ) {
        const auto iter = inv.assigned_invlet.find( it.invlet );
        bool found = iter != inv.assigned_invlet.end();
        if( found ) {
            inv.assigned_invlet.erase( iter );
        }
        if( invlet && ( !found || it.invlet != invlet ) ) {
            inv.assigned_invlet[invlet] = it.typeId();
        }
        inv.reassign_item( it, invlet, remove_old );
    }
}

static bool has_mod( const item &gun, const item &mod )
{
    for( const item *toolmod : gun.gunmods() ) {
        if( &mod == toolmod ) {
            return true;
        }
    }
    return false;
}

bool player::gunmod_remove( item &gun, item &mod )
{
    if( !has_mod( gun, mod ) ) {
        debugmsg( "Cannot remove non-existent gunmod" );
        return false;
    }

    item_location loc = item_location( *this, &mod );
    if( mod.ammo_remaining() && !unload( loc ) ) {
        return false;
    }

    gun.gun_set_mode( gun_mode_id( "DEFAULT" ) );
    //TODO: add activity for removing gunmods

    if( mod.typeId() == itype_brass_catcher ) {
        gun.casings_handle( [&]( item & e ) {
            return i_add_or_drop( e );
        } );
    }

    const itype *modtype = mod.type;

    i_add_or_drop( mod );
    gun.remove_item( mod );

    //If the removed gunmod added mod locations, check to see if any mods are in invalid locations
    if( !modtype->gunmod->add_mod.empty() ) {
        std::map<gunmod_location, int> mod_locations = gun.get_mod_locations();
        for( const auto &slot : mod_locations ) {
            int free_slots = gun.get_free_mod_locations( slot.first );

            for( auto the_mod : gun.gunmods() ) {
                if( the_mod->type->gunmod->location == slot.first && free_slots < 0 ) {
                    gunmod_remove( gun, *the_mod );
                    free_slots++;
                } else if( mod_locations.find( the_mod->type->gunmod->location ) ==
                           mod_locations.end() ) {
                    gunmod_remove( gun, *the_mod );
                }
            }
        }
    }

    //~ %1$s - gunmod, %2$s - gun.
    add_msg_if_player( _( "You remove your %1$s from your %2$s." ), modtype->nname( 1 ),
                       gun.tname() );

    return true;
}

std::pair<int, int> player::gunmod_installation_odds( const item &gun, const item &mod ) const
{
    // Mods with INSTALL_DIFFICULT have a chance to fail, potentially damaging the gun
    if( !mod.has_flag( "INSTALL_DIFFICULT" ) || has_trait( trait_DEBUG_HS ) ) {
        return std::make_pair( 100, 0 );
    }

    int roll = 100; // chance of success (%)
    int risk = 0;   // chance of failure (%)
    int chances = 1; // start with 1 in 6 (~17% chance)

    for( const auto &e : mod.type->min_skills ) {
        // gain an additional chance for every level above the minimum requirement
        skill_id sk = e.first == skill_weapon ? gun.gun_skill() : e.first;
        chances += std::max( get_skill_level( sk ) - e.second, 0 );
    }
    // cap success from skill alone to 1 in 5 (~83% chance)
    roll = std::min( static_cast<double>( chances ), 5.0 ) / 6.0 * 100;
    // focus is either a penalty or bonus of at most +/-10%
    roll += ( std::min( std::max( focus_pool, 140 ), 60 ) - 100 ) / 4;
    // dexterity and intelligence give +/-2% for each point above or below 12
    roll += ( get_dex() - 12 ) * 2;
    roll += ( get_int() - 12 ) * 2;
    // each level of damage to the base gun reduces success by 10%
    roll -= std::max( gun.damage_level( 4 ), 0 ) * 10;
    roll = std::min( std::max( roll, 0 ), 100 );

    // risk of causing damage on failure increases with less durable guns
    risk = ( 100 - roll ) * ( ( 10.0 - std::min( gun.type->gun->durability, 9 ) ) / 10.0 );

    return std::make_pair( roll, risk );
}

void player::gunmod_add( item &gun, item &mod )
{
    if( !gun.is_gunmod_compatible( mod ).success() ) {
        debugmsg( "Tried to add incompatible gunmod" );
        return;
    }

    if( !has_item( gun ) && !has_item( mod ) ) {
        debugmsg( "Tried gunmod installation but mod/gun not in player possession" );
        return;
    }

    // first check at least the minimum requirements are met
    if( !has_trait( trait_DEBUG_HS ) && !can_use( mod, gun ) ) {
        return;
    }

    // any (optional) tool charges that are used during installation
    auto odds = gunmod_installation_odds( gun, mod );
    int roll = odds.first;
    int risk = odds.second;

    std::string tool;
    int qty = 0;
    bool requery = false;

    item modded = gun;
    modded.put_in( mod );
    bool no_magazines = modded.common_ammo_default().is_null();

    std::string query_msg = mod.is_irremovable()
                            ? _( "<color_yellow>Permanently</color> install your %1$s in your %2$s?" )
                            : _( "Attach your %1$s to your %2$s?" );
    if( no_magazines ) {
        query_msg += "\n";
        query_msg += colorize(
                         _( "Warning: after installing this mod, a magazine adapter mod will be required to load it!" ),
                         c_red );
    }

    uilist prompt;
    prompt.text = string_format( query_msg,
                                 colorize( mod.tname(), mod.color_in_inventory() ),
                                 colorize( gun.tname(), gun.color_in_inventory() ) );

    std::vector<std::function<void()>> actions;

    if( roll < 100 ) {
        prompt.addentry( -1, true, 'w',
                         string_format( _( "Try without tools (%i%%) risking damage (%i%%)" ), roll, risk ) );
        actions.emplace_back( [&] {} );

        prompt.addentry( -1, has_charges( itype_small_repairkit, 100 ), 'f',
                         string_format( _( "Use 100 charges of firearm repair kit (%i%%)" ), std::min( roll * 2, 100 ) ) );

        actions.emplace_back( [&] {
            tool = "small_repairkit";
            qty = 100;
            roll *= 2; // firearm repair kit improves success...
            risk /= 2; // ...and reduces the risk of damage upon failure
        } );

        prompt.addentry( -1, has_charges( itype_large_repairkit, 25 ), 'g',
                         string_format( _( "Use 25 charges of gunsmith repair kit (%i%%)" ), std::min( roll * 3, 100 ) ) );

        actions.emplace_back( [&] {
            tool = "large_repairkit";
            qty = 25;
            roll *= 3; // gunsmith repair kit improves success markedly...
            risk = 0;  // ...and entirely prevents damage upon failure
        } );
    } else {
        prompt.addentry( -1, true, 'w', _( "Install without tools" ) );
        actions.emplace_back( [&] {} );
    }

    prompt.addentry( -1, true, 'c', _( "Compare before/after installation" ) );
    actions.emplace_back( [&] {
        requery = true;
        game_menus::inv::compare( gun, modded );
    } );

    do {
        requery = false;
        prompt.query();
        if( prompt.ret < 0 ) {
            add_msg_if_player( _( "Never mind." ) );
            return; // player canceled installation
        }
        actions[ prompt.ret ]();
    } while( requery );

    const int moves = !has_trait( trait_DEBUG_HS ) ? mod.type->gunmod->install_time : 0;

    assign_activity( activity_id( "ACT_GUNMOD_ADD" ), moves, -1, 0, tool );
    activity.targets.push_back( item_location( *this, &gun ) );
    activity.targets.push_back( item_location( *this, &mod ) );
    activity.values.push_back( 0 ); // dummy value
    activity.values.push_back( roll ); // chance of success (%)
    activity.values.push_back( risk ); // chance of damage (%)
    activity.values.push_back( qty ); // tool charges
}

void player::toolmod_add( item_location tool, item_location mod )
{
    if( !tool && !mod ) {
        debugmsg( "Tried toolmod installation but mod/tool not available" );
        return;
    }
    // first check at least the minimum requirements are met
    if( !has_trait( trait_DEBUG_HS ) && !can_use( *mod, *tool ) ) {
        return;
    }

    if( !query_yn( _( "Permanently install your %1$s in your %2$s?" ),
                   colorize( mod->tname(), mod->color_in_inventory() ),
                   colorize( tool->tname(), tool->color_in_inventory() ) ) ) {
        add_msg_if_player( _( "Never mind." ) );
        return; // player canceled installation
    }

    assign_activity( activity_id( "ACT_TOOLMOD_ADD" ), 1, -1 );
    activity.targets.emplace_back( std::move( tool ) );
    activity.targets.emplace_back( std::move( mod ) );
}

bool player::studied_all_recipes( const itype &book ) const
{
    if( !book.book ) {
        return true;
    }
    for( auto &elem : book.book->recipes ) {
        if( !knows_recipe( elem.recipe ) ) {
            return false;
        }
    }
    return true;
}

recipe_subset player::get_recipes_from_books( const inventory &crafting_inv,
        recipe_filter filter ) const
{
    recipe_subset res;

    for( const auto &stack : crafting_inv.const_slice() ) {
        const item &candidate = stack->front();

        for( std::pair<const recipe *, int> recipe_entry :
             candidate.get_available_recipes( *this ) ) {
            if( filter && !filter( *recipe_entry.first ) ) {
                continue;
            }
            res.include( recipe_entry.first, recipe_entry.second );
        }
    }

    return res;
}

recipe_subset player::get_available_recipes( const inventory &crafting_inv,
        const std::vector<npc *> *helpers, recipe_filter filter ) const
{
    recipe_subset res;

    if( filter ) {
        res.include_if( get_learned_recipes(), filter );
    } else {
        res.include( get_learned_recipes() );
    }

    res.include( get_recipes_from_books( crafting_inv, filter ) );

    if( helpers != nullptr ) {
        for( npc *np : *helpers ) {
            // Directly form the helper's inventory
            res.include( get_recipes_from_books( np->inv, filter ) );
            // Being told what to do
            res.include_if( np->get_learned_recipes(), [ this, &filter ]( const recipe & r ) {
                if( filter && !filter( r ) ) {
                    return false;
                }
                // Skilled enough to understand
                return get_skill_level( r.skill_used ) >= static_cast<int>( r.difficulty * 0.8f );
            } );
        }
    }

    return res;
}

void player::practice( const skill_id &id, int amount, int cap, bool suppress_warning )
{
    SkillLevel &level = get_skill_level_object( id );
    const Skill &skill = id.obj();
    std::string skill_name = skill.name();

    if( !level.can_train() && !in_sleep_state() ) {
        // If leveling is disabled, don't train, don't drain focus, don't print anything
        // Leaving as a skill method rather than global for possible future skill cap setting
        return;
    }

    const auto highest_skill = [&]() {
        std::pair<skill_id, int> result( skill_id::NULL_ID(), -1 );
        for( const auto &pair : *_skills ) {
            const SkillLevel &lobj = pair.second;
            if( lobj.level() > result.second ) {
                result = std::make_pair( pair.first, lobj.level() );
            }
        }
        return result.first;
    };

    const bool isSavant = has_trait( trait_SAVANT );
    const skill_id savantSkill = isSavant ? highest_skill() : skill_id::NULL_ID();

    amount = adjust_for_focus( amount );

    if( has_trait( trait_PACIFIST ) && skill.is_combat_skill() ) {
        if( !one_in( 3 ) ) {
            amount = 0;
        }
    }
    if( has_trait_flag( "PRED2" ) && skill.is_combat_skill() ) {
        if( one_in( 3 ) ) {
            amount *= 2;
        }
    }
    if( has_trait_flag( "PRED3" ) && skill.is_combat_skill() ) {
        amount *= 2;
    }

    if( has_trait_flag( "PRED4" ) && skill.is_combat_skill() ) {
        amount *= 3;
    }

    if( isSavant && id != savantSkill ) {
        amount /= 2;
    }

    if( amount > 0 && get_skill_level( id ) > cap ) { //blunt grinding cap implementation for crafting
        amount = 0;
        if( !suppress_warning ) {
            handle_skill_warning( id, false );
        }
    }
    if( amount > 0 && level.isTraining() ) {
        int oldLevel = get_skill_level( id );
        get_skill_level_object( id ).train( amount );
        int newLevel = get_skill_level( id );
        if( is_player() && newLevel > oldLevel ) {
            add_msg( m_good, _( "Your skill in %s has increased to %d!" ), skill_name, newLevel );
        }
        if( is_player() && newLevel > cap ) {
            //inform player immediately that the current recipe can't be used to train further
            add_msg( m_info, _( "You feel that %s tasks of this level are becoming trivial." ),
                     skill_name );
        }

        int chance_to_drop = focus_pool;
        focus_pool -= chance_to_drop / 100;
        // Apex Predators don't think about much other than killing.
        // They don't lose Focus when practicing combat skills.
        if( ( rng( 1, 100 ) <= ( chance_to_drop % 100 ) ) && ( !( has_trait_flag( "PRED4" ) &&
                skill.is_combat_skill() ) ) ) {
            focus_pool--;
        }
    }

    get_skill_level_object( id ).practice();
}

void player::handle_skill_warning( const skill_id &id, bool force_warning )
{
    //remind the player intermittently that no skill gain takes place
    if( is_player() && ( force_warning || one_in( 5 ) ) ) {
        SkillLevel &level = get_skill_level_object( id );

        const Skill &skill = id.obj();
        std::string skill_name = skill.name();
        int curLevel = level.level();

        add_msg( m_info, _( "This task is too simple to train your %s beyond %d." ),
                 skill_name, curLevel );
    }
}

bool player::has_recipe_requirements( const recipe &rec ) const
{
    return get_all_skills().has_recipe_requirements( rec );
}

int player::has_recipe( const recipe *r, const inventory &crafting_inv,
                        const std::vector<npc *> &helpers ) const
{
    if( !r->skill_used ) {
        return 0;
    }

    if( knows_recipe( r ) ) {
        return r->difficulty;
    }

    const auto available = get_available_recipes( crafting_inv, &helpers );
    return available.contains( *r ) ? available.get_custom_difficulty( r ) : -1;
}

bool player::wield_contents( item &container, item *internal_item, bool penalties,
                             int base_cost )
{
    // if index not specified and container has multiple items then ask the player to choose one
    if( internal_item == nullptr ) {
        std::vector<std::string> opts;
        std::list<item *> container_contents = container.contents.all_items_top();
        std::transform( container_contents.begin(), container_contents.end(),
        std::back_inserter( opts ), []( const item * elem ) {
            return elem->display_name();
        } );
        if( opts.size() > 1 ) {
            int pos = uilist( _( "Wield what?" ), opts );
            if( pos < 0 ) {
                return false;
            }
            internal_item = *std::next( container_contents.begin(), pos );
        } else {
            internal_item = &container.contents.front();
        }
    }

    if( !container.has_item( *internal_item ) ) {
        debugmsg( "Tried to wield non-existent item from container (player::wield_contents)" );
        return false;
    }

    const ret_val<bool> ret = can_wield( *internal_item );
    if( !ret.success() ) {
        add_msg_if_player( m_info, "%s", ret.c_str() );
        return false;
    }

    int mv = 0;

    if( is_armed() ) {
        if( !unwield() ) {
            return false;
        }
        inv.unsort();
    }

    weapon = std::move( *internal_item );
    container.remove_item( *internal_item );
    container.on_contents_changed();

    inv.update_invlet( weapon );
    inv.update_cache_with_item( weapon );
    last_item = weapon.typeId();

    /**
     * @EFFECT_PISTOL decreases time taken to draw pistols from holsters
     * @EFFECT_SMG decreases time taken to draw smgs from holsters
     * @EFFECT_RIFLE decreases time taken to draw rifles from holsters
     * @EFFECT_SHOTGUN decreases time taken to draw shotguns from holsters
     * @EFFECT_LAUNCHER decreases time taken to draw launchers from holsters
     * @EFFECT_STABBING decreases time taken to draw stabbing weapons from sheathes
     * @EFFECT_CUTTING decreases time taken to draw cutting weapons from scabbards
     * @EFFECT_BASHING decreases time taken to draw bashing weapons from holsters
     */
    int lvl = get_skill_level( weapon.is_gun() ? weapon.gun_skill() : weapon.melee_skill() );
    mv += item_handling_cost( weapon, penalties, base_cost ) / ( ( lvl + 10.0f ) / 10.0f );

    moves -= mv;

    weapon.on_wield( *this, mv );

    return true;
}

void player::store( item &container, item &put, bool penalties, int base_cost )
{
    moves -= item_store_cost( put, container, penalties, base_cost );
    container.put_in( i_rem( &put ) );
    reset_encumbrance();
}

nc_color encumb_color( int level )
{
    if( level < 0 ) {
        return c_green;
    }
    if( level < 10 ) {
        return c_light_gray;
    }
    if( level < 40 ) {
        return c_yellow;
    }
    if( level < 70 ) {
        return c_light_red;
    }
    return c_red;
}

float player::get_melee() const
{
    return get_skill_level( skill_id( "melee" ) );
}

bool player::uncanny_dodge()
{
    bool is_u = this == &g->u;
    bool seen = g->u.sees( *this );
    if( this->get_power_level() < 74_kJ || !this->has_active_bionic( bio_uncanny_dodge ) ) {
        return false;
    }
    tripoint adjacent = adjacent_tile();
    mod_power_level( -75_kJ );
    if( adjacent.x != posx() || adjacent.y != posy() ) {
        position.x = adjacent.x;
        position.y = adjacent.y;
        if( is_u ) {
            add_msg( _( "Time seems to slow down and you instinctively dodge!" ) );
        } else if( seen ) {
            add_msg( _( "%s dodges… so fast!" ), this->disp_name() );

        }
        return true;
    }
    if( is_u ) {
        add_msg( _( "You try to dodge but there's no room!" ) );
    } else if( seen ) {
        add_msg( _( "%s tries to dodge but there's no room!" ), this->disp_name() );
    }
    return false;
}

//message related stuff
void player::add_msg_if_player( const std::string &msg ) const
{
    Messages::add_msg( msg );
}

void player::add_msg_player_or_npc( const std::string &player_msg,
                                    const std::string &/*npc_msg*/ ) const
{
    Messages::add_msg( player_msg );
}

void player::add_msg_if_player( const game_message_params &params,
                                const std::string &msg ) const
{
    Messages::add_msg( params, msg );
}

void player::add_msg_player_or_npc( const game_message_params &params,
                                    const std::string &player_msg,
                                    const std::string &/*npc_msg*/ ) const
{
    Messages::add_msg( params, player_msg );
}

void player::add_msg_player_or_say( const std::string &player_msg,
                                    const std::string &/*npc_speech*/ ) const
{
    Messages::add_msg( player_msg );
}

void player::add_msg_player_or_say( const game_message_params &params,
                                    const std::string &player_msg,
                                    const std::string &/*npc_speech*/ ) const
{
    Messages::add_msg( params, player_msg );
}

bool player::query_yn( const std::string &mes ) const
{
    return ::query_yn( mes );
}

safe_reference<player> player::get_safe_reference()
{
    return anchor.reference_to( this );
}
