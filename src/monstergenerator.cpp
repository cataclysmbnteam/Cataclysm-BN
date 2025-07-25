#include "mattack_common.h" // IWYU pragma: associated
#include "monstergenerator.h" // IWYU pragma: associated

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <set>
#include <utility>

#include "assign.h"
#include "bodypart.h"
#include "calendar.h"
#include "catacharset.h"
#include "creature.h"
#include "damage.h"
#include "debug.h"
#include "enum_conversions.h"
#include "game.h"
#include "generic_factory.h"
#include "item.h"
#include "item_group.h"
#include "json.h"
#include "mattack_actors.h"
#include "monattack.h"
#include "mondeath.h"
#include "mondefense.h"
#include "monfaction.h"
#include "monster.h"
#include "mtype.h"
#include "options.h"
#include "legacy_pathfinding.h"
#include "rng.h"
#include "string_id.h"
#include "translations.h"
#include "units.h"

namespace io
{

template<>
std::string enum_to_string<mon_trigger>( mon_trigger data )
{
    switch( data ) {
        // *INDENT-OFF*
        case mon_trigger::STALK: return "STALK";
        case mon_trigger::MEAT: return "MEAT";
        case mon_trigger::HOSTILE_WEAK: return "PLAYER_WEAK";
        case mon_trigger::HOSTILE_CLOSE: return "PLAYER_CLOSE";
        case mon_trigger::HURT: return "HURT";
        case mon_trigger::FIRE: return "FIRE";
        case mon_trigger::FRIEND_DIED: return "FRIEND_DIED";
        case mon_trigger::FRIEND_ATTACKED: return "FRIEND_ATTACKED";
        case mon_trigger::SOUND: return "SOUND";
        case mon_trigger::PLAYER_NEAR_BABY: return "PLAYER_NEAR_BABY";
        case mon_trigger::MATING_SEASON: return "MATING_SEASON";
        case mon_trigger::NETHER_ATTENTION: return "NETHER_ATTENTION";
        // *INDENT-ON*
        case mon_trigger::_LAST:
            break;
    }
    debugmsg( "Invalid mon_trigger" );
    abort();
}

template<>
std::string enum_to_string<m_flag>( m_flag data )
{
    // see mtype.h for commentary
    switch( data ) {
        // *INDENT-OFF*
        case MF_SEES: return "SEES";
        case MF_HEARS: return "HEARS";
        case MF_GOODHEARING: return "GOODHEARING";
        case MF_SMELLS: return "SMELLS";
        case MF_KEENNOSE: return "KEENNOSE";
        case MF_STUMBLES: return "STUMBLES";
        case MF_WARM: return "WARM";
        case MF_NOHEAD: return "NOHEAD";
        case MF_HARDTOSHOOT: return "HARDTOSHOOT";
        case MF_GRABS: return "GRABS";
        case MF_BASHES: return "BASHES";
        case MF_GROUP_BASH: return "GROUP_BASH";
        case MF_DESTROYS: return "DESTROYS";
        case MF_BORES: return "BORES";
        case MF_POISON: return "POISON";
        case MF_VENOM: return "VENOM";
        case MF_BADVENOM: return "BADVENOM";
        case MF_PARALYZE: return "PARALYZEVENOM";
        case MF_BLEED: return "BLEED";
        case MF_WEBWALK: return "WEBWALK";
        case MF_DIGS: return "DIGS";
        case MF_CAN_DIG: return "CAN_DIG";
        case MF_CAN_OPEN_DOORS: return "CAN_OPEN_DOORS";
        case MF_FLIES: return "FLIES";
        case MF_AQUATIC: return "AQUATIC";
        case MF_SWIMS: return "SWIMS";
        case MF_UNUSED_76: return "UNUSED_76";
        case MF_FISHABLE: return "FISHABLE";
        case MF_ATTACKMON: return "ATTACKMON";
        case MF_ANIMAL: return "ANIMAL";
        case MF_PLASTIC: return "PLASTIC";
        case MF_SUNDEATH: return "SUNDEATH";
        case MF_ELECTRIC: return "ELECTRIC";
        case MF_ACIDPROOF: return "ACIDPROOF";
        case MF_ACIDTRAIL: return "ACIDTRAIL";
        case MF_SHORTACIDTRAIL: return "SHORTACIDTRAIL";
        case MF_FIREPROOF: return "FIREPROOF";
        case MF_SLUDGEPROOF: return "SLUDGEPROOF";
        case MF_SLUDGETRAIL: return "SLUDGETRAIL";
        case MF_COLDPROOF: return "COLDPROOF";
        case MF_DARKPROOF: return "DARKPROOF";
        case MF_LIGHTPROOF: return "LIGHTPROOF";
        case MF_PSIPROOF: return "PSIPROOF";
        case MF_BIOPROOF: return "BIOPROOF";
        case MF_FIREY: return "FIREY";
        case MF_QUEEN: return "QUEEN";
        case MF_ELECTRONIC: return "ELECTRONIC";
        case MF_FUR: return "FUR";
        case MF_LEATHER: return "LEATHER";
        case MF_WOOL: return "WOOL";
        case MF_FEATHER: return "FEATHER";
        case MF_CBM_CIV: return "CBM_CIV";
        case MF_BONES: return "BONES";
        case MF_FAT: return "FAT";
        case MF_CONSOLE_DESPAWN: return "CONSOLE_DESPAWN";
        case MF_IMMOBILE: return "IMMOBILE";
        case MF_ID_CARD_DESPAWN: return "ID_CARD_DESPAWN";
        case MF_RIDEABLE_MECH: return "RIDEABLE_MECH";
        case MF_CARD_OVERRIDE: return "CARD_OVERRIDE";
        case MF_MILITARY_MECH: return "MILITARY_MECH";
        case MF_MECH_RECON_VISION: return "MECH_RECON_VISION";
        case MF_MECH_DEFENSIVE: return "MECH_DEFENSIVE";
        case MF_HIT_AND_RUN: return "HIT_AND_RUN";
        case MF_GUILT: return "GUILT";
        case MF_PAY_BOT: return "PAY_BOT";
        case MF_HUMAN: return "HUMAN";
        case MF_NO_BREATHE: return "NO_BREATHE";
        case MF_FLAMMABLE: return "FLAMMABLE";
        case MF_REVIVES: return "REVIVES";
        case MF_CHITIN: return "CHITIN";
        case MF_VERMIN: return "VERMIN";
        case MF_NOGIB: return "NOGIB";
        case MF_ABSORBS: return "ABSORBS";
        case MF_ABSORBS_SPLITS: return "ABSORBS_SPLITS";
        case MF_LARVA: return "LARVA";
        case MF_ARTHROPOD_BLOOD: return "ARTHROPOD_BLOOD";
        case MF_ACID_BLOOD: return "ACID_BLOOD";
        case MF_BILE_BLOOD: return "BILE_BLOOD";
        case MF_CBM_POWER: return "CBM_POWER";
        case MF_CBM_SCI: return "CBM_SCI";
        case MF_CBM_OP: return "CBM_OP";
        case MF_CBM_TECH: return "CBM_TECH";
        case MF_CBM_SUBS: return "CBM_SUBS";
        case MF_SWARMS: return "SWARMS";
        case MF_CLIMBS: return "CLIMBS";
        case MF_GROUP_MORALE: return "GROUP_MORALE";
        case MF_INTERIOR_AMMO: return "INTERIOR_AMMO";
        case MF_NIGHT_INVISIBILITY: return "NIGHT_INVISIBILITY";
        case MF_REVIVES_HEALTHY: return "REVIVES_HEALTHY";
        case MF_NO_NECRO: return "NO_NECRO";
        case MF_PACIFIST: return "PACIFIST";
        case MF_PUSH_MON: return "PUSH_MON";
        case MF_PUSH_VEH: return "PUSH_VEH";
        case MF_AVOID_DANGER_1: return "PATH_AVOID_DANGER_1";
        case MF_AVOID_DANGER_2: return "PATH_AVOID_DANGER_2";
        case MF_AVOID_FALL: return "PATH_AVOID_FALL";
        case MF_AVOID_FIRE: return "PATH_AVOID_FIRE";
        case MF_PRIORITIZE_TARGETS: return "PRIORITIZE_TARGETS";
        case MF_NOT_HALLU: return "NOT_HALLUCINATION";
        case MF_CANPLAY: return "CANPLAY";
        case MF_PET_MOUNTABLE: return "PET_MOUNTABLE";
        case MF_PET_HARNESSABLE: return "PET_HARNESSABLE";
        case MF_DOGFOOD: return "DOGFOOD";
        case MF_MILKABLE: return "MILKABLE";
        case MF_SHEARABLE: return "SHEARABLE";
        case MF_NO_BREED: return "NO_BREED";
        case MF_NO_FUNG_DMG: return "NO_FUNG_DMG";
        case MF_PET_WONT_FOLLOW: return "PET_WONT_FOLLOW";
        case MF_DRIPS_NAPALM: return "DRIPS_NAPALM";
        case MF_DRIPS_GASOLINE: return "DRIPS_GASOLINE";
        case MF_ELECTRIC_FIELD: return "ELECTRIC_FIELD";
        case MF_STUN_IMMUNE: return "STUN_IMMUNE";
        case MF_LOUDMOVES: return "LOUDMOVES";
        case MF_DROPS_AMMO: return "DROPS_AMMO";
        case MF_CAN_BE_ORDERED: return "CAN_BE_ORDERED";
        case MF_SMALL_HEAD: return "SMALL_HEAD";
        case MF_TINY_HEAD: return "TINY_HEAD";
        case MF_NO_HEAD_BONUS_CRIT: return "NO_HEAD_BONUS_CRIT";
        case MF_HEAD_BONUS_MAX_CRIT_1: return "HEAD_BONUS_CRIT_1";
        case MF_HEAD_BONUS_MAX_CRIT_2: return "HEAD_BONUS_CRIT_2";
        case MF_TORSO_BONUS_MAX_CRIT_1: return "TORSO_BONUS_CRIT_1";
        case MF_TORSO_BONUS_MAX_CRIT_2: return "TORSO_BONUS_CRIT_2";
        case MF_PROJECTILE_RESISTANT_1: return "PROJECTILE_RESISTANT_1";
        case MF_PROJECTILE_RESISTANT_2: return "PROJECTILE_RESISTANT_2";
        case MF_PROJECTILE_RESISTANT_3: return "PROJECTILE_RESISTANT_3";
        case MF_PROJECTILE_RESISTANT_4: return "PROJECTILE_RESISTANT_4";
        // *INDENT-ON*
        case m_flag::MF_MAX:
            break;
    }
    debugmsg( "Invalid m_flag" );
    abort();
}

} // namespace io

