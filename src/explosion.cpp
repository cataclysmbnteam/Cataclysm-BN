#include "explosion.h" // IWYU pragma: associated
#include "fragment_cloud.h" // IWYU pragma: associated

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include "animation.h"
#include "avatar.h"
#include "ballistics.h"
#include "bodypart.h"
#include "calendar.h"
#include "cata_utility.h"
#include "color.h"
#include "creature.h"
#include "damage.h"
#include "debug.h"
#include "enums.h"
#include "explosion_queue.h"
#include "field_type.h"
#include "flat_set.h"
#include "game.h"
#include "game_constants.h"
#include "int_id.h"
#include "item.h"
#include "item_factory.h"
#include "itype.h"
#include "json.h"
#include "line.h"
#include "map.h"
#include "map_iterator.h"
#include "mapdata.h"
#include "material.h"
#include "math_defines.h"
#include "messages.h"
#include "mongroup.h"
#include "monster.h"
#include "mtype.h"
#include "npc.h"
#include "options.h"
#include "optional.h"
#include "player.h"
#include "point.h"
#include "projectile.h"
#include "rng.h"
#include "shadowcasting.h"
#include "sounds.h"
#include "string_formatter.h"
#include "translations.h"
#include "trap.h"
#include "type_id.h"
#include "units.h"
#include "vehicle.h"
#include "vpart_position.h"

static const ammo_effect_str_id ammo_effect_NULL_SOURCE( "NULL_SOURCE" );

static const efftype_id effect_blind( "blind" );
static const efftype_id effect_deaf( "deaf" );
static const efftype_id effect_emp( "emp" );
static const efftype_id effect_stunned( "stunned" );
static const efftype_id effect_teleglow( "teleglow" );

static const std::string flag_BLIND( "BLIND" );
static const std::string flag_FLASH_PROTECTION( "FLASH_PROTECTION" );

static const species_id ROBOT( "ROBOT" );

static const trait_id trait_LEG_TENT_BRACE( "LEG_TENT_BRACE" );
static const trait_id trait_PER_SLIME( "PER_SLIME" );
static const trait_id trait_PER_SLIME_OK( "PER_SLIME_OK" );

static const mongroup_id GROUP_NETHER( "GROUP_NETHER" );

static const bionic_id bio_ears( "bio_ears" );
static const bionic_id bio_sunglasses( "bio_sunglasses" );

static const itype_id itype_battery( "battery" );
static const itype_id itype_e_handcuffs( "e_handcuffs" );
static const itype_id itype_rm13_armor_on( "rm13_armor_on" );

static float obstacle_blast_percentage( float range, float distance )
{
    return distance > range ? 0.0f : distance > range / 2 ? 0.5f : 1.0f;
}
static float critter_blast_percentage( Creature *c, float range, float distance )
{
    const float radius_reduction = distance > range ? 0.0f : distance > range / 2 ? 0.5f : 1.0f;

    switch( c->get_size() ) {
        case( m_size::MS_TINY ):
            return 0.5 * radius_reduction;
        case( m_size::MS_SMALL ):
            return 0.8 * radius_reduction;
        case( m_size::MS_MEDIUM ):
            return 1.0 * radius_reduction;
        case( m_size::MS_LARGE ):
            return 1.5 * radius_reduction;
        case( m_size::MS_HUGE ):
            return 2.0 * radius_reduction;
        default:
            return 1.0 * radius_reduction;
    }
}

static float item_blast_percentage( float range, float distance )
{
    const float radius_reduction = 1.0f - distance / range;
    return radius_reduction;
}

explosion_data load_explosion_data( const JsonObject &jo )
{
    explosion_data ret;
    // First new explosions
    if( jo.has_int( "damage" ) || jo.has_object( "fragment" ) ) {
        jo.read( "damage", ret.damage );
        jo.read( "radius", ret.radius );
        jo.read( "fragment", ret.fragment );
    } else {
        // Legacy
        float power = jo.get_float( "power" );
        ret.damage = power * explosion_handler::power_to_dmg_mult;
        // Don't reuse old formula, it gave way too big blasts
        float distance_factor = jo.get_float( "distance_factor", 0.8f );
        ret.radius = explosion_handler::blast_radius_from_legacy( power, distance_factor );
        if( jo.has_int( "shrapnel" ) ) {
            ret.fragment = explosion_handler::shrapnel_from_legacy( power, ret.radius );
            // Outdated, unused
            jo.get_int( "shrapnel" );
        } else if( jo.has_object( "shrapnel" ) ) {
            ret.fragment = explosion_handler::shrapnel_from_legacy( power, ret.radius );

            auto shr = jo.get_object( "shrapnel" );
            // Legacy bad design - we don't migrate those
            shr.get_int( "casing_mass" );
            shr.get_float( "fragment_mass", 0.15 );
            shr.get_int( "recovery", 0 );
            shr.get_string( "drop", "" );
        }
    }

    ret.fire = jo.get_bool( "fire", false );

    return ret;
}

