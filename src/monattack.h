#pragma once
#ifndef CATA_SRC_MONATTACK_H
#define CATA_SRC_MONATTACK_H

class monster;
class Creature;

namespace mattack
{
auto none( monster *z ) -> bool;
auto eat_crop( monster *z ) -> bool;
auto eat_food( monster *z ) -> bool;
auto antqueen( monster *z ) -> bool;
auto shriek( monster *z ) -> bool;
auto shriek_alert( monster *z ) -> bool;
auto shriek_stun( monster *z ) -> bool;
auto howl( monster *z ) -> bool;
auto rattle( monster *z ) -> bool;
auto acid( monster *z ) -> bool;
auto acid_accurate( monster *z ) -> bool;
auto acid_barf( monster *z ) -> bool;
auto shockstorm( monster *z ) -> bool;
auto shocking_reveal( monster *z ) -> bool;
auto pull_metal_weapon( monster *z ) -> bool;
auto boomer( monster *z ) -> bool;
auto boomer_glow( monster *z ) -> bool;
auto resurrect( monster *z ) -> bool;
auto smash( monster *z ) -> bool;
void smash_specific( monster *z, Creature *target );
auto science( monster *z ) -> bool;
auto growplants( monster *z ) -> bool;
auto grow_vine( monster *z ) -> bool;
auto vine( monster *z ) -> bool;
auto spit_sap( monster *z ) -> bool;
auto triffid_heartbeat( monster *z ) -> bool;
auto fungus( monster *z ) -> bool;            // Generic fungal spore-launch
auto fungus_corporate( monster *z ) -> bool;   // Used by Crazy Cataclysm; spawns SpOreos(tm).
auto fungus_haze( monster *z ) -> bool;       // Broadly scatter aerobics
auto fungus_big_blossom( monster *z ) -> bool; // Aerobic & anaerobic, as needed
auto fungus_inject( monster *z ) -> bool;     // Directly inject the spores
auto fungus_bristle( monster *z ) -> bool;    // Fungal hedgerow rake & inject
auto fungus_growth( monster *z ) -> bool;     // Sporeling -> fungal creature
auto fungus_sprout( monster *z ) -> bool;     // Grow fungal walls
auto fungus_fortify( monster *z ) -> bool;    // Grow fungal hedgerows
auto impale( monster *z ) -> bool;
auto dermatik( monster *z ) -> bool;
auto dermatik_growth( monster *z ) -> bool;
auto fungal_trail( monster *z ) -> bool;
auto plant( monster *z ) -> bool;
auto disappear( monster *z ) -> bool;
auto formblob( monster *z ) -> bool;
auto callblobs( monster *z ) -> bool;
auto jackson( monster *z ) -> bool;
auto dance( monster *z ) -> bool;
auto dogthing( monster *z ) -> bool;
auto tentacle( monster *z ) -> bool;
auto vortex( monster *z ) -> bool;
auto gene_sting( monster *z ) -> bool;
auto para_sting( monster *z ) -> bool;
auto triffid_growth( monster *z ) -> bool;
auto stare( monster *z ) -> bool;
auto fear_paralyze( monster *z ) -> bool;
auto nurse_check_up( monster *z ) -> bool;
auto nurse_assist( monster *z ) -> bool;
auto nurse_operate( monster *z ) -> bool;
auto check_money_left( monster *z ) -> bool;
auto photograph( monster *z ) -> bool;
auto tazer( monster *z ) -> bool;
auto flamethrower( monster *z ) -> bool;
auto searchlight( monster *z ) -> bool;
auto copbot( monster *z ) -> bool;
auto chickenbot( monster *z ) -> bool;        // Pick from tazer, M4, MGL
auto multi_robot( monster *z ) -> bool;       // Tazer, flame, M4, MGL, or 120mm!
auto ratking( monster *z ) -> bool;
auto generator( monster *z ) -> bool;
auto upgrade( monster *z ) -> bool;
auto breathe( monster *z ) -> bool;
auto brandish( monster *z ) -> bool;
auto flesh_golem( monster *z ) -> bool;
auto absorb_meat( monster *z ) -> bool;
auto lunge( monster *z ) -> bool;
auto longswipe( monster *z ) -> bool;
auto parrot( monster *z ) -> bool;
auto parrot_at_danger( monster *parrot ) -> bool;
auto darkman( monster *z ) -> bool;
auto slimespring( monster *z ) -> bool;
auto evolve_kill_strike( monster *z ) -> bool;
auto leech_spawner( monster *z ) -> bool;
auto mon_leech_evolution( monster *z ) -> bool;
auto tindalos_teleport( monster *z ) -> bool;
auto flesh_tendril( monster *z ) -> bool;
auto bio_op_random_biojutsu( monster *z ) -> bool;
auto bio_op_takedown( monster *z ) -> bool;
auto bio_op_impale( monster *z ) -> bool;
auto bio_op_disarm( monster *z ) -> bool;
auto ranged_pull( monster *z ) -> bool;
auto grab( monster *z ) -> bool;
auto grab_drag( monster *z ) -> bool;
auto suicide( monster *z ) -> bool;
auto thrown_by_judo( monster *z ) -> bool;    //handles zombie getting thrown when u.is_throw_immune()
auto riotbot( monster *z ) -> bool;
auto stretch_attack( monster *z ) -> bool;
auto stretch_bite( monster *z ) -> bool;
auto kamikaze( monster *z ) -> bool;
auto grenadier( monster *z ) -> bool;
auto grenadier_elite( monster *z ) -> bool;
auto doot( monster *z ) -> bool;
auto zombie_fuse( monster *z ) -> bool;

void taze( monster *z, Creature *target );
void rifle( monster *z, Creature *target );             // Automated M4
void frag( monster *z, Creature *target );              // Automated MGL
void tankgun( monster *z, Creature *target );           // Tankbot primary.
void flame( monster *z, Creature *target );

auto dodge_check( monster *z, Creature *target ) -> bool;
} //namespace mattack

#endif // CATA_SRC_MONATTACK_H
