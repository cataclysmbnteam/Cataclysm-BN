#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "animation.h"
#include "avatar.h"
#include "bodypart.h"
#include "calendar.h"
#include "character.h"
#include "color.h"
#include "creature.h"
#include "damage.h"
#include "debug.h"
#include "enums.h"
#include "explosion.h"
#include "field.h"
#include "field_type.h"
#include "game.h"
#include "item.h"
#include "line.h"
#include "magic.h"
#include "magic_spell_effect_helpers.h"
#include "magic_teleporter_list.h"
#include "magic_ter_furn_transform.h"
#include "map.h"
#include "map_iterator.h"
#include "messages.h"
#include "monster.h"
#include "optional.h"
#include "overmapbuffer.h"
#include "player.h"
#include "point.h"
#include "projectile.h"
#include "ret_val.h"
#include "rng.h"
#include "string_id.h"
#include "teleport.h"
#include "timed_event.h"
#include "translations.h"
#include "type_id.h"
#include "units.h"
#include "vehicle.h"
#include "vpart_position.h"

static const ammo_effect_str_id ammo_effect_magic( "magic" );

namespace spell_detail
{
struct line_iterable {
    const std::vector<point> &delta_line;
    point cur_origin;
    point delta;
    size_t index;

    line_iterable( const point &origin, const point &delta, const std::vector<point> &dline )
        : delta_line( dline ), cur_origin( origin ), delta( delta ), index( 0 ) {}

    point get() const {
        return cur_origin + delta_line[index];
    }
    // Move forward along point set, wrap around and move origin forward if necessary
    void next() {
        index = ( index + 1 ) % delta_line.size();
        cur_origin = cur_origin + delta * ( index == 0 );
    }
    // Move back along point set, wrap around and move origin backward if necessary
    void prev() {
        cur_origin = cur_origin - delta * ( index == 0 );
        index = ( index + delta_line.size() - 1 ) % delta_line.size();
    }
    void reset( const point &origin ) {
        cur_origin = origin;
        index = 0;
    }
};
// Orientation of point C relative to line AB
static int side_of( const point &a, const point &b, const point &c )
{
    int cross = ( ( b.x - a.x ) * ( c.y - a.y ) - ( b.y - a.y ) * ( c.x - a.x ) );
    return ( cross > 0 ) - ( cross < 0 );
}
// Tests if point c is between or on lines (a0, a0 + d) and (a1, a1 + d)
static bool between_or_on( const point &a0, const point &a1, const point &d, const point &c )
{
    return side_of( a0, a0 + d, c ) != 1 && side_of( a1, a1 + d, c ) != -1;
}
// Builds line until obstructed or outside of region bound by near and far lines. Stores result in set
static void build_line( spell_detail::line_iterable line, const tripoint &source,
                        const point &delta, const point &delta_perp, bool ( *test )( const tripoint &, const tripoint & ),
                        std::set<tripoint> &result )
{
    tripoint last_point = source;
    while( between_or_on( point_zero, delta, delta_perp, line.get() ) ) {
        if( !test( source + line.get(), last_point ) ) {
            break;
        }
        result.emplace( source + line.get() );
        last_point = source + line.get();
        line.next();
    }
}
} // namespace spell_detail

void spell_effect::teleport_random( const spell &sp, Creature &caster, const tripoint & )
{
    bool safe = !sp.has_flag( spell_flag::UNSAFE_TELEPORT );
    const int min_distance = sp.range();
    const int max_distance = sp.range() + sp.aoe();
    if( min_distance > max_distance || min_distance < 0 || max_distance < 0 ) {
        debugmsg( "ERROR: Teleport argument(s) invalid" );
        return;
    }
    teleport::teleport( caster, min_distance, max_distance, safe, false );
}

static void swap_pos( Creature &caster, const tripoint &target )
{
    Creature *const critter = g->critter_at<Creature>( target );
    critter->setpos( caster.pos() );
    caster.setpos( target );
    //update map in case a monster swapped positions with the player
    g->update_map( get_avatar() );
}

void spell_effect::pain_split( const spell &sp, Creature &caster, const tripoint & )
{
    player *p = caster.as_player();
    if( p == nullptr ) {
        return;
    }
    sp.make_sound( caster.pos() );
    add_msg( m_info, _( "Your injuries even out." ) );
    int num_limbs = 0; // number of limbs effected (broken don't count)
    int total_hp = 0; // total hp among limbs

    for( const std::pair<const bodypart_str_id, bodypart> &elem : p->get_body() ) {
        if( elem.first == bodypart_str_id( "num_bp" ) ) {
            continue;
        }
        num_limbs++;
        total_hp += elem.second.get_hp_cur();
    }
    const int hp_each = total_hp / num_limbs;
    p->set_all_parts_hp_cur( hp_each );
}

