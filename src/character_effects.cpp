#include "character_effects.h"

#include "bionics.h"
#include "calendar.h"
#include "character_martial_arts.h"
#include "character.h"
#include "creature.h"
#include "handle_liquid.h"
#include "itype.h"
#include "make_static.h"
#include "map_iterator.h"
#include "player.h"
#include "rng.h"
#include "submap.h"
#include "trap.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vpart_position.h"
#include "weather_gen.h"
#include "weather.h"

static const trait_id trait_CANNIBAL( "CANNIBAL" );
static const trait_id trait_CENOBITE( "CENOBITE" );
static const trait_id trait_CHITIN_FUR( "CHITIN_FUR" );
static const trait_id trait_CHITIN_FUR2( "CHITIN_FUR2" );
static const trait_id trait_CHITIN_FUR3( "CHITIN_FUR3" );
static const trait_id trait_CHLOROMORPH( "CHLOROMORPH" );
static const trait_id trait_EASYSLEEPER( "EASYSLEEPER" );
static const trait_id trait_EASYSLEEPER2( "EASYSLEEPER2" );
static const trait_id trait_FELINE_FUR( "FELINE_FUR" );
static const trait_id trait_FUR( "FUR" );
static const trait_id trait_INSOMNIA( "INSOMNIA" );
static const trait_id trait_INT_SLIME( "INT_SLIME" );
static const trait_id trait_LIGHTFUR( "LIGHTFUR" );
static const trait_id trait_LOVES_BOOKS( "LOVES_BOOKS" );
static const trait_id trait_LUPINE_FUR( "LUPINE_FUR" );
static const trait_id trait_M_SKIN3( "M_SKIN3" );
static const trait_id trait_NAUSEA( "NAUSEA" );
static const trait_id trait_PER_SLIME_OK( "PER_SLIME_OK" );
static const trait_id trait_PSYCHOPATH( "PSYCHOPATH" );
static const trait_id trait_SAPIOVORE( "SAPIOVORE" );
static const trait_id trait_SHELL2( "SHELL2" );
static const trait_id trait_SLIMY( "SLIMY" );
static const trait_id trait_SPIRITUAL( "SPIRITUAL" );
static const trait_id trait_STRONGSTOMACH( "STRONGSTOMACH" );
static const trait_id trait_THRESH_SPIDER( "THRESH_SPIDER" );
static const trait_id trait_URSINE_FUR( "URSINE_FUR" );
static const trait_id trait_VOMITOUS( "VOMITOUS" );
static const trait_id trait_WATERSLEEP( "WATERSLEEP" );
static const trait_id trait_WEAKSTOMACH( "WEAKSTOMACH" );
static const trait_id trait_WEB_SPINNER( "WEB_SPINNER" );
static const trait_id trait_WEB_WALKER( "WEB_WALKER" );
static const trait_id trait_WEB_WEAVER( "WEB_WEAVER" );

static const std::string flag_FUNGUS( "FUNGUS" );
static const std::string flag_SWIMMABLE( "SWIMMABLE" );

static const efftype_id effect_boomered( "boomered" );
static const efftype_id effect_darkness( "darkness" );
static const efftype_id effect_downed( "downed" );
static const efftype_id effect_drunk( "drunk" );
static const efftype_id effect_meth( "meth" );
static const efftype_id effect_nausea( "nausea" );
static const efftype_id effect_onfire( "onfire" );
static const efftype_id effect_weed_high( "weed_high" );

static const skill_id skill_swimming( "swimming" );

static const bionic_id bio_ground_sonar( "bio_ground_sonar" );
static const bionic_id bio_soporific( "bio_soporific" );

static const itype_id itype_cookbook_human( "cookbook_human" );

