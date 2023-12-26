---
title: Lua API reference
editUrl: false
sidebar:
  badge:
    text: Generated
    status: note
---

:::note

This page is auto-generated from [`data/raw/generate_docs.lua`][generate_docs] and should not be
edited directly.

[generate_docs]: https://github.com/cataclysmbnteam/Cataclysm-BN/blob/main/data/raw/generate_docs.lua

:::

## Avatar

### Bases

- `Player`
- `Character`
- `Creature`

### Constructors

No constructors.

### Members

No members.

## BodyPartTypeId

### Bases

No base classes.

### Constructors

#### `BodyPartTypeId.new()`

#### `BodyPartTypeId.new( BodyPartTypeId )`

#### `BodyPartTypeId.new( BodyPartTypeIntId )`

#### `BodyPartTypeId.new( string )`

### Members

#### obj

Function `( BodyPartTypeId ) -> BodyPartTypeRaw`

#### int_id

Function `( BodyPartTypeId ) -> BodyPartTypeIntId`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( BodyPartTypeId ) -> bool`

#### is_valid

Function `( BodyPartTypeId ) -> bool`

#### str

Function `( BodyPartTypeId ) -> string`

#### NULL_ID

Function `() -> BodyPartTypeId`

#### __tostring

Function `( BodyPartTypeId ) -> string`

#### serialize

Function `( BodyPartTypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( BodyPartTypeId, <cppval: 6JsonIn > )`

## BodyPartTypeIntId

### Bases

No base classes.

### Constructors

#### `BodyPartTypeIntId.new()`

#### `BodyPartTypeIntId.new( BodyPartTypeIntId )`

#### `BodyPartTypeIntId.new( BodyPartTypeId )`

### Members

#### obj

Function `( BodyPartTypeIntId ) -> BodyPartTypeRaw`

#### str_id

Function `( BodyPartTypeIntId ) -> BodyPartTypeId`

#### is_valid

Function `( BodyPartTypeIntId ) -> bool`

#### __tostring

Function `( BodyPartTypeIntId ) -> string`

## Character

### Bases

- `Creature`

### Constructors

No constructors.

### Members

No members.

## Creature

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### get_pos_ms

Position within map Function `( Creature ) -> Tripoint`

#### is_monster

Function `( Creature ) -> bool`

#### as_monster

Function `( Creature ) -> Monster`

#### is_npc

Function `( Creature ) -> bool`

#### as_npc

Function `( Creature ) -> Npc`

#### is_avatar

Function `( Creature ) -> bool`

#### as_avatar

Function `( Creature ) -> Avatar`

#### has_effect

Function `( Creature, EffectTypeId, Opt(BodyPartTypeId) ) -> bool`

#### get_effect_dur

Function `( Creature, EffectTypeId, Opt(BodyPartTypeId) ) -> TimeDuration`

#### get_effect_int

Function `( Creature, EffectTypeId, Opt(BodyPartTypeId) ) -> int`

#### add_effect

Effect type, duration, bodypart and intensity Function
`( Creature, EffectTypeId, TimeDuration, Opt(BodyPartTypeId), Opt(int) )`

#### remove_effect

Function `( Creature, EffectTypeId, Opt(BodyPartTypeId) ) -> bool`

## DistributionGrid

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### get_resource

Boolean argument controls recursive behavior Function `( DistributionGrid, bool ) -> int`

#### mod_resource

Boolean argument controls recursive behavior Function `( DistributionGrid, int, bool ) -> int`

## DistributionGridTracker

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### get_grid_at_abs_ms

Function `( DistributionGridTracker, Tripoint ) -> DistributionGrid`

## EffectTypeId

### Bases

No base classes.

### Constructors

#### `EffectTypeId.new()`

#### `EffectTypeId.new( EffectTypeId )`

#### `EffectTypeId.new( string )`

### Members

#### obj

Function `( EffectTypeId ) -> EffectTypeRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( EffectTypeId ) -> bool`

#### is_valid

Function `( EffectTypeId ) -> bool`

#### str

Function `( EffectTypeId ) -> string`

#### NULL_ID

Function `() -> EffectTypeId`

#### __tostring

Function `( EffectTypeId ) -> string`

#### serialize

Function `( EffectTypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( EffectTypeId, <cppval: 6JsonIn > )`

## FactionId

### Bases

No base classes.

### Constructors

#### `FactionId.new()`

#### `FactionId.new( FactionId )`

#### `FactionId.new( string )`

### Members

#### obj

Function `( FactionId ) -> FactionRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( FactionId ) -> bool`

#### is_valid

Function `( FactionId ) -> bool`

#### str

Function `( FactionId ) -> string`

#### NULL_ID

Function `() -> FactionId`

#### __tostring

Function `( FactionId ) -> string`

#### serialize

Function `( FactionId, <cppval: 7JsonOut > )`

#### deserialize

Function `( FactionId, <cppval: 6JsonIn > )`

## FactionRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### str_id

Function `( FactionRaw ) -> FactionId`

## FieldTypeId

### Bases

No base classes.

### Constructors

#### `FieldTypeId.new()`

#### `FieldTypeId.new( FieldTypeId )`

#### `FieldTypeId.new( FieldTypeIntId )`

#### `FieldTypeId.new( string )`

### Members

#### obj

Function `( FieldTypeId ) -> FieldTypeRaw`

#### int_id

Function `( FieldTypeId ) -> FieldTypeIntId`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( FieldTypeId ) -> bool`

#### is_valid

Function `( FieldTypeId ) -> bool`

#### str

Function `( FieldTypeId ) -> string`

#### NULL_ID

Function `() -> FieldTypeId`

#### __tostring

Function `( FieldTypeId ) -> string`

#### serialize

Function `( FieldTypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( FieldTypeId, <cppval: 6JsonIn > )`

## FieldTypeIntId

### Bases

No base classes.

### Constructors

#### `FieldTypeIntId.new()`

#### `FieldTypeIntId.new( FieldTypeIntId )`

#### `FieldTypeIntId.new( FieldTypeId )`

### Members

#### obj

Function `( FieldTypeIntId ) -> FieldTypeRaw`

#### str_id

Function `( FieldTypeIntId ) -> FieldTypeId`

#### is_valid

Function `( FieldTypeIntId ) -> bool`

#### __tostring

Function `( FieldTypeIntId ) -> string`

## FurnId

### Bases

No base classes.

### Constructors

#### `FurnId.new()`

#### `FurnId.new( FurnId )`

#### `FurnId.new( FurnIntId )`

#### `FurnId.new( string )`

### Members

#### obj

Function `( FurnId ) -> FurnRaw`

#### int_id

Function `( FurnId ) -> FurnIntId`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( FurnId ) -> bool`

#### is_valid

Function `( FurnId ) -> bool`

#### str

Function `( FurnId ) -> string`

#### NULL_ID

Function `() -> FurnId`

#### __tostring

Function `( FurnId ) -> string`

#### serialize

Function `( FurnId, <cppval: 7JsonOut > )`

#### deserialize

Function `( FurnId, <cppval: 6JsonIn > )`

## FurnIntId

### Bases

No base classes.

### Constructors

#### `FurnIntId.new()`

#### `FurnIntId.new( FurnIntId )`

#### `FurnIntId.new( FurnId )`

### Members

#### obj

Function `( FurnIntId ) -> FurnRaw`

#### str_id

Function `( FurnIntId ) -> FurnId`

#### is_valid

Function `( FurnIntId ) -> bool`

#### __tostring

Function `( FurnIntId ) -> string`

## FurnRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### str_id

Function `( FurnRaw ) -> FurnId`

#### int_id

Function `( FurnRaw ) -> FurnIntId`

#### open

Variable of type `FurnId`

#### close

Variable of type `FurnId`

#### transforms_into

Variable of type `FurnId`

## Item

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### get_type

Function `( Item ) -> ItypeId`

#### has_var

Check for variable of any type Function `( Item, string ) -> bool`

#### erase_var

Erase variable Function `( Item, string )`

#### clear_vars

Erase all variables Function `( Item )`

#### get_var_str

Get variable as string Function `( Item, string, string ) -> string`

#### get_var_num

Get variable as float number Function `( Item, string, double ) -> double`

#### get_var_tri

Get variable as tripoint Function `( Item, string, Tripoint ) -> Tripoint`

#### set_var_str

Function `( Item, string, string )`

#### set_var_num

Function `( Item, string, double )`

#### set_var_tri

Function `( Item, string, Tripoint )`

## ItemStack

Iterate over this using pairs()

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### __pairs

Function
`( ItemStack ) -> ( <cppval: FSt5tupleIJN3sol12basic_objectINS0_15basic_referenceILb0EEEEES4_EENS0_4userIR23item_stack_lua_it_stateEENS0_10this_stateEE >, <cppval: N3sol4userI23item_stack_lua_it_stateEE >, nil )`

## ItypeId

### Bases

No base classes.

### Constructors

#### `ItypeId.new()`

#### `ItypeId.new( ItypeId )`

#### `ItypeId.new( string )`

### Members

#### obj

Function `( ItypeId ) -> ItypeRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( ItypeId ) -> bool`

#### is_valid

Function `( ItypeId ) -> bool`

#### str

Function `( ItypeId ) -> string`

#### NULL_ID

Function `() -> ItypeId`

#### __tostring

Function `( ItypeId ) -> string`

#### serialize

Function `( ItypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( ItypeId, <cppval: 6JsonIn > )`

## Map

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### get_abs_ms

Convert local ms -> absolute ms Function `( Map, Tripoint ) -> Tripoint`

#### get_local_ms

Convert absolute ms -> local ms Function `( Map, Tripoint ) -> Tripoint`

#### get_map_size_in_submaps

Function `( Map ) -> int`

#### get_map_size

In map squares Function `( Map ) -> int`

#### has_items_at

Function `( Map, Tripoint ) -> bool`

#### get_items_at

Function `( Map, Tripoint ) -> <cppval: St10unique_ptrI9map_stackSt14default_deleteIS0_EE >`

#### get_ter_at

Function `( Map, Tripoint ) -> TerIntId`

#### set_ter_at

Function `( Map, Tripoint, TerIntId ) -> bool`

#### get_furn_at

Function `( Map, Tripoint ) -> FurnIntId`

#### set_furn_at

Function `( Map, Tripoint, FurnIntId )`

#### has_field_at

Function `( Map, Tripoint, FieldTypeIntId ) -> bool`

#### get_field_int_at

Function `( Map, Tripoint, FieldTypeIntId ) -> int`

#### get_field_age_at

Function `( Map, Tripoint, FieldTypeIntId ) -> TimeDuration`

#### mod_field_int_at

Function `( Map, Tripoint, FieldTypeIntId, int ) -> int`

#### mod_field_age_at

Function `( Map, Tripoint, FieldTypeIntId, TimeDuration ) -> TimeDuration`

#### set_field_int_at

Function `( Map, Tripoint, FieldTypeIntId, int, bool ) -> int`

#### set_field_age_at

Function `( Map, Tripoint, FieldTypeIntId, TimeDuration, bool ) -> TimeDuration`

#### add_field_at

Function `( Map, Tripoint, FieldTypeIntId, int, TimeDuration ) -> bool`

#### remove_field_at

Function `( Map, Tripoint, FieldTypeIntId )`

## MapStack

### Bases

- `ItemStack`

### Constructors

No constructors.

### Members

#### as_item_stack

Function `( MapStack ) -> ItemStack`

## Monster

### Bases

- `Creature`

### Constructors

No constructors.

### Members

No members.

## Npc

### Bases

- `Player`
- `Character`
- `Creature`

### Constructors

No constructors.

### Members

No members.

## Player

### Bases

- `Character`
- `Creature`

### Constructors

No constructors.

### Members

No members.

## Point

### Bases

No base classes.

### Constructors

#### `Point.new()`

#### `Point.new( Point )`

#### `Point.new( int, int )`

### Members

#### x

Variable of type `int`

#### y

Variable of type `int`

#### abs

Function `( Point ) -> Point`

#### rotate

Function `( Point, int, Point ) -> Point`

#### serialize

Function `( Point, <cppval: 7JsonOut > )`

#### deserialize

Function `( Point, <cppval: 6JsonIn > )`

#### __tostring

Function `( Point ) -> string`

#### __eq

Function `( Point, Point ) -> bool`

#### __lt

Function `( Point, Point ) -> bool`

#### __add

Function `( Point, Point ) -> Point`

#### __sub

Function `( Point, Point ) -> Point`

#### __mul

Function `( Point, int ) -> Point`

#### __div

Function `( Point, int ) -> Point`

#### __idiv

Function `( Point, int ) -> Point`

#### __unm

Function `( Point ) -> Point`

## QueryPopup

### Bases

No base classes.

### Constructors

#### `QueryPopup.new()`

### Members

#### message

Function `( QueryPopup, ... )`

#### message_color

Function `( QueryPopup, Color )`

#### allow_any_key

Set whether to allow any key Function `( QueryPopup, bool )`

#### query

Returns selected action Function `( QueryPopup ) -> string`

## TerId

### Bases

No base classes.

### Constructors

#### `TerId.new()`

#### `TerId.new( TerId )`

#### `TerId.new( TerIntId )`

#### `TerId.new( string )`

### Members

#### obj

Function `( TerId ) -> TerRaw`

#### int_id

Function `( TerId ) -> TerIntId`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( TerId ) -> bool`

#### is_valid

Function `( TerId ) -> bool`

#### str

Function `( TerId ) -> string`

#### NULL_ID

Function `() -> TerId`

#### __tostring

Function `( TerId ) -> string`

#### serialize

Function `( TerId, <cppval: 7JsonOut > )`

#### deserialize

Function `( TerId, <cppval: 6JsonIn > )`

## TerIntId

### Bases

No base classes.

### Constructors

#### `TerIntId.new()`

#### `TerIntId.new( TerIntId )`

#### `TerIntId.new( TerId )`

### Members

#### obj

Function `( TerIntId ) -> TerRaw`

#### str_id

Function `( TerIntId ) -> TerId`

#### is_valid

Function `( TerIntId ) -> bool`

#### __tostring

Function `( TerIntId ) -> string`

## TerRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### str_id

Function `( TerRaw ) -> TerId`

#### int_id

Function `( TerRaw ) -> TerIntId`

#### open

Variable of type `TerId`

#### close

Variable of type `TerId`

#### trap_id_str

Variable of type `string`

#### transforms_into

Variable of type `TerId`

#### roof

Variable of type `TerId`

#### heat_radiation

Variable of type `int`

## TimeDuration

Represent duration between 2 fixed points in time

### Bases

No base classes.

### Constructors

#### `TimeDuration.new()`

### Members

#### from_turns

Function `( int ) -> TimeDuration`

#### from_seconds

Function `( int ) -> TimeDuration`

#### from_minutes

Function `( int ) -> TimeDuration`

#### from_hours

Function `( int ) -> TimeDuration`

#### from_days

Function `( int ) -> TimeDuration`

#### from_weeks

Function `( int ) -> TimeDuration`

#### make_random

Function `( TimeDuration, TimeDuration ) -> TimeDuration`

#### to_turns

Function `( TimeDuration ) -> int`

#### to_seconds

Function `( TimeDuration ) -> int`

#### to_minutes

Function `( TimeDuration ) -> int`

#### to_hours

Function `( TimeDuration ) -> int`

#### to_days

Function `( TimeDuration ) -> int`

#### to_weeks

Function `( TimeDuration ) -> int`

#### serialize

Function `( TimeDuration, <cppval: 7JsonOut > )`

#### deserialize

Function `( TimeDuration, <cppval: 6JsonIn > )`

#### __tostring

Function `( TimeDuration ) -> string`

#### __add

Function `( TimeDuration, TimeDuration ) -> TimeDuration`

#### __sub

Function `( TimeDuration, TimeDuration ) -> TimeDuration`

#### __mul

Function `( TimeDuration, int ) -> TimeDuration`

#### __div

Function `( TimeDuration, int ) -> TimeDuration`

#### __unm

Function `( TimeDuration ) -> TimeDuration`

## TimePoint

Represent fixed point in time

### Bases

No base classes.

### Constructors

#### `TimePoint.new()`

### Members

#### from_turn

Function `( int ) -> TimePoint`

#### to_turn

Function `( TimePoint ) -> int`

#### is_night

Function `( TimePoint ) -> bool`

#### is_day

Function `( TimePoint ) -> bool`

#### is_dusk

Function `( TimePoint ) -> bool`

#### is_dawn

Function `( TimePoint ) -> bool`

#### second_of_minute

Function `( TimePoint ) -> int`

#### minute_of_hour

Function `( TimePoint ) -> int`

#### hour_of_day

Function `( TimePoint ) -> int`

#### serialize

Function `( TimePoint, <cppval: 7JsonOut > )`

#### deserialize

Function `( TimePoint, <cppval: 6JsonIn > )`

#### to_string_time_of_day

Function `( TimePoint ) -> string`

#### __tostring

Function `( TimePoint ) -> string`

#### __eq

Function `( TimePoint, TimePoint ) -> bool`

#### __lt

Function `( TimePoint, TimePoint ) -> bool`

#### __add

Function `( TimePoint, TimeDuration ) -> TimePoint`

#### __sub

Function `( TimePoint, TimePoint ) -> TimeDuration` Function
`( TimePoint, TimeDuration ) -> TimePoint`

## Tinymap

### Bases

- `Map`

### Constructors

No constructors.

### Members

No members.

## Tripoint

### Bases

No base classes.

### Constructors

#### `Tripoint.new()`

#### `Tripoint.new( Point, int )`

#### `Tripoint.new( Tripoint )`

#### `Tripoint.new( int, int, int )`

### Members

#### x

Variable of type `int`

#### y

Variable of type `int`

#### z

Variable of type `int`

#### abs

Function `( Tripoint ) -> Tripoint`

#### xy

Function `( Tripoint ) -> Point`

#### rotate_2d

Function `( Tripoint, int, Point ) -> Tripoint`

#### serialize

Function `( Tripoint, <cppval: 7JsonOut > )`

#### deserialize

Function `( Tripoint, <cppval: 6JsonIn > )`

#### __tostring

Function `( Tripoint ) -> string`

#### __eq

Function `( Tripoint, Tripoint ) -> bool`

#### __lt

Function `( Tripoint, Tripoint ) -> bool`

#### __add

Function `( Tripoint, Tripoint ) -> Tripoint` Function `( Tripoint, Point ) -> Tripoint`

#### __sub

Function `( Tripoint, Tripoint ) -> Tripoint` Function `( Tripoint, Point ) -> Tripoint`

#### __mul

Function `( Tripoint, int ) -> Tripoint`

#### __div

Function `( Tripoint, int ) -> Tripoint`

#### __idiv

Function `( Tripoint, int ) -> Tripoint`

#### __unm

Function `( Tripoint ) -> Tripoint`

## UiList

### Bases

No base classes.

### Constructors

#### `UiList.new()`

### Members

#### title

Function `( UiList, string )`

#### add

Return value, text Function `( UiList, int, string )`

#### query

Returns retval for selected entry, or a negative number on fail/cancel Function `( UiList ) -> int`

# Enums

## Color

### Entries

- `c_black` = `0`
- `c_white` = `1`
- `c_light_gray` = `2`
- `c_dark_gray` = `3`
- `c_red` = `4`
- `c_green` = `5`
- `c_blue` = `6`
- `c_cyan` = `7`
- `c_magenta` = `8`
- `c_brown` = `9`
- `c_light_red` = `10`
- `c_light_green` = `11`
- `c_light_blue` = `12`
- `c_light_cyan` = `13`
- `c_pink` = `14`
- `c_yellow` = `15`
- `h_black` = `16`
- `h_white` = `17`
- `h_light_gray` = `18`
- `h_dark_gray` = `19`
- `h_red` = `20`
- `h_green` = `21`
- `h_blue` = `22`
- `h_cyan` = `23`
- `h_magenta` = `24`
- `h_brown` = `25`
- `h_light_red` = `26`
- `h_light_green` = `27`
- `h_light_blue` = `28`
- `h_light_cyan` = `29`
- `h_pink` = `30`
- `h_yellow` = `31`
- `i_black` = `32`
- `i_white` = `33`
- `i_light_gray` = `34`
- `i_dark_gray` = `35`
- `i_red` = `36`
- `i_green` = `37`
- `i_blue` = `38`
- `i_cyan` = `39`
- `i_magenta` = `40`
- `i_brown` = `41`
- `i_light_red` = `42`
- `i_light_green` = `43`
- `i_light_blue` = `44`
- `i_light_cyan` = `45`
- `i_pink` = `46`
- `i_yellow` = `47`
- `c_unset` = `48`
- `c_black_red` = `49`
- `c_white_red` = `50`
- `c_light_gray_red` = `51`
- `c_dark_gray_red` = `52`
- `c_red_red` = `53`
- `c_green_red` = `54`
- `c_blue_red` = `55`
- `c_cyan_red` = `56`
- `c_magenta_red` = `57`
- `c_brown_red` = `58`
- `c_light_red_red` = `59`
- `c_light_green_red` = `60`
- `c_light_blue_red` = `61`
- `c_light_cyan_red` = `62`
- `c_pink_red` = `63`
- `c_yellow_red` = `64`
- `c_black_white` = `65`
- `c_dark_gray_white` = `66`
- `c_light_gray_white` = `67`
- `c_white_white` = `68`
- `c_red_white` = `69`
- `c_light_red_white` = `70`
- `c_green_white` = `71`
- `c_light_green_white` = `72`
- `c_brown_white` = `73`
- `c_yellow_white` = `74`
- `c_blue_white` = `75`
- `c_light_blue_white` = `76`
- `c_magenta_white` = `77`
- `c_pink_white` = `78`
- `c_cyan_white` = `79`
- `c_light_cyan_white` = `80`
- `c_black_green` = `81`
- `c_dark_gray_green` = `82`
- `c_light_gray_green` = `83`
- `c_white_green` = `84`
- `c_red_green` = `85`
- `c_light_red_green` = `86`
- `c_green_green` = `87`
- `c_light_green_green` = `88`
- `c_brown_green` = `89`
- `c_yellow_green` = `90`
- `c_blue_green` = `91`
- `c_light_blue_green` = `92`
- `c_magenta_green` = `93`
- `c_pink_green` = `94`
- `c_cyan_green` = `95`
- `c_light_cyan_green` = `96`
- `c_black_yellow` = `97`
- `c_dark_gray_yellow` = `98`
- `c_light_gray_yellow` = `99`
- `c_white_yellow` = `100`
- `c_red_yellow` = `101`
- `c_light_red_yellow` = `102`
- `c_green_yellow` = `103`
- `c_light_green_yellow` = `104`
- `c_brown_yellow` = `105`
- `c_yellow_yellow` = `106`
- `c_blue_yellow` = `107`
- `c_light_blue_yellow` = `108`
- `c_magenta_yellow` = `109`
- `c_pink_yellow` = `110`
- `c_cyan_yellow` = `111`
- `c_light_cyan_yellow` = `112`
- `c_black_magenta` = `113`
- `c_dark_gray_magenta` = `114`
- `c_light_gray_magenta` = `115`
- `c_white_magenta` = `116`
- `c_red_magenta` = `117`
- `c_light_red_magenta` = `118`
- `c_green_magenta` = `119`
- `c_light_green_magenta` = `120`
- `c_brown_magenta` = `121`
- `c_yellow_magenta` = `122`
- `c_blue_magenta` = `123`
- `c_light_blue_magenta` = `124`
- `c_magenta_magenta` = `125`
- `c_pink_magenta` = `126`
- `c_cyan_magenta` = `127`
- `c_light_cyan_magenta` = `128`
- `c_black_cyan` = `129`
- `c_dark_gray_cyan` = `130`
- `c_light_gray_cyan` = `131`
- `c_white_cyan` = `132`
- `c_red_cyan` = `133`
- `c_light_red_cyan` = `134`
- `c_green_cyan` = `135`
- `c_light_green_cyan` = `136`
- `c_brown_cyan` = `137`
- `c_yellow_cyan` = `138`
- `c_blue_cyan` = `139`
- `c_light_blue_cyan` = `140`
- `c_magenta_cyan` = `141`
- `c_pink_cyan` = `142`
- `c_cyan_cyan` = `143`
- `c_light_cyan_cyan` = `144`

## MsgType

### Entries

- `good` = `0`
- `bad` = `1`
- `mixed` = `2`
- `warning` = `3`
- `info` = `4`
- `neutral` = `5`
- `debug` = `6`
- `headshot` = `7`
- `critical` = `8`
- `grazing` = `9`

# Libraries

## const

Various game constants

### Members

#### OM_OMT_SIZE

Variable of type `int` value: `180`

#### OM_SM_SIZE

Variable of type `int` value: `360`

#### OM_MS_SIZE

Variable of type `int` value: `4320`

#### OMT_SM_SIZE

Variable of type `int` value: `2`

#### OMT_MS_SIZE

Variable of type `int` value: `24`

#### SM_MS_SIZE

Variable of type `int` value: `12`

## coords

Methods for manipulating coord systems and calculating distance

### Members

#### ms_to_sm

Function `( Tripoint ) -> ( Tripoint, Point )`

#### ms_to_omt

Function `( Tripoint ) -> ( Tripoint, Point )`

#### ms_to_om

Function `( Tripoint ) -> ( Point, Tripoint )`

#### sm_to_ms

Function `( Tripoint, Opt(Point) ) -> Tripoint`

#### omt_to_ms

Function `( Tripoint, Opt(Point) ) -> Tripoint`

#### om_to_ms

Function `( Point, Opt(Tripoint) ) -> Tripoint`

#### rl_dist

Function `( Tripoint, Tripoint ) -> int` Function `( Point, Point ) -> int`

#### trig_dist

Function `( Tripoint, Tripoint ) -> double` Function `( Point, Point ) -> double`

#### square_dist

Function `( Tripoint, Tripoint ) -> int` Function `( Point, Point ) -> int`

## gapi

Global game methods

### Members

#### get_avatar

Function `() -> Avatar`

#### get_map

Function `() -> Map`

#### get_distribution_grid_tracker

Function `() -> DistributionGridTracker`

#### get_character_name

Function `( Character ) -> string`

#### add_msg

Function `( MsgType, ... )` Function `( ... )`

#### place_player_overmap_at

Function `( Tripoint )`

#### current_turn

Function `() -> TimePoint`

#### turn_zero

Function `() -> TimePoint`

#### before_time_starts

Function `() -> TimePoint`

#### rng

Function `( int, int ) -> int`

#### add_on_every_x_hook

Function `( TimeDuration, function )`

## gdebug

Debugging and logging API.

### Members

#### log_info

Function `( ... )`

#### log_warn

Function `( ... )`

#### log_error

Function `( ... )`

#### debugmsg

Function `( ... )`

#### clear_lua_log

Function `()`

#### set_log_capacity

Function `( int )`

#### reload_lua_code

Function `()`

#### save_game

Function `() -> bool`

## hooks_doc

Documentation for hooks

### Members

#### on_game_save

Called when game is about to save Function `()`

#### on_game_load

Called right after game has loaded Function `()`

#### on_every_x

Called every in-game period Function `()`

#### on_mapgen_postprocess

Called right after mapgen has completed. Map argument is the tinymap that represents 24x24 area (2x2
submaps, or 1x1 omt), tripoint is the absolute omt pos, and time_point is the current time (for
time-based effects). Function `( Map, Tripoint, TimePoint )`

## locale

Localization API.

### Members

#### gettext

Expects english source string, returns translated string. Function `( string ) -> string`

#### vgettext

First is english singular string, second is english plural string. Number is amount to translate
for. Function `( string, string, int ) -> string`

#### pgettext

First is context string. Second is english source string. Function `( string, string ) -> string`

#### vpgettext

First is context string. Second is english singular string. third is english plural. Number is
amount to translate for. Function `( string, string, string, int ) -> string`

## tests_lib

Library for testing purposes

### Members

#### my_awesome_lambda_1

Function `() -> int`

#### my_awesome_lambda_2

Function `() -> int`