/** @relates string_id */
template<>
const mtype &string_id<mtype>::obj() const
{
    return MonsterGenerator::generator().mon_templates->obj( *this );
}

/** @relates string_id */
template<>
bool string_id<mtype>::is_valid() const
{
    return MonsterGenerator::generator().mon_templates->is_valid( *this );
}

/** @relates string_id */
template<>
const species_type &string_id<species_type>::obj() const
{
    return MonsterGenerator::generator().mon_species->obj( *this );
}

/** @relates string_id */
template<>
bool string_id<species_type>::is_valid() const
{
    return MonsterGenerator::generator().mon_species->is_valid( *this );
}

MonsterGenerator::MonsterGenerator()
    : mon_templates( "monster type" )
    , mon_species( "species" )
{
    mon_templates->insert( mtype() );
    mon_species->insert( species_type() );
    init_phases();
    init_attack();
    init_defense();
    init_death();
}

MonsterGenerator::~MonsterGenerator() = default;

void MonsterGenerator::reset()
{
    mon_templates->reset();
    mon_templates->insert( mtype() );

    mon_species->reset();
    mon_species->insert( species_type() );

    hallucination_monsters.clear();

    attack_map.clear();
    // Hardcode attacks need to be re-added here
    // TODO: Move initialization from constructor to init()
    init_attack();
}

static int calc_bash_skill( const mtype &t )
{
    // IOW, the critter's max bashing damage
    int ret = t.melee_dice * t.melee_sides;
    // This is for stuff that goes through solid rock: minerbots, dark wyrms, etc
    if( t.has_flag( MF_BORES ) ) {
        ret *= 15;
    } else if( t.has_flag( MF_DESTROYS ) ) {
        ret *= 2.5;
    } else if( !t.has_flag( MF_BASHES ) ) {
        ret = 0;
    }

    return ret;
}

static creature_size volume_to_size( const units::volume &vol )
{
    if( vol <= 7500_ml ) {
        return creature_size::tiny;
    } else if( vol <= 46250_ml ) {
        return creature_size::small;
    } else if( vol <= 77500_ml ) {
        return creature_size::medium;
    } else if( vol <= 483750_ml ) {
        return creature_size::large;
    }
    return creature_size::huge;
}

struct monster_adjustment {
    species_id species;
    std::string stat;
    float stat_adjust;
    std::string flag;
    bool flag_val;
    std::string special;
    void apply( mtype &mon );
};

void monster_adjustment::apply( mtype &mon )
{
    if( !mon.in_species( species ) ) {
        return;
    }
    if( !stat.empty() ) {
        if( stat == "speed" ) {
            mon.speed *= stat_adjust;
        } else if( stat == "hp" ) {
            mon.hp *= stat_adjust;
        }
    }
    if( !flag.empty() ) {
        mon.set_flag( io::string_to_enum<m_flag>( flag ), flag_val );
    }
    if( !special.empty() ) {
        if( special == "nightvision" ) {
            mon.vision_night = mon.vision_day;
        } else if( special == "no_zombify" ) {
            mon.zombify_into = mtype_id::NULL_ID();
        }
    }
}

static std::vector<monster_adjustment> adjustments;

void reset_monster_adjustment()
{
    adjustments.clear();
}

void load_monster_adjustment( const JsonObject &jsobj )
{
    monster_adjustment adj;
    adj.species = species_id( jsobj.get_string( "species" ) );
    if( jsobj.has_member( "stat" ) ) {
        JsonObject stat = jsobj.get_object( "stat" );
        stat.read( "name", adj.stat );
        stat.read( "modifier", adj.stat_adjust );
    }
    if( jsobj.has_member( "flag" ) ) {
        JsonObject flag = jsobj.get_object( "flag" );
        flag.read( "name", adj.flag );
        flag.read( "value", adj.flag_val );
    }
    if( jsobj.has_member( "special" ) ) {
        jsobj.read( "special", adj.special );
    }
    adjustments.push_back( adj );
}

static void build_behavior_tree( mtype &type )
{
    type.set_strategy();
    if( type.has_flag( MF_ABSORBS ) || type.has_flag( MF_ABSORBS_SPLITS ) ) {
        type.add_goal( "absorb_items" );
    }
}

void MonsterGenerator::finalize_mtypes()
{

    mon_templates->finalize();
    for( const auto &elem : mon_templates->get_all() ) {
        mtype &mon = const_cast<mtype &>( elem );
        apply_species_attributes( mon );
        set_species_ids( mon );
        mon.size = volume_to_size( mon.volume );

        if( !mon.has_flag( MF_RIDEABLE_MECH ) ) {
            // adjust for worldgen difficulty parameters
            mon.speed *= get_option<int>( "MONSTER_SPEED" )      / 100.0;
            mon.hp    *= get_option<int>( "MONSTER_RESILIENCE" ) / 100.0;
        }

        for( monster_adjustment adj : adjustments ) {
            adj.apply( mon );
        }

        if( mon.bash_skill < 0 ) {
            mon.bash_skill = calc_bash_skill( mon );
        }

        if( mon.armor_bash < 0 ) {
            mon.armor_bash = 0;
        }
        if( mon.armor_cut < 0 ) {
            mon.armor_cut = 0;
        }
        if( mon.armor_stab < 0 ) {
            mon.armor_stab = mon.armor_cut * 0.8;
        }
        if( mon.armor_bullet < 0 ) {
            mon.armor_bullet = 0;
        }
        if( mon.armor_acid < 0 ) {
            mon.armor_acid = mon.armor_cut * 0.5;
        }
        if( mon.armor_fire < 0 ) {
            mon.armor_fire = 0;
        }
        if( mon.armor_cold < 0 ) {
            mon.armor_cold = 0;
        }
        if( mon.armor_electric < 0 ) {
            mon.armor_electric = 0;
        }

        // Lower bound for hp scaling
        mon.hp = std::max( mon.hp, 1 );

        build_behavior_tree( mon );
        mon.setup_pathfinding_deferred();
    }

    for( const auto &mon : mon_templates->get_all() ) {
        if( !mon.has_flag( MF_NOT_HALLU ) ) {
            hallucination_monsters.push_back( mon.id );
        }

        for( auto &attack : mon.special_attacks ) {
            const mattack_actor &actor = *attack.second;
            mattack_actor &actor_nonconst_hack = const_cast<mattack_actor &>( actor );
            actor_nonconst_hack.finalize();
        }
    }
}

void MonsterGenerator::apply_species_attributes( mtype &mon )
{
    for( const auto &spec : mon.species ) {
        if( !spec.is_valid() ) {
            continue;
        }
        const species_type &mspec = spec.obj();

        mon.flags |= mspec.flags;
        mon.anger |= mspec.anger;
        mon.fear |= mspec.fear;
        mon.placate |= mspec.placate;
    }
}

