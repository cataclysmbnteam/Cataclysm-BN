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

## ActivityTypeId

### Bases

No base classes.

### Constructors

#### `ActivityTypeId.new()`

#### `ActivityTypeId.new( ActivityTypeId )`

#### `ActivityTypeId.new( string )`

### Members

#### obj

Function `( ActivityTypeId ) -> ActivityTypeRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( ActivityTypeId ) -> bool`

#### is_valid

Function `( ActivityTypeId ) -> bool`

#### str

Function `( ActivityTypeId ) -> string`

#### NULL_ID

Function `() -> ActivityTypeId`

#### __tostring

Function `( ActivityTypeId ) -> string`

#### serialize

Function `( ActivityTypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( ActivityTypeId, <cppval: 6JsonIn > )`

## Angle

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### from_radians

Function `( double ) -> Angle`

#### to_radians

Function `( Angle ) -> double`

#### from_degrees

Function `( double ) -> Angle`

#### to_degrees

Function `( Angle ) -> double`

#### from_arcmin

Function `( double ) -> Angle`

#### to_arcmin

Function `( Angle ) -> double`

#### __eq

Function `( Angle, Angle ) -> bool`

#### __lt

Function `( Angle, Angle ) -> bool`

#### __le

Function `( Angle, Angle ) -> bool`

## Avatar

### Bases

- `Player`
- `Character`
- `Creature`

### Constructors

No constructors.

### Members

No members.

## BionicDataId

### Bases

No base classes.

### Constructors

#### `BionicDataId.new()`

#### `BionicDataId.new( BionicDataId )`

#### `BionicDataId.new( string )`

### Members

#### obj

Function `( BionicDataId ) -> BionicDataRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( BionicDataId ) -> bool`

#### is_valid

Function `( BionicDataId ) -> bool`

#### str

Function `( BionicDataId ) -> string`

#### NULL_ID

Function `() -> BionicDataId`

#### __tostring

Function `( BionicDataId ) -> string`

#### serialize

Function `( BionicDataId, <cppval: 7JsonOut > )`

#### deserialize

Function `( BionicDataId, <cppval: 6JsonIn > )`

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

#### name

Variable of type `string`

#### male

Variable of type `bool`

#### focus_pool

Variable of type `int`

#### cash

Variable of type `int`

#### follower_ids

Variable of type `Set( CharacterId )`

#### mutation_category_level

Variable of type `Map( MutationCategoryTraitId, int )`

#### getID

Function `( Character ) -> CharacterId`

#### setID

Function `( Character, CharacterId, bool )`

#### get_str

Function `( Character ) -> int`

#### get_dex

Function `( Character ) -> int`

#### get_per

Function `( Character ) -> int`

#### get_int

Function `( Character ) -> int`

#### get_str_base

Function `( Character ) -> int`

#### get_dex_base

Function `( Character ) -> int`

#### get_per_base

Function `( Character ) -> int`

#### get_int_base

Function `( Character ) -> int`

#### get_str_bonus

Function `( Character ) -> int`

#### get_dex_bonus

Function `( Character ) -> int`

#### get_per_bonus

Function `( Character ) -> int`

#### get_int_bonus

Function `( Character ) -> int`

#### set_str_bonus

Function `( Character, int )`

#### set_dex_bonus

Function `( Character, int )`

#### set_per_bonus

Function `( Character, int )`

#### set_int_bonus

Function `( Character, int )`

#### mod_str_bonus

Function `( Character, int )`

#### mod_dex_bonus

Function `( Character, int )`

#### mod_per_bonus

Function `( Character, int )`

#### mod_int_bonus

Function `( Character, int )`

#### get_healthy

Function `( Character ) -> int`

#### get_healthy_mod

Function `( Character ) -> int`

#### mod_healthy

Function `( Character, int )`

#### mod_healthy_mod

Function `( Character, int, int )`

#### set_healthy

Function `( Character, int )`

#### set_healthy_mod

Function `( Character, int )`

#### get_stored_kcal

Function `( Character ) -> int`

#### max_stored_kcal

Function `( Character ) -> int`

#### get_kcal_percent

Function `( Character ) -> double`

#### get_thirst

Function `( Character ) -> int`

#### get_fatigue

Function `( Character ) -> int`

#### get_sleep_deprivation

Function `( Character ) -> int`

#### mod_stored_kcal

Function `( Character, int )`

#### mod_thirst

Function `( Character, int )`

#### mod_fatigue

Function `( Character, int )`

#### mod_sleep_deprivation

Function `( Character, int )`

#### set_stored_kcal

Function `( Character, int )`

#### set_thirst

Function `( Character, int )`

#### set_fatigue

Function `( Character, int )`

#### set_sleep_deprivation

Function `( Character, int )`

#### get_faction_id

Function `( Character ) -> FactionId`

#### set_faction_id

Function `( Character, FactionId )`

#### sight_impaired

Function `( Character ) -> bool`

#### has_alarm_clock

Function `( Character ) -> bool`

#### has_watch

Function `( Character ) -> bool`

#### blood_loss

Function `( Character, BodyPartTypeIntId ) -> int`

#### get_part_encumbrance

Function `( Character, BodyPart ) -> int`

#### is_wearing_power_armor

Function `( Character, bool ) -> bool`

#### is_wearing_active_power_armor

Function `( Character ) -> bool`

#### is_wearing_active_optcloak

Function `( Character ) -> bool`

#### in_climate_control

Function `( Character ) -> bool`

#### is_blind

Function `( Character ) -> bool`

#### is_invisible

Function `( Character ) -> bool`

#### get_movement_mode

Function `( Character ) -> CharacterMoveMode`

#### set_movement_mode

Function `( Character, CharacterMoveMode )`

#### expose_to_disease

Function `( Character, DiseaseTypeId )`

#### is_quiet

Function `( Character ) -> bool`

#### is_stealthy

Function `( Character ) -> bool`

#### cough

Function `( Character, bool, int )`

#### bionic_armor_bonus

Function `( Character, BodyPartTypeIntId, DamageType ) -> double`

#### mabuff_armor_bonus

Function `( Character, DamageType ) -> int`

#### has_base_trait

Function `( Character, MutationBranchId ) -> bool`

#### has_trait_flag

Function `( Character, JsonTraitFlagId ) -> bool`

#### has_opposite_trait

Function `( Character, MutationBranchId ) -> bool`

#### set_mutation

Function `( Character, MutationBranchId )`

#### unset_mutation

Function `( Character, MutationBranchId )`

#### can_mount

Function `( Character, Monster ) -> bool`

#### mount_creature

