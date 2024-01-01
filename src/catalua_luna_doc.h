#pragma once
#ifndef CATA_SRC_CATALUA_LUNA_DOC_H
#define CATA_SRC_CATALUA_LUNA_DOC_H

#include "catalua_luna.h"
#include "type_id.h"

#include "units_angle.h"  // 'units' namespace 'types'/'classes' are actually defined
#include "units_energy.h" // through 'using' with a template class.
#include "units_mass.h"
#include "units_volume.h"

enum color_id : int;
enum game_message_type : int;

enum Attitude : int;
enum body_part : int;
enum character_movemode : int;
enum class add_type : int;
enum damage_type : int;
enum mf_attitude : int;
enum npc_attitude : int;
enum npc_need : int;

class avatar;
class Character;
class Creature;
class distribution_grid_tracker;
class distribution_grid;
class effect_type;
class item_stack;
class item;
class map_stack;
class map;
class monster;
class npc;
class player;
class query_popup;
class time_duration;
class time_point;
class tinymap;
class uilist;
struct body_part_type;
struct field_type;
struct point;
struct tripoint;

class character_id;
class Skill;
class SkillLevel;
class SkillLevelMap;
class recipe;
struct npc_opinion;
struct npc_personality;


// These definitions help the doc generator
LUNA_DOC( bool, "bool" );
LUNA_DOC( int, "int" );
LUNA_DOC( size_t, "int" );
LUNA_DOC( float, "double" );
LUNA_DOC( double, "double" );
LUNA_DOC( void, "nil" );
LUNA_DOC( char, "char" );
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
LUNA_VAL( color_id, "Color" );
LUNA_VAL( Creature, "Creature" );
LUNA_VAL( distribution_grid_tracker, "DistributionGridTracker" );
LUNA_VAL( distribution_grid, "DistributionGrid" );
LUNA_VAL( item_stack, "ItemStack" );
LUNA_VAL( item, "Item" );
LUNA_VAL( map_stack, "MapStack" );
LUNA_VAL( map, "Map" );
LUNA_VAL( monster, "Monster" );
LUNA_VAL( npc, "Npc" );
LUNA_VAL( player, "Player" );
LUNA_VAL( point, "Point" );
LUNA_VAL( query_popup, "QueryPopup" );
LUNA_VAL( time_duration, "TimeDuration" );
LUNA_VAL( time_point, "TimePoint" );
LUNA_VAL( tinymap, "Tinymap" );
LUNA_VAL( tripoint, "Tripoint" );
LUNA_VAL( uilist, "UiList" );

LUNA_VAL( character_id, "CharacterId" );
LUNA_VAL( npc_opinion, "NpcOpinion" );
LUNA_VAL( npc_personality, "NpcPersonality" );
LUNA_VAL( units::angle, "UnitsAngle" );
LUNA_VAL( units::energy, "UnitsEnergy" );
LUNA_VAL( units::mass, "UnitsMass" );
LUNA_VAL( units::volume, "UnitsVolume" );
LUNA_VAL( SkillLevel, "SkillLevel" );
LUNA_VAL( SkillLevelMap, "SkillLevelMap" );


// Ids for in-game objects
LUNA_ID( body_part_type, "BodyPartType" )
LUNA_ID( effect_type, "EffectType" )
LUNA_ID( faction, "Faction" )
LUNA_ID( field_type, "FieldType" )
LUNA_ID( furn_t, "Furn" )
LUNA_ID( itype, "Itype" )
LUNA_ID( ter_t, "Ter" )

LUNA_ID( activity_type, "ActivityType" )
LUNA_ID( bionic_data, "BionicData" )
LUNA_ID( disease_type, "DiseaseType" )
LUNA_ID( monfaction, "MonsterFaction" )
LUNA_ID( morale_type_data, "MoraleTypeData" )
LUNA_ID( mutation_branch, "MutationBranch" )
LUNA_ID( mutation_category_trait, "MutationCategoryTrait" )
LUNA_ID( Skill, "Skill" )
LUNA_ID( json_flag, "JsonFlag" )
LUNA_ID( json_trait_flag, "JsonTraitFlag" )
LUNA_ID( recipe, "Recipe" )

// Enums
LUNA_ENUM( game_message_type, "MsgType" )

LUNA_ENUM( add_type, "AddictionType" )
LUNA_ENUM( Attitude, "Attitude" )
LUNA_ENUM( body_part, "BodyPart" )
LUNA_ENUM( character_movemode, "CharacterMoveMode" )
LUNA_ENUM( damage_type, "DamageType" )
LUNA_ENUM( mf_attitude, "MonsterFactionAttitude" )
LUNA_ENUM( npc_attitude, "NpcAttitude" )
LUNA_ENUM( npc_need, "NpcNeed" )


#endif // CATA_SRC_CATALUA_LUNA_DOC_H