void MonsterGenerator::init_phases()
{
    phase_map["NULL"] = PNULL;
    phase_map["SOLID"] = SOLID;
    phase_map["LIQUID"] = LIQUID;
    phase_map["GAS"] = GAS;
    phase_map["PLASMA"] = PLASMA;
}

void MonsterGenerator::init_death()
{
    // Drop a body
    death_map["NORMAL"] = &mdeath::normal;
    // Explodes in gibs and chunks
    death_map["SPLATTER"] = &mdeath::splatter;
    // Acid instead of a body
    death_map["ACID"] = &mdeath::acid;
    // Explodes in vomit :3
    death_map["BOOMER"] = &mdeath::boomer;
    // Explodes in glowing vomit :3
    death_map["BOOMER_GLOW"] = &mdeath::boomer_glow;
    // Kill all nearby vines
    death_map["KILL_VINES"] = &mdeath::kill_vines;
    // Kill adjacent vine if it's cut
    death_map["VINE_CUT"] = &mdeath::vine_cut;
    // Destroy all roots
    death_map["TRIFFID_HEART"] = &mdeath::triffid_heart;
    // Explodes in spores D:
    death_map["FUNGUS"] = &mdeath::fungus;
    // Falls apart
    death_map["DISINTEGRATE"] = &mdeath::disintegrate;
    // Spawns 2 half-worms
    death_map["WORM"] = &mdeath::worm;
    // Hallucination disappears
    death_map["DISAPPEAR"] = &mdeath::disappear;
    // Morale penalty
    death_map["GUILT"] = &mdeath::guilt;
    // Frees blobs, redirects to brainblob()
    death_map["BRAINBLOB"] = &mdeath::brainblob;
    // Creates more blobs
    death_map["BLOBSPLIT"] = &mdeath::blobsplit;
    // Reverts dancers
    death_map["JACKSON"] = &mdeath::jackson;
    // Normal death, but melts
    death_map["MELT"] = &mdeath::melt;
    // Removes hypnosis if last one
    death_map["AMIGARA"] = &mdeath::amigara;
    // Turn into a full thing
    death_map["THING"] = &mdeath::thing;
    // Damaging explosion
    death_map["EXPLODE"] = &mdeath::explode;
    // Blinding ray
    death_map["FOCUSEDBEAM"] = &mdeath::focused_beam;
    // Spawns a broken robot.
    death_map["BROKEN"] = &mdeath::broken;
    // Cure verminitis
    death_map["RATKING"] = &mdeath::ratking;
    // Sight returns to normal
    death_map["DARKMAN"] = &mdeath::darkman;
    // Explodes in toxic gas
    death_map["GAS"] = &mdeath::gas;
    // All breathers die
    death_map["KILL_BREATHERS"] = &mdeath::kill_breathers;
    // Gives a message about destroying ammo and then calls "BROKEN"
    death_map["BROKEN_AMMO"] = &mdeath::broken_ammo;
    // Explode like a huge smoke bomb.
    death_map["SMOKEBURST"] = &mdeath::smokeburst;
    // Explode with a cloud of fungal haze.
    death_map["FUNGALBURST"] = &mdeath::fungalburst;
    // Snicker-snack!
    death_map["JABBERWOCKY"] = &mdeath::jabberwock;
    // Take them with you
    death_map["DETONATE"] = &mdeath::detonate;
    // Game over!  Defense mode
    death_map["GAMEOVER"] = &mdeath::gameover;
    // Spawn some cockroach nymphs
    death_map["PREG_ROACH"] = &mdeath::preg_roach;
    // Explode in a fireball
    death_map["FIREBALL"] = &mdeath::fireball;
    // Explode in a huge fireball
    death_map["CONFLAGRATION"] = &mdeath::conflagration;

    /* Currently Unimplemented */
    // Screams loudly
    //death_map["SHRIEK"] = &mdeath::shriek;
    // Wolf's howling
    //death_map["HOWL"] = &mdeath::howl;
    // Rattles like a rattlesnake
    //death_map["RATTLE"] = &mdeath::rattle;
}

void MonsterGenerator::init_attack()
{
    add_hardcoded_attack( "NONE", mattack::none );
    add_hardcoded_attack( "EAT_CROP", mattack::eat_crop );
    add_hardcoded_attack( "EAT_FOOD", mattack::eat_food );
    add_hardcoded_attack( "ANTQUEEN", mattack::antqueen );
    add_hardcoded_attack( "CHECK_UP", mattack::nurse_check_up );
    add_hardcoded_attack( "ASSIST", mattack::nurse_assist );
    add_hardcoded_attack( "OPERATE", mattack::nurse_operate );
    add_hardcoded_attack( "PAID_BOT", mattack::check_money_left );
    add_hardcoded_attack( "SHRIEK", mattack::shriek );
    add_hardcoded_attack( "SHRIEK_ALERT", mattack::shriek_alert );
    add_hardcoded_attack( "SHRIEK_STUN", mattack::shriek_stun );
    add_hardcoded_attack( "RATTLE", mattack::rattle );
    add_hardcoded_attack( "HOWL", mattack::howl );
    add_hardcoded_attack( "ACID", mattack::acid );
    add_hardcoded_attack( "ACID_BARF", mattack::acid_barf );
    add_hardcoded_attack( "ACID_ACCURATE", mattack::acid_accurate );
    add_hardcoded_attack( "SHOCKSTORM", mattack::shockstorm );
    add_hardcoded_attack( "SHOCKING_REVEAL", mattack::shocking_reveal );
    add_hardcoded_attack( "PULL_METAL_WEAPON", mattack::pull_metal_weapon );
    add_hardcoded_attack( "BOOMER", mattack::boomer );
    add_hardcoded_attack( "BOOMER_GLOW", mattack::boomer_glow );
    add_hardcoded_attack( "RESURRECT", mattack::resurrect );
    add_hardcoded_attack( "SMASH", mattack::smash );
    add_hardcoded_attack( "SCIENCE", mattack::science );
    add_hardcoded_attack( "GROWPLANTS", mattack::growplants );
    add_hardcoded_attack( "GROW_VINE", mattack::grow_vine );
    add_hardcoded_attack( "VINE", mattack::vine );
    add_hardcoded_attack( "SPIT_SAP", mattack::spit_sap );
    add_hardcoded_attack( "TRIFFID_HEARTBEAT", mattack::triffid_heartbeat );
    add_hardcoded_attack( "FUNGUS", mattack::fungus );
    add_hardcoded_attack( "FUNGUS_ADVANCED", mattack::fungus_advanced );
    add_hardcoded_attack( "FUNGUS_CORPORATE", mattack::fungus_corporate );
    add_hardcoded_attack( "FUNGUS_HAZE", mattack::fungus_haze );
    add_hardcoded_attack( "FUNGUS_BIG_BLOSSOM", mattack::fungus_big_blossom );
    add_hardcoded_attack( "FUNGUS_INJECT", mattack::fungus_inject );
    add_hardcoded_attack( "FUNGUS_BRISTLE", mattack::fungus_bristle );
    add_hardcoded_attack( "FUNGUS_GROWTH", mattack::fungus_growth );
    add_hardcoded_attack( "FUNGUS_SPROUT", mattack::fungus_sprout );
    add_hardcoded_attack( "FUNGUS_FORTIFY", mattack::fungus_fortify );
    add_hardcoded_attack( "DERMATIK", mattack::dermatik );
    add_hardcoded_attack( "DERMATIK_GROWTH", mattack::dermatik_growth );
    add_hardcoded_attack( "FUNGAL_TRAIL", mattack::fungal_trail );
    add_hardcoded_attack( "PLANT", mattack::plant );
    add_hardcoded_attack( "DISAPPEAR", mattack::disappear );
    add_hardcoded_attack( "FORMBLOB", mattack::formblob );
    add_hardcoded_attack( "CALLBLOBS", mattack::callblobs );
    add_hardcoded_attack( "JACKSON", mattack::jackson );
    add_hardcoded_attack( "DANCE", mattack::dance );
    add_hardcoded_attack( "DOGTHING", mattack::dogthing );
    add_hardcoded_attack( "TENTACLE", mattack::tentacle );
    add_hardcoded_attack( "GENE_STING", mattack::gene_sting );
    add_hardcoded_attack( "PARA_STING", mattack::para_sting );
    add_hardcoded_attack( "TRIFFID_GROWTH", mattack::triffid_growth );
    add_hardcoded_attack( "STARE", mattack::stare );
    add_hardcoded_attack( "FEAR_PARALYZE", mattack::fear_paralyze );
    add_hardcoded_attack( "PHOTOGRAPH", mattack::photograph );
    add_hardcoded_attack( "TAZER", mattack::tazer );
    add_hardcoded_attack( "SEARCHLIGHT", mattack::searchlight );
    add_hardcoded_attack( "FLAMETHROWER", mattack::flamethrower );
    add_hardcoded_attack( "COPBOT", mattack::copbot );
    add_hardcoded_attack( "CHICKENBOT", mattack::chickenbot );
    add_hardcoded_attack( "MULTI_ROBOT", mattack::multi_robot );
    add_hardcoded_attack( "RATKING", mattack::ratking );
    add_hardcoded_attack( "GENERATOR", mattack::generator );
    add_hardcoded_attack( "UPGRADE", mattack::upgrade );
    add_hardcoded_attack( "BREATHE", mattack::breathe );
    add_hardcoded_attack( "IMPALE", mattack::impale );
    add_hardcoded_attack( "BRANDISH", mattack::brandish );
    add_hardcoded_attack( "FLESH_GOLEM", mattack::flesh_golem );
    add_hardcoded_attack( "ABSORB_MEAT", mattack::absorb_meat );
    add_hardcoded_attack( "LUNGE", mattack::lunge );
    add_hardcoded_attack( "LONGSWIPE", mattack::longswipe );
    add_hardcoded_attack( "PARROT", mattack::parrot );
    add_hardcoded_attack( "PARROT_AT_DANGER", mattack::parrot_at_danger );
    add_hardcoded_attack( "DARKMAN", mattack::darkman );
    add_hardcoded_attack( "SLIMESPRING", mattack::slimespring );
    add_hardcoded_attack( "EVOLVE_KILL_STRIKE", mattack::evolve_kill_strike );
    add_hardcoded_attack( "LEECH_SPAWNER", mattack::leech_spawner );
    add_hardcoded_attack( "MON_LEECH_EVOLUTION", mattack::mon_leech_evolution );
    add_hardcoded_attack( "TINDALOS_TELEPORT", mattack::tindalos_teleport );
    add_hardcoded_attack( "FLESH_TENDRIL", mattack::flesh_tendril );
    add_hardcoded_attack( "BIO_OP_BIOJUTSU", mattack::bio_op_random_biojutsu );
    add_hardcoded_attack( "BIO_OP_TAKEDOWN", mattack::bio_op_takedown );
    add_hardcoded_attack( "BIO_OP_IMPALE", mattack::bio_op_impale );
    add_hardcoded_attack( "BIO_OP_DISARM", mattack::bio_op_disarm );
    add_hardcoded_attack( "SUICIDE", mattack::suicide );
    add_hardcoded_attack( "KAMIKAZE", mattack::kamikaze );
    add_hardcoded_attack( "GRENADIER", mattack::grenadier );
    add_hardcoded_attack( "GRENADIER_ELITE", mattack::grenadier_elite );
    add_hardcoded_attack( "RIOTBOT", mattack::riotbot );
    add_hardcoded_attack( "STRETCH_ATTACK", mattack::stretch_attack );
    add_hardcoded_attack( "STRETCH_BITE", mattack::stretch_bite );
    add_hardcoded_attack( "RANGED_PULL", mattack::ranged_pull );
    add_hardcoded_attack( "GRAB", mattack::grab );
    add_hardcoded_attack( "GRAB_DRAG", mattack::grab_drag );
    add_hardcoded_attack( "DOOT", mattack::doot );
    add_hardcoded_attack( "ZOMBIE_FUSE", mattack::zombie_fuse );
    add_hardcoded_attack( "COMMAND_BUFF", mattack::command_buff );
}