static bool in_spell_aoe( const tripoint &start, const tripoint &end, const int &radius,
                          const bool ignore_walls )
{
    if( rl_dist( start, end ) > radius ) {
        return false;
    }
    if( ignore_walls ) {
        return true;
    }
    map &here = get_map();
    const std::vector<tripoint> trajectory = line_to( start, end );
    tripoint last_point = start;
    for( const tripoint &pt : trajectory ) {
        if( ( here.impassable( pt ) && !here.has_flag( "THIN_OBSTACLE", pt ) ) ||
            here.obstructed_by_vehicle_rotation( pt, last_point ) ) {
            return false;
        }
        last_point = pt;
    }
    return true;
}

std::set<tripoint> spell_effect::spell_effect_blast( const spell &, const tripoint &,
        const tripoint &target, const int aoe_radius, const bool ignore_walls )
{
    std::set<tripoint> targets;
    // TODO: Make this breadth-first
    for( const tripoint &potential_target : get_map().points_in_radius( target, aoe_radius ) ) {
        if( in_spell_aoe( target, potential_target, aoe_radius, ignore_walls ) ) {
            targets.emplace( potential_target );
        }
    }
    return targets;
}

std::set<tripoint> spell_effect::spell_effect_cone( const spell &sp, const tripoint &source,
        const tripoint &target, const int aoe_radius, const bool ignore_walls )
{
    std::set<tripoint> targets;
    // cones go all the way to end (if they don't hit an obstacle)
    const int range = sp.range() + 1;
    const units::angle initial_angle = coord_to_angle( source, target );
    const units::angle half_width = units::from_degrees( aoe_radius / 2.0 );
    const units::angle start_angle = initial_angle - half_width;
    const units::angle end_angle = initial_angle + half_width;
    std::set<tripoint> end_points;
    for( units::angle angle = start_angle; angle <= end_angle; angle += 1_degrees ) {
        tripoint potential;
        calc_ray_end( angle, range, source, potential );
        end_points.emplace( potential );
    }
    map &here = get_map();
    for( const tripoint &ep : end_points ) {
        std::vector<tripoint> trajectory = line_to( source, ep );
        tripoint last_point = source;
        for( const tripoint &tp : trajectory ) {
            if( ignore_walls || ( !here.obstructed_by_vehicle_rotation( tp, last_point ) &&
                                  ( here.passable( tp ) || here.has_flag( "THIN_OBSTACLE", tp ) ) ) ) {
                targets.emplace( tp );
            } else {
                break;
            }
            last_point = tp;
        }
    }
    // we don't want to hit ourselves in the blast!
    targets.erase( source );
    return targets;
}

static bool test_always_true( const tripoint &, const tripoint & )
{
    return true;
}
static bool test_passable( const tripoint &p, const tripoint &prev )
{
    map &here = get_map();
    return ( !here.obstructed_by_vehicle_rotation( prev, p ) && ( here.passable( p ) ||
             here.has_flag( "THIN_OBSTACLE", p ) ) );
}

