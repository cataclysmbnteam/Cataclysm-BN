#pragma once
#ifndef CATA_SRC_CATALUA_LUNA_DOC_H
#define CATA_SRC_CATALUA_LUNA_DOC_H

#include "catalua_luna.h"
#include "type_id.h"

enum Attitude : int;
enum body_part : int;
enum character_movemode : int;
enum class add_type : int;
enum color_id : int;
enum damage_type : int;
enum game_message_type : int;
enum m_flag : int;
enum creature_size : int;
enum mf_attitude : int;
enum monster_attitude : int;
enum npc_attitude : int;
enum npc_need : int;
namespace sfx
{
enum class channel : int;
}

class avatar;
class Character;
class character_id;
class Creature;
class distribution_grid;
class distribution_grid_tracker;
class effect_type;
class item;
class item_stack;
class ma_buff;
class map;
class map_stack;
class monster;
class npc;
class player;
class query_popup;
class recipe;
class Skill;
class SkillLevel;
class SkillLevelMap;
class time_duration;
class time_point;
class tinymap;
class uilist;
struct body_part_type;
struct damage_instance;
struct damage_unit;
struct dealt_damage_instance;
struct field_type;
struct npc_opinion;
struct npc_personality;
struct point;
struct species_type;
struct tripoint;
namespace units
{
template<typename V, typename U>
class quantity;

class angle_in_radians_tag;
using angle = quantity<double, angle_in_radians_tag>;

class energy_in_joule_tag;
using energy = quantity<int, energy_in_joule_tag>;

class mass_in_milligram_tag;
using mass = quantity<std::int64_t, mass_in_milligram_tag>;

class volume_in_milliliter_tag;
using volume = quantity<int, volume_in_milliliter_tag>;
}


// These definitions help the doc generator
LUNA_DOC( bool, "bool" );
LUNA_DOC( int, "int" );
LUNA_DOC( unsigned int, "int" );
LUNA_DOC( std::int64_t, "int" );
//LUNA_DOC( size_t, "int" ); This conflicts with the previous unsigned int def on some systems
LUNA_DOC( float, "double" );
LUNA_DOC( double, "double" );
LUNA_DOC( void, "nil" );
LUNA_DOC( char, "char" );
LUNA_DOC( signed char, "char" );
LUNA_DOC( unsigned char, "char" );
LUNA_DOC( const char *, "string" );
LUNA_DOC( std::string, "string" );
LUNA_DOC( std::string_view, "string" );
LUNA_DOC( sol::lua_nil_t, "nil" );
LUNA_DOC( sol::variadic_args, "..." );
LUNA_DOC( sol::this_state, "<this_state>" );
LUNA_DOC( sol::protected_function, "function" );


// These definitions are for the bindings generator
LUNA_VAL( avatar, "Avatar" );
LUNA_VAL( Character, "Character" );
LUNA_VAL( character_id, "CharacterId" );
LUNA_VAL( color_id, "Color" );
LUNA_VAL( Creature, "Creature" );
LUNA_VAL( damage_instance, "DamageInstance" );
LUNA_VAL( damage_unit, "DamageUnit" );
LUNA_VAL( dealt_damage_instance, "DealtDamageInstance" );
LUNA_VAL( distribution_grid, "DistributionGrid" );
LUNA_VAL( distribution_grid_tracker, "DistributionGridTracker" );
LUNA_VAL( item, "Item" );
LUNA_VAL( item_stack, "ItemStack" );
LUNA_VAL( map, "Map" );
LUNA_VAL( map_stack, "MapStack" );
LUNA_VAL( monster, "Monster" );
LUNA_VAL( npc, "Npc" );
LUNA_VAL( npc_opinion, "NpcOpinion" );
LUNA_VAL( npc_personality, "NpcPersonality" );
LUNA_VAL( player, "Player" );
LUNA_VAL( point, "Point" );
LUNA_VAL( query_popup, "QueryPopup" );
LUNA_VAL( SkillLevelMap, "SkillLevelMap" );
LUNA_VAL( SkillLevel, "SkillLevel" );
LUNA_VAL( time_duration, "TimeDuration" );
LUNA_VAL( time_point, "TimePoint" );
LUNA_VAL( tinymap, "Tinymap" );
LUNA_VAL( tripoint, "Tripoint" );
LUNA_VAL( uilist, "UiList" );
LUNA_VAL( units::angle, "Angle" );
LUNA_VAL( units::energy, "Energy" );
LUNA_VAL( units::mass, "Mass" );
LUNA_VAL( units::volume, "Volume" );


// Ids for in-game objects
LUNA_ID( activity_type, "ActivityType" )
LUNA_ID( bionic_data, "BionicData" )
LUNA_ID( body_part_type, "BodyPartType" )
LUNA_ID( disease_type, "DiseaseType" )
LUNA_ID( effect_type, "EffectType" )
LUNA_ID( faction, "Faction" )
LUNA_ID( field_type, "FieldType" )
LUNA_ID( furn_t, "Furn" )
LUNA_ID( itype, "Itype" )
LUNA_ID( json_flag, "JsonFlag" )
LUNA_ID( json_trait_flag, "JsonTraitFlag" )
LUNA_ID( ma_buff, "MartialArtsBuff" )
LUNA_ID( monfaction, "MonsterFaction" )
LUNA_ID( morale_type_data, "MoraleTypeData" )
LUNA_ID( mutation_branch, "MutationBranch" )
LUNA_ID( mutation_category_trait, "MutationCategoryTrait" )
LUNA_ID( recipe, "Recipe" )
LUNA_ID( Skill, "Skill" )
LUNA_ID( species_type, "SpeciesType" )
LUNA_ID( ter_t, "Ter" )

// Enums
LUNA_ENUM( add_type, "AddictionType" )
LUNA_ENUM( Attitude, "Attitude" )
LUNA_ENUM( body_part, "BodyPart" )
LUNA_ENUM( character_movemode, "CharacterMoveMode" )
LUNA_ENUM( damage_type, "DamageType" )
LUNA_ENUM( game_message_type, "MsgType" )
LUNA_ENUM( mf_attitude, "MonsterFactionAttitude" )
LUNA_ENUM( m_flag, "MonsterFlag" )
LUNA_ENUM( monster_attitude, "MonsterAttitude" )
LUNA_ENUM( creature_size, "MonsterSize" )
LUNA_ENUM( npc_attitude, "NpcAttitude" )
LUNA_ENUM( npc_need, "NpcNeed" )
LUNA_ENUM( sfx::channel, "SfxChannel" )


#endif // CATA_SRC_CATALUA_LUNA_DOC_H