Function `( Character, Monster )`

#### is_mounted

Function `( Character ) -> bool`

#### check_mount_will_move

Function `( Character, Tripoint ) -> bool`

#### check_mount_is_spooked

Function `( Character ) -> bool`

#### dismount

Function `( Character )`

#### forced_dismount

Function `( Character )`

#### is_deaf

Function `( Character ) -> bool`

#### has_two_arms

Function `( Character ) -> bool`

#### get_working_arm_count

Function `( Character ) -> int`

#### get_working_leg_count

Function `( Character ) -> int`

#### is_limb_disabled

Function `( Character, BodyPartTypeIntId ) -> bool`

#### is_limb_broken

Function `( Character, BodyPartTypeIntId ) -> bool`

#### can_run

Function `( Character ) -> bool`

#### hurtall

Function `( Character, int, Creature, bool )`

#### hitall

Function `( Character, int, int, Creature ) -> int`

#### heal

Function `( Character, BodyPartTypeIntId, int )`

#### healall

Function `( Character, int )`

#### global_square_location

Function `( Character ) -> Tripoint`

#### global_sm_location

Function `( Character ) -> Tripoint`

#### has_mabuff

Function `( Character, MartialArtsBuffId ) -> bool`

#### mabuff_tohit_bonus

Function `( Character ) -> double`

#### mabuff_dodge_bonus

Function `( Character ) -> double`

#### mabuff_block_bonus

Function `( Character ) -> int`

#### mabuff_speed_bonus

Function `( Character ) -> int`

#### mabuff_arpen_bonus

Function `( Character, DamageType ) -> int`

#### mabuff_damage_mult

Function `( Character, DamageType ) -> double`

#### mabuff_damage_bonus

Function `( Character, DamageType ) -> int`

#### mabuff_attack_cost_penalty

Function `( Character ) -> int`

#### mabuff_attack_cost_mult

Function `( Character ) -> double`

#### mutation_effect

Function `( Character, MutationBranchId )`

#### mutation_loss_effect

Function `( Character, MutationBranchId )`

#### has_active_mutation

Function `( Character, MutationBranchId ) -> bool`

#### mutate

Function `( Character )`

#### mutation_ok

Function `( Character, MutationBranchId, bool, bool ) -> bool`

#### mutate_category

Function `( Character, MutationCategoryTraitId )`

#### mutate_towards

Function `( Character, Vector( MutationBranchId ), int ) -> bool`

#### mutate_towards

Function `( Character, MutationBranchId ) -> bool`

#### remove_mutation

Function `( Character, MutationBranchId, bool )`

#### has_child_flag

Function `( Character, MutationBranchId ) -> bool`

#### remove_child_flag

Function `( Character, MutationBranchId )`

#### get_highest_category

Function `( Character ) -> MutationCategoryTraitId`

#### is_weak_to_water

Function `( Character ) -> bool`

#### mutation_armor

Function `( Character, BodyPartTypeIntId, DamageType ) -> double`

#### get_bionics

Function `( Character ) -> Vector( BionicDataId )`

#### has_bionic

Function `( Character, BionicDataId ) -> bool`

#### has_active_bionic

Function `( Character, BionicDataId ) -> bool`

#### has_any_bionic

Function `( Character ) -> bool`

#### has_bionics

Function `( Character ) -> bool`

#### clear_bionics

Function `( Character )`

#### get_used_bionics_slots

Function `( Character, BodyPartTypeIntId ) -> int`

#### get_total_bionics_slots

Function `( Character, BodyPartTypeIntId ) -> int`

#### get_free_bionics_slots

Function `( Character, BodyPartTypeIntId ) -> int`

#### remove_bionic

Function `( Character, BionicDataId )`

#### add_bionic

Function `( Character, BionicDataId )`

#### get_power_level

Function `( Character ) -> Energy`

#### get_max_power_level

Function `( Character ) -> Energy`

#### mod_power_level

Function `( Character, Energy )`

#### mod_max_power_level

Function `( Character, Energy )`

#### set_power_level

Function `( Character, Energy )`

#### set_max_power_level

Function `( Character, Energy )`

#### is_max_power

Function `( Character ) -> bool`

#### has_power

Function `( Character ) -> bool`

#### has_max_power

Function `( Character ) -> bool`

#### is_worn

Function `( Character, Item ) -> bool`

#### weight_carried

Function `( Character ) -> Mass`

#### volume_carried

Function `( Character ) -> Volume`

#### volume_capacity

Function `( Character ) -> Volume`

#### can_pick_volume

Function `( Character, Volume ) -> bool`

#### can_pick_weight

Function `( Character, Mass, bool ) -> bool`

#### is_armed

Function `( Character ) -> bool`

#### can_wield

Function `( Character, Item ) -> bool`

#### wield

Function `( Character, Item ) -> bool`

#### can_unwield

Function `( Character, Item ) -> bool`

#### unwield

Function `( Character ) -> bool`

#### is_wielding

Function `( Character, Item ) -> bool`

#### is_wearing

Function `( Character, Item ) -> bool`

#### is_wearing_on_bp

Function `( Character, ItypeId, BodyPartTypeIntId ) -> bool`

#### worn_with_flag

Function `( Character, JsonFlagId, BodyPartTypeIntId ) -> bool`

#### item_worn_with_flag

Function `( Character, JsonFlagId, BodyPartTypeIntId ) -> Item`

#### get_skill_level

Function `( Character, SkillId ) -> int`

#### get_all_skills

Function `( Character ) -> SkillLevelMap`

#### get_skill_level_object

Function `( Character, SkillId ) -> SkillLevel`

#### set_skill_level

Function `( Character, SkillId, int )`

#### mod_skill_level

Function `( Character, SkillId, int )`

#### rust_rate

Function `( Character ) -> int`

#### practice

Function `( Character, SkillId, int, int, bool )`

#### read_speed

Function `( Character, bool ) -> int`

#### get_time_died

Function `( Character ) -> TimePoint`

#### is_rad_immune

Function `( Character ) -> bool`

#### is_throw_immune

Function `( Character ) -> bool`

#### rest_quality

Function `( Character ) -> double`

#### healing_rate

Function `( Character, double ) -> double`

#### healing_rate_medicine

Function `( Character, double, BodyPartTypeIntId ) -> double`

#### mutation_value

Function `( Character, string ) -> double`

#### get_base_traits

Function `( Character ) -> Vector( MutationBranchId )`

#### get_mutations

Function `( Character, bool ) -> Vector( MutationBranchId )`

#### clear_skills

Function `( Character )`

#### clear_mutations