std::set<tripoint> spell_effect::spell_effect_line( const spell &, const tripoint &source,
        const tripoint &target, const int aoe_radius, const bool ignore_walls )
{
    const point delta = ( target - source ).xy();
    const int dist = square_dist( point_zero, delta );
    // Early out to prevent unnecessary calculations
    if( dist == 0 ) {
        return std::set<tripoint>();
    }
    // Clockwise Perpendicular of Delta vector
    const point delta_perp( -delta.y, delta.x );

    const point abs_delta = delta.abs();
    // Primary axis of delta vector
    const point axis_delta = abs_delta.x > abs_delta.y ? point( delta.x, 0 ) : point( 0, delta.y );
    // Clockwise Perpendicular of axis vector
    const point cw_perp_axis( -axis_delta.y, axis_delta.x );
    const point unit_cw_perp_axis( sgn( cw_perp_axis.x ), sgn( cw_perp_axis.y ) );
    // bias leg length toward cw side if uneven
    int ccw_len = aoe_radius / 2;
    int cw_len = aoe_radius - ccw_len;

    if( !trigdist ) {
        ccw_len = ( ccw_len * ( abs_delta.x + abs_delta.y ) ) / dist;
        cw_len = ( cw_len * ( abs_delta.x + abs_delta.y ) ) / dist;
    }

    // is delta aligned with, cw, or ccw of primary axis
    int delta_side = spell_detail::side_of( point_zero, axis_delta, delta );

    bool ( *test )( const tripoint &,
                    const tripoint & ) = ignore_walls ? test_always_true : test_passable;

    // Canonical path from source to target, offset to local space
    std::vector<point> path_to_target = line_to( point_zero, delta );
    // Remove endpoint,
    path_to_target.pop_back();
    // and insert startpoint. Path is now prepared for wrapped iteration
    path_to_target.insert( path_to_target.begin(), point_zero );

    spell_detail::line_iterable base_line( point_zero, delta, path_to_target );

    std::set<tripoint> result;

    // Add midline points (source -> target )
    spell_detail::build_line( base_line, source, delta, delta_perp, test, result );

    // Add cw and ccw legs
    if( delta_side == 0 ) { // delta is already axis aligned, only need straight lines
        // cw leg
        point prev_point;
        for( const point &p : line_to( point_zero, unit_cw_perp_axis * cw_len ) ) {
            base_line.reset( p );
            if( !test( source + p, source + prev_point ) ) {
                break;
            }

            spell_detail::build_line( base_line, source, delta, delta_perp, test, result );
            prev_point = p;
        }
        // ccw leg
        prev_point = point_zero;
        for( const point &p : line_to( point_zero, unit_cw_perp_axis * -ccw_len ) ) {
            base_line.reset( p );
            if( !test( source + p, source + prev_point ) ) {
                break;
            }

            spell_detail::build_line( base_line, source, delta, delta_perp, test, result );
            prev_point = p;
        }
    } else if( delta_side == 1 ) { // delta is cw of primary axis
        // ccw leg is behind perp axis
        point prev_point;
        for( const point &p : line_to( point_zero, unit_cw_perp_axis * -ccw_len ) ) {
            base_line.reset( p );

            // forward until in
            while( spell_detail::side_of( point_zero, delta_perp, base_line.get() ) == 1 ) {
                base_line.next();
            }
            if( !test( source + p, source + prev_point ) ) {
                break;
            }
            spell_detail::build_line( base_line, source, delta, delta_perp, test, result );
            prev_point = p;
        }
        prev_point = point_zero;
        // cw leg is before perp axis
        for( const point &p : line_to( point_zero, unit_cw_perp_axis * cw_len ) ) {
            base_line.reset( p );

            // move back
            while( spell_detail::side_of( point_zero, delta_perp, base_line.get() ) != 1 ) {
                base_line.prev();
            }
            base_line.next();
            if( !test( source + p, source + prev_point ) ) {
                break;
            }
            spell_detail::build_line( base_line, source, delta, delta_perp, test, result );
            prev_point = p;
        }
    } else if( delta_side == -1 ) { // delta is ccw of primary axis
        // ccw leg is before perp axis
        point prev_point;
        for( const point &p : line_to( point_zero, unit_cw_perp_axis * -ccw_len ) ) {
            base_line.reset( p );

            // move back
            while( spell_detail::side_of( point_zero, delta_perp, base_line.get() ) != 1 ) {
                base_line.prev();
            }
            base_line.next();
            if( !test( source + p, source + prev_point ) ) {
                break;
            }
            spell_detail::build_line( base_line, source, delta, delta_perp, test, result );
            prev_point = p;
        }
        prev_point = point_zero;
        // cw leg is behind perp axis
        for( const point &p : line_to( point_zero, unit_cw_perp_axis * cw_len ) ) {
            base_line.reset( p );

            // forward until in
            while( spell_detail::side_of( point_zero, delta_perp, base_line.get() ) == 1 ) {
                base_line.next();
            }
            if( !test( source + p, source + prev_point ) ) {
                break;
            }
            spell_detail::build_line( base_line, source, delta, delta_perp, test, result );
            prev_point = p;
        }
    }

    result.erase( source );
    return result;
}

// spells do not reduce in damage the further away from the epicenter the targets are
// rather they do their full damage in the entire area of effect
std::set<tripoint> calculate_spell_effect_area( const spell &sp, const tripoint &target,
        std::function<std::set<tripoint>( const spell &, const tripoint &, const tripoint &, int, bool )>
        aoe_func, const Creature &caster, bool ignore_walls )
{
    std::set<tripoint> targets = { target }; // initialize with epicenter
    if( sp.aoe() <= 1 && sp.effect() != "line_attack" ) {
        return targets;
    }

    const int aoe_radius = sp.aoe();
    targets = aoe_func( sp, caster.pos(), target, aoe_radius, ignore_walls );

    for( std::set<tripoint>::iterator it = targets.begin(); it != targets.end(); ) {
        if( !sp.is_valid_target( caster, *it ) ) {
            it = targets.erase( it );
        } else {
            ++it;
        }
    }

    return targets;
}