namespace explosion_handler
{
// (C1001) Compiler Internal Error on Visual Studio 2015 with Update 2
static std::map<const Creature *, int> do_blast( const tripoint &p, const float power,
        const float radius, const bool fire )
{
    const float tile_dist = 1.0f;
    const float diag_dist = trigdist ? M_SQRT2 * tile_dist : 1.0f * tile_dist;
    const float zlev_dist = 4.0f; // Penalty for going up/down
    // 7 3 5
    // 1 . 2
    // 6 4 8
    // 9 and 10 are up and down
    static const int x_offset[10] = { -1, 1,  0, 0,  1, -1, -1, 1, 0, 0 };
    static const int y_offset[10] = { 0, 0, -1, 1, -1,  1, -1, 1, 0, 0 };
    static const int z_offset[10] = { 0, 0,  0, 0,  0,  0,  0, 0, 1, -1 };
    map &here = get_map();
    const size_t max_index = here.has_zlevels() ? 10 : 8;

    std::map<const Creature *, int> blasted;

    here.bash( p, fire ? power : ( 2 * power ), true, false, false );

    std::priority_queue< std::pair<float, tripoint>, std::vector< std::pair<float, tripoint> >, pair_greater_cmp_first >
    open;
    std::set<tripoint> closed;
    std::map<tripoint, float> dist_map;
    open.push( std::make_pair( 0.0f, p ) );
    dist_map[p] = 0.0f;
    // Find all points to blast
    while( !open.empty() ) {
        const float distance = open.top().first;
        const tripoint pt = open.top().second;
        open.pop();

        if( closed.count( pt ) != 0 ) {
            continue;
        }

        closed.insert( pt );

        const float force = power * obstacle_blast_percentage( radius, distance );
        if( force <= 1.0f ) {
            continue;
        }

        if( here.impassable( pt ) && pt != p ) {
            // Don't propagate further
            continue;
        }

        // Iterate over all neighbors. Bash all of them, propagate to some
        for( size_t i = 0; i < max_index; i++ ) {
            tripoint dest( pt + tripoint( x_offset[i], y_offset[i], z_offset[i] ) );
            if( closed.count( dest ) != 0 || !here.inbounds( dest ) ||
                here.obstructed_by_vehicle_rotation( pt, dest ) ) {
                continue;
            }

            if( z_offset[i] == 0 ) {
                // Horizontal - no floor bashing
                here.bash( dest, force, true, false, false );
            } else if( z_offset[i] > 0 ) {
                // Should actually bash through the floor first, but that's not really possible yet
                here.bash( dest, force, true, false, true );
            } else if( !here.valid_move( pt, dest, false, true ) ) {
                // Only bash through floor if it doesn't exist
                // Bash the current tile's floor, not the one's below
                here.bash( pt, force, true, false, true );
            }

            float next_dist = distance;
            next_dist += ( x_offset[i] == 0 || y_offset[i] == 0 ) ? tile_dist : diag_dist;
            if( z_offset[i] != 0 ) {
                if( !here.valid_move( pt, dest, false, true ) ) {
                    continue;
                }

                next_dist += zlev_dist;
            }

            if( dist_map.count( dest ) == 0 || dist_map[dest] > next_dist ) {
                open.push( std::make_pair( next_dist, dest ) );
                dist_map[dest] = next_dist;
            }
        }
    }

    // Draw the explosion
    std::map<tripoint, nc_color> explosion_colors;
    for( auto &pt : closed ) {
        if( here.impassable( pt ) ) {
            continue;
        }

        float percentage = obstacle_blast_percentage( radius, dist_map.at( pt ) );
        if( percentage > 0.0f ) {
            static const std::array<nc_color, 3> colors = { {
                    c_red, c_yellow, c_white
                }
            };
            size_t color_index = ( power > 30 ? 1 : 0 ) + ( percentage > 0.5f ? 1 : 0 );

            explosion_colors[pt] = colors[color_index];
        }
    }

    draw_custom_explosion( g->u.pos(), explosion_colors, "explosion" );

    for( const tripoint &pt : closed ) {
        const float force = power * obstacle_blast_percentage( radius, dist_map.at( pt ) );
        if( force < 1.0f ) {
            // Too weak to matter
            continue;
        }

        here.smash_items( pt, force, _( "force of the explosion" ), true );

        if( fire ) {
            int intensity = 1 + ( force > 10.0f ) + ( force > 30.0f );

            if( !here.has_zlevels() && here.is_outside( pt ) && intensity == 2 ) {
                // In 3D mode, it would have fire fields above, which would then fall
                // and fuel the fire on this tile
                intensity++;
            }

            here.add_field( pt, fd_fire, intensity );
        }

        if( const optional_vpart_position vp = here.veh_at( pt ) ) {
            // TODO: Make this weird unit used by vehicle::damage more sensible
            vp->vehicle().damage( vp->part_index(), force, fire ? DT_HEAT : DT_BASH, false );
        }

        Creature *critter = g->critter_at( pt, true );
        if( critter == nullptr ) {
            continue;
        }

        player *pl = dynamic_cast<player *>( critter );
        if( pl == nullptr ) {
            // TODO: player's fault?
            const double dmg = std::max( force - critter->get_armor_bash( bodypart_id( "torso" ) ) / 3.0, 0.0 );
            const int actual_dmg = rng_float( dmg, dmg * 2 );
            critter->apply_damage( nullptr, bodypart_id( "torso" ), actual_dmg );
            critter->check_dead_state();
            blasted[critter] = actual_dmg;
            continue;
        }

        // Print messages for all NPCs
        pl->add_msg_if_player( m_bad, _( "You're caught in the explosion!" ) );

        struct blastable_part {
            bodypart_id bp;
            float low_mul;
            float high_mul;
            float armor_mul;
        };

        static const std::array<blastable_part, 6> blast_parts = { {
                { bodypart_id( "torso" ), 0.5f, 1.0f, 0.5f },
                { bodypart_id( "head" ),  0.5f, 1.0f, 0.5f },
                // Hit limbs harder so that it hurts more without being much more deadly
                { bodypart_id( "leg_l" ), 0.75f, 1.25f, 0.4f },
                { bodypart_id( "leg_r" ), 0.75f, 1.25f, 0.4f },
                { bodypart_id( "arm_l" ), 0.75f, 1.25f, 0.4f },
                { bodypart_id( "arm_r" ), 0.75f, 1.25f, 0.4f },
            }
        };

        for( const auto &blp : blast_parts ) {
            const int part_dam = rng( force * blp.low_mul, force * blp.high_mul );
            const std::string hit_part_name = body_part_name_accusative( blp.bp->token );
            const auto dmg_instance = damage_instance( DT_BASH, part_dam, 0, blp.armor_mul );
            const auto result = pl->deal_damage( nullptr, blp.bp, dmg_instance );
            const int res_dmg = result.total_damage();

            add_msg( m_debug, "%s for %d raw, %d actual", hit_part_name, part_dam, res_dmg );
            if( res_dmg > 0 ) {
                blasted[critter] += res_dmg;
            }
        }
    }

    return blasted;
}

static std::map<const Creature *, int> do_blast_new( const tripoint &blast_center,
        const float raw_blast_force,
        const float raw_blast_radius )
{
    /*
    Explosions are completed in 3 stages.

    1. Shrapnel
    The very first component: shrapnel is thrown all around.
    Impassable terrain around has not yet been broken down by the blast, so it will shield mobs.

    2. Blast wave
    This propagates the explosion outwards and:
        1. Damage all items in the tile. Done first to prevent loot from being destroyed too much.
        2. Bashes critters (unless they had already been bashed before) in the tile.
        The main bulk of damage is inflicted here.
        3. Flings still alive critters.
        This causes some extra damage depending on how the explosion is set up and may fling the same mob several times.
        This is effective inside buildings since this causes the mobs to be thrown against walls.
        4. Bashes terrain (and vehicles).
        All vehicle parts in the tile are bashed 2 times at full force,
        both to compensate for their tankiness and to make sure they get actually destroyed.
        Terrain is destroyed in a consistent manner.
    */
    using dist_point_pair = std::pair<float, tripoint>;

    // TODO: Move the consts outside the function and make static when this information becomes needed for UI

    // Distance between z-levels
    const int Z_LEVEL_DIST = 4;

    // Since terrain is bashed multiple times, the blast power needs to dissipate with each blast.
    // This factor determines by how much it is dissipated (thus determining multibash amt).
    const float TERRAIN_DISSIPATION_FACTOR = 0.15;

    // Terrain bashing uses relative distance from the epicenter to determine the force needed to break down a piece of furniture
    // By default this makes explosives predictable. To counteract it, a small random value is added
    // to add slightly more jaggedness. This factor determines the maximum value added.
    const float TERRAIN_RANDOM_FACTOR = 0.1;

    // Flinging creatures uses a different scale to determine its damage and range.
    // More specifically, FLING_POWER_FACTOR * FORCE * RADIUS determines the weight
    // in grams that a fling will move by one tile. Half of the weight will move 2 tiles and so on...

    // Calibrated to this value as regular zombies weigh 40750 grams and we want to throw them a bit more than one radius away per 100 blast damage.
    // With 100 blast damage coming from baseline dynamite explosion.
    const float FLING_POWER_FACTOR = 420.0;

    // Flinging light creatures causes them to fly the entire reality and inflicts insane damage
    // This constant limits the maximum fling distance (and indirectly damage) to FLING_HARD_CAP * BLAST RADIUS.
    const float FLING_HARD_CAP = 2.0;

    const int z_levels_affected = raw_blast_radius / Z_LEVEL_DIST;
    const tripoint_range<tripoint> affected_block(
        blast_center + tripoint( -raw_blast_radius, -raw_blast_radius, -z_levels_affected ),
        blast_center + tripoint( raw_blast_radius, raw_blast_radius, z_levels_affected )
    );

    static std::vector<dist_point_pair> blast_map( MAPSIZE_X * MAPSIZE_Y );
    static std::map<tripoint, bool> blast_shield_map;
    static std::map<tripoint, nc_color> explosion_colors;
    blast_map.clear();
    explosion_colors.clear();
    blast_shield_map.clear();

    for( const tripoint &target : affected_block ) {
        if( !get_map().inbounds( target ) ) {
            continue;
        }

        // Uses this ternany check instead of rl_dist because it converts trig_dist's distance to int implicitly
        const float distance = (
                                   trigdist ?
                                   trig_dist( blast_center, target ) :
                                   square_dist( blast_center, target )
                               );
        const float z_distance = abs( target.z - blast_center.z );
        const float z_aware_distance = distance + ( Z_LEVEL_DIST - 1 ) * z_distance;
        if( z_aware_distance <= raw_blast_radius ) {
            blast_map.emplace_back( std::make_pair( z_aware_distance, target ) );
        }
    }

    std::stable_sort( blast_map.begin(), blast_map.end(), []( dist_point_pair pair1,
    dist_point_pair pair2 ) {
        return pair1.first <= pair2.first;
    } );


    int animated_explosion_range = 0.0f;
    std::map<const Creature *, int> blasted;
    std::set<const Creature *> already_flung;
    player *player_flung = nullptr;

    for( const dist_point_pair &pair : blast_map ) {
        float distance;
        tripoint position;
        tripoint last_position = blast_center;
        std::tie( distance, position ) = pair;

        const std::vector<tripoint> line_of_movement = line_to( blast_center, position );
        const bool has_obstacles = std::any_of( line_of_movement.begin(),
        line_of_movement.end(), [position, &last_position]( tripoint ray_position ) {
            if( get_map().obstructed_by_vehicle_rotation( last_position, ray_position ) ) {
                return true;
            }
            last_position = ray_position;
            return ray_position != position && get_map().impassable( ray_position );
        } );

        // Don't bother animating explosions that are on other levels
        const bool to_animate = get_player_character().posz() == position.z;

        // Animate the explosion by drawing the shock wave rather than the whole explosion
        if( to_animate && distance > animated_explosion_range ) {
            draw_custom_explosion( blast_center, explosion_colors, "explosion" );
            explosion_colors.clear();
            animated_explosion_range++;
        }

        if( has_obstacles ) {
            continue;
        }

        if( to_animate ) {
            explosion_colors[position] = c_white;
        }

        // Item damage comes first in order to prevent dropped loot from being destroyed immediately.
        const int smash_force = raw_blast_force * item_blast_percentage( raw_blast_radius, distance );
        get_map().smash_items( position, smash_force, _( "force of the explosion" ), true );

        // Critter damage occurs next to reduce the amount of flung enemies, leading to much less predictable damage output
        if( Creature *critter = g->critter_at( position, true ) ) {
            if( blasted.count( critter ) ) {
                // Prevent multibashes to monsters due to flinging.
                continue;
            }

            const int blast_force = raw_blast_force * critter_blast_percentage( critter, raw_blast_radius,
                                    distance );
            const auto shockwave_dmg = damage_instance::physical( blast_force, 0, 0, 0.4f );

            if( player *player_ptr = dynamic_cast<player *>( critter ) ) {
                player_ptr->add_msg_if_player( m_bad, _( "You're caught in the explosion!" ) );

                struct blastable_part {
                    bodypart_id bp;
                    float low_mul;
                    float high_mul;
                    float armor_mul;
                };

                static const std::array<blastable_part, 6> blast_parts = { {
                        { bodypart_id( "torso" ), 0.5f, 1.0f, 0.5f },
                        { bodypart_id( "head" ),  0.5f, 1.0f, 0.5f },
                        // Hit limbs harder so that it hurts more without being much more deadly
                        { bodypart_id( "leg_l" ), 0.75f, 1.25f, 0.4f },
                        { bodypart_id( "leg_r" ), 0.75f, 1.25f, 0.4f },
                        { bodypart_id( "arm_l" ), 0.75f, 1.25f, 0.4f },
                        { bodypart_id( "arm_r" ), 0.75f, 1.25f, 0.4f },
                    }
                };

                for( const auto &blast_part : blast_parts ) {
                    const int part_dam = rng( blast_force * blast_part.low_mul, blast_force * blast_part.high_mul );
                    const std::string hit_part_name = body_part_name_accusative( blast_part.bp->token );
                    const auto dmg_instance = damage_instance( DT_BASH, part_dam, 0, blast_part.armor_mul );
                    const auto result = player_ptr->deal_damage( nullptr, blast_part.bp, dmg_instance );
                    const int res_dmg = result.total_damage();

                    if( res_dmg > 0 ) {
                        blasted[critter] += res_dmg;
                    }
                }
            } else {
                critter->deal_damage( nullptr, bodypart_id( "torso" ), shockwave_dmg );
                critter->check_dead_state();
                blasted[critter] = blast_force;
            }
        }

        // rng_float is needed to make sure critters at the center get thrown in a random direction.
        units::angle angle = units::atan2( position.y - blast_center.y + rng_float( -0.5f, 0.5f ),
                                           position.x - blast_center.x + rng_float( -0.5f, 0.5f ) );

        // How many grams can the blast fling 1 tile away?
        // Multiplied by ten because this is coupled with fling_creature
        const float move_power = 10 * FLING_POWER_FACTOR * ( raw_blast_radius - distance ) *
                                 raw_blast_force;

        if( Creature *critter = g->critter_at( position, true ) ) {
            if( !already_flung.count( critter ) ) {
                player *pl = dynamic_cast<player *>( critter );

                const int weight = to_gram( critter->get_weight() );
                const int fling_vel = std::min( move_power / std::max( weight, 1 ),
                                                10 * FLING_HARD_CAP * raw_blast_radius );

                if( pl == nullptr ) {
                    g->fling_creature( critter, angle, fling_vel );
                } else {
                    player_flung = pl;
                    g->fling_creature( pl, angle, fling_vel, false, true );
                }
                // Prevent multiflings
                already_flung.insert( critter );
            }
        }

        // Terrain bashes occur last to ensure enemies being bashed against walls if flung.

        // This reduces the randomness factor in terrain bash significantly
        // Which makes explosions have a more well-defined shape.
        const float terrain_factor = std::max( distance - 1.0, 0.0 ) / raw_blast_radius + rng_float( 0.0,
                                     TERRAIN_RANDOM_FACTOR );
        const int terrain_blast_force = raw_blast_force * obstacle_blast_percentage( raw_blast_radius,
                                        distance );

        bash_params shockwave_bash{
            terrain_blast_force,
            false, // Bashing down terrain should not be silent
            false,
            !get_map().impassable( position + tripoint_below ), // We will only try to break the floor down if there is nothing underneath
            terrain_factor,
            blast_center.z > position.z
        };

        if( const optional_vpart_position &vp = get_map().veh_at( position ) ) {
            // HP values of vehicle parts aren't really on the same scale

            // Would be better if explosives had a separate damage value for vehicle parts
            // But for now, this will suffice.

            vehicle &veh_ptr = vp->vehicle();

            const std::vector<int> &affected_part_nums = veh_ptr.parts_at_relative( vp->mount(), false );

            for( const auto part_indx : affected_part_nums ) {
                // Double bash to bypass XX state if possible.
                veh_ptr.damage( part_indx, shockwave_bash.strength, DT_BASH, true );
                veh_ptr.damage( part_indx, shockwave_bash.strength, DT_BASH, true );
            }
        } else {
            // Multibash is done by bashing the tile with decaying force.
            // The reason for this existing is because a number of tiles undergo multiple bashed states
            // Things like doors and wall -> floor -> ground.
            while( shockwave_bash.strength > 0 ) {
                get_map().bash_ter_furn( position, shockwave_bash );
                shockwave_bash.strength = std::max( static_cast<int>( shockwave_bash.strength -
                                                    TERRAIN_DISSIPATION_FACTOR * raw_blast_force ), 0 );
            }
        }
    }

    // Final blast wave points
    draw_custom_explosion( blast_center, explosion_colors, "explosion" );

    if( player_flung ) {
        g->update_map( *player_flung );
    }

    return blasted;
}


static std::map<const Creature *, int> shrapnel( const tripoint &src, const projectile &fragment )
{
    std::map<const Creature *, int> damaged;

    projectile proj = fragment;
    proj.add_effect( ammo_effect_NULL_SOURCE );

    float obstacle_cache[MAPSIZE_X][MAPSIZE_Y] = {};
    float visited_cache[MAPSIZE_X][MAPSIZE_Y] = {};

    map &here = get_map();

    diagonal_blocks( &blocked_cache )[MAPSIZE_X][MAPSIZE_Y] = here.access_cache(
                src.z ).vehicle_obstructed_cache;

    // TODO: Calculate range based on max effective range for projectiles.
    // Basically bisect between 0 and map diameter using shrapnel_calc().
    // Need to update shadowcasting to support limiting range without adjusting initial distance.
    const tripoint_range<tripoint> area = here.points_on_zlevel( src.z );

    here.build_obstacle_cache( area.min(), area.max() + tripoint_south_east, obstacle_cache );

    // Shadowcasting normally ignores the origin square,
    // so apply it manually to catch monsters standing on the explosive.
    // This "blocks" some fragments, but does not apply deceleration.
    visited_cache[src.x][src.y] = 1.0f;

    // This is used to limit radius
    // By default, the radius is 60, so negative values can be helpful here
    const int offset_distance = 60 - 1 - fragment.range;
    castLightAll<float, float, shrapnel_calc, shrapnel_check,
                 update_fragment_cloud, accumulate_fragment_cloud>
                 ( visited_cache, obstacle_cache, blocked_cache, src.xy(),
                   offset_distance, fragment.range + 1.0f );

    // Now visited_caches are populated with density and velocity of fragments.
    for( const tripoint &target : area ) {
        if( visited_cache[target.x][target.y] <= 0.0f || rl_dist( src, target ) > fragment.range ) {
            continue;
        }
        auto critter = g->critter_at( target );
        if( critter && !critter->is_dead_state() ) {
            // dealt_dag->m.total_damage() == 0 means armor block
            // dealt_dag->m.total_damage() > 0 means took damage
            // Need to diffentiate target among player, npc, and monster
            int damage_taken = 0;
            auto bps = critter->get_all_body_parts( true );
            // Humans get hit in all body parts
            if( critter->is_player() ) {
                for( bodypart_id bp : bps ) {
                    // TODO: This shouldn't be needed, get_bps should do it
                    if( Character::bp_to_hp( bp->token ) == num_hp_parts ) {
                        continue;
                    }
                    // TODO: Apply projectile effects
                    // TODO: Penalize low coverage armor
                    // Halve damage to be closer to what monsters take
                    damage_instance half_impact = proj.impact;
                    half_impact.mult_damage( 0.5f );
                    dealt_damage_instance dealt = critter->deal_damage( nullptr, bp, proj.impact );
                    if( dealt.total_damage() > 0 ) {
                        damage_taken += dealt.total_damage();
                    }
                }
            } else {
                dealt_damage_instance dealt = critter->deal_damage( nullptr, bps[0], proj.impact );
                if( dealt.total_damage() > 0 ) {
                    damage_taken += dealt.total_damage();
                }
            }
            damaged[critter] = damage_taken;
        }
        if( here.impassable( target ) ) {
            int damage = proj.impact.total_damage();
            if( optional_vpart_position vp = here.veh_at( target ) ) {
                vp->vehicle().damage( vp->part_index(), damage / 100 );
            } else {
                here.bash( target, damage / 100, true );
            }
        }
    }

    return damaged;
}

void explosion( const tripoint &p, float power, float factor, bool fire, int legacy_casing_mass,
                float )
{
    if( factor >= 1.0f ) {
        debugmsg( "called game::explosion with factor >= 1.0 (infinite size)" );
    }
    explosion_data data;
    data.damage = power * explosion_handler::power_to_dmg_mult;
    data.radius = explosion_handler::blast_radius_from_legacy( power, factor );
    data.fire = fire;
    if( legacy_casing_mass > 0 ) {
        data.fragment = explosion_handler::shrapnel_from_legacy( power, data.radius );
    }
    explosion( p, data );
}

void explosion( const tripoint &p, const explosion_data &ex )
{
    queued_explosion qe( p, ExplosionType::Regular );
    qe.exp_data = ex;
    get_explosion_queue().add( std::move( qe ) );
}

void explosion_funcs::regular( const queued_explosion &qe )
{
    const tripoint &p = qe.pos;
    const explosion_data &ex = qe.exp_data;

    const int noise = ex.damage / explosion_handler::power_to_dmg_mult * ( ex.fire ? 2 : 10 );
    if( noise >= 30 ) {
        sounds::sound( p, noise, sounds::sound_t::combat, _( "a huge explosion!" ), false, "explosion",
                       "huge" );
    } else if( noise >= 4 ) {
        sounds::sound( p, noise, sounds::sound_t::combat, _( "an explosion!" ), false, "explosion",
                       "default" );
    } else if( noise > 0 ) {
        sounds::sound( p, 3, sounds::sound_t::combat, _( "a loud pop!" ), false, "explosion", "small" );
    }

    std::map<const Creature *, int> damaged_by_blast;
    std::map<const Creature *, int> damaged_by_shrapnel;
    const auto &shr = ex.fragment;
    if( shr ) {
        damaged_by_shrapnel = shrapnel( p, shr.value() );
    }

    if( ex.radius >= 0.0f && ex.damage > 0.0f ) {
        if( get_option<bool>( "NEW_EXPLOSIONS" ) && !ex.fire ) {
            damaged_by_blast = do_blast_new( p, ex.damage, ex.radius );
        } else {
            damaged_by_blast = do_blast( p, ex.damage, ex.radius, ex.fire );
        }
    }

    // Not the cleanest way to do it
    std::map<const Creature *, int> total_damaged;
    for( const auto &pr : damaged_by_blast ) {
        total_damaged[pr.first] += pr.second;
    }
    for( const auto &pr : damaged_by_shrapnel ) {
        total_damaged[pr.first] += pr.second;
    }

    const auto print_damage = [&]( const std::pair<const Creature *, int> &pr,
    std::function<bool( const Creature & )> predicate ) {
        if( predicate( *pr.first ) && g->u.sees( *pr.first ) ) {
            const Creature *critter = pr.first;
            bool blasted = damaged_by_blast.find( critter ) != damaged_by_blast.end();
            bool shredded = damaged_by_shrapnel.find( critter ) != damaged_by_shrapnel.end();
            std::string cause_description = ( blasted && shredded ) ? _( "the explosion and shrapnel" ) :
                                            blasted ? _( "the explosion" ) :
                                            _( "the shrapnel" );
            std::string damage_description = ( pr.second > 0 ) ?
                                             string_format( _( "taking %d damage" ), pr.second ) :
                                             _( "but takes no damage" );
            if( critter->is_player() ) {
                add_msg( _( "You are hit by %s, %s." ),
                         cause_description, damage_description );
            } else if( critter->is_npc() ) {
                critter->add_msg_if_npc(
                    _( "<npcname> is hit by %s, %s." ),
                    cause_description, damage_description );
            } else {
                add_msg( _( "%s is hit by %s, %s." ),
                         critter->disp_name( false, true ), cause_description, damage_description );
            }

        }
    };

    // TODO: Clean this up, without affecting the results
    for( const auto &pr : total_damaged ) {
        print_damage( pr, []( const Creature & c ) {
            return c.is_monster();
        } );
    }
    for( const auto &pr : total_damaged ) {
        print_damage( pr, []( const Creature & c ) {
            return c.is_npc();
        } );
    }
    for( const auto &pr : total_damaged ) {
        print_damage( pr, []( const Creature & c ) {
            return c.is_avatar();
        } );
    }
}

void flashbang( const tripoint &p, bool player_immune, const std::string &exp_name )
{
    queued_explosion qe( p, ExplosionType::Flashbang );
    qe.affects_player = !player_immune;
    qe.graphics_name = exp_name;
    get_explosion_queue().add( std::move( qe ) );
}

void explosion_funcs::flashbang( const queued_explosion &qe )
{
    const tripoint &p = qe.pos;
    map &here = get_map();

    draw_explosion( p, 8, c_white, qe.graphics_name );
    int dist = rl_dist( g->u.pos(), p );
    if( dist <= 8 && qe.affects_player ) {
        if( !g->u.has_bionic( bio_ears ) && !g->u.is_wearing( itype_rm13_armor_on ) ) {
            g->u.add_effect( effect_deaf, time_duration::from_turns( 40 - dist * 4 ) );
        }
        if( here.sees( g->u.pos(), p, 8 ) ) {
            int flash_mod = 0;
            if( g->u.has_trait( trait_PER_SLIME ) ) {
                if( one_in( 2 ) ) {
                    flash_mod = 3; // Yay, you weren't looking!
                }
            } else if( g->u.has_trait( trait_PER_SLIME_OK ) ) {
                flash_mod = 8; // Just retract those and extrude fresh eyes
            } else if( g->u.has_bionic( bio_sunglasses ) ||
                       g->u.is_wearing( itype_rm13_armor_on ) ) {
                flash_mod = 6;
            } else if( g->u.worn_with_flag( flag_BLIND ) || g->u.worn_with_flag( flag_FLASH_PROTECTION ) ) {
                flash_mod = 3; // Not really proper flash protection, but better than nothing
            }
            g->u.add_env_effect( effect_blind, bp_eyes, ( 12 - flash_mod - dist ) / 2,
                                 time_duration::from_turns( 10 - dist ) );
        }
    }
    for( monster &critter : g->all_monsters() ) {
        if( critter.type->in_species( ROBOT ) ) {
            continue;
        }
        // TODO: can the following code be called for all types of creatures
        dist = rl_dist( critter.pos(), p );
        if( dist <= 8 ) {
            if( dist <= 4 ) {
                critter.add_effect( effect_stunned, time_duration::from_turns( 10 - dist ) );
            }
            if( critter.has_flag( MF_SEES ) && here.sees( critter.pos(), p, 8 ) ) {
                critter.add_effect( effect_blind, time_duration::from_turns( 18 - dist ) );
            }
            if( critter.has_flag( MF_HEARS ) ) {
                critter.add_effect( effect_deaf, time_duration::from_turns( 60 - dist * 4 ) );
            }
        }
    }
    sounds::sound( p, 12, sounds::sound_t::combat, _( "a huge boom!" ), false, "misc", "flashbang" );
    // TODO: Blind/deafen NPC
}

void shockwave( const tripoint &p, const shockwave_data &sw, const std::string &exp_name )
{
    queued_explosion qe( p, ExplosionType::Shockwave );
    qe.swave_data = sw;
    qe.graphics_name = exp_name;
    get_explosion_queue().add( std::move( qe ) );
}

void explosion_funcs::shockwave( const queued_explosion &qe )
{
    const tripoint &p = qe.pos;
    const shockwave_data &sw = qe.swave_data;

    draw_explosion( p, sw.radius, c_blue, qe.graphics_name );

    sounds::sound( p, sw.force * sw.force * sw.dam_mult / 2, sounds::sound_t::combat, _( "Crack!" ),
                   false,
                   "misc", "shockwave" );

    for( monster &critter : g->all_monsters() ) {
        if( critter.posz() != p.z ) {
            continue;
        }
        if( rl_dist( critter.pos(), p ) <= sw.radius ) {
            add_msg( _( "%s is caught in the shockwave!" ), critter.name() );
            g->knockback( p, critter.pos(), sw.force, sw.stun, sw.dam_mult );
        }
    }
    // TODO: combine the two loops and the case for g->u using all_creatures()
    for( npc &guy : g->all_npcs() ) {
        if( guy.posz() != p.z ) {
            continue;
        }
        if( rl_dist( guy.pos(), p ) <= sw.radius ) {
            add_msg( _( "%s is caught in the shockwave!" ), guy.name );
            g->knockback( p, guy.pos(), sw.force, sw.stun, sw.dam_mult );
        }
    }
    if( rl_dist( g->u.pos(), p ) <= sw.radius && sw.affects_player &&
        ( !g->u.has_trait( trait_LEG_TENT_BRACE ) || g->u.footwear_factor() == 1 ||
          ( g->u.footwear_factor() == .5 && one_in( 2 ) ) ) ) {
        add_msg( m_bad, _( "You're caught in the shockwave!" ) );
        g->knockback( p, g->u.pos(), sw.force, sw.stun, sw.dam_mult );
    }
}

void scrambler_blast( const tripoint &p )
{
    if( monster *const mon_ptr = g->critter_at<monster>( p ) ) {
        monster &critter = *mon_ptr;
        if( critter.has_flag( MF_ELECTRONIC ) ) {
            critter.make_friendly();
        }
        add_msg( m_warning, _( "The %s sparks and begins searching for a target!" ),
                 critter.name() );
    }
}

void emp_blast( const tripoint &p )
{
    map &here = get_map();
    Character &u = get_player_character();
    const bool sight = u.sees( p );
    if( here.has_flag( "CONSOLE", p ) ) {
        if( sight ) {
            add_msg( _( "The %s is rendered non-functional!" ), here.tername( p ) );
        }
        here.ter_set( p, t_console_broken );
        return;
    }
    // TODO: More terrain effects.
    if( here.ter( p ) == t_card_science || here.ter( p ) == t_card_military ||
        here.ter( p ) == t_card_industrial ) {
        int rn = rng( 1, 100 );
        if( rn > 92 || rn < 40 ) {
            if( sight ) {
                add_msg( _( "The card reader is rendered non-functional." ) );
            }
            here.ter_set( p, t_card_reader_broken );
        }
        if( rn > 80 ) {
            if( sight ) {
                add_msg( _( "The nearby doors slide open!" ) );
            }
            for( int i = -3; i <= 3; i++ ) {
                for( int j = -3; j <= 3; j++ ) {
                    tripoint p2 = p + tripoint( i, j, 0 );
                    if( here.ter( p2 ) == t_door_metal_locked ) {
                        here.ter_set( p2, t_floor );
                    }
                }
            }
        }
        if( rn >= 40 && rn <= 80 ) {
            if( sight ) {
                add_msg( _( "Nothing happens." ) );
            }
        }
    }
    if( monster *const mon_ptr = g->critter_at<monster>( p ) ) {
        monster &critter = *mon_ptr;
        if( critter.has_flag( MF_ELECTRONIC ) ) {
            int deact_chance = 0;
            const auto mon_item_id = critter.type->revert_to_itype;
            switch( critter.get_size() ) {
                case MS_TINY:
                    deact_chance = 6;
                    break;
                case MS_SMALL:
                    deact_chance = 3;
                    break;
                default:
                    // Currently not used, I have no idea what chances bigger bots should have,
                    // Maybe export this to json?
                    break;
            }
            if( !mon_item_id.is_empty() && deact_chance != 0 && one_in( deact_chance ) ) {
                if( sight ) {
                    add_msg( _( "The %s beeps erratically and deactivates!" ), critter.name() );
                }
                here.add_item_or_charges( p, critter.to_item() );
                for( auto &ammodef : critter.ammo ) {
                    if( ammodef.second > 0 ) {
                        here.spawn_item( p, ammodef.first, 1, ammodef.second, calendar::turn );
                    }
                }
                g->remove_zombie( critter );
            } else {
                if( sight ) {
                    add_msg( _( "The EMP blast fries the %s!" ), critter.name() );
                }
                int dam = dice( 10, 10 );
                critter.apply_damage( nullptr, bodypart_id( "torso" ), dam );
                critter.check_dead_state();
                if( !critter.is_dead() && one_in( 6 ) ) {
                    critter.make_friendly();
                }
            }
        } else if( critter.has_flag( MF_ELECTRIC_FIELD ) ) {
            if( !critter.has_effect( effect_emp ) ) {
                if( sight ) {
                    add_msg( m_good, _( "The %s's electrical field momentarily goes out!" ), critter.name() );
                }
                critter.add_effect( effect_emp, 3_minutes );
            } else if( critter.has_effect( effect_emp ) ) {
                int dam = dice( 3, 5 );
                if( sight ) {
                    add_msg( m_good, _( "The %s's disabled electrical field reverses polarity!" ),
                             critter.name() );
                    add_msg( m_good, _( "It takes %d damage." ), dam );
                }
                critter.add_effect( effect_emp, 1_minutes );
                critter.apply_damage( nullptr, bodypart_id( "torso" ), dam );
                critter.check_dead_state();
            }
        } else if( sight ) {
            add_msg( _( "The %s is unaffected by the EMP blast." ), critter.name() );
        }
    }
    if( u.pos() == p ) {
        if( u.get_power_level() > 0_kJ ) {
            add_msg( m_bad, _( "The EMP blast drains your power." ) );
            int max_drain = ( u.get_power_level() > 1000_kJ ? 1000 : units::to_kilojoule(
                                  u.get_power_level() ) );
            u.mod_power_level( units::from_kilojoule( -rng( 1 + max_drain / 3, max_drain ) ) );
        }
        // TODO: More effects?
        //e-handcuffs effects
        if( u.weapon.typeId() == itype_e_handcuffs && u.weapon.charges > 0 ) {
            u.weapon.unset_flag( "NO_UNWIELD" );
            u.weapon.charges = 0;
            u.weapon.active = false;
            add_msg( m_good, _( "The %s on your wrists spark briefly, then release your hands!" ),
                     u.weapon.tname() );
        }
    }
    // Drain any items of their battery charge
    for( auto &it : here.i_at( p ) ) {
        if( it.is_tool() && it.ammo_current() == itype_battery ) {
            it.charges = 0;
        }
    }
    // TODO: Drain NPC energy reserves
}

void resonance_cascade( const tripoint &p )
{
    get_explosion_queue().add( queued_explosion( p, ExplosionType::ResonanceCascade ) );
}

void explosion_funcs::resonance_cascade( const queued_explosion &qe )
{
    map &here = get_map();
    const tripoint &p = qe.pos;

    const time_duration maxglow = time_duration::from_turns( 100 - 5 * trig_dist( p, g->u.pos() ) );
    if( maxglow > 0_turns ) {
        const time_duration minglow = std::max( 0_turns, time_duration::from_turns( 60 - 5 * trig_dist( p,
                                                g->u.pos() ) ) );
        g->u.add_effect( effect_teleglow, rng( minglow, maxglow ) * 100 );
    }

    constexpr half_open_rectangle<point> map_bounds( point_zero, point( MAPSIZE_X, MAPSIZE_Y ) );
    constexpr point cascade_reach( 8, 8 );

    point start = clamp( p.xy() - cascade_reach, map_bounds );
    point end = clamp( p.xy() + cascade_reach, map_bounds );

    std::vector<int> rolls;

    tripoint dest = p;
    for( dest.y = start.y; dest.y < end.y; dest.y++ ) {
        for( dest.x = start.x; dest.x < end.x; dest.x++ ) {
            switch( rng( 0, 80 ) ) {
                case 1:
                case 2:
                    emp_blast( dest );
                    break;
                case 3:
                case 4:
                case 5:
                    for( int k = -1; k <= 1; k++ ) {
                        for( int l = -1; l <= 1; l++ ) {
                            field_type_id type = fd_null;
                            switch( rng( 1, 7 ) ) {
                                case 1:
                                    type = fd_blood;
                                    break;
                                case 2:
                                    type = fd_bile;
                                    break;
                                case 3:
                                case 4:
                                    type = fd_slime;
                                    break;
                                case 5:
                                    type = fd_fire;
                                    break;
                                case 6:
                                case 7:
                                    type = fd_nuke_gas;
                                    break;
                            }
                            if( !one_in( 3 ) ) {
                                here.add_field( dest + point( k, l ), type, 3 );
                            }
                        }
                    }
                    break;
                case  6:
                case  7:
                case  8:
                case  9:
                case 10:
                    here.trap_set( dest, tr_portal );
                    break;
                case 11:
                case 12:
                    here.trap_set( dest, tr_goo );
                    break;
                case 13:
                case 14:
                case 15: {
                    MonsterGroupResult spawn_details = MonsterGroupManager::GetResultFromGroup( GROUP_NETHER );
                    g->place_critter_at( spawn_details.name, dest );
                    break;
                }
                case 16:
                case 17:
                case 18:
                    here.destroy( dest );
                    break;
                case 19: {
                    explosion_data ex;
                    ex.radius = 1;
                    ex.damage = rng( 1, 10 );
                    ex.fire = one_in( 4 );
                    explosion( dest, ex );
                    break;
                }
                default:
                    break;
            }
        }
    }
}

projectile shrapnel_from_legacy( int power, float blast_radius )
{
    int range = 2 * blast_radius;
    // Damage approximately equal to blast damage at epicenter
    int damage = power * power_to_dmg_mult;
    projectile proj;
    proj.speed = 1000;
    proj.range = range;
    proj.impact.add_damage( DT_CUT, damage, 0.0f, 3.0f );

    return proj;
}

float blast_radius_from_legacy( int power, float distance_factor )
{
    return std::pow( power * power_to_dmg_mult, ( 1.0 / 4.0 ) ) *
           ( std::log( 0.75f ) / std::log( distance_factor ) );
}

explosion_queue &get_explosion_queue()
{
    static explosion_queue singleton;
    return singleton;
}

void explosion_queue::execute()
{
    while( !elems.empty() ) {
        queued_explosion exp = std::move( elems.front() );
        elems.pop_front();
        switch( exp.type ) {
            case ExplosionType::Regular:
                explosion_funcs::regular( exp );
                break;
            case ExplosionType::Flashbang:
                explosion_funcs::flashbang( exp );
                break;
            case ExplosionType::ResonanceCascade:
                explosion_funcs::resonance_cascade( exp );
                break;
            case ExplosionType::Shockwave:
                explosion_funcs::shockwave( exp );
                break;
            default:
                debugmsg( "Explosion type not implemented." );
                break;
        }
    }
}

} // namespace explosion_handler

float shrapnel_calc( const float &intensity, const float &last_obstacle, const int & )
{
    return intensity - last_obstacle;
}

bool shrapnel_check( const float &obstacle, const float &last_intensity )
{
    return last_intensity - obstacle > 0.0f;
}

void update_fragment_cloud( float &output, const float &new_intensity, quadrant )
{
    output = std::max( output, new_intensity );
}

float accumulate_fragment_cloud( const float &cumulative_obstacle, const float &current_obstacle,
                                 const int & )
{
    return std::max( cumulative_obstacle, current_obstacle ) + 1;
}

int explosion_data::safe_range() const
{
    return std::max<int>( radius, fragment ? fragment->range : 0 ) + 1;
}

explosion_data::operator bool() const
{
    return damage > 0 || fragment;
}