void MonsterGenerator::init_defense()
{
    // No special attack-back
    defense_map["NONE"] = &mdefense::none;
    // Shock attacker on hit
    defense_map["ZAPBACK"] = &mdefense::zapback;
    // Splash acid on the attacker
    defense_map["ACIDSPLASH"] = &mdefense::acidsplash;
    // Blind fire on unseen attacker
    defense_map["RETURN_FIRE"] = &mdefense::return_fire;
    // Make allies aggro on target
    defense_map["REVENGE_AGGRO"] = &mdefense::revenge_aggro;
}

void MonsterGenerator::set_species_ids( mtype &mon )
{
    for( const auto &s : mon.species ) {
        if( s.is_valid() ) {
            mon.species_ptrs.insert( &s.obj() );
        } else {
            debugmsg( "Tried to assign species %s to monster %s, but no entry for the species exists",
                      s.c_str(), mon.id.c_str() );
        }
    }
}

void MonsterGenerator::load_monster( const JsonObject &jo, const std::string &src )
{
    mon_templates->load( jo, src );
}

mon_effect_data load_mon_effect_data( const JsonObject &e )
{
    bool permanent = e.get_bool( "permanent", false );
    if( permanent && json_report_strict ) {
        try {
            e.throw_error( "Effect permanence has been moved to effect_type.  Set permanence there.",
                           "permanent" );
        } catch( const JsonError &ex ) {
            debugmsg( "\n%s", ex.what() );
        }
    }
    return mon_effect_data( efftype_id( e.get_string( "id" ) ), e.get_int( "duration", 0 ),
                            e.get_bool( "affect_hit_bp", false ),
                            get_body_part_token( e.get_string( "bp", "NUM_BP" ) ),
                            e.get_int( "chance", 100 ),
                            permanent );
}

class mon_attack_effect_reader : public generic_typed_reader<mon_attack_effect_reader>
{
    public:
        mon_effect_data get_next( JsonIn &jin ) const {
            JsonObject e = jin.get_object();
            return load_mon_effect_data( e );
        }
        template<typename C>
        void erase_next( JsonIn &jin, C &container ) const {
            const efftype_id id = efftype_id( jin.get_string() );
            reader_detail::handler<C>().erase_if( container, [&id]( const mon_effect_data & e ) {
                return e.id == id;
            } );
        }
};


void pet_food_data::load( const JsonObject &jo )
{
    mandatory( jo, was_loaded, "food", food );
    optional( jo, was_loaded, "feed", feed );
    optional( jo, was_loaded, "pet", pet );
}

void pet_food_data::deserialize( JsonIn &jsin )
{
    JsonObject data = jsin.get_object();
    load( data );
}