static std::set<tripoint> spell_effect_area( const spell &sp, const tripoint &target,
        std::function<std::set<tripoint>( const spell &, const tripoint &, const tripoint &, int, bool )>
        aoe_func, const Creature &caster, bool ignore_walls = false )
{
    // calculate spell's effect area
    std::set<tripoint> targets = calculate_spell_effect_area( sp, target, aoe_func, caster,
                                 ignore_walls );

    // Draw the explosion
    std::map<tripoint, nc_color> explosion_colors;
    for( auto &pt : targets ) {
        explosion_colors[pt] = sp.damage_type_color();
    }

    if( sp.id()->sprite.length() > 0 ) {
        explosion_handler::draw_custom_explosion( get_player_character().pos(), explosion_colors,
                sp.id()->sprite );
    } else {
        explosion_handler::draw_custom_explosion( get_player_character().pos(), explosion_colors,
                "explosion" );
    }

    return targets;
}

static void add_effect_to_target( const tripoint &target, const spell &sp )
{
    Creature *const critter = g->critter_at<Creature>( target );
    Character *const guy = g->critter_at<Character>( target );
    efftype_id spell_effect( sp.effect_data() );

    // TODO: migrate duration from moves to time_duration
    const int dur_turns = sp.duration() / 100;
    // Ensure permanent effect has at last 1 second of duration,
    // so it won't be instantly removed as expired.
    time_duration dur_td = ( spell_effect->is_permanent() && dur_turns == 0 )
                           ? 1_seconds
                           : 1_turns * dur_turns;

    bool bodypart_effected = false;

    if( guy ) {
        for( const body_part bp : all_body_parts ) {
            if( sp.bp_is_affected( bp ) ) {
                guy->add_effect( spell_effect, dur_td, bp );
                bodypart_effected = true;
            }
        }
    }
    if( !bodypart_effected ) {
        critter->add_effect( spell_effect, dur_td, num_bp );
    }
}

static void damage_targets( const spell &sp, Creature &caster,
                            const std::set<tripoint> &targets )
{
    for( const tripoint &target : targets ) {
        if( !sp.is_valid_target( caster, target ) ) {
            continue;
        }
        sp.make_sound( target );
        sp.create_field( target );
        Creature *const cr = g->critter_at<Creature>( target );
        if( !cr ) {
            continue;
        }

        projectile bolt;
        bolt.speed = 10000;
        bolt.impact = sp.get_damage_instance();
        bolt.add_effect( ammo_effect_magic );

        dealt_projectile_attack atk;
        atk.end_point = target;
        atk.hit_critter = cr;
        atk.proj = bolt;
        atk.missed_by = 0.0;
        if( !sp.effect_data().empty() ) {
            add_effect_to_target( target, sp );
        }
        if( sp.damage() > 0 ) {
            cr->deal_projectile_attack( &caster, atk );
        } else if( sp.damage() < 0 ) {
            sp.heal( target );
            if( get_avatar().sees( cr->pos() ) ) {
                add_msg( m_good, _( "%s wounds are closing up!" ), cr->disp_name( true ) );
            }
        }
    }
}

void spell_effect::projectile_attack( const spell &sp, Creature &caster,
                                      const tripoint &target )
{
    std::vector<tripoint> trajectory = line_to( caster.pos(), target );
    tripoint prev_point = caster.pos();
    map &here = get_map();
    for( std::vector<tripoint>::iterator iter = trajectory.begin(); iter != trajectory.end(); iter++ ) {
        if( ( here.impassable( *iter ) && !here.has_flag( "THIN_OBSTACLE", *iter ) ) ||
            here.obstructed_by_vehicle_rotation( prev_point, *iter ) ) {
            if( iter != trajectory.begin() ) {
                target_attack( sp, caster, *( iter - 1 ) );
            } else {
                target_attack( sp, caster, *iter );
            }
            return;
        }
        prev_point = *iter;
    }
    target_attack( sp, caster, trajectory.back() );
}

void spell_effect::target_attack( const spell &sp, Creature &caster,
                                  const tripoint &epicenter )
{
    damage_targets( sp, caster, spell_effect_area( sp, epicenter, spell_effect_blast, caster,
                    sp.has_flag( spell_flag::IGNORE_WALLS ) ) );
    if( sp.has_flag( spell_flag::SWAP_POS ) ) {
        swap_pos( caster, epicenter );
    }
}

