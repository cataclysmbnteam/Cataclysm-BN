#pragma once
#ifndef CATA_SRC_CATALUA_LUNA_DOC_H
#define CATA_SRC_CATALUA_LUNA_DOC_H

#include "catalua_luna.h"
#include "type_id.h"

enum color_id : int;
enum game_message_type : int;

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


// Ids for in-game objects
LUNA_ID( body_part_type, "BodyPartType" )
LUNA_ID( effect_type, "EffectType" )
LUNA_ID( faction, "Faction" )
LUNA_ID( field_type, "FieldType" )
LUNA_ID( furn_t, "Furn" )
LUNA_ID( itype, "Itype" )
LUNA_ID( ter_t, "Ter" )


// Enums
LUNA_ENUM( game_message_type, "MsgType" )


#endif // CATA_SRC_CATALUA_LUNA_DOC_H