void mtype::load( const JsonObject &jo, const std::string &src )
{
    const bool strict = is_strict_enabled( src );

    MonsterGenerator &gen = MonsterGenerator::generator();

    name.make_plural();
    mandatory( jo, was_loaded, "name", name );

    optional( jo, was_loaded, "description", description );

    optional( jo, was_loaded, "material", mat, auto_flags_reader<material_id> {} );
    optional( jo, was_loaded, "species", species, auto_flags_reader<species_id> {} );
    optional( jo, was_loaded, "categories", categories, auto_flags_reader<> {} );

    // See monfaction.cpp
    if( !was_loaded || jo.has_member( "default_faction" ) ) {
        const auto faction = mfaction_str_id( jo.get_string( "default_faction" ) );
        default_faction = monfactions::get_or_add_faction( faction );
    }

    if( !was_loaded || jo.has_member( "symbol" ) ) {
        sym = jo.get_string( "symbol" );
        if( utf8_wrapper( sym ).display_width() != 1 ) {
            jo.throw_error( "monster symbol should be exactly one console cell width", "symbol" );
        }
    }
    if( was_loaded && jo.has_member( "copy-from" ) && looks_like.empty() ) {
        looks_like = jo.get_string( "copy-from" );
    }
    jo.read( "looks_like", looks_like );

    assign( jo, "bodytype", bodytype );
    assign( jo, "color", color );
    assign( jo, "volume", volume, strict, 0_ml );
    assign( jo, "weight", weight, strict, 0_gram );

    optional( jo, was_loaded, "phase", phase, make_flag_reader( gen.phase_map, "phase id" ), SOLID );

    assign( jo, "diff", difficulty_base, strict, 0 );
    assign( jo, "hp", hp, strict, 1 );
    assign( jo, "speed", speed, strict, 0 );
    assign( jo, "aggression", agro, strict, -100, 100 );
    assign( jo, "morale", morale, strict );

    assign( jo, "mountable_weight_ratio", mountable_weight_ratio, strict );

    assign( jo, "attack_cost", attack_cost, strict, 0 );
    assign( jo, "melee_skill", melee_skill, strict, 0 );
    assign( jo, "melee_dice", melee_dice, strict, 0 );
    assign( jo, "melee_dice_sides", melee_sides, strict, 0 );

    assign( jo, "grab_strength", grab_strength, strict, 0 );

    assign( jo, "dodge", sk_dodge, strict, 0 );
    assign( jo, "armor_bash", armor_bash, strict, 0 );
    assign( jo, "armor_cut", armor_cut, strict, 0 );
    assign( jo, "armor_dark", armor_dark, strict, 0 );
    assign( jo, "armor_light", armor_light, strict, 0 );
    assign( jo, "armor_psi", armor_psi, strict, 0 );
    assign( jo, "armor_bullet", armor_bullet, strict, 0 );
    assign( jo, "armor_stab", armor_stab, strict, 0 );
    assign( jo, "armor_acid", armor_acid, strict, 0 );
    assign( jo, "armor_fire", armor_fire, strict, 0 );
    assign( jo, "armor_cold", armor_cold, strict, 0 );
    assign( jo, "armor_electric", armor_electric, strict, 0 );

    assign( jo, "vision_day", vision_day, strict, 0 );
    assign( jo, "vision_night", vision_night, strict, 0 );

    optional( jo, was_loaded, "regenerates", regenerates, 0 );
    optional( jo, was_loaded, "regenerates_in_dark", regenerates_in_dark, false );
    optional( jo, was_loaded, "regen_morale", regen_morale, false );

    if( !was_loaded || jo.has_member( "regeneration_modifiers" ) ) {
        regeneration_modifiers.clear();
        add_regeneration_modifiers( jo, "regeneration_modifiers", src );
    } else {
        // Note: regeneration_modifiers left as is, new modifiers are added to it!
        // Note: member name prefixes are compatible with those used by generic_typed_reader
        if( jo.has_object( "extend" ) ) {
            JsonObject tmp = jo.get_object( "extend" );
            tmp.allow_omitted_members();
            add_regeneration_modifiers( tmp, "regeneration_modifiers", src );
        }
        if( jo.has_object( "delete" ) ) {
            JsonObject tmp = jo.get_object( "delete" );
            tmp.allow_omitted_members();
            remove_regeneration_modifiers( tmp, "regeneration_modifiers", src );
        }
    }

    optional( jo, was_loaded, "starting_ammo", starting_ammo );
    optional( jo, was_loaded, "luminance", luminance, 0 );
    optional( jo, was_loaded, "revert_to_itype", revert_to_itype, itype_id() );
    optional( jo, was_loaded, "attack_effs", atk_effs, mon_attack_effect_reader{} );
    optional( jo, was_loaded, "mech_weapon", mech_weapon, itype_id() );
    optional( jo, was_loaded, "mech_str_bonus", mech_str_bonus, 0 );
    optional( jo, was_loaded, "mech_battery", mech_battery, itype_id() );
    optional( jo, was_loaded, "aggro_character", aggro_character, true );

    // TODO: make this work with `was_loaded`
    if( jo.has_array( "melee_damage" ) ) {
        melee_damage = load_damage_instance( jo.get_array( "melee_damage" ) );
    } else if( jo.has_object( "melee_damage" ) ) {
        melee_damage = load_damage_instance( jo );
    }

    // Load pet food data
    optional( jo, was_loaded, "petfood", petfood );

    if( jo.has_array( "scents_tracked" ) ) {
        for( const std::string line : jo.get_array( "scents_tracked" ) ) {
            scents_tracked.emplace( line );
        }
    }

    if( jo.has_array( "scents_ignored" ) ) {
        for( const std::string line : jo.get_array( "scents_ignored" ) ) {
            scents_ignored.emplace( line );
        }
    }

    int bonus_cut = 0;
    if( jo.has_int( "melee_cut" ) ) {
        bonus_cut = jo.get_int( "melee_cut" );
        melee_damage.add_damage( DT_CUT, bonus_cut );
    }

    if( jo.has_member( "death_drops" ) ) {
        death_drops = item_group::load_item_group( jo.get_member( "death_drops" ),
                      "distribution" );
    }

    assign( jo, "harvest", harvest );

    /* Load "on_death": object */
    if( jo.has_object( "on_death" ) ) {
        JsonObject od = jo.get_object( "on_death" );
        // on_death::death_function
        const auto death_reader = make_flag_reader( gen.death_map, "monster death function" );
        optional( od, was_loaded, "death_function", dies, death_reader );

        // on_death::spawn_mon
        std::vector<std::pair<int, mtype_id>> mon_spawns;
        if( od.has_string( "spawn_mon" ) ) {
            mtype_id spawn_mon = mtype_id::NULL_ID();
            assign( od, "spawn_mon", spawn_mon );

            mon_spawns.emplace_back( 0, spawn_mon );
        }

        // on_death::spawn_mon_near
        if( od.has_object( "spawn_mon_near" ) ) {
            JsonObject od_smn = od.get_object( "spawn_mon_near" );
            int dist;
            assign( od_smn, "distance", dist );

            std::vector<mtype_id> mons;
            optional( od_smn, was_loaded, "ids", mons, auto_flags_reader<mtype_id> {} );

            for( const auto &id : mons ) {
                mon_spawns.emplace_back( dist, id );
            }
        }

        // Set the function that generates the spawns.
        if( !mon_spawns.empty() ) {
            on_death.emplace_back( [mon_spawns]( monster & z ) {
                for( const auto &pair : mon_spawns ) {
                    g->place_critter_around( pair.second, z.pos(), pair.first );
                }
            } );
        }
    }
    /* Load "death_function": instead*/
    else {
        const auto death_reader = make_flag_reader( gen.death_map, "monster death function" );
        optional( jo, was_loaded, "death_function", dies, death_reader );
    }

    if( dies.empty() && on_death.empty() ) {
        // TODO: really needed? Is an empty `dies` container not allowed?
        dies.push_back( mdeath::normal );
    }

    if( jo.has_array( "emit_fields" ) ) {
        JsonArray jar = jo.get_array( "emit_fields" );
        if( jar.has_string( 0 ) ) { // TEMPORARY until 0.F
            for( const std::string id : jar ) {
                emit_fields.emplace( emit_id( id ), 1_seconds );
            }
        } else {
            while( jar.has_more() ) {
                JsonObject obj = jar.next_object();
                emit_fields.emplace( emit_id( obj.get_string( "emit_id" ) ),
                                     read_from_json_string<time_duration>( *obj.get_raw( "delay" ), time_duration::units ) );
            }
        }
    }

    if( jo.has_member( "special_when_hit" ) ) {
        JsonArray jsarr = jo.get_array( "special_when_hit" );
        const auto iter = gen.defense_map.find( jsarr.get_string( 0 ) );
        if( iter == gen.defense_map.end() ) {
            jsarr.throw_error( "Invalid monster defense function" );
        }
        sp_defense = iter->second;
        def_chance = jsarr.get_int( 1 );
    } else if( !was_loaded ) {
        sp_defense = &mdefense::none;
        def_chance = 0;
    }

    if( !was_loaded || jo.has_member( "special_attacks" ) ) {
        special_attacks.clear();
        special_attacks_names.clear();
        add_special_attacks( jo, "special_attacks", src );
    } else {
        // Note: special_attacks left as is, new attacks are added to it!
        // Note: member name prefixes are compatible with those used by generic_typed_reader
        if( jo.has_object( "extend" ) ) {
            JsonObject tmp = jo.get_object( "extend" );
            tmp.allow_omitted_members();
            add_special_attacks( tmp, "special_attacks", src );
        }
        if( jo.has_object( "delete" ) ) {
            JsonObject tmp = jo.get_object( "delete" );
            tmp.allow_omitted_members();
            remove_special_attacks( tmp, "special_attacks", src );
        }
    }

    // Disable upgrading when JSON contains `"upgrades": false`, but fallback to the
    // normal behavior (including error checking) if "upgrades" is not boolean or not `false`.
    if( jo.has_bool( "upgrades" ) && !jo.get_bool( "upgrades" ) ) {
        upgrade_group = mongroup_id::NULL_ID();
        upgrade_into = mtype_id::NULL_ID();
        upgrades = false;
    } else if( jo.has_member( "upgrades" ) ) {
        JsonObject up = jo.get_object( "upgrades" );
        optional( up, was_loaded, "half_life", half_life, -1 );
        optional( up, was_loaded, "age_grow", age_grow, -1 );
        optional( up, was_loaded, "into_group", upgrade_group, auto_flags_reader<mongroup_id> {},
                  mongroup_id::NULL_ID() );
        optional( up, was_loaded, "into", upgrade_into, auto_flags_reader<mtype_id> {},
                  mtype_id::NULL_ID() );
        upgrades = true;
    }

    //Reproduction
    if( jo.has_member( "reproduction" ) ) {
        JsonObject repro = jo.get_object( "reproduction" );
        optional( repro, was_loaded, "baby_count", baby_count, -1 );
        if( repro.has_int( "baby_timer" ) ) {
            baby_timer = time_duration::from_days( repro.get_int( "baby_timer" ) );
        } else if( repro.has_string( "baby_timer" ) ) {
            baby_timer = read_from_json_string<time_duration>( *repro.get_raw( "baby_timer" ),
                         time_duration::units );
        }
        optional( repro, was_loaded, "baby_monster", baby_monster, auto_flags_reader<mtype_id> {},
                  mtype_id::NULL_ID() );
        optional( repro, was_loaded, "baby_egg", baby_egg, auto_flags_reader<itype_id> {},
                  itype_id::NULL_ID() );
        reproduces = true;
    }

    if( jo.has_member( "baby_flags" ) ) {
        // Because this determines mating season and some monsters have a mating season but not in-game offspring, declare this separately
        baby_flags.clear();
        for( const std::string line : jo.get_array( "baby_flags" ) ) {
            baby_flags.push_back( line );
        }
    }

    optional( jo, was_loaded, "burn_into", burn_into, auto_flags_reader<mtype_id> {},
              mtype_id::NULL_ID() );
    assign( jo, "zombify_into", zombify_into );
    assign( jo, "fungalize_into", fungalize_into );

    const auto flag_reader = enum_flags_reader<m_flag> { "monster flag" };
    optional( jo, was_loaded, "flags", flags, flag_reader );
    // Can't calculate yet - we want all flags first
    optional( jo, was_loaded, "bash_skill", bash_skill, -1 );

    const auto trigger_reader = enum_flags_reader<mon_trigger> { "monster trigger" };
    optional( jo, was_loaded, "anger_triggers", anger, trigger_reader );
    optional( jo, was_loaded, "placate_triggers", placate, trigger_reader );
    optional( jo, was_loaded, "fear_triggers", fear, trigger_reader );

    if( jo.has_member( "path_settings" ) ) {
        JsonObject jop = jo.get_object( "path_settings" );

        const auto inject_int = [&jop, this]( std::string field ) {
            if( jop.has_member( field ) ) {
                this->recorded_path_settings.insert_or_assign( field, jop.get_int( field ) );
            }
        };
        const auto inject_float = [&jop, this]( std::string field ) {
            if( jop.has_member( field ) ) {
                this->recorded_path_settings.insert_or_assign( field,
                        static_cast<float>( jop.get_float( field ) ) );
            }
        };
        const auto inject_bool = [&jop, this]( std::string field ) {
            if( jop.has_member( field ) ) {
                this->recorded_path_settings.insert_or_assign( field, jop.get_bool( field ) );
            }
        };

        inject_int( "max_dist" );
        inject_int( "max_length" );
        inject_int( "bash_strength" );
        inject_bool( "allow_open_doors" );
        inject_bool( "avoid_traps" );
        inject_bool( "allow_climb_stairs" );
        inject_bool( "avoid_sharp" );
        // New fields start here
        inject_bool( "avoid_rough" );
        inject_float( "h_coeff" );
        inject_float( "alpha" );
        inject_float( "search_radius_coeff" );
        inject_float( "search_cone_angle" );
        inject_float( "max_f_coeff" );
        inject_float( "mob_presence_penalty" );
    }

    // blacklisted_specials was originally ported from DDA PRs 75716 and 75804 and thus CC-BY-SA 3.0
    std::unordered_set<std::string> blacklisted_specials {"PARROT", "PARROT_AT_DANGER", "EAT_CROP", "EAT_FOOD"};
    int special_attacks_diff = 0;
    for( const auto &special : special_attacks ) {
        if( !blacklisted_specials.contains( special.first ) ) {
            special_attacks_diff++;
        }
    }
    difficulty = ( melee_skill + 1 ) * melee_dice * ( bonus_cut + melee_sides ) * 0.04 +
                 ( sk_dodge + 1 ) * ( 3 + armor_bash + armor_cut ) * 0.04 +
                 ( difficulty_base + special_attacks_diff + 8 * emit_fields.size() );
    difficulty *= ( hp + speed - attack_cost + ( morale + agro ) * 0.1 ) * 0.01 +
                  ( vision_day + 2 * vision_night ) * 0.01;
}