void spell_effect::cone_attack( const spell &sp, Creature &caster,
                                const tripoint &target )
{
    damage_targets( sp, caster, spell_effect_area( sp, target, spell_effect_cone, caster,
                    sp.has_flag( spell_flag::IGNORE_WALLS ) ) );
}

void spell_effect::line_attack( const spell &sp, Creature &caster,
                                const tripoint &target )
{
    damage_targets( sp, caster, spell_effect_area( sp, target, spell_effect_line, caster,
                    sp.has_flag( spell_flag::IGNORE_WALLS ) ) );
}

area_expander::area_expander() : frontier( area_node_comparator( area ) )
{
}

// Check whether we have already visited this node.
int area_expander::contains( const tripoint &pt ) const
{
    return area_search.count( pt ) > 0;
}

// Adds node to a search tree. Returns true if new node is allocated.
bool area_expander::enqueue( const tripoint &from, const tripoint &to, float cost )
{
    if( contains( to ) ) {
        // We will modify existing node if its cost is lower.
        int index = area_search[to];
        node &node = area[index];
        if( cost < node.cost ) {
            node.from = from;
            node.cost = cost;
        }
        return false;
    }
    int index = area.size();
    area.push_back( {to, from, cost} );
    frontier.push( index );
    area_search[to] = index;
    return true;
}

// Run wave propagation
int area_expander::run( const tripoint &center )
{
    enqueue( center, center, 0.0 );

    static constexpr std::array<int, 8> x_offset = {{ -1, 1,  0, 0,  1, -1, -1, 1  }};
    static constexpr std::array<int, 8> y_offset = {{  0, 0, -1, 1, -1,  1, -1, 1  }};

    // Number of nodes expanded.
    int expanded = 0;

    map &here = get_map();

    while( !frontier.empty() ) {
        int best_index = frontier.top();
        frontier.pop();
        node &best = area[best_index];

        for( size_t i = 0; i < 8; i++ ) {
            tripoint pt = best.position + point( x_offset[ i ], y_offset[ i ] );

            if( ( here.impassable( pt ) && !here.has_flag( "THIN_OBSTACLE", pt ) ) ||
                here.obstructed_by_vehicle_rotation( best.position, pt ) ) {
                continue;
            }

            float center_range = static_cast<float>( rl_dist( center, pt ) );
            if( max_range > 0 && center_range > max_range ) {
                continue;
            }

            if( max_expand > 0 && expanded > max_expand && contains( pt ) ) {
                continue;
            }

            float delta_range = trig_dist( best.position, pt );

            if( enqueue( best.position, pt, best.cost + delta_range ) ) {
                expanded++;
            }
        }
    }
    return expanded;
}

// Sort nodes by its cost.
void area_expander::sort_ascending()
{
    // Since internal caches like 'area_search' and 'frontier' use indexes inside 'area',
    // these caches will be invalidated.
    std::sort( area.begin(), area.end(),
    []( const node & a, const node & b )  -> bool {
        return a.cost < b.cost;
    } );
}

void area_expander::sort_descending()
{
    // Since internal caches like 'area_search' and 'frontier' use indexes inside 'area',
    // these caches will be invalidated.
    std::sort( area.begin(), area.end(),
    []( const node & a, const node & b ) -> bool {
        return a.cost > b.cost;
    } );
}

// Moving all objects from one point to another by the power of magic.
static void spell_move( const spell &sp, const Creature &caster,
                        const tripoint &from, const tripoint &to )
{
    if( from == to ) {
        return;
    }

    // Moving creatures
    bool can_target_creature = sp.is_valid_effect_target( target_self ) ||
                               sp.is_valid_effect_target( target_ally ) ||
                               sp.is_valid_effect_target( target_hostile );

    if( can_target_creature ) {
        if( Creature *victim = g->critter_at<Creature>( from ) ) {
            Creature::Attitude cr_att = victim->attitude_to( get_avatar() );
            bool valid = cr_att != Creature::A_FRIENDLY && sp.is_valid_effect_target( target_hostile );
            valid |= cr_att == Creature::A_FRIENDLY && sp.is_valid_effect_target( target_ally );
            valid |= victim == &caster && sp.is_valid_effect_target( target_self );
            if( valid ) {
                victim->knock_back_to( to );
            }
        }
    }

    map &here = get_map();
    // Moving items
    if( sp.is_valid_effect_target( target_item ) ) {
        auto src_items = here.i_at( from );
        auto dst_items = here.i_at( to );
        for( const item &item : src_items ) {
            dst_items.insert( item );
        }
        src_items.clear();
    }

    // Helper function to move particular field type if corresponding target flag is enabled.
    auto move_field = [&sp, &here, from, to]( valid_target target, field_type_id fid ) {
        if( !sp.is_valid_effect_target( target ) ) {
            return;
        }
        auto &src_field = here.field_at( from );
        if( field_entry *entry = src_field.find_field( fid ) ) {
            int intensity = entry->get_field_intensity();
            here.remove_field( from, fid );
            here.set_field_intensity( to, fid, intensity );
        }
    };
    // Moving fields.
    move_field( target_fd_fire, fd_fire );
    move_field( target_fd_blood, fd_blood );
    move_field( target_fd_blood, fd_gibs_flesh );
}

