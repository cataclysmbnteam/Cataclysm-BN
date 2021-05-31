#include "mondefense.h"

#include <algorithm>
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "avatar.h"
#include "ballistics.h"
#include "bodypart.h"
#include "creature.h"
#include "damage.h"
#include "game.h"
#include "map.h"
#include "map_iterator.h"
#include "dispersion.h"
#include "enums.h"
#include "gun_mode.h"
#include "item.h"
#include "line.h"
#include "mattack_actors.h"
#include "mattack_common.h"
#include "messages.h"
#include "monster.h"
#include "mtype.h"
#include "npc.h"
#include "player.h"
#include "point.h"
#include "projectile.h"
#include "rng.h"
#include "sounds.h"
#include "string_id.h"
#include "translations.h"
#include "type_id.h"

static const skill_id skill_gun( "gun" );
static const skill_id skill_rifle( "rifle" );

void mdefense::none( monster &, Creature *, const dealt_projectile_attack * )
{
}

void mdefense::zapback( monster &m, Creature *const source,
                        dealt_projectile_attack const *proj )
{
    if( source == nullptr ) {
        return;
    }
    // If we have a projectile, we're a ranged attack, no zapback.
    if( proj != nullptr ) {
        return;
    }

    if( const player *const foe = dynamic_cast<player *>( source ) ) {
        // Players/NPCs can avoid the shock if they wear non-conductive gear on their hands
        for( const item &i : foe->worn ) {
            if( ( i.covers( bp_hand_l ) || i.covers( bp_hand_r ) ) &&
                !i.conductive() && i.get_coverage() >= 95 ) {
                return;
            }
        }
        // Players/NPCs can avoid the shock by using non-conductive weapons
        if( !foe->weapon.conductive() ) {
            if( foe->reach_attacking ) {
                return;
            }
            if( !foe->used_weapon().is_null() ) {
                return;
            }
        }
    }

    if( source->is_elec_immune() ) {
        return;
    }

    if( get_avatar().sees( source->pos() ) ) {
        const auto msg_type = source == &get_avatar() ? m_bad : m_info;
        add_msg( msg_type, _( "Striking the %1$s shocks %2$s!" ),
                 m.name(), source->disp_name() );
    }

    const damage_instance shock {
        DT_ELECTRIC, static_cast<float>( rng( 1, 5 ) )
    };
    source->deal_damage( &m, bodypart_id( "arm_l" ), shock );
    source->deal_damage( &m, bodypart_id( "arm_r" ), shock );

    source->check_dead_state();
}

void mdefense::acidsplash( monster &m, Creature *const source,
                           dealt_projectile_attack const *const proj )
{
    if( source == nullptr ) {
        return;
    }
    size_t num_drops = rng( 4, 6 );
    // Would be useful to have the attack data here, for cutting vs. bashing etc.
    if( proj ) {
        // Projectile didn't penetrate the target, no acid will splash out of it.
        if( proj->dealt_dam.total_damage() <= 0 ) {
            return;
        }
        // Less likely for a projectile to deliver enough force
        if( !one_in( 3 ) ) {
            return;
        }
    } else {
        if( const player *const foe = dynamic_cast<player *>( source ) ) {
            if( foe->weapon.is_melee( DT_CUT ) || foe->weapon.is_melee( DT_STAB ) ) {
                num_drops += rng( 3, 4 );
            }
            if( foe->unarmed_attack() ) {
                const damage_instance acid_burn{
                    DT_ACID, static_cast<float>( rng( 1, 5 ) )
                };
                source->deal_damage( &m, one_in( 2 ) ? bodypart_id( "hand_l" ) : bodypart_id( "hand_r" ),
                                     acid_burn );
                source->add_msg_if_player( m_bad, _( "Acid covering %s burns your hand!" ), m.disp_name() );
            }
        }
    }

    // Don't splatter directly on the `m`, that doesn't work well
    std::vector<tripoint> pts = closest_tripoints_first( source->pos(), 1 );
    pts.erase( std::remove( pts.begin(), pts.end(), m.pos() ), pts.end() );

    projectile prj;
    prj.speed = 10;
    prj.range = 4;
    prj.proj_effects.insert( "DRAW_AS_LINE" );
    prj.proj_effects.insert( "NO_DAMAGE_SCALING" );
    prj.impact.add_damage( DT_ACID, rng( 1, 3 ) );
    for( size_t i = 0; i < num_drops; i++ ) {
        const tripoint &target = random_entry( pts );
        projectile_attack( prj, m.pos(), target, dispersion_sources{ 1200 }, &m );
    }

    if( get_avatar().sees( m.pos() ) ) {
        add_msg( m_warning, _( "Acid sprays out of %s as it is hit!" ), m.disp_name() );
    }
}

void mdefense::return_fire( monster &m, Creature *source, const dealt_projectile_attack *proj )
{
    // No return fire for untargeted projectiles, i.e. from explosions.
    if( source == nullptr ) {
        return;
    }

    // No return fire from dead monsters.
    if( m.is_dead_state() ) {
        return;
    }

    // If target actually was not damaged - then do not bother
    // Also it covers potential exploit - peek throwing potential can be used to exhaust turret ammo
    if( proj->dealt_dam.total_damage() == 0 ) {
        return;
    }

    // No return fire if attacker is seen
    if( m.sees( *source ) ) {
        return;
    }
    ;
    // Simple universal rule for now
    // TODO implement complex rule, dependent on sound and projectile (throwing/bullet)
    int distance_to_source = rl_dist( m.pos(), source->pos() );
    //tripoint fire_point = distance_to_source < 3/* ||
    //                      one_in( 2 )*/ ? source->pos() : move_along_line( m.pos(),
    //                              get_map().find_clear_path( m.pos(), source->pos() ), distance_to_source + 3 );
    tripoint fire_point = source->pos();
    // Add some random innacuracy since it is blind fire
    int dispersion = rng( 0, 400 );
    for( const std::pair<const std::string, mtype_special_attack> &attack : m.type->special_attacks ) {
        if( attack.second->id == "gun" ) {
            sounds::sound( m.pos(), 50, sounds::sound_t::alert,
                           _( "Detected shots from unseen attacker, return fire mode engaged." ) );

            const gun_actor *gunactor = dynamic_cast<const gun_actor *>( attack.second.get() );

            gunactor->shoot( m, fire_point, gun_mode_id( "DEFAULT" ), dispersion );

        }
    }
}