Function `( Character )`

#### crossed_threshold

Function `( Character ) -> bool`

#### add_addiction

Function `( Character, AddictionType, int )`

#### rem_addiction

Function `( Character, AddictionType )`

#### has_addiction

Function `( Character, AddictionType ) -> bool`

#### addiction_level

Function `( Character, AddictionType ) -> int`

#### is_hauling

Function `( Character ) -> bool`

#### has_item_with_flag

Function `( Character, JsonFlagId, bool ) -> bool`

#### all_items_with_flag

Function `( Character, JsonFlagId ) -> Vector( Item )`

#### assign_activity

Function `( Character, ActivityTypeId, int, int, int, string )`

#### has_activity

Function `( Character, ActivityTypeId ) -> bool`

#### cancel_activity

Function `( Character )`

#### metabolic_rate

Function `( Character ) -> double`

#### base_age

Function `( Character ) -> int`

#### set_base_age

Function `( Character, int )`

#### mod_base_age

Function `( Character, int )`

#### age

Function `( Character ) -> int`

#### base_height

Function `( Character ) -> int`

#### set_base_height

Function `( Character, int )`

#### mod_base_height

Function `( Character, int )`

#### height

Function `( Character ) -> int`

#### bodyweight

Function `( Character ) -> Mass`

#### bionics_weight

Function `( Character ) -> Mass`

#### get_armor_acid

Function `( Character, BodyPartTypeIntId ) -> int`

#### get_stim

Function `( Character ) -> int`

#### set_stim

Function `( Character, int )`

#### mod_stim

Function `( Character, int )`

#### get_rad

Function `( Character ) -> int`

#### set_rad

Function `( Character, int )`

#### mod_rad

Function `( Character, int )`

#### get_stamina

Function `( Character ) -> int`

#### get_stamina_max

Function `( Character ) -> int`

#### set_stamina

Function `( Character, int )`

#### mod_stamina

Function `( Character, int )`

#### wake_up

Function `( Character )`

#### get_shout_volume

Function `( Character ) -> int`

#### shout

Function `( Character, string, bool )`

#### vomit

Function `( Character )`

#### restore_scent

Function `( Character )`

#### mod_painkiller

Function `( Character, int )`

#### set_painkiller

Function `( Character, int )`

#### get_painkiller

Function `( Character ) -> int`

#### spores

Function `( Character )`

#### blossoms

Function `( Character )`

#### rooted

Function `( Character )`

#### fall_asleep

Function `( Character )`

#### fall_asleep

Function `( Character, TimeDuration )`

#### get_hostile_creatures

Function `( Character, int ) -> Vector( Creature )`

#### get_visible_creatures

Function `( Character, int ) -> Vector( Creature )`

#### wearing_something_on

Function `( Character, BodyPartTypeIntId ) -> bool`

#### is_wearing_helmet

Function `( Character ) -> bool`

#### get_morale_level

Function `( Character ) -> int`

#### add_morale

Function `( Character, MoraleTypeDataId, int, int, TimeDuration, TimeDuration, bool, ItypeRaw )`

#### has_morale

Function `( Character, MoraleTypeDataId ) -> bool`

#### get_morale

Function `( Character, MoraleTypeDataId ) -> int`

#### rem_morale

Function `( Character, MoraleTypeDataId )`

#### clear_morale

Function `( Character )`

#### has_morale_to_read

Function `( Character ) -> bool`

#### has_morale_to_craft

Function `( Character ) -> bool`

#### knows_recipe

Function `( Character, RecipeId ) -> bool`

#### learn_recipe

Function `( Character, RecipeId )`

#### suffer

Function `( Character )`

#### irradiate

Function `( Character, double, bool ) -> bool`

#### can_hear

Function `( Character, Tripoint, int ) -> bool`

#### hearing_ability

Function `( Character ) -> double`

#### get_lowest_hp

Function `( Character ) -> int`

#### bodypart_exposure

Function `( Character ) -> Map( BodyPartTypeIntId, double )`

## CharacterId

### Bases

No base classes.

### Constructors

#### `CharacterId.new()`

#### `CharacterId.new( int )`

### Members

#### is_valid

Function `( CharacterId ) -> bool`

#### get_value

Function `( CharacterId ) -> int`

## Creature

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### get_name

Function `( Creature ) -> string`

#### disp_name

Function `( Creature, bool, bool ) -> string`

#### skin_name

Function `( Creature ) -> string`

#### get_grammatical_genders

Function `( Creature ) -> Vector( string )`

#### is_avatar

Function `( Creature ) -> bool`

#### is_npc

Function `( Creature ) -> bool`

#### is_monster

Function `( Creature ) -> bool`

#### as_monster

Function `( Creature ) -> Monster`

#### as_npc

Function `( Creature ) -> Npc`

#### as_character

Function `( Creature ) -> Character`

#### as_avatar

Function `( Creature ) -> Avatar`

#### hit_roll

Function `( Creature ) -> double`

#### dodge_roll

Function `( Creature ) -> double`

#### stability_roll

Function `( Creature ) -> double`

#### attitude_to

Function `( Creature, Creature ) -> Attitude`

#### sees

Function `( Creature, Creature ) -> bool`

#### sight_range

Function `( Creature, int ) -> int`

#### power_rating

Function `( Creature ) -> double`

#### speed_rating

Function `( Creature ) -> double`

#### ranged_target_size

Function `( Creature ) -> double`

#### knock_back_to

Function `( Creature, Tripoint )`

#### deal_damage

Function `( Creature, Creature, BodyPartTypeIntId, DamageInstance ) -> DealtDamageInstance`

#### apply_damage

Function `( Creature, Creature, BodyPartTypeIntId, int, bool )`

#### size_melee_penalty

Function `( Creature ) -> int`

#### digging

Function `( Creature ) -> bool`

#### is_on_ground

Function `( Creature ) -> bool`

#### is_underwater

Function `( Creature ) -> bool`

#### set_underwater

Function `( Creature, bool )`

#### is_warm

Function `( Creature ) -> bool`

#### in_species

Function `( Creature, SpeciesTypeId ) -> bool`

#### has_weapon

Function `( Creature ) -> bool`

#### is_hallucination

Function `( Creature ) -> bool`

#### is_dead

Function `( Creature ) -> bool`

#### is_elec_immune

Function `( Creature ) -> bool`

#### is_immune_effect

Function `( Creature, EffectTypeId ) -> bool`

#### is_immune_damage

Function `( Creature, DamageType ) -> bool`

#### get_pos_ms

Function `( Creature ) -> Tripoint`