void spell_effect::area_pull( const spell &sp, Creature &caster, const tripoint &center )
{
    area_expander expander;

    expander.max_range = sp.aoe();
    expander.run( center );
    expander.sort_ascending();

    for( const auto &node : expander.area ) {
        if( node.from == node.position ) {
            continue;
        }

        spell_move( sp, caster, node.position, node.from );
    }
    sp.make_sound( caster.pos() );
}

void spell_effect::area_push( const spell &sp, Creature &caster, const tripoint &center )
{
    area_expander expander;

    expander.max_range = sp.aoe();
    expander.run( center );
    expander.sort_descending();

    for( const auto &node : expander.area ) {
        if( node.from == node.position ) {
            continue;
        }

        spell_move( sp, caster, node.from, node.position );
    }
    sp.make_sound( caster.pos() );
}

void spell_effect::spawn_ethereal_item( const spell &sp, Creature &caster, const tripoint & )
{
    item granted( sp.effect_data(), calendar::turn );
    if( !granted.is_comestible() && !( sp.has_flag( spell_flag::PERMANENT ) && sp.is_max_level() ) ) {
        granted.set_var( "ethereal", to_turns<int>( sp.duration_turns() ) );
        granted.set_flag( "ETHEREAL_ITEM" );
    }
    if( granted.count_by_charges() && sp.damage() > 0 ) {
        granted.charges = sp.damage();
    }
    avatar &you = get_avatar();
    if( you.can_wear( granted ).success() ) {
        granted.set_flag( "FIT" );
        you.wear_item( granted, false );
    } else if( !you.is_armed() ) {
        you.weapon = granted;
    } else {
        you.i_add( granted );
    }
    if( !granted.count_by_charges() ) {
        for( int i = 1; i < sp.damage(); i++ ) {
            you.i_add( granted );
        }
    }
    sp.make_sound( caster.pos() );
}

void spell_effect::recover_energy( const spell &sp, Creature &caster, const tripoint &target )
{
    // this spell is not appropriate for healing
    const int healing = sp.damage();
    const std::string energy_source = sp.effect_data();
    // TODO: Change to Character
    // current limitation is that Character does not have stamina or power_level members
    player *p = g->critter_at<player>( target );
    if( !p ) {
        return;
    }

    if( energy_source == "MANA" ) {
        p->magic->mod_mana( *p, healing );
    } else if( energy_source == "STAMINA" ) {
        p->mod_stamina( healing );
    } else if( energy_source == "FATIGUE" ) {
        // fatigue is backwards
        p->mod_fatigue( -healing );
    } else if( energy_source == "BIONIC" ) {
        if( healing > 0 ) {
            p->mod_power_level( units::from_kilojoule( healing ) );
        } else {
            p->mod_stamina( healing );
        }
    } else if( energy_source == "PAIN" ) {
        // pain is backwards
        if( sp.has_flag( PAIN_NORESIST ) ) {
            p->mod_pain_noresist( -healing );
        } else {
            p->mod_pain( -healing );
        }
    } else if( energy_source == "HEALTH" ) {
        p->mod_healthy( healing );
    } else {
        debugmsg( "Invalid effect_str %s for spell %s", energy_source, sp.name() );
    }
    sp.make_sound( caster.pos() );
}

void spell_effect::timed_event( const spell &sp, Creature &caster, const tripoint & )
{
    const std::map<std::string, timed_event_type> timed_event_map{
        { "help", timed_event_type::TIMED_EVENT_HELP },
        { "wanted", timed_event_type::TIMED_EVENT_WANTED },
        { "robot_attack", timed_event_type::TIMED_EVENT_ROBOT_ATTACK },
        { "spawn_wyrms", timed_event_type::TIMED_EVENT_SPAWN_WYRMS },
        { "amigara", timed_event_type::TIMED_EVENT_AMIGARA },
        { "roots_die", timed_event_type::TIMED_EVENT_ROOTS_DIE },
        { "temple_open", timed_event_type::TIMED_EVENT_TEMPLE_OPEN },
        { "temple_flood", timed_event_type::TIMED_EVENT_TEMPLE_FLOOD },
        { "temple_spawn", timed_event_type::TIMED_EVENT_TEMPLE_SPAWN },
        { "dim", timed_event_type::TIMED_EVENT_DIM },
        { "artifact_light", timed_event_type::TIMED_EVENT_ARTIFACT_LIGHT }
    };

    timed_event_type spell_event = timed_event_type::TIMED_EVENT_NULL;

    const auto iter = timed_event_map.find( sp.effect_data() );
    if( iter != timed_event_map.cend() ) {
        spell_event = iter->second;
    }

    sp.make_sound( caster.pos() );
    g->timed_events.add( spell_event, calendar::turn + sp.duration_turns() );
}