void mtype::setup_pathfinding_deferred()
{
    // Initialize with default settings
    this->path_settings.mob_presence_penalty =
        get_option<float>( "PATHFINDING_MOB_PRESENCE_PENALTY_DEFAULT" );
    this->route_settings.h_coeff = get_option<float>( "PATHFINDING_H_COEFF_DEFAULT" );
    this->route_settings.alpha = get_option<float>( "PATHFINDING_ALPHA_DEFAULT" );
    this->route_settings.search_radius_coeff =
        get_option<float>( "PATHFINDING_SEARCH_RADIUS_COEFF_DEFAULT" );
    this->route_settings.search_cone_angle =
        get_option<float>( "PATHFINDING_SEARCH_CONE_ANGLE_DEFAULT" );
    this->route_settings.max_f_coeff = get_option<float>( "PATHFINDING_MAX_F_COEFF_DEFAULT" );
    this->route_settings.f_limit_based_on_max_dist =
        get_option<bool>( "PATHFINDING_MAX_F_LIMIT_BASED_ON_MAX_DIST" );

    const bool default_override = get_option<bool>( "PATHFINDING_DEFAULT_IS_OVERRIDE" );
    const float range_mult = get_option<float>( "PATHFINDING_RANGE_MULT" );

    if( this->has_flag( MF_CLIMBS ) ) {
        this->legacy_path_settings.climb_cost = 3;
        this->path_settings.climb_cost = 3.0;
    }

    if( this->has_flag( MF_FLIES ) ) {
        this->path_settings.can_fly = true;
    }

    const auto extract_into = [this]<typename T>( std::string field, T & out ) {
        if( this->recorded_path_settings.contains( field ) ) {
            out = std::get<T>( this->recorded_path_settings[field] );
        }
    };

    const auto extract_into_with_default =
    [this]<typename T>( std::string field, T & out, T default_val ) {
        if( this->recorded_path_settings.contains( field ) ) {
            out = std::get<T>( this->recorded_path_settings[field] );
        } else {
            out = default_val;
        }
    };

    // Legacy init
    extract_into_with_default( "max_dist", legacy_path_settings.max_dist, 0 );
    extract_into_with_default( "max_length",
                               legacy_path_settings.max_length,
                               this->legacy_path_settings.max_dist * 5 );
    extract_into_with_default( "bash_strength", legacy_path_settings.bash_strength, this->bash_skill );
    extract_into_with_default( "allow_open_doors", legacy_path_settings.allow_open_doors, false );
    extract_into_with_default( "avoid_traps", legacy_path_settings.avoid_traps, false );
    extract_into_with_default( "allow_climb_stairs", legacy_path_settings.allow_climb_stairs, true );
    extract_into_with_default( "avoid_sharp", legacy_path_settings.avoid_sharp, false );

    // New pathfinding init
    extract_into_with_default( "bash_strength", this->path_settings.bash_strength_val,
                               this->bash_skill );
    this->path_settings.bash_strength_val /= this->path_settings.bash_strength_quanta;

    extract_into_with_default( "allow_climb_stairs", this->path_settings.can_climb_stairs, true );

    {
        bool allow_open_doors;
        extract_into_with_default( "allow_open_doors", allow_open_doors, false );
        this->path_settings.door_open_cost = allow_open_doors ? 2.0 : INFINITY;
    }
    {
        bool avoid_traps;
        extract_into_with_default( "avoid_traps", avoid_traps, false );
        this->path_settings.trap_cost = avoid_traps ? INFINITY : 0.0;
    }
    {
        bool avoid_sharp;
        extract_into_with_default( "avoid_sharp", avoid_sharp, false );
        this->path_settings.sharp_terrain_cost = avoid_sharp ? INFINITY : 0.0;
    }
    {
        int max_dist;
        extract_into_with_default( "max_dist", max_dist, 0 );
        this->route_settings.max_dist = static_cast<float>( max_dist );
    }
    {
        int max_length;
        extract_into_with_default( "max_length", max_length, -1 );
        if( max_length >= 0 ) {
            // Explicitly defined max_length requires special handling
            //   and implies f_limit_based_on_max_dist
            if( this->route_settings.max_dist > 0 ) {
                this->route_settings.f_limit_based_on_max_dist = true;
                // multiplied by 0.5 because legacy pathfinding's max_length is scaled by 2.
                this->route_settings.max_f_coeff = 0.5 *
                                                   static_cast<float>( max_length ) /
                                                   this->route_settings.max_dist;
            }
        }
    }
    this->path_settings.move_cost_coeff = this->speed != 0 ? 1.0 / this->speed : INFINITY;

    // Entirely new settings that are not present in legacy pathfinding
    {
        bool avoid_rough;
        extract_into_with_default( "avoid_rough", avoid_rough, false );
        this->path_settings.rough_terrain_cost = avoid_rough ? 16.0 : 0.0;
    }

    if( !default_override ) {
        extract_into( "h_coeff", this->route_settings.h_coeff );
        extract_into( "alpha", this->route_settings.alpha );
        extract_into( "search_radius_coeff", this->route_settings.search_radius_coeff );
        extract_into( "search_cone_angle", this->route_settings.search_cone_angle );
        extract_into( "max_f_coeff", this->route_settings.max_f_coeff );
        extract_into( "mob_presence_penalty", this->path_settings.mob_presence_penalty );
    }

    if( range_mult < 0 ) {
        this->legacy_path_settings.max_dist = INT_MAX;
        this->legacy_path_settings.max_length = INT_MAX;
        this->route_settings.max_dist = INFINITY;
    } else {
        this->legacy_path_settings.max_dist *= range_mult;
        this->legacy_path_settings.max_length *= range_mult;
        this->route_settings.max_dist *= range_mult;
        if( this->route_settings.f_limit_based_on_max_dist ) {
            this->route_settings.max_f_coeff *= range_mult;
        }
    }
    if( this->route_settings.max_f_coeff < 0 ) {
        this->route_settings.max_f_coeff = INFINITY;
    }
    if( this->route_settings.search_radius_coeff < 0 ) {
        this->route_settings.search_radius_coeff = INFINITY;
    }
    if( this->path_settings.mob_presence_penalty < 0 ) {
        this->path_settings.mob_presence_penalty = INFINITY;
    }

    // Set up buffed settings
    pathfinding_settings buffed_legacy_settings = this->legacy_path_settings;

    buffed_legacy_settings.avoid_traps = true;
    buffed_legacy_settings.avoid_sharp = true;
    buffed_legacy_settings.allow_climb_stairs = true;
    buffed_legacy_settings.max_length = std::max( 30, buffed_legacy_settings.max_length );
    buffed_legacy_settings.max_dist = std::max( buffed_legacy_settings.max_length * 5,
                                      buffed_legacy_settings.max_dist );
    this->legacy_path_settings_buffed = buffed_legacy_settings;

    PathfindingSettings buffed_path_settings = this->path_settings;
    RouteSettings buffed_route_settings = this->route_settings;
    // TODO: Make it assign a stockfish preset instead
    buffed_path_settings.bash_cost = 1.0;
    buffed_path_settings.trap_cost = INFINITY;
    buffed_path_settings.sharp_terrain_cost = INFINITY;
    buffed_path_settings.can_climb_stairs = true;
    buffed_route_settings.max_dist = INFINITY;
    buffed_route_settings.max_f_coeff = INFINITY;
    buffed_route_settings.max_s_coeff = INFINITY;
    buffed_route_settings.search_cone_angle = 180.0;
    buffed_route_settings.search_radius_coeff = INFINITY;

    this->path_settings_buffed = buffed_path_settings;
    this->route_settings_buffed = buffed_route_settings;

    this->recorded_path_settings.clear();
}