#### set_pos_ms

Function `( Creature, Tripoint )`

#### has_effect

Function `( Creature, EffectTypeId, Opt( BodyPartTypeId ) ) -> bool`

#### has_effect_with_flag

Function `( Creature, JsonFlagId, Opt( BodyPartTypeId ) ) -> bool`

#### get_effect_dur

Function `( Creature, EffectTypeId, Opt( BodyPartTypeId ) ) -> TimeDuration`

#### get_effect_int

Function `( Creature, EffectTypeId, Opt( BodyPartTypeId ) ) -> int`

#### add_effect

Effect type, duration, bodypart and intensity Function
`( Creature, EffectTypeId, TimeDuration, Opt( BodyPartTypeId ), Opt( int ) )`

#### remove_effect

Function `( Creature, EffectTypeId, Opt( BodyPartTypeId ) ) -> bool`

#### clear_effects

Function `( Creature )`

#### set_value

Function `( Creature, string, string )`

#### remove_value

Function `( Creature, string )`

#### get_value

Function `( Creature, string ) -> string`

#### get_weight

Function `( Creature ) -> Mass`

#### has_trait

Function `( Creature, MutationBranchId ) -> bool`

#### mod_pain

Function `( Creature, int )`

#### mod_pain_noresist

Function `( Creature, int )`

#### set_pain

Function `( Creature, int )`

#### get_pain

Function `( Creature ) -> int`

#### get_perceived_pain

Function `( Creature ) -> int`

#### get_moves

Function `( Creature ) -> int`

#### mod_moves

Function `( Creature, int )`

#### set_moves

Function `( Creature, int )`

#### get_num_blocks

Function `( Creature ) -> int`

#### get_num_dodges

Function `( Creature ) -> int`

#### get_env_resist

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_bash

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_cut

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_bullet

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_bash_base

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_cut_base

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_bullet_base

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_armor_bash_bonus

Function `( Creature ) -> int`

#### get_armor_cut_bonus

Function `( Creature ) -> int`

#### get_armor_bullet_bonus

Function `( Creature ) -> int`

#### get_armor_type

Function `( Creature, DamageType, BodyPartTypeIntId ) -> int`

#### get_dodge

Function `( Creature ) -> double`

#### get_melee

Function `( Creature ) -> double`

#### get_hit

Function `( Creature ) -> double`

#### get_speed

Function `( Creature ) -> int`

#### get_size

Function `( Creature ) -> MonsterSize`

#### get_hp

Function `( Creature, Opt( BodyPartTypeIntId ) ) -> int`

#### get_hp_max

Function `( Creature, Opt( BodyPartTypeIntId ) ) -> int`

#### hp_percentage

Function `( Creature ) -> int`

#### has_flag

Function `( Creature, MonsterFlag ) -> bool`

#### get_part_hp_cur

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_part_hp_max

Function `( Creature, BodyPartTypeIntId ) -> int`

#### get_part_healed_total

Function `( Creature, BodyPartTypeIntId ) -> int`

#### set_part_hp_cur

Function `( Creature, BodyPartTypeIntId, int )`

#### set_part_hp_max

Function `( Creature, BodyPartTypeIntId, int )`

#### mod_part_hp_cur

Function `( Creature, BodyPartTypeIntId, int )`

#### mod_part_hp_max

Function `( Creature, BodyPartTypeIntId, int )`

#### set_all_parts_hp_cur

Function `( Creature, int )`

#### set_all_parts_hp_to_max

Function `( Creature )`

#### get_speed_base

Function `( Creature ) -> int`

#### get_speed_bonus

Function `( Creature ) -> int`

#### get_speed_mult

Function `( Creature ) -> double`

#### get_block_bonus

Function `( Creature ) -> int`

#### get_dodge_base

Function `( Creature ) -> double`

#### get_hit_base

Function `( Creature ) -> double`

#### get_dodge_bonus

Function `( Creature ) -> double`

#### get_hit_bonus

Function `( Creature ) -> double`

#### has_grab_break_tec

Function `( Creature ) -> bool`

#### get_weight_capacity

Function `( Creature ) -> int`

## DamageInstance

new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)

### Bases

No base classes.

### Constructors

#### `DamageInstance.new()`

#### `DamageInstance.new( DamageType, double, double, double, double )`

### Members

#### damage_units

Variable of type `Vector( DamageUnit )`

#### mult_damage

Function `( DamageInstance, double, bool )`

#### type_damage

Function `( DamageInstance, DamageType ) -> double`

#### total_damage

Function `( DamageInstance ) -> double`

#### clear

Function `( DamageInstance )`

#### empty

Function `( DamageInstance ) -> bool`

#### add_damage

Function `( DamageInstance, DamageType, double, double, double, double )`

#### add

Function `( DamageInstance, DamageUnit )`

#### __eq

Function `( DamageInstance, DamageInstance ) -> bool`

## DamageUnit

new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)

### Bases

No base classes.

### Constructors

#### `DamageUnit.new( DamageType, double, double, double, double )`

### Members

#### type

Variable of type `DamageType`

#### amount

Variable of type `double`

#### res_pen

Variable of type `double`

#### res_mult

Variable of type `double`

#### damage_multiplier

Variable of type `double`

#### __eq

Function `( DamageUnit, DamageUnit ) -> bool`

## DealtDamageInstance

Represents the final dealt damage

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### dealt_dams

Variable of type `Array( int, 11 )`

#### bp_hit

Variable of type `BodyPart`

#### type_damage

Function `( DealtDamageInstance, DamageType ) -> int`

#### total_damage

Function `( DealtDamageInstance ) -> int`

## DiseaseTypeId

### Bases

No base classes.

### Constructors

#### `DiseaseTypeId.new()`

#### `DiseaseTypeId.new( DiseaseTypeId )`

#### `DiseaseTypeId.new( string )`

### Members

#### obj

Function `( DiseaseTypeId ) -> DiseaseTypeRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( DiseaseTypeId ) -> bool`

#### is_valid

Function `( DiseaseTypeId ) -> bool`

#### str

Function `( DiseaseTypeId ) -> string`

#### NULL_ID

Function `() -> DiseaseTypeId`

#### __tostring

Function `( DiseaseTypeId ) -> string`

#### serialize

Function `( DiseaseTypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( DiseaseTypeId, <cppval: 6JsonIn > )`

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

## Energy

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### from_joule

Function `( int ) -> Energy`

#### to_joule

Function `( Energy ) -> int`

#### from_kilojoule

Function `( int ) -> Energy`

#### to_kilojoule

Function `( Energy ) -> int`