static bool is_summon_friendly( const spell &sp )
{
    const bool hostile = sp.has_flag( spell_flag::HOSTILE_SUMMON );
    bool friendly = !hostile;
    if( sp.has_flag( spell_flag::HOSTILE_50 ) ) {
        friendly = friendly && rng( 0, 1000 ) < 500;
    }
    return friendly;
}

static bool add_summoned_mon( const mtype_id &id, const tripoint &pos, const time_duration &time,
                              const spell &sp )
{
    monster *const mon_ptr = g->place_critter_at( id, pos );
    if( !mon_ptr ) {
        return false;
    }
    const bool permanent = sp.has_flag( spell_flag::PERMANENT );
    monster &spawned_mon = *mon_ptr;
    if( is_summon_friendly( sp ) ) {
        spawned_mon.friendly = INT_MAX;
    } else {
        spawned_mon.friendly = 0;
    }
    if( !permanent ) {
        spawned_mon.set_summon_time( time );
    }
    spawned_mon.no_extra_death_drops = true;
    return true;
}

void spell_effect::spawn_summoned_monster( const spell &sp, Creature &caster,
        const tripoint &target )
{
    const mtype_id mon_id( sp.effect_data() );
    std::set<tripoint> area = spell_effect_area( sp, target, spell_effect_blast, caster );
    // this should never be negative, but this'll keep problems from happening
    size_t num_mons = std::abs( sp.damage() );
    const time_duration summon_time = sp.duration_turns();
    while( num_mons > 0 && !area.empty() ) {
        const size_t mon_spot = rng( 0, area.size() - 1 );
        auto iter = area.begin();
        std::advance( iter, mon_spot );
        if( add_summoned_mon( mon_id, *iter, summon_time, sp ) ) {
            num_mons--;
            sp.make_sound( *iter );
        } else {
            add_msg( m_bad, "failed to place monster" );
        }
        // whether or not we succeed in spawning a monster, we don't want to try this tripoint again
        area.erase( iter );
    }
}

void spell_effect::spawn_summoned_vehicle( const spell &sp, Creature &caster,
        const tripoint &target )
{
    map &here = get_map();
    if( here.veh_at( target ) ) {
        caster.add_msg_if_player( m_bad, _( "There is already a vehicle there." ) );
        return;
    }
    if( vehicle *veh = here.add_vehicle( sp.summon_vehicle_id(), target, -90_degrees, 100, 0 ) ) {
        veh->magic = true;
        const time_duration summon_time = sp.duration_turns();
        if( !sp.has_flag( spell_flag::PERMANENT ) ) {
            veh->summon_time_limit = summon_time;
        }
        if( caster.as_character() ) {
            veh->set_owner( *caster.as_character() );
        }
    }
}

void spell_effect::translocate( const spell &sp, Creature &caster, const tripoint &target )
{
    avatar *you = caster.as_avatar();
    if( you == nullptr ) {
        return;
    }
    you->translocators->translocate( spell_effect_area( sp, target, spell_effect_blast, caster,
                                     true ) );
}

void spell_effect::none( const spell &sp, Creature &, const tripoint & )
{
    debugmsg( "ERROR: %s has invalid spell effect.", sp.name() );
}

void spell_effect::transform_blast( const spell &sp, Creature &caster,
                                    const tripoint &target )
{
    ter_furn_transform_id transform( sp.effect_data() );
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), true );
    for( const tripoint &location : area ) {
        if( one_in( sp.damage() ) ) {
            transform->transform( location );
            transform->add_all_messages( caster, location );
        }
    }
}

void spell_effect::noise( const spell &sp, Creature &, const tripoint &target )
{
    sp.make_sound( target, sp.damage() );
}

void spell_effect::vomit( const spell &sp, Creature &caster, const tripoint &target )
{
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), true );
    for( const tripoint &potential_target : area ) {
        if( !sp.is_valid_target( caster, potential_target ) ) {
            continue;
        }
        Character *const ch = g->critter_at<Character>( potential_target );
        if( !ch ) {
            continue;
        }
        sp.make_sound( target );
        ch->vomit();
    }
}