namespace character_effects
{

stat_mod get_pain_penalty( const Character &ch )
{
    stat_mod ret;
    int pain = ch.get_perceived_pain();
    if( pain <= 0 ) {
        return ret;
    }

    int stat_penalty = std::floor( std::pow( pain, 0.8f ) / 10.0f );

    bool ceno = ch.has_trait( trait_CENOBITE );
    if( !ceno ) {
        ret.strength = stat_penalty;
        ret.dexterity = stat_penalty;
    }

    if( !ch.has_trait( trait_INT_SLIME ) ) {
        ret.intelligence = stat_penalty;
    } else {
        ret.intelligence = pain / 5;
    }

    ret.perception = stat_penalty * 2 / 3;

    ret.speed = std::pow( pain, 0.7f );
    if( ceno ) {
        ret.speed /= 2;
    }

    ret.speed = std::min( ret.speed, 30 );
    return ret;
}

int get_kcal_speed_penalty( float kcal_percent )
{
    static const std::vector<std::pair<float, float>> starv_thresholds = { {
            std::make_pair( 0.0f, -90.0f ),
            std::make_pair( 0.1f, -50.f ),
            std::make_pair( 0.3f, -25.0f ),
            std::make_pair( 0.5f, 0.0f )
        }
    };
    if( kcal_percent > 0.95f ) {
        return 0;
    } else {
        return std::round( multi_lerp( starv_thresholds, kcal_percent ) );
    }
}

int get_thirst_speed_penalty( int thirst )
{
    // We die at 1200 thirst
    // Start by dropping speed really fast, but then level it off a bit
    static const std::vector<std::pair<float, float>> thirst_thresholds = {{
            std::make_pair( static_cast<float>( thirst_levels::very_thirsty ), 0.0f ),
            std::make_pair( static_cast<float>( thirst_levels::dehydrated ), -25.0f ),
            std::make_pair( static_cast<float>( thirst_levels::parched ), -50.0f ),
            std::make_pair( static_cast<float>( thirst_levels::dead ), -75.0f )
        }
    };
    return static_cast<int>( multi_lerp( thirst_thresholds, thirst ) );
}

int calc_morale_fatigue_cap( int fatigue )
{
    if( fatigue >= fatigue_levels::massive ) {
        return 20;
    } else if( fatigue >= fatigue_levels::exhausted ) {
        return 40;
    } else if( fatigue >= fatigue_levels::dead_tired ) {
        return 60;
    } else if( fatigue >= fatigue_levels::tired ) {
        return 80;
    }
    return 0;
}

double vomit_mod( const Character &ch )
{
    double mod = 1;
    if( ch.has_effect( effect_weed_high ) ) {
        mod *= .1;
    }
    if( ch.has_trait( trait_STRONGSTOMACH ) ) {
        mod *= .5;
    }
    if( ch.has_trait( trait_WEAKSTOMACH ) ) {
        mod *= 2;
    }
    if( ch.has_trait( trait_NAUSEA ) ) {
        mod *= 3;
    }
    if( ch.has_trait( trait_VOMITOUS ) ) {
        mod *= 3;
    }
    // If you're already nauseous, any food in your stomach greatly
    // increases chance of vomiting. Water doesn't provoke vomiting, though.
    if( ch.stomach.get_calories() > 0 && ch.has_effect( effect_nausea ) ) {
        mod *= 5 * ch.get_effect_int( effect_nausea );
    }
    return mod;
}

int talk_skill( const Character &ch )
{
    /** @EFFECT_INT slightly increases talking skill */

    /** @EFFECT_PER slightly increases talking skill */

    /** @EFFECT_SPEECH increases talking skill */
    int ret = ch.get_int() + ch.get_per() + ch.get_skill_level( skill_id( "speech" ) ) * 3;
    return ret;
}

int intimidation( const Character &ch )
{
    /** @EFFECT_STR increases intimidation factor */
    int ret = ch.get_str() * 2;
    if( ch.weapon.is_gun() ) {
        ret += 10;
    }
    if( ch.weapon.damage_melee( DT_BASH ) >= 12 ||
        ch.weapon.damage_melee( DT_CUT ) >= 12 ||
        ch.weapon.damage_melee( DT_STAB ) >= 12 ) {
        ret += 5;
    }

    if( ch.get_stim() > 20 ) {
        ret += 2;
    }
    if( ch.has_effect( effect_drunk ) ) {
        ret -= 4;
    }

    return ret;
}

} // namespace character_effects