#### __eq

Function `( Energy, Energy ) -> bool`

#### __lt

Function `( Energy, Energy ) -> bool`

#### __le

Function `( Energy, Energy ) -> bool`

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

## JsonFlagId

### Bases

No base classes.

### Constructors

#### `JsonFlagId.new()`

#### `JsonFlagId.new( JsonFlagId )`

#### `JsonFlagId.new( string )`

### Members

#### obj

Function `( JsonFlagId ) -> JsonFlagRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( JsonFlagId ) -> bool`

#### is_valid

Function `( JsonFlagId ) -> bool`

#### str

Function `( JsonFlagId ) -> string`

#### NULL_ID

Function `() -> JsonFlagId`

#### __tostring

Function `( JsonFlagId ) -> string`

#### serialize

Function `( JsonFlagId, <cppval: 7JsonOut > )`

#### deserialize

Function `( JsonFlagId, <cppval: 6JsonIn > )`

## JsonTraitFlagId

### Bases

No base classes.

### Constructors

#### `JsonTraitFlagId.new()`

#### `JsonTraitFlagId.new( JsonTraitFlagId )`

#### `JsonTraitFlagId.new( string )`

### Members

#### obj

Function `( JsonTraitFlagId ) -> JsonTraitFlagRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( JsonTraitFlagId ) -> bool`

#### is_valid

Function `( JsonTraitFlagId ) -> bool`

#### str

Function `( JsonTraitFlagId ) -> string`

#### NULL_ID

Function `() -> JsonTraitFlagId`

#### __tostring

Function `( JsonTraitFlagId ) -> string`

#### serialize

Function `( JsonTraitFlagId, <cppval: 7JsonOut > )`

#### deserialize

Function `( JsonTraitFlagId, <cppval: 6JsonIn > )`

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

## MartialArtsBuffId

### Bases

No base classes.

### Constructors

#### `MartialArtsBuffId.new()`

#### `MartialArtsBuffId.new( MartialArtsBuffId )`

#### `MartialArtsBuffId.new( string )`

### Members

#### obj

Function `( MartialArtsBuffId ) -> MartialArtsBuffRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( MartialArtsBuffId ) -> bool`

#### is_valid

Function `( MartialArtsBuffId ) -> bool`

#### str

Function `( MartialArtsBuffId ) -> string`

#### NULL_ID

Function `() -> MartialArtsBuffId`

#### __tostring

Function `( MartialArtsBuffId ) -> string`

#### serialize

Function `( MartialArtsBuffId, <cppval: 7JsonOut > )`

#### deserialize

Function `( MartialArtsBuffId, <cppval: 6JsonIn > )`

## Mass

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### from_milligram

Function `( int ) -> Mass`

#### to_milligram

Function `( Mass ) -> int`

#### from_gram

Function `( int ) -> Mass`

#### to_gram

Function `( Mass ) -> int`

#### from_kilogram

Function `( int ) -> Mass`

#### to_kilogram

Function `( Mass ) -> int`

#### from_newton

Function `( int ) -> Mass`

#### to_newton

Function `( Mass ) -> int`

#### __eq

Function `( Mass, Mass ) -> bool`

#### __lt

Function `( Mass, Mass ) -> bool`

#### __le

Function `( Mass, Mass ) -> bool`

## Monster

### Bases

- `Creature`

### Constructors

No constructors.

### Members

#### friendly

Variable of type `int`

#### anger

Variable of type `int`

#### morale

Variable of type `int`

#### faction

Variable of type `MonsterFactionIntId`

#### death_drops

Variable of type `bool`

#### unique_name

Variable of type `string`

#### can_upgrade

Function `( Monster ) -> bool`

#### hasten_upgrade

Function `( Monster )`

#### get_upgrade_time

Function `( Monster ) -> int`

#### try_upgrade

Function `( Monster, bool )`

#### try_reproduce

Function `( Monster )`

#### refill_udders

Function `( Monster )`

#### spawn

Function `( Monster, Tripoint )`

#### name

Function `( Monster, int ) -> string`

#### name_with_armor

Function `( Monster ) -> string`

#### can_see

Function `( Monster ) -> bool`

#### can_hear

Function `( Monster ) -> bool`

#### can_submerge

Function `( Monster ) -> bool`

#### can_drown

Function `( Monster ) -> bool`

#### can_climb

Function `( Monster ) -> bool`

#### can_dig

Function `( Monster ) -> bool`

#### digs

Function `( Monster ) -> bool`

#### flies

Function `( Monster ) -> bool`

#### climbs

Function `( Monster ) -> bool`

#### swims

Function `( Monster ) -> bool`

#### move_target

Function `( Monster ) -> Tripoint`

#### is_wandering

Function `( Monster ) -> bool`

#### wander_to

Function `( Monster, Tripoint, int )`

#### move_to

Function `( Monster, Tripoint, bool, bool, double ) -> bool`

#### attitude

Function `( Monster, Character ) -> MonsterAttitude`

#### heal

Function `( Monster, int, bool ) -> int`

#### set_hp

Function `( Monster, int )`

#### make_fungus

Function `( Monster ) -> bool`

#### make_friendly

Function `( Monster )`

#### make_ally

Function `( Monster, Monster )`

## MonsterFactionId

### Bases

No base classes.

### Constructors

#### `MonsterFactionId.new()`

#### `MonsterFactionId.new( MonsterFactionId )`

#### `MonsterFactionId.new( MonsterFactionIntId )`

#### `MonsterFactionId.new( string )`

### Members

#### obj

Function `( MonsterFactionId ) -> MonsterFactionRaw`

#### int_id