void spell_effect::explosion( const spell &sp, Creature &caster, const tripoint &target )
{
    explosion_handler::explosion( target, &caster, sp.damage(), sp.aoe() / 10.0, true );
}

void spell_effect::flashbang( const spell &sp, Creature &caster, const tripoint &target )
{
    if( sp.id()->sprite.length() > 0 ) {
        explosion_handler::flashbang( target, caster.is_avatar() &&
                                      !sp.is_valid_target( valid_target::target_self ), sp.id()->sprite );
    } else {
        explosion_handler::flashbang( target, caster.is_avatar() &&
                                      !sp.is_valid_target( valid_target::target_self ), "explosion" );
    }
}

void spell_effect::mod_moves( const spell &sp, Creature &caster, const tripoint &target )
{
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), false );
    for( const tripoint &potential_target : area ) {
        if( !sp.is_valid_target( caster, potential_target ) ) {
            continue;
        }
        Creature *critter = g->critter_at<Creature>( potential_target );
        if( !critter ) {
            continue;
        }
        sp.make_sound( potential_target );
        critter->moves += sp.damage();
    }
}

void spell_effect::map_area( const spell &sp, Creature &caster, const tripoint & )
{
    const avatar *you = caster.as_avatar();
    if( !you ) {
        // revealing the map only makes sense for the avatar
        return;
    }
    const tripoint_abs_omt center = you->global_omt_location();
    overmap_buffer.reveal( center.xy(), sp.aoe(), center.z() );
}

void spell_effect::morale( const spell &sp, Creature &caster, const tripoint &target )
{
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), false );
    if( sp.effect_data().empty() ) {
        debugmsg( "ERROR: %s must have a valid morale_type as effect_str.  None specified.",
                  sp.id().c_str() );
        return;
    }
    if( !morale_type( sp.effect_data() ).is_valid() ) {
        debugmsg( "ERROR: %s must have a valid morale_type as effect_str.  %s is invalid.", sp.id().c_str(),
                  sp.effect_data() );
        return;
    }
    for( const tripoint &potential_target : area ) {
        player *player_target;
        if( !( sp.is_valid_target( caster, potential_target ) &&
               ( player_target = g->critter_at<player>( potential_target ) ) ) ) {
            continue;
        }
        player_target->add_morale( morale_type( sp.effect_data() ), sp.damage(), 0, sp.duration_turns(),
                                   sp.duration_turns() / 10, false );
        sp.make_sound( potential_target );
    }
}

void spell_effect::charm_monster( const spell &sp, Creature &caster, const tripoint &target )
{
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), false );
    for( const tripoint &potential_target : area ) {
        if( !sp.is_valid_target( caster, potential_target ) ) {
            continue;
        }
        monster *mon = g->critter_at<monster>( potential_target );
        if( !mon ) {
            continue;
        }
        sp.make_sound( potential_target );
        if( mon->friendly == 0 && mon->get_hp() <= sp.damage() ) {
            mon->unset_dest();
            mon->friendly += sp.duration() / 100;
        }
    }
}

void spell_effect::mutate( const spell &sp, Creature &caster, const tripoint &target )
{
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), false );
    for( const tripoint &potential_target : area ) {
        if( !sp.is_valid_target( caster, potential_target ) ) {
            continue;
        }
        Character *guy = g->critter_at<Character>( potential_target );
        if( !guy ) {
            continue;
        }
        // 10000 represents 100.00% to increase granularity without swapping everything to a float
        if( sp.damage() < rng( 1, 10000 ) ) {
            // chance failure! but keep trying for other targets
            continue;
        }
        if( sp.effect_data().empty() ) {
            guy->mutate();
        } else {
            if( sp.has_flag( spell_flag::MUTATE_TRAIT ) ) {
                guy->mutate_towards( trait_id( sp.effect_data() ) );
            } else {
                guy->mutate_category( sp.effect_data() );
            }
        }
        sp.make_sound( potential_target );
    }
}

void spell_effect::bash( const spell &sp, Creature &caster, const tripoint &target )
{
    const std::set<tripoint> area = spell_effect_blast( sp, caster.pos(), target, sp.aoe(), true );
    for( const tripoint &potential_target : area ) {
        if( !sp.is_valid_target( caster, potential_target ) ) {
            continue;
        }
        // the bash already makes noise, so no need for spell::make_sound()
        get_map().bash( potential_target, sp.damage(), sp.has_flag( spell_flag::SILENT ) );
    }
}