void MonsterGenerator::load_species( const JsonObject &jo, const std::string &src )
{
    mon_species->load( jo, src );
}

void species_type::load( const JsonObject &jo, const std::string & )
{
    name.make_plural();
    if( !jo.has_member( "name" ) ) {
        debugmsg( "Species %s lacks a name field, will default to id string.", id.c_str() );
    }
    optional( jo, was_loaded, "name", name, no_translation( id.c_str() ) );
    optional( jo, was_loaded, "description", description );
    optional( jo, was_loaded, "footsteps", footsteps, to_translation( "footsteps." ) );
    const auto flag_reader = enum_flags_reader<m_flag> { "monster flag" };
    optional( jo, was_loaded, "flags", flags, flag_reader );

    const auto trigger_reader = enum_flags_reader<mon_trigger> { "monster trigger" };
    optional( jo, was_loaded, "anger_triggers", anger, trigger_reader );
    optional( jo, was_loaded, "placate_triggers", placate, trigger_reader );
    optional( jo, was_loaded, "fear_triggers", fear, trigger_reader );
}

const std::vector<mtype> &MonsterGenerator::get_all_mtypes() const
{
    return mon_templates->get_all();
}

mtype_id MonsterGenerator::get_valid_hallucination() const
{
    return random_entry( hallucination_monsters );
}

class mattack_hardcoded_wrapper : public mattack_actor
{
    private:
        mon_action_attack cpp_function;
    public:
        mattack_hardcoded_wrapper( const mattack_id &id, const mon_action_attack f )
            : mattack_actor( id )
            , cpp_function( f ) { }

        ~mattack_hardcoded_wrapper() override = default;
        bool call( monster &m ) const override {
            return cpp_function( &m );
        }
        std::unique_ptr<mattack_actor> clone() const override {
            return std::make_unique<mattack_hardcoded_wrapper>( *this );
        }

        void load_internal( const JsonObject &, const std::string & ) override {}
};

mtype_special_attack::mtype_special_attack( const mattack_id &id, const mon_action_attack f )
    : mtype_special_attack( std::make_unique<mattack_hardcoded_wrapper>( id, f ) ) {}

void MonsterGenerator::add_hardcoded_attack( const std::string &type, const mon_action_attack f )
{
    add_attack( mtype_special_attack( type, f ) );
}

void MonsterGenerator::add_attack( std::unique_ptr<mattack_actor> ptr )
{
    add_attack( mtype_special_attack( std::move( ptr ) ) );
}

void MonsterGenerator::add_attack( const mtype_special_attack &wrapper )
{
    if( attack_map.contains( wrapper->id ) ) {
        if( test_mode ) {
            debugmsg( "Overwriting monster attack with id %s", wrapper->id.c_str() );
        }

        attack_map.erase( wrapper->id );
    }

    attack_map.emplace( wrapper->id, wrapper );
}

mtype_special_attack MonsterGenerator::create_actor( const JsonObject &obj,
        const std::string &src ) const
{
    // Legacy support: tolerate attack types being specified as the type
    const std::string type = obj.get_string( "type", "monster_attack" );
    const std::string attack_type = obj.get_string( "attack_type", type );

    if( type != "monster_attack" && attack_type != type ) {
        obj.throw_error(
            R"(Specifying "attack_type" is only allowed when "type" is "monster_attack" or not specified)",
            "type" );
    }

    std::unique_ptr<mattack_actor> new_attack;
    if( attack_type == "monster_attack" ) {
        const std::string id = obj.get_string( "id" );
        const auto &iter = attack_map.find( id );
        if( iter == attack_map.end() ) {
            obj.throw_error( "Monster attacks must specify type and/or id", "type" );
        }

        new_attack = iter->second->clone();
    } else if( attack_type == "leap" ) {
        new_attack = std::make_unique<leap_actor>();
    } else if( attack_type == "melee" ) {
        new_attack = std::make_unique<melee_actor>();
    } else if( attack_type == "bite" ) {
        new_attack = std::make_unique<bite_actor>();
    } else if( attack_type == "gun" ) {
        new_attack = std::make_unique<gun_actor>();
    } else if( attack_type == "spell" ) {
        new_attack = std::make_unique<mon_spellcasting_actor>();
    } else {
        obj.throw_error( "unknown monster attack", "attack_type" );
    }

    new_attack->load( obj, src );
    return mtype_special_attack( std::move( new_attack ) );
}

void mattack_actor::load( const JsonObject &jo, const std::string &src )
{
    const bool strict = is_strict_enabled( src );

    // Legacy support
    if( !jo.has_string( "id" ) ) {
        id = jo.get_string( "type" );
    } else {
        // Loading ids can't be strict at the moment, since it has to match the stored version
        assign( jo, "id", id, false );
    }

    assign( jo, "cooldown", cooldown, strict );

    load_internal( jo, src );
    // Set was_loaded manually because we don't have generic_factory to do it for us
    was_loaded = true;
}

void MonsterGenerator::load_monster_attack( const JsonObject &jo, const std::string &src )
{
    add_attack( create_actor( jo, src ) );
}

void mtype::add_special_attack( const JsonObject &obj, const std::string &src )
{
    mtype_special_attack new_attack = MonsterGenerator::generator().create_actor( obj, src );

    if( special_attacks.contains( new_attack->id ) ) {
        special_attacks.erase( new_attack->id );
        const auto iter = std::ranges::find( special_attacks_names,
                                             new_attack->id );
        if( iter != special_attacks_names.end() ) {
            special_attacks_names.erase( iter );
        }
        if( test_mode ) {
            debugmsg( "%s specifies more than one attack of (sub)type %s, ignoring all but the last",
                      id.c_str(), new_attack->id.c_str() );
        }
    }

    special_attacks.emplace( new_attack->id, new_attack );
    special_attacks_names.push_back( new_attack->id );
}

void mtype::add_special_attack( JsonArray inner, const std::string & )
{
    MonsterGenerator &gen = MonsterGenerator::generator();
    const std::string name = inner.get_string( 0 );
    const auto iter = gen.attack_map.find( name );
    if( iter == gen.attack_map.end() ) {
        inner.throw_error( "Invalid special_attacks" );
    }

    if( special_attacks.contains( name ) ) {
        special_attacks.erase( name );
        const auto iter = std::ranges::find( special_attacks_names, name );
        if( iter != special_attacks_names.end() ) {
            special_attacks_names.erase( iter );
        }
        if( test_mode ) {
            debugmsg( "%s specifies more than one attack of (sub)type %s, ignoring all but the last",
                      id.c_str(), name );
        }
    }
    auto new_attack = mtype_special_attack( iter->second );
    new_attack.actor->cooldown = inner.get_int( 1 );
    special_attacks.emplace( name, new_attack );
    special_attacks_names.push_back( name );
}