Function `( MonsterFactionId ) -> MonsterFactionIntId`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( MonsterFactionId ) -> bool`

#### is_valid

Function `( MonsterFactionId ) -> bool`

#### str

Function `( MonsterFactionId ) -> string`

#### NULL_ID

Function `() -> MonsterFactionId`

#### __tostring

Function `( MonsterFactionId ) -> string`

#### serialize

Function `( MonsterFactionId, <cppval: 7JsonOut > )`

#### deserialize

Function `( MonsterFactionId, <cppval: 6JsonIn > )`

## MonsterFactionIntId

### Bases

No base classes.

### Constructors

#### `MonsterFactionIntId.new()`

#### `MonsterFactionIntId.new( MonsterFactionIntId )`

#### `MonsterFactionIntId.new( MonsterFactionId )`

### Members

#### obj

Function `( MonsterFactionIntId ) -> MonsterFactionRaw`

#### str_id

Function `( MonsterFactionIntId ) -> MonsterFactionId`

#### is_valid

Function `( MonsterFactionIntId ) -> bool`

#### __tostring

Function `( MonsterFactionIntId ) -> string`

## MoraleTypeDataId

### Bases

No base classes.

### Constructors

#### `MoraleTypeDataId.new()`

#### `MoraleTypeDataId.new( MoraleTypeDataId )`

#### `MoraleTypeDataId.new( string )`

### Members

#### obj

Function `( MoraleTypeDataId ) -> MoraleTypeDataRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( MoraleTypeDataId ) -> bool`

#### is_valid

Function `( MoraleTypeDataId ) -> bool`

#### str

Function `( MoraleTypeDataId ) -> string`

#### NULL_ID

Function `() -> MoraleTypeDataId`

#### __tostring

Function `( MoraleTypeDataId ) -> string`

#### serialize

Function `( MoraleTypeDataId, <cppval: 7JsonOut > )`

#### deserialize

Function `( MoraleTypeDataId, <cppval: 6JsonIn > )`

## MutationBranchId

### Bases

No base classes.

### Constructors

#### `MutationBranchId.new()`

#### `MutationBranchId.new( MutationBranchId )`

#### `MutationBranchId.new( string )`

### Members

#### obj

Function `( MutationBranchId ) -> MutationBranchRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( MutationBranchId ) -> bool`

#### is_valid

Function `( MutationBranchId ) -> bool`

#### str

Function `( MutationBranchId ) -> string`

#### NULL_ID

Function `() -> MutationBranchId`

#### __tostring

Function `( MutationBranchId ) -> string`

#### serialize

Function `( MutationBranchId, <cppval: 7JsonOut > )`

#### deserialize

Function `( MutationBranchId, <cppval: 6JsonIn > )`

## MutationCategoryTraitId

### Bases

No base classes.

### Constructors

#### `MutationCategoryTraitId.new()`

#### `MutationCategoryTraitId.new( MutationCategoryTraitId )`

#### `MutationCategoryTraitId.new( string )`

### Members

#### obj

Function `( MutationCategoryTraitId ) -> MutationCategoryTraitRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( MutationCategoryTraitId ) -> bool`

#### is_valid

Function `( MutationCategoryTraitId ) -> bool`

#### str

Function `( MutationCategoryTraitId ) -> string`

#### NULL_ID

Function `() -> MutationCategoryTraitId`

#### __tostring

Function `( MutationCategoryTraitId ) -> string`

#### serialize

Function `( MutationCategoryTraitId, <cppval: 7JsonOut > )`

#### deserialize

Function `( MutationCategoryTraitId, <cppval: 6JsonIn > )`

## Npc

### Bases

- `Player`
- `Character`
- `Creature`

### Constructors

No constructors.

### Members

#### current_activity_id

Variable of type `ActivityTypeId`

#### personality

Variable of type `NpcPersonality`

#### op_of_u

Variable of type `NpcOpinion`

#### patience

Variable of type `int`

#### marked_for_death

Variable of type `bool`

#### hit_by_player

Variable of type `bool`

#### needs

Variable of type `Vector( NpcNeed )`

#### set_faction_id

Function `( Npc, FactionId )`

#### turned_hostile

Function `( Npc ) -> bool`

#### hostile_anger_level

Function `( Npc ) -> int`

#### make_angry

Function `( Npc )`

#### is_enemy

Function `( Npc ) -> bool`

#### is_following

Function `( Npc ) -> bool`

#### is_obeying

Function `( Npc, Character ) -> bool`

#### is_friendly

Function `( Npc, Character ) -> bool`

#### is_leader

Function `( Npc ) -> bool`

#### is_walking_with

Function `( Npc ) -> bool`

#### is_ally

Function `( Npc, Character ) -> bool`

#### is_player_ally

Function `( Npc ) -> bool`

#### is_stationary

Function `( Npc, bool ) -> bool`

#### is_guarding

Function `( Npc ) -> bool`

#### is_patrolling

Function `( Npc ) -> bool`

#### within_boundaries_of_camp

Function `( Npc ) -> bool`

#### has_player_activity

Function `( Npc ) -> bool`

#### is_travelling

Function `( Npc ) -> bool`

#### is_minion

Function `( Npc ) -> bool`

#### guaranteed_hostile

Function `( Npc ) -> bool`

#### mutiny

Function `( Npc )`

#### get_monster_faction

Function `( Npc ) -> MonsterFactionIntId`

#### follow_distance

Function `( Npc ) -> int`

#### current_target

Function `( Npc ) -> Creature`

#### current_ally

Function `( Npc ) -> Creature`

#### danger_assessment

Function `( Npc ) -> double`

#### say

Function `( Npc, string )`

#### smash_ability

Function `( Npc ) -> int`

#### complain_about

Function `( Npc, string, TimeDuration, string, Opt( bool ) ) -> bool`

#### warn_about

Function `( Npc, string, TimeDuration, string, int, Tripoint )`

#### complain

Function `( Npc ) -> bool`

#### evaluate_enemy

Function `( Npc, Creature ) -> double`

#### can_open_door

Function `( Npc, Tripoint, bool ) -> bool`

#### can_move_to

Function `( Npc, Tripoint, bool ) -> bool`

#### saw_player_recently

Function `( Npc ) -> bool`

#### has_omt_destination

Function `( Npc ) -> bool`

#### get_attitude

Function `( Npc ) -> NpcAttitude`

#### set_attitude

Function `( Npc, NpcAttitude )`

#### has_activity

Function `( Npc ) -> bool`

#### has_job

Function `( Npc ) -> bool`

## NpcOpinion

### Bases

No base classes.

### Constructors

#### `NpcOpinion.new()`

#### `NpcOpinion.new( int, int, int, int, int )`

### Members

#### trust

Variable of type `int`

#### fear

Variable of type `int`

#### value

Variable of type `int`

#### anger

Variable of type `int`

#### owed

Variable of type `int`

## NpcPersonality

### Bases

No base classes.

### Constructors

#### `NpcPersonality.new()`

### Members

#### aggression

Variable of type `char`

#### bravery

Variable of type `char`

#### collector

Variable of type `char`

#### altruism

Variable of type `char`

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

## RecipeId

### Bases

No base classes.

### Constructors

#### `RecipeId.new()`

#### `RecipeId.new( RecipeId )`

#### `RecipeId.new( string )`

### Members

#### obj

Function `( RecipeId ) -> RecipeRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( RecipeId ) -> bool`

#### is_valid

Function `( RecipeId ) -> bool`

#### str

Function `( RecipeId ) -> string`

#### NULL_ID

Function `() -> RecipeId`

#### __tostring

Function `( RecipeId ) -> string`

#### serialize

Function `( RecipeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( RecipeId, <cppval: 6JsonIn > )`

## SkillId

### Bases

No base classes.

### Constructors

#### `SkillId.new()`

#### `SkillId.new( SkillId )`

#### `SkillId.new( string )`

### Members

#### obj

Function `( SkillId ) -> SkillRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( SkillId ) -> bool`

#### is_valid

Function `( SkillId ) -> bool`

#### str

Function `( SkillId ) -> string`

#### NULL_ID

Function `() -> SkillId`

#### __tostring

Function `( SkillId ) -> string`

#### serialize

Function `( SkillId, <cppval: 7JsonOut > )`

#### deserialize

Function `( SkillId, <cppval: 6JsonIn > )`

## SkillLevel

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### is_training

Function `( SkillLevel ) -> bool`

#### level

Function `( SkillLevel ) -> int`

#### highest_level

Function `( SkillLevel ) -> int`

#### train

Function `( SkillLevel, int, bool )`

#### can_train

Function `( SkillLevel ) -> bool`

## SkillLevelMap

### Bases

- `Map( SkillId, SkillLevel )`

### Constructors

No constructors.

### Members

#### mod_skill_level

Function `( SkillLevelMap, SkillId, int )`

#### get_skill_level

Function `( SkillLevelMap, SkillId ) -> int`

#### get_skill_level_object

Function `( SkillLevelMap, SkillId ) -> SkillLevel`

## SpeciesTypeId

### Bases

No base classes.

### Constructors

#### `SpeciesTypeId.new()`

#### `SpeciesTypeId.new( SpeciesTypeId )`

#### `SpeciesTypeId.new( string )`

### Members

#### obj

Function `( SpeciesTypeId ) -> SpeciesTypeRaw`

#### implements_int_id

Function `() -> bool`

#### is_null

Function `( SpeciesTypeId ) -> bool`

#### is_valid

Function `( SpeciesTypeId ) -> bool`

#### str

Function `( SpeciesTypeId ) -> string`

#### NULL_ID

Function `() -> SpeciesTypeId`

#### __tostring

Function `( SpeciesTypeId ) -> string`

#### serialize

Function `( SpeciesTypeId, <cppval: 7JsonOut > )`

#### deserialize

Function `( SpeciesTypeId, <cppval: 6JsonIn > )`

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

## Volume

### Bases

No base classes.

### Constructors

No constructors.

### Members

#### from_milliliter

Function `( int ) -> Volume`

#### from_liter

Function `( int ) -> Volume`

#### to_milliliter

Function `( Volume ) -> int`

#### to_liter

Function `( Volume ) -> double`

#### __eq

Function `( Volume, Volume ) -> bool`

#### __lt

Function `( Volume, Volume ) -> bool`

#### __le

Function `( Volume, Volume ) -> bool`

# Enums

## AddictionType

### Entries

- `NONE` = `0`
- `CAFFEINE` = `1`
- `ALCOHOL` = `2`
- `SLEEP` = `3`
- `PKILLER` = `4`
- `SPEED` = `5`
- `CIG` = `6`
- `COKE` = `7`
- `CRACK` = `8`
- `MUTAGEN` = `9`
- `DIAZEPAM` = `10`
- `MARLOSS_R` = `11`
- `MARLOSS_B` = `12`
- `MARLOSS_Y` = `13`

## Attitude

### Entries

- `Hostile` = `0`
- `Neutral` = `1`
- `Friendly` = `2`
- `Any` = `3`

## BodyPart

### Entries

- `TORSO` = `0`
- `HEAD` = `1`
- `EYES` = `2`
- `MOUTH` = `3`
- `ARM_L` = `4`
- `ARM_R` = `5`
- `HAND_L` = `6`
- `HAND_R` = `7`
- `LEG_L` = `8`
- `LEG_R` = `9`
- `FOOT_L` = `10`
- `FOOT_R` = `11`

## CharacterMoveMode

### Entries

- `walk` = `0`
- `run` = `1`
- `crouch` = `2`

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

## DamageType

### Entries

- `DT_NULL` = `0`
- `DT_TRUE` = `1`
- `DT_BIOLOGICAL` = `2`
- `DT_BASH` = `3`
- `DT_CUT` = `4`
- `DT_ACID` = `5`
- `DT_STAB` = `6`
- `DT_HEAT` = `7`
- `DT_COLD` = `8`
- `DT_ELECTRIC` = `9`
- `DT_BULLET` = `10`

## MonsterAttitude

### Entries

- `MATT_NULL` = `0`
- `MATT_FRIEND` = `1`
- `MATT_FPASSIVE` = `2`
- `MATT_FLEE` = `3`
- `MATT_IGNORE` = `4`
- `MATT_FOLLOW` = `5`
- `MATT_ATTACK` = `6`
- `MATT_ZLAVE` = `7`

## MonsterFactionAttitude

### Entries

- `ByMood` = `0`
- `Neutral` = `1`
- `Friendly` = `2`
- `Hate` = `3`

## MonsterFlag

### Entries

- `SEES` = `0`
- `HEARS` = `1`
- `GOODHEARING` = `2`
- `SMELLS` = `3`
- `KEENNOSE` = `4`
- `STUMBLES` = `5`
- `WARM` = `6`
- `NOHEAD` = `7`
- `HARDTOSHOOT` = `8`
- `GRABS` = `9`
- `BASHES` = `10`
- `DESTROYS` = `11`
- `BORES` = `12`
- `POISON` = `13`
- `VENOM` = `14`
- `BADVENOM` = `15`
- `PARALYZEVENOM` = `16`
- `BLEED` = `17`
- `WEBWALK` = `18`
- `DIGS` = `19`
- `CAN_DIG` = `20`
- `FLIES` = `21`
- `AQUATIC` = `22`
- `SWIMS` = `23`
- `ATTACKMON` = `24`
- `ANIMAL` = `25`
- `PLASTIC` = `26`
- `SUNDEATH` = `27`
- `ELECTRIC` = `28`
- `ACIDPROOF` = `29`
- `ACIDTRAIL` = `30`
- `SHORTACIDTRAIL` = `31`
- `FIREPROOF` = `32`
- `SLUDGEPROOF` = `33`
- `SLUDGETRAIL` = `34`
- `COLDPROOF` = `35`
- `BIOPROOF` = `36`
- `FIREY` = `37`
- `QUEEN` = `38`
- `ELECTRONIC` = `39`
- `FUR` = `40`
- `LEATHER` = `41`
- `WOOL` = `42`
- `FEATHER` = `43`
- `BONES` = `44`
- `FAT` = `45`
- `CONSOLE_DESPAWN` = `46`
- `IMMOBILE` = `47`
- `ID_CARD_DESPAWN` = `48`
- `RIDEABLE_MECH` = `49`
- `MILITARY_MECH` = `50`
- `MECH_RECON_VISION` = `51`
- `MECH_DEFENSIVE` = `52`
- `HIT_AND_RUN` = `53`
- `GUILT` = `54`
- `PAY_BOT` = `55`
- `HUMAN` = `56`
- `NO_BREATHE` = `57`
- `FLAMMABLE` = `58`
- `REVIVES` = `59`
- `CHITIN` = `60`
- `VERMIN` = `61`
- `NOGIB` = `62`
- `LARVA` = `63`
- `ARTHROPOD_BLOOD` = `64`
- `ACID_BLOOD` = `65`
- `BILE_BLOOD` = `66`
- `ABSORBS` = `67`
- `ABSORBS_SPLITS` = `68`
- `CBM_CIV` = `69`
- `CBM_POWER` = `70`
- `CBM_SCI` = `71`
- `CBM_OP` = `72`
- `CBM_TECH` = `73`
- `CBM_SUBS` = `74`
- `FILTHY` = `75`
- `FISHABLE` = `76`
- `GROUP_BASH` = `77`
- `SWARMS` = `78`
- `GROUP_MORALE` = `79`
- `INTERIOR_AMMO` = `80`
- `CLIMBS` = `81`
- `PACIFIST` = `82`
- `PUSH_MON` = `83`
- `PUSH_VEH` = `84`
- `NIGHT_INVISIBILITY` = `85`
- `REVIVES_HEALTHY` = `86`
- `NO_NECRO` = `87`
- `PATH_AVOID_DANGER_1` = `88`
- `PATH_AVOID_DANGER_2` = `89`
- `PATH_AVOID_FIRE` = `90`
- `PATH_AVOID_FALL` = `91`
- `PRIORITIZE_TARGETS` = `92`
- `NOT_HALLUCINATION` = `93`
- `CATFOOD` = `94`
- `CATTLEFODDER` = `95`
- `BIRDFOOD` = `96`
- `CANPLAY` = `97`
- `PET_MOUNTABLE` = `98`
- `PET_HARNESSABLE` = `99`
- `DOGFOOD` = `100`
- `MILKABLE` = `101`
- `SHEARABLE` = `102`
- `NO_BREED` = `103`
- `NO_FUNG_DMG` = `104`
- `PET_WONT_FOLLOW` = `105`
- `DRIPS_NAPALM` = `106`
- `DRIPS_GASOLINE` = `107`
- `ELECTRIC_FIELD` = `108`
- `LOUDMOVES` = `109`
- `CAN_OPEN_DOORS` = `110`
- `STUN_IMMUNE` = `111`
- `DROPS_AMMO` = `112`

## MonsterSize

### Entries

- `TINY` = `0`
- `SMALL` = `1`
- `MEDIUM` = `2`
- `LARGE` = `3`
- `HUGE` = `4`

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

## NpcAttitude

### Entries

- `NPCATT_NULL` = `0`
- `NPCATT_TALK` = `1`
- `NPCATT_LEGACY_1` = `2`
- `NPCATT_FOLLOW` = `3`
- `NPCATT_LEGACY_2` = `4`
- `NPCATT_LEAD` = `5`
- `NPCATT_WAIT` = `6`
- `NPCATT_LEGACY_6` = `7`
- `NPCATT_MUG` = `8`
- `NPCATT_WAIT_FOR_LEAVE` = `9`
- `NPCATT_KILL` = `10`
- `NPCATT_FLEE` = `11`
- `NPCATT_LEGACY_3` = `12`
- `NPCATT_HEAL` = `13`
- `NPCATT_LEGACY_4` = `14`
- `NPCATT_LEGACY_5` = `15`
- `NPCATT_ACTIVITY` = `16`
- `NPCATT_FLEE_TEMP` = `17`
- `NPCATT_RECOVER_GOODS` = `18`

## NpcNeed

### Entries

- `need_none` = `0`
- `need_ammo` = `1`
- `need_weapon` = `2`
- `need_gun` = `3`
- `need_food` = `4`
- `need_drink` = `5`
- `need_safety` = `6`

## SfxChannel

### Entries

- `daytime_outdoors_env` = `0`
- `nighttime_outdoors_env` = `1`
- `underground_env` = `2`
- `indoors_env` = `3`
- `indoors_rain_env` = `4`
- `outdoors_snow_env` = `5`
- `outdoors_flurry_env` = `6`
- `outdoors_thunderstorm_env` = `7`
- `outdoors_rain_env` = `8`
- `outdoors_drizzle_env` = `9`
- `outdoor_blizzard` = `10`
- `deafness_tone` = `11`
- `danger_extreme_theme` = `12`
- `danger_high_theme` = `13`
- `danger_medium_theme` = `14`
- `danger_low_theme` = `15`
- `stamina_75` = `16`
- `stamina_50` = `17`
- `stamina_35` = `18`
- `idle_chainsaw` = `19`
- `chainsaw_theme` = `20`
- `player_activities` = `21`
- `exterior_engine_sound` = `22`
- `interior_engine_sound` = `23`
- `radio` = `24`

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

Function `( Tripoint, Opt( Point ) ) -> Tripoint`

#### omt_to_ms

Function `( Tripoint, Opt( Point ) ) -> Tripoint`

#### om_to_ms

Function `( Point, Opt( Tripoint ) ) -> Tripoint`

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

#### get_creature_at

Function `( Tripoint, Opt( bool ) ) -> Creature`

#### get_monster_at

Function `( Tripoint, Opt( bool ) ) -> Monster`

#### get_character_at

Function `( Tripoint, Opt( bool ) ) -> Character`

#### get_npc_at

Function `( Tripoint, Opt( bool ) ) -> Npc`

#### choose_adjacent

Function `( string, Opt( bool ) ) -> Opt( Tripoint )`

#### choose_direction

Function `( string, Opt( bool ) ) -> Opt( Tripoint )`

#### look_around

Function `() -> Opt( Tripoint )`

#### play_variant_sound

Function `( string, string, int )` Function `( string, string, int, Angle, double, double )`

#### play_ambient_variant_sound

Function `( string, string, int, SfxChannel, int, double, int )`

#### add_npc_follower

Function `( Npc )`

#### remove_npc_follower

Function `( Npc )`

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