void mtype::add_special_attacks( const JsonObject &jo, const std::string &member,
                                 const std::string &src )
{

    if( !jo.has_array( member ) ) {
        return;
    }

    for( const JsonValue entry : jo.get_array( member ) ) {
        if( entry.test_array() ) {
            add_special_attack( entry.get_array(), src );
        } else if( entry.test_object() ) {
            add_special_attack( entry.get_object(), src );
        } else {
            entry.throw_error( "array element is neither array nor object." );
        }
    }
}

void mtype::remove_special_attacks( const JsonObject &jo, const std::string &member_name,
                                    const std::string & )
{
    for( const std::string &name : jo.get_tags( member_name ) ) {
        special_attacks.erase( name );
        const auto iter = std::ranges::find( special_attacks_names, name );
        if( iter != special_attacks_names.end() ) {
            special_attacks_names.erase( iter );
        }
    }
}

void mtype::add_regeneration_modifier( const JsonObject &inner, const std::string & )
{
    const std::string effect_name = inner.get_string( "effect" );
    const efftype_id effect( effect_name );
    //TODO: if invalid effect, throw error
    //  inner.throw_error( "Invalid regeneration_modifiers" );

    if( regeneration_modifiers.contains( effect ) ) {
        regeneration_modifiers.erase( effect );
        debugmsg( "%s specifies more than one regeneration modifer for effect %s, ignoring all but the last",
                  id.c_str(), effect_name );
    }
    const float base_mod = inner.get_float( "base_mod", 0.0f );
    const float scaling_mod = inner.get_float( "scaling_mod", 0.0f );

    regeneration_modifiers.emplace( effect, regen_modifier{ base_mod, scaling_mod } );
}

void mtype::add_regeneration_modifiers( const JsonObject &jo, const std::string &member,
                                        const std::string &src )
{
    if( !jo.has_array( member ) ) {
        return;
    }

    for( const JsonValue entry : jo.get_array( member ) ) {
        if( entry.test_object() ) {
            add_regeneration_modifier( entry.get_object(), src );
            // TODO: add support for regeneration_modifer objects
            //} else if ( entry.test_object() ) {
            //    add_regeneration_modifier( entry.get_object(), src );
        } else {
            entry.throw_error( "array element is not an object " );
        }
    }
}

void mtype::remove_regeneration_modifiers( const JsonObject &jo, const std::string &member_name,
        const std::string & )
{
    for( const std::string &name : jo.get_tags( member_name ) ) {
        const efftype_id effect( name );
        regeneration_modifiers.erase( effect );
    }
}

void MonsterGenerator::check_monster_definitions() const
{
    for( const auto &mon : mon_templates->get_all() ) {
        if( !mon.harvest && !mon.has_flag( MF_ELECTRONIC ) && mon.id ) {
            debugmsg( "monster %s has no harvest entry", mon.id.c_str(), mon.harvest.c_str() );
        }
        if( mon.has_flag( MF_MILKABLE ) && mon.starting_ammo.empty() ) {
            debugmsg( "monster %s is flagged milkable, but has no starting ammo", mon.id.c_str() );
        }
        //TODO!: move temporary upwards
        if( mon.has_flag( MF_MILKABLE ) && !mon.starting_ammo.empty() &&
            !item::spawn_temporary( mon.starting_ammo.begin()->first )->made_of( LIQUID ) ) {
            debugmsg( "monster % is flagged milkable, but starting ammo %s is not a liquid type",
                      mon.id.c_str(), mon.starting_ammo.begin()->first.str() );
        }
        if( mon.has_flag( MF_MILKABLE ) && mon.starting_ammo.size() > 1 ) {
            debugmsg( "monster % is flagged milkable, but has multiple starting_ammo defined", mon.id.c_str() );
        }
        for( auto &spec : mon.species ) {
            if( !spec.is_valid() ) {
                debugmsg( "monster %s has invalid species %s", mon.id.c_str(), spec.c_str() );
            }
        }
        if( mon.death_drops && !item_group::group_is_defined( mon.death_drops ) ) {
            debugmsg( "monster %s has unknown death drop item group: %s", mon.id.c_str(),
                      mon.death_drops.c_str() );
        }
        for( auto &m : mon.mat ) {
            if( m.str() == "null" || !m.is_valid() ) {
                debugmsg( "monster %s has unknown material: %s", mon.id.c_str(), m.c_str() );
            }
        }
        if( !mon.revert_to_itype.is_empty() && !mon.revert_to_itype.is_valid() ) {
            debugmsg( "monster %s has unknown revert_to_itype: %s", mon.id.c_str(),
                      mon.revert_to_itype.c_str() );
        }
        if( !mon.mech_weapon.is_empty() && !mon.mech_weapon.is_valid() ) {
            debugmsg( "monster %s has unknown mech_weapon: %s", mon.id.c_str(),
                      mon.mech_weapon.c_str() );
        }
        if( !mon.mech_battery.is_empty() && !mon.mech_battery.is_valid() ) {
            debugmsg( "monster %s has unknown mech_battery: %s", mon.id.c_str(),
                      mon.mech_battery.c_str() );
        }
        for( const scenttype_id &s_id : mon.scents_tracked ) {
            if( !s_id.is_empty() && !s_id.is_valid() ) {
                debugmsg( "monster %s has unknown scents_tracked %s", mon.id.c_str(), s_id.c_str() );
            }
        }
        for( const scenttype_id &s_id : mon.scents_ignored ) {
            if( !s_id.is_empty() && !s_id.is_valid() ) {
                debugmsg( "monster %s has unknown scents_ignored %s", mon.id.c_str(), s_id.c_str() );
            }
        }
        for( auto &s : mon.starting_ammo ) {
            if( !s.first.is_valid() ) {
                debugmsg( "starting ammo %s of monster %s is unknown", s.first.c_str(), mon.id.c_str() );
            }
        }
        for( auto &e : mon.atk_effs ) {
            if( !e.id.is_valid() ) {
                debugmsg( "attack effect %s of monster %s is unknown", e.id.c_str(), mon.id.c_str() );
            }
        }

        for( const std::pair<const emit_id, time_duration> &e : mon.emit_fields ) {
            const emit_id emid = e.first;
            if( !emid.is_valid() ) {
                debugmsg( "monster %s has invalid emit source %s", mon.id.c_str(), emid.c_str() );
            }
        }

        if( mon.upgrades ) {
            if( mon.half_life < 0 && mon.age_grow < 0 ) {
                debugmsg( "half_life %d and age_grow %d (<0) of monster %s is invalid",
                          mon.half_life, mon.age_grow, mon.id.c_str() );
            }
            if( !mon.upgrade_into && !mon.upgrade_group ) {
                debugmsg( "no into nor into_group defined for monster %s", mon.id.c_str() );
            }
            if( mon.upgrade_into && mon.upgrade_group ) {
                debugmsg( "both into and into_group defined for monster %s", mon.id.c_str() );
            }
            if( !mon.upgrade_into.is_valid() ) {
                debugmsg( "upgrade_into %s of monster %s is not a valid monster id",
                          mon.upgrade_into.c_str(), mon.id.c_str() );
            }
            if( !mon.upgrade_group.is_valid() ) {
                debugmsg( "upgrade_group %s of monster %s is not a valid monster group id",
                          mon.upgrade_group.c_str(), mon.id.c_str() );
            }
        }

        if( mon.reproduces ) {
            if( !mon.baby_timer || *mon.baby_timer <= 0_seconds ) {
                debugmsg( "Time between reproductions (%d) is invalid for %s",
                          mon.baby_timer ? to_turns<int>( *mon.baby_timer ) : -1, mon.id.c_str() );
            }
            if( mon.baby_count < 1 ) {
                debugmsg( "Number of children (%d) is invalid for %s",
                          mon.baby_count, mon.id.c_str() );
            }
            if( !mon.baby_monster && mon.baby_egg.is_null() ) {
                debugmsg( "No baby or egg defined for monster %s", mon.id.c_str() );
            }
            if( mon.baby_monster && !mon.baby_egg.is_null() ) {
                debugmsg( "Both an egg and a live birth baby are defined for %s", mon.id.c_str() );
            }
            if( !mon.baby_monster.is_valid() ) {
                debugmsg( "baby_monster %s of monster %s is not a valid monster id",
                          mon.baby_monster.c_str(), mon.id.c_str() );
            }
            if( !mon.baby_egg.is_valid() ) {
                debugmsg( "item_id %s of monster %s is not a valid item id",
                          mon.baby_egg.c_str(), mon.id.c_str() );
            }
        }
    }
}
