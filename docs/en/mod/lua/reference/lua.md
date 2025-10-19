---
edit: false
---

# Lua API reference

> [!NOTE]
>
> This page is auto-generated from [`data/raw/generate_docs.lua`][generate_docs]
> and should not be edited directly.

[generate_docs]: https://github.com/cataclysmbnteam/Cataclysm-BN/blob/main/data/raw/generate_docs.lua

## ActivityTypeId

### Bases

No base classes.

### Constructors

- #### `ActivityTypeId.new()`
- #### `ActivityTypeId.new( ActivityTypeId )`
- #### `ActivityTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> ActivityTypeId`

- #### obj
  🇲 Method --> `() -> ActivityTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## AmmunitionTypeId

### Bases

No base classes.

### Constructors

- #### `AmmunitionTypeId.new()`
- #### `AmmunitionTypeId.new( AmmunitionTypeId )`
- #### `AmmunitionTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> AmmunitionTypeId`

- #### obj
  🇲 Method --> `() -> AmmunitionTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## Angle

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### from_arcmin
  🇫 Function --> `( double ) -> Angle`

- #### from_degrees
  🇫 Function --> `( double ) -> Angle`

- #### from_radians
  🇫 Function --> `( double ) -> Angle`

- #### to_arcmin
  🇲 Method --> `() -> double`

- #### to_degrees
  🇲 Method --> `() -> double`

- #### to_radians
  🇲 Method --> `() -> double`

## Avatar

### Bases

- `Player`
- `Character`
- `Creature`

### Constructors

No constructors.

### Members

- #### get_active_missions
  🇲 Method --> `() -> Vector( Mission )`

- #### get_completed_missions
  🇲 Method --> `() -> Vector( Mission )`

- #### get_failed_missions
  🇲 Method --> `() -> Vector( Mission )`

## BionicDataId

### Bases

No base classes.

### Constructors

- #### `BionicDataId.new()`
- #### `BionicDataId.new( BionicDataId )`
- #### `BionicDataId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> BionicDataId`

- #### obj
  🇲 Method --> `() -> BionicDataRaw`

- #### str
  🇲 Method --> `() -> string`

## BodyPartTypeId

### Bases

No base classes.

### Constructors

- #### `BodyPartTypeId.new()`
- #### `BodyPartTypeId.new( BodyPartTypeId )`
- #### `BodyPartTypeId.new( BodyPartTypeIntId )`
- #### `BodyPartTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### int_id
  🇲 Method --> `() -> BodyPartTypeIntId`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> BodyPartTypeId`

- #### obj
  🇲 Method --> `() -> BodyPartTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## BodyPartTypeIntId

### Bases

No base classes.

### Constructors

- #### `BodyPartTypeIntId.new()`
- #### `BodyPartTypeIntId.new( BodyPartTypeIntId )`
- #### `BodyPartTypeIntId.new( BodyPartTypeId )`

### Members

- #### is_valid
  🇲 Method --> `() -> bool`

- #### obj
  🇲 Method --> `() -> BodyPartTypeRaw`

- #### str_id
  🇲 Method --> `() -> BodyPartTypeId`

## BookRecipe

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### hidden
  🇻 Variable --> `bool`

- #### name
  🇻 Variable --> `string`

- #### recipe
  🇻 Variable --> `RecipeRaw`

- #### skill_level
  🇻 Variable --> `int`

## Character

### Bases

- `Creature`

### Constructors

No constructors.

### Members

- #### activate_mutation
  🇲 Method --> `( MutationBranchId )`

- #### add_addiction
  🇲 Method --> `( AddictionType, int )`

- #### add_bionic
  🇲 Method --> `( BionicDataId )`

- #### addiction_level
  🇲 Method --> `( AddictionType ) -> int`

- #### add_item_with_id
  🇲 Method --> `( ItypeId, int )`
  > Adds an item with the given id and amount

- #### add_morale
  🇲 Method --> `( MoraleTypeDataId, int, int, TimeDuration, TimeDuration, bool, ItypeRaw )`

- #### age
  🇲 Method --> `() -> int`

- #### all_items
  🇲 Method --> `( bool ) -> Vector( Item )`
  > Gets all items

- #### all_items_with_flag
  🇲 Method --> `( JsonFlagId, bool ) -> Vector( Item )`
  > Gets all items with the given flag

- #### assign_activity
  🇲 Method --> `( ActivityTypeId, int, int, int, string )`

- #### base_age
  🇲 Method --> `() -> int`

- #### base_height
  🇲 Method --> `() -> int`

- #### bionic_armor_bonus
  🇲 Method --> `( BodyPartTypeIntId, DamageType ) -> double`

- #### bionics_weight
  🇲 Method --> `() -> Mass`

- #### blood_loss
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### blossoms
  🇲 Method --> `()`

- #### bodypart_exposure
  🇲 Method --> `() -> Map( BodyPartTypeIntId, double )`

- #### bodyweight
  🇲 Method --> `() -> Mass`

- #### cancel_activity
  🇲 Method --> `()`

- #### can_hear
  🇲 Method --> `( Tripoint, int ) -> bool`

- #### can_mount
  🇲 Method --> `( Monster ) -> bool`

- #### can_pick_volume
  🇲 Method --> `( Volume ) -> bool`

- #### can_pick_weight
  🇲 Method --> `( Mass, bool ) -> bool`

- #### can_run
  🇲 Method --> `() -> bool`

- #### can_unwield
  🇲 Method --> `( Item ) -> bool`

- #### can_wield
  🇲 Method --> `( Item ) -> bool`

- #### cash
  🇻 Variable --> `int`

- #### check_mount_is_spooked
  🇲 Method --> `() -> bool`

- #### check_mount_will_move
  🇲 Method --> `( Tripoint ) -> bool`

- #### clear_bionics
  🇲 Method --> `()`

- #### clear_morale
  🇲 Method --> `()`

- #### clear_mutations
  🇲 Method --> `()`

- #### clear_skills
  🇲 Method --> `()`

- #### cough
  🇲 Method --> `( bool, int )`

- #### crossed_threshold
  🇲 Method --> `() -> bool`

- #### deactivate_mutation
  🇲 Method --> `( MutationBranchId )`

- #### dismount
  🇲 Method --> `()`

- #### expose_to_disease
  🇲 Method --> `( DiseaseTypeId )`

- #### fall_asleep
  🇲 Method --> `()`\
  🇲 Method --> `( TimeDuration )`

- #### focus_pool
  🇻 Variable --> `int`

- #### follower_ids
  🇻 Variable --> `Set( CharacterId )`

- #### forced_dismount
  🇲 Method --> `()`

- #### get_all_skills
  🇲 Method --> `() -> SkillLevelMap`

- #### get_armor_acid
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_base_traits
  🇲 Method --> `() -> Vector( MutationBranchId )`

- #### get_bionics
  🇲 Method --> `() -> Vector( BionicDataId )`

- #### get_dex
  🇲 Method --> `() -> int`

- #### get_dex_base
  🇲 Method --> `() -> int`

- #### get_dex_bonus
  🇲 Method --> `() -> int`

- #### get_faction_id
  🇲 Method --> `() -> FactionId`

- #### get_fatigue
  🇲 Method --> `() -> int`

- #### get_free_bionics_slots
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_healthy
  🇲 Method --> `() -> double`

- #### get_healthy_mod
  🇲 Method --> `() -> double`

- #### get_highest_category
  🇲 Method --> `() -> MutationCategoryTraitId`

- #### get_hostile_creatures
  🇲 Method --> `( int ) -> Vector( Creature )`

- #### getID
  🇲 Method --> `() -> CharacterId`

- #### get_int
  🇲 Method --> `() -> int`

- #### get_int_base
  🇲 Method --> `() -> int`

- #### get_int_bonus
  🇲 Method --> `() -> int`

- #### get_item_with_id
  🇲 Method --> `( ItypeId, bool ) -> Item`
  > Gets the first occurrence of an item with the given id

- #### get_kcal_percent
  🇲 Method --> `() -> double`

- #### get_lowest_hp
  🇲 Method --> `() -> int`

- #### get_max_power_level
  🇲 Method --> `() -> Energy`

- #### get_melee_stamina_cost
  🇲 Method --> `( Item ) -> int`

- #### get_morale
  🇲 Method --> `( MoraleTypeDataId ) -> int`

- #### get_morale_level
  🇲 Method --> `() -> int`

- #### get_movement_mode
  🇲 Method --> `() -> CharacterMoveMode`

- #### get_mutations
  🇲 Method --> `( bool ) -> Vector( MutationBranchId )`

- #### get_painkiller
  🇲 Method --> `() -> int`

- #### get_part_encumbrance
  🇲 Method --> `( BodyPartTypeId ) -> int`

- #### get_part_temp_btu
  🇲 Method --> `( BodyPartTypeIntId ) -> int`
  > Gets the current temperature of a specific body part (in Body Temperature Units).

- #### get_per
  🇲 Method --> `() -> int`

- #### get_per_base
  🇲 Method --> `() -> int`

- #### get_per_bonus
  🇲 Method --> `() -> int`

- #### get_power_level
  🇲 Method --> `() -> Energy`

- #### get_rad
  🇲 Method --> `() -> int`

- #### get_shout_volume
  🇲 Method --> `() -> int`

- #### get_skill_level
  🇲 Method --> `( SkillId ) -> int`

- #### get_skill_level_object
  🇲 Method --> `( SkillId ) -> SkillLevel`

- #### get_sleep_deprivation
  🇲 Method --> `() -> int`

- #### get_stamina
  🇲 Method --> `() -> int`

- #### get_stamina_max
  🇲 Method --> `() -> int`

- #### get_stim
  🇲 Method --> `() -> int`

- #### get_stored_kcal
  🇲 Method --> `() -> int`

- #### get_str
  🇲 Method --> `() -> int`

- #### get_str_base
  🇲 Method --> `() -> int`

- #### get_str_bonus
  🇲 Method --> `() -> int`

- #### get_temp_btu
  🇲 Method --> `() -> Map( BodyPartTypeIntId, int )`
  > Gets all bodyparts and their associated temperatures (in Body Temperature Units).

- #### get_thirst
  🇲 Method --> `() -> int`

- #### get_time_died
  🇲 Method --> `() -> TimePoint`

- #### get_total_bionics_slots
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_used_bionics_slots
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_visible_creatures
  🇲 Method --> `( int ) -> Vector( Creature )`

- #### get_working_arm_count
  🇲 Method --> `() -> int`

- #### get_working_leg_count
  🇲 Method --> `() -> int`

- #### global_sm_location
  🇲 Method --> `() -> Tripoint`

- #### global_square_location
  🇲 Method --> `() -> Tripoint`

- #### has_active_bionic
  🇲 Method --> `( BionicDataId ) -> bool`

- #### has_active_mutation
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### has_activity
  🇲 Method --> `( ActivityTypeId ) -> bool`

- #### has_addiction
  🇲 Method --> `( AddictionType ) -> bool`

- #### has_alarm_clock
  🇲 Method --> `() -> bool`

- #### has_any_bionic
  🇲 Method --> `() -> bool`

- #### has_base_trait
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### has_bionic
  🇲 Method --> `( BionicDataId ) -> bool`

- #### has_bionics
  🇲 Method --> `() -> bool`

- #### has_child_flag
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### has_item_with_flag
  🇲 Method --> `( JsonFlagId, bool ) -> bool`
  > Checks for an item with the given flag

- #### has_item_with_id
  🇲 Method --> `( ItypeId, bool ) -> bool`
  > Checks for an item with the given id

- #### has_mabuff
  🇲 Method --> `( MartialArtsBuffId ) -> bool`

- #### has_max_power
  🇲 Method --> `() -> bool`

- #### has_morale
  🇲 Method --> `( MoraleTypeDataId ) -> bool`

- #### has_morale_to_craft
  🇲 Method --> `() -> bool`

- #### has_morale_to_read
  🇲 Method --> `() -> bool`

- #### has_opposite_trait
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### has_power
  🇲 Method --> `() -> bool`

- #### has_trait_flag
  🇲 Method --> `( JsonTraitFlagId ) -> bool`

- #### has_two_arms
  🇲 Method --> `() -> bool`

- #### has_watch
  🇲 Method --> `() -> bool`

- #### heal
  🇲 Method --> `( BodyPartTypeIntId, int )`

- #### healall
  🇲 Method --> `( int )`

- #### healing_rate
  🇲 Method --> `( double ) -> double`

- #### healing_rate_medicine
  🇲 Method --> `( double, BodyPartTypeIntId ) -> double`

- #### hearing_ability
  🇲 Method --> `() -> double`

- #### height
  🇲 Method --> `() -> int`

- #### hitall
  🇲 Method --> `( int, int, Creature ) -> int`

- #### hurtall
  🇲 Method --> `( int, Creature, bool )`

- #### in_climate_control
  🇲 Method --> `() -> bool`

- #### inv_remove_item
  🇲 Method --> `( Item ) -> <cppval: detached_ptr<item> >`
  > Removes given `Item` from character's inventory. The `Item` must be in the inventory, neither wielded nor worn.

- #### irradiate
  🇲 Method --> `( double, bool ) -> bool`

- #### is_armed
  🇲 Method --> `() -> bool`

- #### is_blind
  🇲 Method --> `() -> bool`

- #### is_deaf
  🇲 Method --> `() -> bool`

- #### is_hauling
  🇲 Method --> `() -> bool`

- #### is_invisible
  🇲 Method --> `() -> bool`

- #### is_limb_broken
  🇲 Method --> `( BodyPartTypeIntId ) -> bool`

- #### is_limb_disabled
  🇲 Method --> `( BodyPartTypeIntId ) -> bool`

- #### is_max_power
  🇲 Method --> `() -> bool`

- #### is_mounted
  🇲 Method --> `() -> bool`

- #### is_quiet
  🇲 Method --> `() -> bool`

- #### is_rad_immune
  🇲 Method --> `() -> bool`

- #### is_stealthy
  🇲 Method --> `() -> bool`

- #### is_throw_immune
  🇲 Method --> `() -> bool`

- #### is_weak_to_water
  🇲 Method --> `() -> bool`

- #### is_wearing
  🇲 Method --> `( Item ) -> bool`

- #### is_wearing_active_optcloak
  🇲 Method --> `() -> bool`

- #### is_wearing_active_power_armor
  🇲 Method --> `() -> bool`

- #### is_wearing_helmet
  🇲 Method --> `() -> bool`

- #### is_wearing_on_bp
  🇲 Method --> `( ItypeId, BodyPartTypeIntId ) -> bool`

- #### is_wearing_power_armor
  🇲 Method --> `( bool ) -> bool`

- #### is_wielding
  🇲 Method --> `( Item ) -> bool`

- #### is_worn
  🇲 Method --> `( Item ) -> bool`

- #### items_with
  🇲 Method --> `( <cppval: const std::function<bool (const item &)> & > ) -> Vector( Item )`
  > Filters items

- #### item_worn_with_flag
  🇲 Method --> `( JsonFlagId, BodyPartTypeIntId ) -> Item`

- #### item_worn_with_id
  🇲 Method --> `( ItypeId, BodyPartTypeIntId ) -> Item`

- #### knows_recipe
  🇲 Method --> `( RecipeId ) -> bool`

- #### learn_recipe
  🇲 Method --> `( RecipeId )`

- #### mabuff_armor_bonus
  🇲 Method --> `( DamageType ) -> int`

- #### mabuff_arpen_bonus
  🇲 Method --> `( DamageType ) -> int`

- #### mabuff_attack_cost_mult
  🇲 Method --> `() -> double`

- #### mabuff_attack_cost_penalty
  🇲 Method --> `() -> int`

- #### mabuff_block_bonus
  🇲 Method --> `() -> int`

- #### mabuff_damage_bonus
  🇲 Method --> `( DamageType ) -> int`

- #### mabuff_damage_mult
  🇲 Method --> `( DamageType ) -> double`

- #### mabuff_dodge_bonus
  🇲 Method --> `() -> double`

- #### mabuff_speed_bonus
  🇲 Method --> `() -> int`

- #### mabuff_tohit_bonus
  🇲 Method --> `() -> double`

- #### male
  🇻 Variable --> `bool`

- #### max_stored_kcal
  🇲 Method --> `() -> int`

- #### metabolic_rate
  🇲 Method --> `() -> double`

- #### mod_base_age
  🇲 Method --> `( int )`

- #### mod_base_height
  🇲 Method --> `( int )`

- #### mod_dex_bonus
  🇲 Method --> `( int )`

- #### mod_fatigue
  🇲 Method --> `( int )`

- #### mod_healthy
  🇲 Method --> `( double )`

- #### mod_healthy_mod
  🇲 Method --> `( double, double )`

- #### mod_int_bonus
  🇲 Method --> `( int )`

- #### mod_max_power_level
  🇲 Method --> `( Energy )`

- #### mod_painkiller
  🇲 Method --> `( int )`

- #### mod_per_bonus
  🇲 Method --> `( int )`

- #### mod_power_level
  🇲 Method --> `( Energy )`

- #### mod_rad
  🇲 Method --> `( int )`

- #### mod_skill_level
  🇲 Method --> `( SkillId, int )`

- #### mod_sleep_deprivation
  🇲 Method --> `( int )`

- #### mod_speed_bonus
  🇲 Method --> `( int )`

- #### mod_stamina
  🇲 Method --> `( int )`

- #### mod_stim
  🇲 Method --> `( int )`

- #### mod_stored_kcal
  🇲 Method --> `( int )`

- #### mod_str_bonus
  🇲 Method --> `( int )`

- #### mod_thirst
  🇲 Method --> `( int )`

- #### mount_creature
  🇲 Method --> `( Monster )`

- #### mutate
  🇲 Method --> `()`

- #### mutate_category
  🇲 Method --> `( MutationCategoryTraitId )`

- #### mutate_towards
  🇲 Method --> `( Vector( MutationBranchId ), int ) -> bool`

- #### mutate_towards
  🇲 Method --> `( Vector( MutationBranchId ), int ) -> bool`\
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### mutate_towards
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### mutation_armor
  🇲 Method --> `( BodyPartTypeIntId, DamageType ) -> double`

- #### mutation_category_level
  🇻 Variable --> `Map( MutationCategoryTraitId, int )`

- #### mutation_effect
  🇲 Method --> `( MutationBranchId )`

- #### mutation_loss_effect
  🇲 Method --> `( MutationBranchId )`

- #### mutation_ok
  🇲 Method --> `( MutationBranchId, bool, bool ) -> bool`

- #### mutation_value
  🇲 Method --> `( string ) -> double`

- #### name
  🇻 Variable --> `string`

- #### practice
  🇲 Method --> `( SkillId, int, int, bool )`

- #### read_speed
  🇲 Method --> `( bool ) -> int`

- #### rem_addiction
  🇲 Method --> `( AddictionType )`

- #### rem_morale
  🇲 Method --> `( MoraleTypeDataId )`

- #### remove_bionic
  🇲 Method --> `( BionicDataId )`

- #### remove_child_flag
  🇲 Method --> `( MutationBranchId )`

- #### remove_mutation
  🇲 Method --> `( MutationBranchId, bool )`

- #### restore_scent
  🇲 Method --> `()`

- #### rest_quality
  🇲 Method --> `() -> double`

- #### rooted
  🇲 Method --> `()`

- #### rust_rate
  🇲 Method --> `() -> int`

- #### set_base_age
  🇲 Method --> `( int )`

- #### set_base_height
  🇲 Method --> `( int )`

- #### set_dex_bonus
  🇲 Method --> `( int )`

- #### set_faction_id
  🇲 Method --> `( FactionId )`

- #### set_fatigue
  🇲 Method --> `( int )`

- #### set_healthy
  🇲 Method --> `( double )`

- #### set_healthy_mod
  🇲 Method --> `( double )`

- #### setID
  🇲 Method --> `( CharacterId, bool )`

- #### set_int_bonus
  🇲 Method --> `( int )`

- #### set_max_power_level
  🇲 Method --> `( Energy )`

- #### set_movement_mode
  🇲 Method --> `( CharacterMoveMode )`

- #### set_mutation
  🇲 Method --> `( MutationBranchId )`

- #### set_painkiller
  🇲 Method --> `( int )`

- #### set_part_temp_btu
  🇲 Method --> `( BodyPartTypeIntId, int )`
  > Sets a specific body part to a given temperature (in Body Temperature Units).

- #### set_per_bonus
  🇲 Method --> `( int )`

- #### set_power_level
  🇲 Method --> `( Energy )`

- #### set_rad
  🇲 Method --> `( int )`

- #### set_skill_level
  🇲 Method --> `( SkillId, int )`

- #### set_sleep_deprivation
  🇲 Method --> `( int )`

- #### set_speed_bonus
  🇲 Method --> `( int )`

- #### set_stamina
  🇲 Method --> `( int )`

- #### set_stim
  🇲 Method --> `( int )`

- #### set_stored_kcal
  🇲 Method --> `( int )`

- #### set_str_bonus
  🇲 Method --> `( int )`

- #### set_temp_btu
  🇲 Method --> `( int )`
  > Sets ALL body parts on a creature to the given temperature (in Body Temperature Units).

- #### set_thirst
  🇲 Method --> `( int )`

- #### shout
  🇲 Method --> `( string, bool )`

- #### sight_impaired
  🇲 Method --> `() -> bool`

- #### spores
  🇲 Method --> `()`

- #### suffer
  🇲 Method --> `()`

- #### uncanny_dodge
  🇲 Method --> `() -> bool`

- #### unset_mutation
  🇲 Method --> `( MutationBranchId )`

- #### unwield
  🇲 Method --> `() -> bool`

- #### use_charges
  🇲 Method --> `( ItypeId, int, <cppval: const std::function<bool (const item &)> & > ) -> Vector( <cppval: detached_ptr<item> > )`

- #### use_charges_if_avail
  🇲 Method --> `( ItypeId, int ) -> bool`

- #### volume_capacity
  🇲 Method --> `() -> Volume`

- #### volume_carried
  🇲 Method --> `() -> Volume`

- #### vomit
  🇲 Method --> `()`

- #### wake_up
  🇲 Method --> `()`

- #### wearing_something_on
  🇲 Method --> `( BodyPartTypeIntId ) -> bool`

- #### weight_carried
  🇲 Method --> `() -> Mass`

- #### wield
  🇲 Method --> `( Item ) -> bool`

- #### worn_with_flag
  🇲 Method --> `( JsonFlagId, BodyPartTypeIntId ) -> bool`

- #### worn_with_id
  🇲 Method --> `( ItypeId, BodyPartTypeIntId ) -> bool`

## CharacterId

### Bases

No base classes.

### Constructors

- #### `CharacterId.new()`
- #### `CharacterId.new( int )`

### Members

- #### get_value
  🇲 Method --> `() -> int`

- #### is_valid
  🇲 Method --> `() -> bool`

## Creature

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### add_effect
  🇲 Method --> `( EffectTypeId, TimeDuration, Opt( BodyPartTypeId ), Opt( int ) )`
  > Effect type, duration, bodypart and intensity

- #### apply_damage
  🇲 Method --> `( Creature, BodyPartTypeIntId, int, bool )`

- #### as_avatar
  🇲 Method --> `() -> Avatar`

- #### as_character
  🇲 Method --> `() -> Character`

- #### as_monster
  🇲 Method --> `() -> Monster`

- #### as_npc
  🇲 Method --> `() -> Npc`

- #### attitude_to
  🇲 Method --> `( Creature ) -> Attitude`

- #### clear_effects
  🇲 Method --> `()`

- #### deal_damage
  🇲 Method --> `( Creature, BodyPartTypeIntId, DamageInstance ) -> DealtDamageInstance`

- #### digging
  🇲 Method --> `() -> bool`

- #### disp_name
  🇲 Method --> `( bool, bool ) -> string`

- #### dodge_roll
  🇲 Method --> `() -> double`

- #### get_armor_bash
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_armor_bash_base
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_armor_bash_bonus
  🇲 Method --> `() -> int`

- #### get_armor_bullet
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_armor_bullet_base
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_armor_bullet_bonus
  🇲 Method --> `() -> int`

- #### get_armor_cut
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_armor_cut_base
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_armor_cut_bonus
  🇲 Method --> `() -> int`

- #### get_armor_type
  🇲 Method --> `( DamageType, BodyPartTypeIntId ) -> int`

- #### get_block_bonus
  🇲 Method --> `() -> int`

- #### get_dodge
  🇲 Method --> `() -> double`

- #### get_dodge_base
  🇲 Method --> `() -> double`

- #### get_dodge_bonus
  🇲 Method --> `() -> double`

- #### get_effect_dur
  🇲 Method --> `( EffectTypeId, Opt( BodyPartTypeId ) ) -> TimeDuration`

- #### get_effect_int
  🇲 Method --> `( EffectTypeId, Opt( BodyPartTypeId ) ) -> int`

- #### get_env_resist
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_grammatical_genders
  🇲 Method --> `() -> Vector( string )`

- #### get_hit
  🇲 Method --> `() -> double`

- #### get_hit_base
  🇲 Method --> `() -> double`

- #### get_hit_bonus
  🇲 Method --> `() -> double`

- #### get_hp
  🇲 Method --> `( Opt( BodyPartTypeIntId ) ) -> int`

- #### get_hp_max
  🇲 Method --> `( Opt( BodyPartTypeIntId ) ) -> int`

- #### get_melee
  🇲 Method --> `() -> double`

- #### get_moves
  🇲 Method --> `() -> int`

- #### get_name
  🇲 Method --> `() -> string`

- #### get_num_blocks
  🇲 Method --> `() -> int`

- #### get_num_dodges
  🇲 Method --> `() -> int`

- #### get_pain
  🇲 Method --> `() -> int`

- #### get_part_healed_total
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_part_hp_cur
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_part_hp_max
  🇲 Method --> `( BodyPartTypeIntId ) -> int`

- #### get_perceived_pain
  🇲 Method --> `() -> int`

- #### get_pos_ms
  🇲 Method --> `() -> Tripoint`

- #### get_size
  🇲 Method --> `() -> MonsterSize`

- #### get_speed
  🇲 Method --> `() -> int`

- #### get_speed_base
  🇲 Method --> `() -> int`

- #### get_speed_bonus
  🇲 Method --> `() -> int`

- #### get_speed_mult
  🇲 Method --> `() -> double`

- #### get_value
  🇲 Method --> `( string ) -> string`

- #### get_weight
  🇲 Method --> `() -> Mass`

- #### get_weight_capacity
  🇲 Method --> `() -> int`

- #### has_effect
  🇲 Method --> `( EffectTypeId, Opt( BodyPartTypeId ) ) -> bool`

- #### has_effect_with_flag
  🇲 Method --> `( JsonFlagId, Opt( BodyPartTypeId ) ) -> bool`

- #### has_flag
  🇲 Method --> `( MonsterFlag ) -> bool`

- #### has_grab_break_tec
  🇲 Method --> `() -> bool`

- #### has_trait
  🇲 Method --> `( MutationBranchId ) -> bool`

- #### has_weapon
  🇲 Method --> `() -> bool`

- #### hp_percentage
  🇲 Method --> `() -> int`

- #### in_species
  🇲 Method --> `( SpeciesTypeId ) -> bool`

- #### is_avatar
  🇲 Method --> `() -> bool`

- #### is_dead
  🇲 Method --> `() -> bool`

- #### is_elec_immune
  🇲 Method --> `() -> bool`

- #### is_hallucination
  🇲 Method --> `() -> bool`

- #### is_immune_damage
  🇲 Method --> `( DamageType ) -> bool`

- #### is_immune_effect
  🇲 Method --> `( EffectTypeId ) -> bool`

- #### is_monster
  🇲 Method --> `() -> bool`

- #### is_npc
  🇲 Method --> `() -> bool`

- #### is_on_ground
  🇲 Method --> `() -> bool`

- #### is_underwater
  🇲 Method --> `() -> bool`

- #### is_warm
  🇲 Method --> `() -> bool`

- #### knock_back_to
  🇲 Method --> `( Tripoint )`

- #### mod_moves
  🇲 Method --> `( int )`

- #### mod_pain
  🇲 Method --> `( int )`

- #### mod_pain_noresist
  🇲 Method --> `( int )`

- #### mod_part_hp_cur
  🇲 Method --> `( BodyPartTypeIntId, int )`

- #### mod_part_hp_max
  🇲 Method --> `( BodyPartTypeIntId, int )`

- #### power_rating
  🇲 Method --> `() -> double`

- #### ranged_target_size
  🇲 Method --> `() -> double`

- #### remove_effect
  🇲 Method --> `( EffectTypeId, Opt( BodyPartTypeId ) ) -> bool`

- #### remove_value
  🇲 Method --> `( string )`

- #### sees
  🇲 Method --> `( Creature ) -> bool`

- #### set_all_parts_hp_cur
  🇲 Method --> `( int )`

- #### set_all_parts_hp_to_max
  🇲 Method --> `()`

- #### set_moves
  🇲 Method --> `( int )`

- #### set_pain
  🇲 Method --> `( int )`

- #### set_part_hp_cur
  🇲 Method --> `( BodyPartTypeIntId, int )`

- #### set_part_hp_max
  🇲 Method --> `( BodyPartTypeIntId, int )`

- #### set_pos_ms
  🇲 Method --> `( Tripoint )`

- #### set_underwater
  🇲 Method --> `( bool )`

- #### set_value
  🇲 Method --> `( string, string )`

- #### sight_range
  🇲 Method --> `( int ) -> int`

- #### size_melee_penalty
  🇲 Method --> `() -> int`

- #### skin_name
  🇲 Method --> `() -> string`

- #### speed_rating
  🇲 Method --> `() -> double`

- #### stability_roll
  🇲 Method --> `() -> double`

## DamageInstance

new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)

### Bases

No base classes.

### Constructors

- #### `DamageInstance.new()`
- #### `DamageInstance.new( DamageType, double, double, double, double )`

### Members

- #### add
  🇲 Method --> `( DamageUnit )`

- #### add_damage
  🇲 Method --> `( DamageType, double, double, double, double )`

- #### clear
  🇲 Method --> `()`

- #### damage_units
  🇻 Variable --> `Vector( DamageUnit )`

- #### empty
  🇲 Method --> `() -> bool`

- #### mult_damage
  🇲 Method --> `( double, bool )`

- #### total_damage
  🇲 Method --> `() -> double`

- #### type_damage
  🇲 Method --> `( DamageType ) -> double`

## DamageUnit

new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)

### Bases

No base classes.

### Constructors

- #### `DamageUnit.new( DamageType, double, double, double, double )`

### Members

- #### amount
  🇻 Variable --> `double`

- #### damage_multiplier
  🇻 Variable --> `double`

- #### res_mult
  🇻 Variable --> `double`

- #### res_pen
  🇻 Variable --> `double`

- #### type
  🇻 Variable --> `DamageType`

## DealtDamageInstance

Represents the final dealt damage

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### bp_hit
  🇻 Variable --> `BodyPartTypeId`

- #### dealt_dams
  🇻 Variable --> `Array( int, 14 )`

- #### total_damage
  🇲 Method --> `() -> int`

- #### type_damage
  🇲 Method --> `( DamageType ) -> int`

## DiseaseTypeId

### Bases

No base classes.

### Constructors

- #### `DiseaseTypeId.new()`
- #### `DiseaseTypeId.new( DiseaseTypeId )`
- #### `DiseaseTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> DiseaseTypeId`

- #### obj
  🇲 Method --> `() -> DiseaseTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## DistributionGrid

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### get_resource
  🇲 Method --> `( bool ) -> int`
  > Boolean argument controls recursive behavior

- #### mod_resource
  🇲 Method --> `( int, bool ) -> int`
  > Boolean argument controls recursive behavior

## DistributionGridTracker

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### get_grid_at_abs_ms
  🇲 Method --> `( Tripoint ) -> DistributionGrid`

## EffectTypeId

### Bases

No base classes.

### Constructors

- #### `EffectTypeId.new()`
- #### `EffectTypeId.new( EffectTypeId )`
- #### `EffectTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> EffectTypeId`

- #### obj
  🇲 Method --> `() -> EffectTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## Energy

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### from_joule
  🇫 Function --> `( int ) -> Energy`

- #### from_kilojoule
  🇫 Function --> `( int ) -> Energy`

- #### to_joule
  🇲 Method --> `() -> int`

- #### to_kilojoule
  🇲 Method --> `() -> int`

## FactionId

### Bases

No base classes.

### Constructors

- #### `FactionId.new()`
- #### `FactionId.new( FactionId )`
- #### `FactionId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> FactionId`

- #### obj
  🇲 Method --> `() -> FactionRaw`

- #### str
  🇲 Method --> `() -> string`

## FactionRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### str_id
  🇲 Method --> `() -> FactionId`

## FieldTypeId

### Bases

No base classes.

### Constructors

- #### `FieldTypeId.new()`
- #### `FieldTypeId.new( FieldTypeId )`
- #### `FieldTypeId.new( FieldTypeIntId )`
- #### `FieldTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### int_id
  🇲 Method --> `() -> FieldTypeIntId`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> FieldTypeId`

- #### obj
  🇲 Method --> `() -> FieldTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## FieldTypeIntId

### Bases

No base classes.

### Constructors

- #### `FieldTypeIntId.new()`
- #### `FieldTypeIntId.new( FieldTypeIntId )`
- #### `FieldTypeIntId.new( FieldTypeId )`

### Members

- #### is_valid
  🇲 Method --> `() -> bool`

- #### obj
  🇲 Method --> `() -> FieldTypeRaw`

- #### str_id
  🇲 Method --> `() -> FieldTypeId`

## FurnId

### Bases

No base classes.

### Constructors

- #### `FurnId.new()`
- #### `FurnId.new( FurnId )`
- #### `FurnId.new( FurnIntId )`
- #### `FurnId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### int_id
  🇲 Method --> `() -> FurnIntId`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> FurnId`

- #### obj
  🇲 Method --> `() -> FurnRaw`

- #### str
  🇲 Method --> `() -> string`

## FurnIntId

### Bases

No base classes.

### Constructors

- #### `FurnIntId.new()`
- #### `FurnIntId.new( FurnIntId )`
- #### `FurnIntId.new( FurnId )`

### Members

- #### is_valid
  🇲 Method --> `() -> bool`

- #### obj
  🇲 Method --> `() -> FurnRaw`

- #### str_id
  🇲 Method --> `() -> FurnId`

## FurnRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### close
  🇻 Variable --> `FurnId`

- #### get_coverage
  🇲 Method --> `() -> int`

- #### get_flags
  🇲 Method --> `() -> <cppval: const std::set<std::basic_string<char>> & >`

- #### get_light_emitted
  🇲 Method --> `() -> int`

- #### get_max_volume
  🇲 Method --> `() -> Volume`

- #### get_movecost
  🇲 Method --> `() -> int`

- #### has_flag
  🇲 Method --> `( string ) -> bool`

- #### int_id
  🇲 Method --> `() -> FurnIntId`

- #### name
  🇲 Method --> `() -> string`

- #### open
  🇻 Variable --> `FurnId`

- #### set_coverage
  🇲 Method --> `( int )`

- #### set_flag
  🇲 Method --> `( string )`

- #### set_light_emitted
  🇲 Method --> `( int )`

- #### set_max_volume
  🇲 Method --> `( Volume )`

- #### set_movecost
  🇲 Method --> `( int )`

- #### str_id
  🇲 Method --> `() -> FurnId`

- #### transforms_into
  🇻 Variable --> `FurnId`

## IslotAmmo

### Bases

- `RangedData`

### Constructors

No constructors.

### Members

- #### ammo_id
  🇻 Variable --> `AmmunitionTypeId`

- #### casing_id
  🇻 Variable --> `Opt( ItypeId )`

- #### cookoff
  🇻 Variable --> `bool`

- #### def_charges
  🇻 Variable --> `int`

- #### dont_recover_one_in
  🇻 Variable --> `int`

- #### drop
  🇻 Variable --> `ItypeId`

- #### drop_active
  🇻 Variable --> `bool`

- #### drop_count
  🇻 Variable --> `int`

- #### force_stat_display
  🇻 Variable --> `Opt( bool )`

- #### loudness
  🇻 Variable --> `int`

- #### recoil
  🇻 Variable --> `int`

- #### shape
  🇻 Variable --> `Opt( <cppval: shape_factory > )`

- #### special_cookoff
  🇻 Variable --> `bool`

## IslotArmor

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### env_resist
  🇻 Variable --> `int`
  > Resistance to environmental effects

- #### env_resist_w_filter
  🇻 Variable --> `int`
  > Environmental protection of a gas mask with installed filter

- #### layer_data
  🇻 Variable --> `Vector( <cppval: armor_portion_data > )`
  > Layer, encumbrance and coverage information

- #### resistance
  🇻 Variable --> `<cppval: resistances >`
  > Damage negated by this armor. Usually calculated from materials+thickness

- #### sided
  🇻 Variable --> `bool`
  > Whether this item can be worn on either side of the body

- #### storage
  🇻 Variable --> `Volume`
  > How much storage this items provides when worn

- #### thickness
  🇻 Variable --> `int`
  > Multiplier on resistances provided by armor's materials.\
  > Damaged armors have lower effective thickness, low capped at 1.\
  > Note: 1 thickness means item retains full resistance when damaged.

- #### valid_mods
  🇻 Variable --> `Vector( string )`
  > Whitelisted clothing mods.\
  > Restricted clothing mods must be listed here by id to be compatible.

- #### warmth
  🇻 Variable --> `int`
  > How much warmth this item provides

- #### weight_capacity_bonus
  🇻 Variable --> `Mass`
  > Bonus to weight capacity

- #### weight_capacity_modifier
  🇻 Variable --> `double`
  > Factor modifying weight capacity

## IslotArtifact

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### charge_req
  🇻 Variable --> `ArtifactChargeReq`

- #### charge_type
  🇻 Variable --> `ArtifactCharge`

- #### dream_freq_met
  🇻 Variable --> `int`

- #### dream_freq_unmet
  🇻 Variable --> `int`

- #### dream_msg_met
  🇻 Variable --> `Vector( string )`

- #### dream_msg_unmet
  🇻 Variable --> `Vector( string )`

- #### effects_activated
  🇻 Variable --> `Vector( ArtifactEffectPassive )`

- #### effects_carried
  🇻 Variable --> `Vector( ArtifactEffectActive )`

- #### effects_wielded
  🇻 Variable --> `Vector( ArtifactEffectActive )`

- #### effects_worn
  🇻 Variable --> `Vector( ArtifactEffectActive )`

## IslotBattery

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### max_capacity
  🇻 Variable --> `Energy`

## IslotBionic

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### bionic_id
  🇻 Variable --> `BionicDataId`

- #### difficulty
  🇻 Variable --> `int`

- #### installation_data
  🇻 Variable --> `ItypeId`

- #### is_upgrade
  🇻 Variable --> `bool`

## IslotBook

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### chapters
  🇻 Variable --> `int`
  > Fun books have chapters; after all are read, the book is less fun.

- #### fun
  🇻 Variable --> `int`
  > How fun reading this is, can be negative

- #### intelligence
  🇻 Variable --> `int`
  > Intelligence required to read it

- #### martial_art
  🇻 Variable --> `MartialArtsId`
  > Which martial art it teaches. Can be MartialArtsId.NULL_ID

- #### recipes
  🇻 Variable --> `Set( BookRecipe )`
  > Recipes contained in this book

- #### skill
  🇻 Variable --> `SkillId`
  > Which skill it upgrades, if any. Can be SkillId.NULL_ID

- #### skill_max
  🇻 Variable --> `int`
  > The skill level the book provides

- #### skill_min
  🇻 Variable --> `int`
  > The skill level required to understand it

- #### time
  🇻 Variable --> `int`
  > How long in minutes it takes to read.\
  > "To read" means getting 1 skill point, not all of them.

## IslotBrewable

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### results
  🇻 Variable --> `Vector( ItypeId )`
  > What are the results of fermenting this item

- #### time
  🇻 Variable --> `TimeDuration`
  > How long for this brew to ferment

## IslotComestible

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### addict_type
  🇻 Variable --> `AddictionType`
  > effects of addiction

- #### addict_value
  🇻 Variable --> `int`
  > addiction potential

- #### comest_type
  🇻 Variable --> `string`
  > comestible subtype - eg. FOOD, DRINK, MED

- #### contamination
  🇻 Variable --> `Map( DiseaseTypeId, int )`
  > List of diseases carried by this comestible and their associated probability

- #### cooks_like
  🇻 Variable --> `ItypeId`
  > Reference to other item that replaces this one as a component in recipe results

- #### default_nutrition
  🇻 Variable --> `<cppval: nutrients >`
  > Nutrition values to use for this type when they aren't calculated from components

- #### def_charges
  🇻 Variable --> `int`
  > Defaults # of charges (drugs, loaf of bread? etc)

- #### fatigue_mod
  🇻 Variable --> `int`
  > fatigue altering effect

- #### freeze_point
  🇻 Variable --> `int`
  > freezing point in degrees Fahrenheit, below this temperature item can freeze

- #### get_default_nutr
  🇲 Method --> `() -> int`

- #### has_calories
  🇲 Method --> `() -> bool`

- #### healthy
  🇻 Variable --> `int`

- #### latent_heat
  🇻 Variable --> `double`

- #### monotony_penalty
  🇻 Variable --> `int`
  > A penalty applied to fun for every time this food has been eaten in the last 48 hours

- #### parasites
  🇻 Variable --> `int`
  > chance (odds) of becoming parasitised when eating (zero if never occurs)

- #### petfood
  🇻 Variable --> `Set( string )`
  > pet food category

- #### quench
  🇻 Variable --> `int`
  > effect on character thirst (may be negative)

- #### radiation
  🇻 Variable --> `int`
  > Amount of radiation you get from this comestible

- #### rot_spawn
  🇻 Variable --> `MonsterGroupId`
  > The monster group that is drawn from when the item rots away

- #### rot_spawn_chance
  🇻 Variable --> `int`
  > Chance the above monster group spawns

- #### smoking_result
  🇻 Variable --> `ItypeId`
  > Reference to item that will be received after smoking current item

- #### specific_heat_liquid
  🇻 Variable --> `double`
  > specific heats in J/(g K) and latent heat in J/g

- #### specific_heat_solid
  🇻 Variable --> `double`

- #### spoils
  🇻 Variable --> `TimeDuration`
  > Time until becomes rotten at standard temperature, or zero if never spoils

- #### stimulant_type
  🇻 Variable --> `int`
  > stimulant effect

- #### tool
  🇻 Variable --> `ItypeId`
  > tool needed to consume (e.g. lighter for cigarettes)

## IslotContainer

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### contains
  🇻 Variable --> `Volume`
  > Inner volume of the container

- #### preserves
  🇻 Variable --> `bool`
  > Contents do not spoil

- #### seals
  🇻 Variable --> `bool`
  > Can be resealed

- #### unseals_into
  🇻 Variable --> `ItypeId`
  > If this is set to anything but "null", changing this container's contents in any way will turn this item into that type

- #### watertight
  🇻 Variable --> `bool`
  > Can hold liquids

## IslotEngine

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### displacement
  🇻 Variable --> `int`
  > For combustion engines, the displacement

## IslotFuel

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### energy
  🇻 Variable --> `double`
  > Energy of the fuel (kilojoules per charge)

- #### explosion_data
  🇻 Variable --> `<cppval: fuel_explosion >`

- #### has_explosion_data
  🇻 Variable --> `bool`

- #### pump_terrain
  🇻 Variable --> `TerIntId`

## IslotGun

### Bases

- `RangedData`

### Constructors

No constructors.

### Members

No members.

## IslotGunmod

### Bases

- `RangedData`

### Constructors

No constructors.

### Members

No members.

## IslotMagazine

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### ammo_type
  🇻 Variable --> `Set( AmmunitionTypeId )`

- #### capacity
  🇻 Variable --> `int`

- #### count
  🇻 Variable --> `int`

- #### default_ammo
  🇻 Variable --> `ItypeId`

- #### linkage
  🇻 Variable --> `Opt( ItypeId )`

- #### protects_contents
  🇻 Variable --> `bool`

- #### reliability
  🇻 Variable --> `int`

- #### reload_time
  🇻 Variable --> `int`

## IslotMilling

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### conversion_rate
  🇻 Variable --> `int`

- #### converts_into
  🇻 Variable --> `ItypeId`

## IslotMod

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### acceptable_ammo
  🇻 Variable --> `Set( AmmunitionTypeId )`
  > If non-empty restrict mod to items with those base (before modifiers) ammo types

- #### ammo_modifier
  🇻 Variable --> `Set( AmmunitionTypeId )`
  > If set modifies parent ammo to this type

- #### capacity_multiplier
  🇻 Variable --> `double`
  > Proportional adjustment of parent item ammo capacity

- #### magazine_adaptor
  🇻 Variable --> `Map( AmmunitionTypeId, Set( ItypeId ) )`
  > If non-empty replaces the compatible magazines for the parent item

## IslotPetArmor

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### bodytype
  🇻 Variable --> `string`
  > What animal bodytype can wear this armor

- #### env_resist
  🇻 Variable --> `int`
  > Resistance to environmental effects

- #### env_resist_w_filter
  🇻 Variable --> `int`
  > Environmental protection of a gas mask with installed filter

- #### max_vol
  🇻 Variable --> `Volume`
  > The maximum volume a pet can be and wear this armor

- #### min_vol
  🇻 Variable --> `Volume`
  > The minimum volume a pet can be and wear this armor

- #### storage
  🇻 Variable --> `Volume`
  > How much storage this items provides when worn

- #### thickness
  🇻 Variable --> `int`
  > Multiplier on resistances provided by this armor

## IslotSeed

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### byproducts
  🇻 Variable --> `Vector( ItypeId )`

- #### fruit_div
  🇻 Variable --> `int`

- #### fruit_id
  🇻 Variable --> `ItypeId`

- #### get_plant_name
  🇲 Method --> `( int ) -> string`

- #### grow
  🇻 Variable --> `TimeDuration`

## IslotTool

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### ammo_id
  🇻 Variable --> `Set( AmmunitionTypeId )`

- #### charge_factor
  🇻 Variable --> `int`

- #### charges_per_use
  🇻 Variable --> `int`

- #### default_ammo
  🇻 Variable --> `ItypeId`

- #### def_charges
  🇻 Variable --> `int`

- #### max_charges
  🇻 Variable --> `int`

- #### power_draw
  🇻 Variable --> `int`

- #### rand_charges
  🇻 Variable --> `Vector( int )`

- #### revert_msg
  🇻 Variable --> `string`

- #### revert_to
  🇻 Variable --> `Opt( ItypeId )`

- #### subtype
  🇻 Variable --> `ItypeId`

- #### turns_active
  🇻 Variable --> `int`

- #### turns_per_charge
  🇻 Variable --> `int`

- #### ups_eff_mult
  🇻 Variable --> `int`

- #### ups_recharge_rate
  🇻 Variable --> `int`

## IslotWheel

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### diameter
  🇻 Variable --> `int`
  > Diameter of wheel in inches

- #### width
  🇻 Variable --> `int`
  > Width of wheel in inches

## Item

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### activate
  🇲 Method --> `()`

- #### add_item_with_id
  🇲 Method --> `( ItypeId, int )`
  > Adds an item(s) to contents

- #### add_technique
  🇲 Method --> `( MartialArtsTechniqueId )`
  > Adds the technique. It isn't treated original, but additional.

- #### ammo_capacity
  🇲 Method --> `( bool ) -> int`
  > Gets the maximum capacity of a magazine

- #### ammo_consume
  🇲 Method --> `( int, Tripoint ) -> int`

- #### ammo_current
  🇲 Method --> `() -> ItypeId`

- #### ammo_data
  🇲 Method --> `() -> ItypeRaw`

- #### ammo_remaining
  🇲 Method --> `() -> int`
  > Get remaining ammo, works with batteries & stuff too

- #### ammo_required
  🇲 Method --> `() -> int`

- #### ammo_set
  🇲 Method --> `( ItypeId, int )`

- #### ammo_unset
  🇲 Method --> `()`

- #### attack_cost
  🇲 Method --> `() -> int`

- #### can_contain
  🇲 Method --> `( Item ) -> bool`
  > Checks if this item can contain another

- #### charges
  🇻 Variable --> `int`

- #### clear_vars
  🇲 Method --> `()`
  > Erase all variables

- #### conductive
  🇲 Method --> `() -> bool`

- #### convert
  🇲 Method --> `( ItypeId )`
  > Converts the item as given `ItypeId`.

- #### covers
  🇲 Method --> `( BodyPartTypeIntId ) -> bool`
  > Checks if the item covers a bodypart

- #### current_magazine
  🇲 Method --> `() -> Item`
  > Gets the current magazine

- #### deactivate
  🇲 Method --> `()`

- #### display_name
  🇲 Method --> `( int ) -> string`
  > Display name with all bells and whistles like ammo and prefixes

- #### energy_remaining
  🇲 Method --> `() -> Energy`

- #### erase_var
  🇲 Method --> `( string )`
  > Erase variable

- #### get_category_id
  🇲 Method --> `() -> string`
  > Gets the category id this item is in

- #### get_comestible_fun
  🇲 Method --> `() -> int`

- #### get_kcal
  🇲 Method --> `() -> int`

- #### get_mtype
  🇲 Method --> `() -> MtypeId`
  > Almost for a corpse.

- #### get_owner
  🇲 Method --> `() -> FactionId`
  > Gets the faction id that owns this item

- #### get_owner_name
  🇲 Method --> `() -> string`

- #### get_quench
  🇲 Method --> `() -> int`

- #### get_reload_time
  🇲 Method --> `() -> int`

- #### get_rot
  🇲 Method --> `() -> TimeDuration`
  > Gets the TimeDuration until this item rots

- #### get_techniques
  🇲 Method --> `() -> Set( MartialArtsTechniqueId )`
  > Gets all techniques. Including original techniques.

- #### get_type
  🇲 Method --> `() -> ItypeId`

- #### get_var_num
  🇲 Method --> `( string, double ) -> double`
  > Get variable as float number

- #### get_var_str
  🇲 Method --> `( string, string ) -> string`
  > Get variable as string

- #### get_var_tri
  🇲 Method --> `( string, Tripoint ) -> Tripoint`
  > Get variable as tripoint

- #### has_flag
  🇲 Method --> `( JsonFlagId ) -> bool`

- #### has_infinite_charges
  🇲 Method --> `() -> bool`

- #### has_item_with_id
  🇲 Method --> `( ItypeId ) -> bool`
  > Checks item contents for a given item id

- #### has_own_flag
  🇲 Method --> `( JsonFlagId ) -> bool`

- #### has_technique
  🇲 Method --> `( MartialArtsTechniqueId ) -> bool`
  > Checks if this item has the technique as an addition. Doesn't check original techniques.

- #### has_var
  🇲 Method --> `( string ) -> bool`
  > Check for variable of any type

- #### is_active
  🇲 Method --> `() -> bool`

- #### is_ammo
  🇲 Method --> `() -> bool`

- #### is_ammo_belt
  🇲 Method --> `() -> bool`

- #### is_ammo_container
  🇲 Method --> `() -> bool`

- #### is_armor
  🇲 Method --> `() -> bool`

- #### is_artifact
  🇲 Method --> `() -> bool`

- #### is_bandolier
  🇲 Method --> `() -> bool`

- #### is_battery
  🇲 Method --> `() -> bool`
  > DEPRECATED: Is this a battery? (spoiler: it isn't)

- #### is_bionic
  🇲 Method --> `() -> bool`

- #### is_book
  🇲 Method --> `() -> bool`

- #### is_brewable
  🇲 Method --> `() -> bool`

- #### is_bucket
  🇲 Method --> `() -> bool`

- #### is_bucket_nonempty
  🇲 Method --> `() -> bool`

- #### is_comestible
  🇲 Method --> `() -> bool`

- #### is_container
  🇲 Method --> `() -> bool`

- #### is_container_empty
  🇲 Method --> `() -> bool`

- #### is_corpse
  🇲 Method --> `() -> bool`

- #### is_craft
  🇲 Method --> `() -> bool`

- #### is_dangerous
  🇲 Method --> `() -> bool`

- #### is_deployable
  🇲 Method --> `() -> bool`

- #### is_emissive
  🇲 Method --> `() -> bool`

- #### is_engine
  🇲 Method --> `() -> bool`

- #### is_faulty
  🇲 Method --> `() -> bool`

- #### is_filthy
  🇫 Function --> `() -> bool`
  > DEPRECATED: Items are no longer filthy

- #### is_firearm
  🇲 Method --> `() -> bool`

- #### is_food
  🇲 Method --> `() -> bool`

- #### is_food_container
  🇲 Method --> `() -> bool`

- #### is_fuel
  🇲 Method --> `() -> bool`

- #### is_gun
  🇲 Method --> `() -> bool`

- #### is_gunmod
  🇲 Method --> `() -> bool`

- #### is_holster
  🇲 Method --> `() -> bool`

- #### is_irremovable
  🇲 Method --> `() -> bool`

- #### is_made_of
  🇲 Method --> `( MaterialTypeId ) -> bool`

- #### is_magazine
  🇲 Method --> `() -> bool`
  > Is this a magazine? (batteries are magazines)

- #### is_map
  🇲 Method --> `() -> bool`

- #### is_med_container
  🇲 Method --> `() -> bool`

- #### is_medication
  🇲 Method --> `() -> bool`

- #### is_melee
  🇲 Method --> `( DamageType ) -> bool`
  > Is this item an effective melee weapon for the given damage type?

- #### is_money
  🇲 Method --> `() -> bool`

- #### is_non_resealable_container
  🇲 Method --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_owned_by
  🇲 Method --> `( Character, bool ) -> bool`
  > Checks if this item owned by a character

- #### is_power_armor
  🇲 Method --> `() -> bool`

- #### is_relic
  🇲 Method --> `() -> bool`

- #### is_reloadable
  🇲 Method --> `() -> bool`

- #### is_salvageable
  🇲 Method --> `( bool ) -> bool`

- #### is_seed
  🇲 Method --> `() -> bool`

- #### is_sided
  🇲 Method --> `() -> bool`

- #### is_silent
  🇲 Method --> `() -> bool`

- #### is_soft
  🇲 Method --> `() -> bool`

- #### is_stackable
  🇲 Method --> `() -> bool`

- #### is_tainted
  🇲 Method --> `() -> bool`

- #### is_tool
  🇲 Method --> `() -> bool`

- #### is_toolmod
  🇲 Method --> `() -> bool`

- #### is_transformable
  🇲 Method --> `() -> bool`

- #### is_unarmed_weapon
  🇲 Method --> `() -> bool`

- #### is_upgrade
  🇲 Method --> `() -> bool`

- #### is_watertight_container
  🇲 Method --> `() -> bool`

- #### is_wheel
  🇲 Method --> `() -> bool`

- #### made_of
  🇲 Method --> `() -> Vector( MaterialTypeId )`

- #### mod_charges
  🇲 Method --> `( int )`

- #### price
  🇲 Method --> `( bool ) -> double`
  > Cents of the item. `bool` is whether it is a post-cataclysm value.

- #### remaining_capacity_for_id
  🇲 Method --> `( ItypeId, bool ) -> int`
  > Gets the remaining space available for a type of liquid

- #### remove_technique
  🇲 Method --> `( MartialArtsTechniqueId )`
  > Removes the additional technique. Doesn't affect originial techniques.

- #### set_charges
  🇲 Method --> `( int )`

- #### set_countdown
  🇲 Method --> `( int )`

- #### set_flag
  🇲 Method --> `( JsonFlagId )`

- #### set_flag_recursive
  🇲 Method --> `( JsonFlagId )`

- #### set_owner
  🇲 Method --> `( FactionId )`
  > Sets the ownership of this item to a faction

- #### set_owner
  🇲 Method --> `( Character )`
  > Sets the ownership of this item to a character

- #### set_var_num
  🇲 Method --> `( string, double )`

- #### set_var_str
  🇲 Method --> `( string, string )`

- #### set_var_tri
  🇲 Method --> `( string, Tripoint )`

- #### stamina_cost
  🇲 Method --> `() -> int`

- #### tname
  🇲 Method --> `( int, bool, int ) -> string`
  > Translated item name with prefixes

- #### total_capacity
  🇲 Method --> `() -> Volume`
  > Gets maximum volume this item can hold (liquids, ammo, etc)

- #### unset_flag
  🇲 Method --> `( JsonFlagId )`

- #### unset_flags
  🇲 Method --> `()`

- #### volume
  🇲 Method --> `( Opt( bool ) ) -> Volume`
  > Volume of the item. `bool` is whether it is `integral_volume`.

- #### weight
  🇲 Method --> `( Opt( bool ), Opt( bool ) ) -> Mass`
  > Weight of the item. The first `bool` is whether including contents, second `bool` is whether it is `integral_weight`.

## ItemStack

Iterate over this using pairs()

### Bases

No base classes.

### Constructors

No constructors.

### Members

## ItypeId

### Bases

No base classes.

### Constructors

- #### `ItypeId.new()`
- #### `ItypeId.new( ItypeId )`
- #### `ItypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> ItypeId`

- #### obj
  🇲 Method --> `() -> ItypeRaw`

- #### str
  🇲 Method --> `() -> string`

## ItypeRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### ammo
  🇲 Method --> `() -> IslotAmmo`

- #### armor
  🇲 Method --> `() -> IslotArmor`

- #### artifact
  🇲 Method --> `() -> IslotArtifact`

- #### battery
  🇲 Method --> `() -> IslotBattery`

- #### bionic
  🇲 Method --> `() -> IslotBionic`

- #### book
  🇲 Method --> `() -> IslotBook`

- #### brewable
  🇲 Method --> `() -> IslotBrewable`

- #### comestible
  🇲 Method --> `() -> IslotComestible`

- #### container
  🇲 Method --> `() -> IslotContainer`

- #### engine
  🇲 Method --> `() -> IslotEngine`

- #### fuel
  🇲 Method --> `() -> IslotFuel`

- #### gun
  🇲 Method --> `() -> IslotGun`

- #### gunmod
  🇲 Method --> `() -> IslotGunmod`

- #### magazine
  🇲 Method --> `() -> IslotMagazine`

- #### milling
  🇲 Method --> `() -> IslotMilling`

- #### mod
  🇲 Method --> `() -> IslotMod`

- #### nname
  🇲 Method --> `( int ) -> string`

- #### pet_armor
  🇲 Method --> `() -> IslotPetArmor`

- #### relic
  🇲 Method --> `() -> Relic`

- #### seed
  🇲 Method --> `() -> IslotSeed`

- #### tool
  🇲 Method --> `() -> IslotTool`

- #### wheel
  🇲 Method --> `() -> IslotWheel`

## JsonFlagId

### Bases

No base classes.

### Constructors

- #### `JsonFlagId.new()`
- #### `JsonFlagId.new( JsonFlagId )`
- #### `JsonFlagId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> JsonFlagId`

- #### obj
  🇲 Method --> `() -> JsonFlagRaw`

- #### str
  🇲 Method --> `() -> string`

## JsonTraitFlagId

### Bases

No base classes.

### Constructors

- #### `JsonTraitFlagId.new()`
- #### `JsonTraitFlagId.new( JsonTraitFlagId )`
- #### `JsonTraitFlagId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> JsonTraitFlagId`

- #### obj
  🇲 Method --> `() -> JsonTraitFlagRaw`

- #### str
  🇲 Method --> `() -> string`

## Map

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### add_field_at
  🇲 Method --> `( Tripoint, FieldTypeIntId, int, TimeDuration ) -> bool`

- #### clear_items_at
  🇲 Method --> `( Tripoint )`

- #### create_corpse_at
  🇲 Method --> `( Tripoint, Opt( MtypeId ), Opt( TimePoint ), Opt( string ), Opt( int ) )`
  > Creates a new corpse at a position on the map. You can skip `Opt` ones by omitting them or passing `nil`. `MtypeId` specifies which monster's body it is, `TimePoint` indicates when it died, `string` gives it a custom name, and `int` determines the revival time if the monster has the `REVIVES` flag.

- #### create_item_at
  🇲 Method --> `( Tripoint, ItypeId, int )`
  > Creates a new item(s) at a position on the map.

- #### disarm_trap_at
  🇲 Method --> `( Tripoint )`
  > Disarms a trap using your skills and stats, with consequences depending on success or failure.

- #### get_abs_ms
  🇲 Method --> `( Tripoint ) -> Tripoint`
  > Convert local ms -> absolute ms

- #### get_field_age_at
  🇲 Method --> `( Tripoint, FieldTypeIntId ) -> TimeDuration`

- #### get_field_int_at
  🇲 Method --> `( Tripoint, FieldTypeIntId ) -> int`

- #### get_furn_at
  🇲 Method --> `( Tripoint ) -> FurnIntId`

- #### get_items_at
  🇲 Method --> `( Tripoint ) -> <cppval: std::unique_ptr<map_stack> >`

- #### get_items_at_with
  🇲 Method --> `( Tripoint, <cppval: const std::function<bool (const item &)> & > ) -> Vector( Item )`

- #### get_items_in_radius
  🇲 Method --> `( Tripoint, int ) -> Vector( Item )`

- #### get_items_in_radius_with
  🇲 Method --> `( Tripoint, int, <cppval: const std::function<bool (const item &)> & > ) -> Vector( Item )`

- #### get_local_ms
  🇲 Method --> `( Tripoint ) -> Tripoint`
  > Convert absolute ms -> local ms

- #### get_map_size
  🇲 Method --> `() -> int`
  > In map squares

- #### get_map_size_in_submaps
  🇲 Method --> `() -> int`

- #### get_ter_at
  🇲 Method --> `( Tripoint ) -> TerIntId`

- #### get_trap_at
  🇲 Method --> `( Tripoint ) -> TrapIntId`

- #### has_field_at
  🇲 Method --> `( Tripoint, FieldTypeIntId ) -> bool`

- #### has_items_at
  🇲 Method --> `( Tripoint ) -> bool`

- #### mod_field_age_at
  🇲 Method --> `( Tripoint, FieldTypeIntId, TimeDuration ) -> TimeDuration`

- #### mod_field_int_at
  🇲 Method --> `( Tripoint, FieldTypeIntId, int ) -> int`

- #### remove_field_at
  🇲 Method --> `( Tripoint, FieldTypeIntId )`

- #### remove_item_at
  🇲 Method --> `( Tripoint, Item )`

- #### remove_trap_at
  🇲 Method --> `( Tripoint )`
  > Simpler version of `set_trap_at` with `trap_null`.

- #### set_field_age_at
  🇲 Method --> `( Tripoint, FieldTypeIntId, TimeDuration, bool ) -> TimeDuration`

- #### set_field_int_at
  🇲 Method --> `( Tripoint, FieldTypeIntId, int, bool ) -> int`

- #### set_furn_at
  🇲 Method --> `( Tripoint, FurnIntId )`

- #### set_ter_at
  🇲 Method --> `( Tripoint, TerIntId ) -> bool`

- #### set_trap_at
  🇲 Method --> `( Tripoint, TrapIntId )`
  > Set a trap at a position on the map. It can also replace existing trap, even with `trap_null`.

## MapStack

### Bases

- `ItemStack`

### Constructors

No constructors.

### Members

- #### as_item_stack
  🇲 Method --> `() -> ItemStack`

## MartialArtsBuffId

### Bases

No base classes.

### Constructors

- #### `MartialArtsBuffId.new()`
- #### `MartialArtsBuffId.new( MartialArtsBuffId )`
- #### `MartialArtsBuffId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MartialArtsBuffId`

- #### obj
  🇲 Method --> `() -> MartialArtsBuffRaw`

- #### str
  🇲 Method --> `() -> string`

## MartialArtsId

### Bases

No base classes.

### Constructors

- #### `MartialArtsId.new()`
- #### `MartialArtsId.new( MartialArtsId )`
- #### `MartialArtsId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MartialArtsId`

- #### obj
  🇲 Method --> `() -> MartialArtsRaw`

- #### str
  🇲 Method --> `() -> string`

## MartialArtsTechniqueId

### Bases

No base classes.

### Constructors

- #### `MartialArtsTechniqueId.new()`
- #### `MartialArtsTechniqueId.new( MartialArtsTechniqueId )`
- #### `MartialArtsTechniqueId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MartialArtsTechniqueId`

- #### obj
  🇲 Method --> `() -> MartialArtsTechniqueRaw`

- #### str
  🇲 Method --> `() -> string`

## MartialArtsTechniqueRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### avatar_message
  🇫 Function --> `()`

- #### block_counter
  🇫 Function --> `()`

- #### crit_ok
  🇫 Function --> `()`

- #### crit_tec
  🇫 Function --> `()`

- #### defensive
  🇫 Function --> `()`

- #### disarms
  🇫 Function --> `()`

- #### dodge_counter
  🇫 Function --> `()`

- #### down_dur
  🇫 Function --> `()`

- #### get_description
  🇲 Method --> `() -> string`

- #### grab_break
  🇫 Function --> `()`

- #### knockback_dist
  🇫 Function --> `()`

- #### knockback_follow
  🇫 Function --> `()`

- #### knockback_spread
  🇫 Function --> `()`

- #### miss_recovery
  🇫 Function --> `()`

- #### name
  🇫 Function --> `()`

- #### npc_message
  🇫 Function --> `()`

- #### powerful_knockback
  🇫 Function --> `()`

- #### side_switch
  🇫 Function --> `()`

- #### stun_dur
  🇫 Function --> `()`

- #### take_weapon
  🇫 Function --> `()`

## Mass

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### from_gram
  🇫 Function --> `( int ) -> Mass`

- #### from_kilogram
  🇫 Function --> `( int ) -> Mass`

- #### from_milligram
  🇫 Function --> `( int ) -> Mass`

- #### from_newton
  🇫 Function --> `( int ) -> Mass`

- #### to_gram
  🇲 Method --> `() -> int`

- #### to_kilogram
  🇲 Method --> `() -> int`

- #### to_milligram
  🇲 Method --> `() -> int`

- #### to_newton
  🇲 Method --> `() -> int`

## MaterialTypeId

### Bases

No base classes.

### Constructors

- #### `MaterialTypeId.new()`
- #### `MaterialTypeId.new( MaterialTypeId )`
- #### `MaterialTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MaterialTypeId`

- #### obj
  🇲 Method --> `() -> MaterialTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## MaterialTypeRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### name
  🇲 Method --> `() -> string`

- #### str_id
  🇲 Method --> `() -> MaterialTypeId`

## Mission

### Bases

No base classes.

### Constructors

- #### `Mission.new()`

### Members

- #### assign
  🇲 Method --> `( Avatar )`
  > Assigns this mission to the given avatar.

- #### fail
  🇲 Method --> `()`
  > Fails the mission.

- #### get_deadline
  🇲 Method --> `() -> TimePoint`
  > Returns the mission's deadline as a time_point.

- #### get_description
  🇲 Method --> `() -> string`
  > Returns the mission description.

- #### get_follow_up
  🇲 Method --> `() -> MissionTypeIdRaw`
  > Returns the follow-up mission type ID.

- #### get_id
  🇲 Method --> `() -> int`
  > Returns the mission's unique ID.

- #### get_item_id
  🇲 Method --> `() -> ItypeId`
  > Returns the item ID associated with the mission.

- #### get_likely_rewards
  🇲 Method --> `() -> <cppval: const std::vector<std::pair<int, string_id<itype>>> & >`
  > Returns the likely rewards of the mission (vector of (int chance, itype_id) pairs).

- #### get_npc_id
  🇲 Method --> `() -> CharacterId`
  > Returns the NPC character ID associated with the mission.

- #### get_target_point
  🇲 Method --> `() -> Tripoint`
  > Returns the target of the mission (pointer to tripoint_abs_omt).

- #### get_type
  🇲 Method --> `() -> MissionType`
  > Returns the mission type of the target (pointer to mission_type).

- #### get_value
  🇲 Method --> `() -> int`
  > Returns the mission's value as an integer.

- #### has_deadline
  🇲 Method --> `() -> bool`
  > Returns true if the mission has a deadline.

- #### has_failed
  🇲 Method --> `() -> bool`
  > Returns true if the mission has failed.

- #### has_follow_up
  🇲 Method --> `() -> bool`
  > Returns true if the mission has a follow-up mission.

- #### has_generic_rewards
  🇲 Method --> `() -> bool`
  > Returns true if the mission has generic rewards.

- #### has_target
  🇲 Method --> `() -> bool`
  > Returns true if the mission has a target.

- #### in_progress
  🇲 Method --> `() -> bool`
  > Returns true if the mission is currently in progress.

- #### is_assigned
  🇲 Method --> `() -> bool`
  > Returns true if the mission is currently assigned.

- #### mission_id
  🇲 Method --> `() -> MissionTypeIdRaw`
  > Returns the mission type ID of this mission.

- #### name
  🇲 Method --> `() -> string`
  > Returns the mission's name as a string.

- #### reserve_new
  🇫 Function --> `( MissionTypeIdRaw, CharacterId ) -> Mission`
  > Reserves a new mission of the given type for the specified NPC. Returns the new mission.

- #### reserve_random
  🇫 Function --> `( MissionOrigin, Tripoint, CharacterId ) -> Mission`
  > Reserves a random mission at the specified origin and position for the given NPC. Returns the new mission.

- #### step_complete
  🇲 Method --> `( int )`
  > Marks a mission step as complete, taking an integer step index.

- #### wrap_up
  🇲 Method --> `()`
  > Wraps up the mission successfully.

## MissionType

### Bases

No base classes.

### Constructors

- #### `MissionType.new()`

### Members

- #### deadline_high
  🇻 Variable --> `TimeDuration`
  > Returns the maximum allowed deadline for the mission.

- #### deadline_low
  🇻 Variable --> `TimeDuration`
  > Returns the minimum allowed deadline for the mission.

- #### description
  🇻 Variable --> `<cppval: translation >`
  > Returns the mission's description as a string.

- #### dialogue
  🇻 Variable --> `Map( string, <cppval: translation > )`
  > Returns any associated dialogue for the mission.

- #### difficulty
  🇻 Variable --> `int`
  > Returns the mission's difficulty as an integer.

- #### empty_container
  🇻 Variable --> `ItypeId`
  > Returns true if the mission requires the container to be empty.

- #### follow_up
  🇻 Variable --> `MissionTypeIdRaw`
  > Returns any follow-up mission type ID.

- #### get_all
  🇫 Function --> `() -> <cppval: const std::vector<mission_type> & >`
  > Returns all available missions.

- #### get_random_mission_id
  🇫 Function --> `( MissionOrigin, Tripoint ) -> MissionTypeIdRaw`
  > Returns a random mission type ID at the specified origin and overmap tile position.

- #### goal
  🇻 Variable --> `MissionGoal`
  > Returns the mission's goal text.

- #### has_generic_rewards
  🇻 Variable --> `bool`
  > Returns true if the mission has generic rewards.

- #### item_count
  🇻 Variable --> `int`
  > Returns the count of items involved in the mission.

- #### item_id
  🇻 Variable --> `ItypeId`
  > Returns the ID of the mission's main item target, if applicable.

- #### likely_rewards
  🇻 Variable --> `Vector( <cppval: std::pair<int, string_id<itype>> > )`
  > Returns a vector of likely rewards (chance, itype_id pairs).

- #### monster_kill_goal
  🇻 Variable --> `int`
  > Returns the number of monsters required to kill for this mission.

- #### monster_type
  🇻 Variable --> `MtypeId`
  > Returns the monster type associated with the mission, if any.

- #### origins
  🇻 Variable --> `Vector( MissionOrigin )`
  > Returns a list of origins from which this mission can be generated.

- #### remove_container
  🇻 Variable --> `bool`
  > Returns true if the mission requires removing a container.

- #### target_npc_id
  🇻 Variable --> `CharacterId`
  > Returns the ID of the target NPC for the mission, if any.

- #### tname
  🇲 Method --> `() -> string`

- #### urgent
  🇻 Variable --> `bool`
  > Returns true if the mission is marked as urgent.

- #### value
  🇻 Variable --> `int`
  > Returns the mission's reward value as an integer.

## MissionTypeIdRaw

### Bases

No base classes.

### Constructors

- #### `MissionTypeIdRaw.new( string )`

### Members

No members.

## Monster

### Bases

- `Creature`

### Constructors

No constructors.

### Members

- #### anger
  🇻 Variable --> `int`

- #### attitude
  🇲 Method --> `( Character ) -> MonsterAttitude`

- #### can_climb
  🇲 Method --> `() -> bool`

- #### can_dig
  🇲 Method --> `() -> bool`

- #### can_drown
  🇲 Method --> `() -> bool`

- #### can_hear
  🇲 Method --> `() -> bool`

- #### can_see
  🇲 Method --> `() -> bool`

- #### can_submerge
  🇲 Method --> `() -> bool`

- #### can_upgrade
  🇲 Method --> `() -> bool`

- #### climbs
  🇲 Method --> `() -> bool`

- #### death_drops
  🇻 Variable --> `bool`

- #### digs
  🇲 Method --> `() -> bool`

- #### faction
  🇻 Variable --> `MonsterFactionIntId`

- #### flies
  🇲 Method --> `() -> bool`

- #### friendly
  🇻 Variable --> `int`

- #### get_type
  🇲 Method --> `() -> MtypeId`

- #### get_upgrade_time
  🇲 Method --> `() -> int`

- #### hasten_upgrade
  🇲 Method --> `()`

- #### heal
  🇲 Method --> `( int, bool ) -> int`

- #### is_wandering
  🇲 Method --> `() -> bool`

- #### make_ally
  🇲 Method --> `( Monster )`

- #### make_friendly
  🇲 Method --> `()`

- #### make_fungus
  🇲 Method --> `() -> bool`

- #### morale
  🇻 Variable --> `int`

- #### move_target
  🇲 Method --> `() -> Tripoint`

- #### move_to
  🇲 Method --> `( Tripoint, bool, bool, double ) -> bool`

- #### name
  🇲 Method --> `( int ) -> string`

- #### name_with_armor
  🇲 Method --> `() -> string`

- #### refill_udders
  🇲 Method --> `()`

- #### set_hp
  🇲 Method --> `( int )`

- #### spawn
  🇲 Method --> `( Tripoint )`

- #### swims
  🇲 Method --> `() -> bool`

- #### try_reproduce
  🇲 Method --> `()`

- #### try_upgrade
  🇲 Method --> `( bool )`

- #### unique_name
  🇻 Variable --> `string`

- #### wander_to
  🇲 Method --> `( Tripoint, int )`

## MonsterFactionId

### Bases

No base classes.

### Constructors

- #### `MonsterFactionId.new()`
- #### `MonsterFactionId.new( MonsterFactionId )`
- #### `MonsterFactionId.new( MonsterFactionIntId )`
- #### `MonsterFactionId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### int_id
  🇲 Method --> `() -> MonsterFactionIntId`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MonsterFactionId`

- #### obj
  🇲 Method --> `() -> MonsterFactionRaw`

- #### str
  🇲 Method --> `() -> string`

## MonsterFactionIntId

### Bases

No base classes.

### Constructors

- #### `MonsterFactionIntId.new()`
- #### `MonsterFactionIntId.new( MonsterFactionIntId )`
- #### `MonsterFactionIntId.new( MonsterFactionId )`

### Members

- #### is_valid
  🇲 Method --> `() -> bool`

- #### obj
  🇲 Method --> `() -> MonsterFactionRaw`

- #### str_id
  🇲 Method --> `() -> MonsterFactionId`

## MoraleTypeDataId

### Bases

No base classes.

### Constructors

- #### `MoraleTypeDataId.new()`
- #### `MoraleTypeDataId.new( MoraleTypeDataId )`
- #### `MoraleTypeDataId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MoraleTypeDataId`

- #### obj
  🇲 Method --> `() -> MoraleTypeDataRaw`

- #### str
  🇲 Method --> `() -> string`

## MtypeId

### Bases

No base classes.

### Constructors

- #### `MtypeId.new()`
- #### `MtypeId.new( MtypeId )`
- #### `MtypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MtypeId`

- #### obj
  🇲 Method --> `() -> MtypeRaw`

- #### str
  🇲 Method --> `() -> string`

## MutationBranchId

### Bases

No base classes.

### Constructors

- #### `MutationBranchId.new()`
- #### `MutationBranchId.new( MutationBranchId )`
- #### `MutationBranchId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MutationBranchId`

- #### obj
  🇲 Method --> `() -> MutationBranchRaw`

- #### str
  🇲 Method --> `() -> string`

## MutationBranchRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### activated
  🇻 Variable --> `bool`
  > Whether this mutation can be activated at will.

- #### addition_mutations
  🇲 Method --> `() -> Vector( MutationBranchId )`

- #### allow_soft_gear
  🇻 Variable --> `bool`
  > Mutation allows soft gear to be worn over otherwise-restricted parts.

- #### attackcost_modifier
  🇻 Variable --> `double`

- #### bleed_resist
  🇻 Variable --> `double`

- #### bodytemp_max_btu
  🇻 Variable --> `int`

- #### bodytemp_min_btu
  🇻 Variable --> `int`

- #### bodytemp_sleep_btu
  🇻 Variable --> `int`

- #### categories
  🇲 Method --> `() -> Vector( MutationCategoryTraitId )`
  > Lists the categories this mutation belongs to.

- #### conflicts_with
  🇲 Method --> `() -> Vector( MutationBranchId )`
  > Lists conflicting mutations.

- #### construction_speed_modifier
  🇻 Variable --> `double`
  > Construction speed multiplier. 2.0 doubles construction speed; 0.5 halves it.

- #### cooldown
  🇻 Variable --> `int`
  > Costs are incurred every 'cooldown' turns.

- #### cost
  🇻 Variable --> `int`

- #### crafting_speed_modifier
  🇻 Variable --> `double`
  > Crafting speed multiplier. 2.0 doubles crafting speed; 0.5 halves it.

- #### debug
  🇻 Variable --> `bool`
  > Whether or not this mutation is limited to debug use.

- #### desc
  🇲 Method --> `() -> string`

- #### dodge_modifier
  🇻 Variable --> `double`

- #### falling_damage_multiplier
  🇻 Variable --> `double`

- #### fatigue
  🇻 Variable --> `bool`
  > Mutation causes fatigue when used.

- #### fatigue_modifier
  🇻 Variable --> `double`

- #### fatigue_regen_modifier
  🇻 Variable --> `double`

- #### get_all
  🇫 Function --> `() -> <cppval: const std::vector<mutation_branch> & >`
  > Returns a (long) list of every mutation in the game.

- #### healing_awake
  🇻 Variable --> `double`
  > Healing per turn from mutation.

- #### healing_resting
  🇻 Variable --> `double`
  > Healing per turn from mutation, while asleep.

- #### healthy_rate
  🇻 Variable --> `double`
  > How quickly health (not HP) trends toward healthy_mod.

- #### hearing_modifier
  🇻 Variable --> `double`

- #### hp_adjustment
  🇻 Variable --> `double`
  > Flat adjustment to HP.

- #### hp_modifier
  🇻 Variable --> `double`
  > Bonus HP multiplier. 1.0 doubles HP; -0.5 halves it.

- #### hp_modifier_secondary
  🇻 Variable --> `double`
  > Secondary HP multiplier; stacks with the other one. 1.0 doubles HP; -0.5 halves it.

- #### hunger
  🇻 Variable --> `bool`
  > Mutation deducts calories when used.

- #### id
  🇻 Variable --> `MutationBranchId`

- #### max_stamina_modifier
  🇻 Variable --> `double`

- #### mending_modifier
  🇻 Variable --> `double`
  > Multiplier applied to broken limb regeneration. Normally 0.25; clamped to 0.25..1.0.

- #### metabolism_modifier
  🇻 Variable --> `double`

- #### mixed_effect
  🇻 Variable --> `bool`
  > Whether this mutation has positive /and/ negative effects.

- #### movecost_flatground_modifier
  🇻 Variable --> `double`

- #### movecost_modifier
  🇻 Variable --> `double`

- #### movecost_obstacle_modifier
  🇻 Variable --> `double`

- #### movecost_swim_modifier
  🇻 Variable --> `double`

- #### mutation_types
  🇲 Method --> `() -> Set( string )`
  > Lists the type(s) of this mutation. Mutations of a given type are mutually exclusive.

- #### name
  🇲 Method --> `() -> string`

- #### night_vision_range
  🇻 Variable --> `double`

- #### noise_modifier
  🇻 Variable --> `double`

- #### other_prerequisites
  🇲 Method --> `() -> Vector( MutationBranchId )`
  > Lists the secondary mutation(s) needed to gain this mutation.

- #### overmap_multiplier
  🇻 Variable --> `double`

- #### overmap_sight
  🇻 Variable --> `double`

- #### packmule_modifier
  🇻 Variable --> `double`
  > Packmule multiplier. 2.0 doubles backpack/container volume; 0.5 halves it.

- #### pain_recovery
  🇻 Variable --> `double`
  > Pain recovery per turn from mutation.

- #### player_display
  🇻 Variable --> `bool`
  > Whether or not this mutation shows up in the status (`@`) menu.

- #### points
  🇻 Variable --> `int`
  > Point cost in character creation(?).

- #### prerequisites
  🇲 Method --> `() -> Vector( MutationBranchId )`
  > Lists the primary mutation(s) needed to gain this mutation.

- #### profession
  🇻 Variable --> `bool`
  > Whether this trait is ONLY gained through professional training/experience (and/or quests).

- #### purifiable
  🇻 Variable --> `bool`
  > Whether this mutation is possible to remove through Purifier. False for 'special' mutations.

- #### reading_speed_multiplier
  🇻 Variable --> `double`

- #### replaced_by
  🇲 Method --> `() -> Vector( MutationBranchId )`
  > Lists mutations that replace (e.g. evolve from) this one.

- #### scent_modifier
  🇻 Variable --> `double`

- #### skill_rust_multiplier
  🇻 Variable --> `double`

- #### speed_modifier
  🇻 Variable --> `double`

- #### stamina_regen_modifier
  🇻 Variable --> `double`

- #### starting_trait
  🇻 Variable --> `bool`
  > Whether this trait can normally be taken during character generation.

- #### starts_active
  🇻 Variable --> `bool`
  > Whether a mutation activates when granted.

- #### stealth_modifier
  🇻 Variable --> `double`

- #### str_modifier
  🇻 Variable --> `double`
  > Adjustment to Strength that doesn't affect HP.

- #### temperature_speed_modifier
  🇻 Variable --> `double`

- #### thirst
  🇻 Variable --> `bool`
  > Mutation dehydrates when used.

- #### thirst_modifier
  🇻 Variable --> `double`

- #### threshold
  🇻 Variable --> `bool`
  > Whether this is a Threshold mutation, and thus especially difficult to mutate. One per character.

- #### thresh_requirements
  🇲 Method --> `() -> Vector( MutationBranchId )`
  > Lists the threshold mutation(s) required to gain this mutation.

- #### ugliness
  🇻 Variable --> `int`
  > How physically unappealing the mutation is. Can be negative.

- #### valid
  🇻 Variable --> `bool`
  > Whether this mutation is available through generic mutagen.

- #### visibility
  🇻 Variable --> `int`
  > How visible the mutation is to others.

- #### weight_capacity_modifier
  🇻 Variable --> `double`

## MutationCategoryTraitId

### Bases

No base classes.

### Constructors

- #### `MutationCategoryTraitId.new()`
- #### `MutationCategoryTraitId.new( MutationCategoryTraitId )`
- #### `MutationCategoryTraitId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> MutationCategoryTraitId`

- #### obj
  🇲 Method --> `() -> MutationCategoryTraitRaw`

- #### str
  🇲 Method --> `() -> string`

## Npc

### Bases

- `Player`
- `Character`
- `Creature`

### Constructors

No constructors.

### Members

- #### can_move_to
  🇲 Method --> `( Tripoint, bool ) -> bool`

- #### can_open_door
  🇲 Method --> `( Tripoint, bool ) -> bool`

- #### complain
  🇲 Method --> `() -> bool`

- #### complain_about
  🇲 Method --> `( string, TimeDuration, string, Opt( bool ) ) -> bool`

- #### current_activity_id
  🇻 Variable --> `ActivityTypeId`

- #### current_ally
  🇲 Method --> `() -> Creature`

- #### current_target
  🇲 Method --> `() -> Creature`

- #### danger_assessment
  🇲 Method --> `() -> double`

- #### evaluate_enemy
  🇲 Method --> `( Creature ) -> double`

- #### follow_distance
  🇲 Method --> `() -> int`

- #### get_attitude
  🇲 Method --> `() -> NpcAttitude`

- #### get_monster_faction
  🇲 Method --> `() -> MonsterFactionIntId`

- #### guaranteed_hostile
  🇲 Method --> `() -> bool`

- #### has_activity
  🇲 Method --> `() -> bool`

- #### has_omt_destination
  🇲 Method --> `() -> bool`

- #### has_player_activity
  🇲 Method --> `() -> bool`

- #### hit_by_player
  🇻 Variable --> `bool`

- #### hostile_anger_level
  🇲 Method --> `() -> int`

- #### is_ally
  🇲 Method --> `( Character ) -> bool`

- #### is_enemy
  🇲 Method --> `() -> bool`

- #### is_following
  🇲 Method --> `() -> bool`

- #### is_friendly
  🇲 Method --> `( Character ) -> bool`

- #### is_guarding
  🇲 Method --> `() -> bool`

- #### is_leader
  🇲 Method --> `() -> bool`

- #### is_minion
  🇲 Method --> `() -> bool`

- #### is_obeying
  🇲 Method --> `( Character ) -> bool`

- #### is_patrolling
  🇲 Method --> `() -> bool`

- #### is_player_ally
  🇲 Method --> `() -> bool`

- #### is_stationary
  🇲 Method --> `( bool ) -> bool`

- #### is_travelling
  🇲 Method --> `() -> bool`

- #### is_walking_with
  🇲 Method --> `() -> bool`

- #### make_angry
  🇲 Method --> `()`

- #### marked_for_death
  🇻 Variable --> `bool`

- #### mutiny
  🇲 Method --> `()`

- #### needs
  🇻 Variable --> `Vector( NpcNeed )`

- #### op_of_u
  🇻 Variable --> `NpcOpinion`

- #### patience
  🇻 Variable --> `int`

- #### personality
  🇻 Variable --> `NpcPersonality`

- #### saw_player_recently
  🇲 Method --> `() -> bool`

- #### say
  🇲 Method --> `( string )`

- #### set_attitude
  🇲 Method --> `( NpcAttitude )`

- #### set_faction_id
  🇲 Method --> `( FactionId )`

- #### smash_ability
  🇲 Method --> `() -> int`

- #### turned_hostile
  🇲 Method --> `() -> bool`

- #### warn_about
  🇲 Method --> `( string, TimeDuration, string, int, Tripoint )`

## NpcOpinion

### Bases

No base classes.

### Constructors

- #### `NpcOpinion.new()`
- #### `NpcOpinion.new( int, int, int, int, int )`

### Members

- #### anger
  🇻 Variable --> `int`

- #### fear
  🇻 Variable --> `int`

- #### owed
  🇻 Variable --> `int`

- #### trust
  🇻 Variable --> `int`

- #### value
  🇻 Variable --> `int`

## NpcPersonality

### Bases

No base classes.

### Constructors

- #### `NpcPersonality.new()`

### Members

- #### aggression
  🇻 Variable --> `char`

- #### altruism
  🇻 Variable --> `char`

- #### bravery
  🇻 Variable --> `char`

- #### collector
  🇻 Variable --> `char`

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

- #### `Point.new()`
- #### `Point.new( Point )`
- #### `Point.new( int, int )`

### Members

- #### abs
  🇲 Method --> `() -> Point`

- #### rotate
  🇲 Method --> `( int, Point ) -> Point`

- #### x
  🇻 Variable --> `int`

- #### y
  🇻 Variable --> `int`

## PopupInputStr

### Bases

No base classes.

### Constructors

- #### `PopupInputStr.new()`

### Members

- #### desc
  🇲 Method --> `( string )`
  > `desc` is above input field.

- #### query_int
  🇲 Method --> `() -> int`
  > Returns your input, but allows numbers only.

- #### query_str
  🇲 Method --> `() -> string`
  > Returns your input.

- #### title
  🇲 Method --> `( string )`
  > `title` is on the left of input field.

## QueryPopup

### Bases

No base classes.

### Constructors

- #### `QueryPopup.new()`

### Members

- #### allow_any_key
  🇲 Method --> `( bool )`
  > Set whether to allow any key

- #### message
  🇲 Method --> `( ... )`

- #### message_color
  🇲 Method --> `( Color )`

- #### query
  🇲 Method --> `() -> string`
  > Returns selected action

- #### query_yn
  🇲 Method --> `() -> string`
  > Returns `YES` or `NO`. If ESC pressed, returns `NO`.

- #### query_ynq
  🇲 Method --> `() -> string`
  > Returns `YES`, `NO` or `QUIT`. If ESC pressed, returns `QUIT`.

## RangedData

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### aimed_crit_bonus
  🇻 Variable --> `double`

- #### aimed_crit_max_bonus
  🇻 Variable --> `double`

- #### damage
  🇻 Variable --> `DamageInstance`

- #### dispersion
  🇻 Variable --> `int`

- #### range
  🇻 Variable --> `int`

- #### speed
  🇻 Variable --> `int`

## RecipeId

### Bases

No base classes.

### Constructors

- #### `RecipeId.new()`
- #### `RecipeId.new( RecipeId )`
- #### `RecipeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> RecipeId`

- #### obj
  🇲 Method --> `() -> RecipeRaw`

- #### str
  🇲 Method --> `() -> string`

## RecipeRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### booksets
  🇻 Variable --> `Map( ItypeId, int )`

- #### category
  🇻 Variable --> `string`

- #### difficulty
  🇻 Variable --> `int`

- #### get_all
  🇫 Function --> `() -> Vector( RecipeRaw )`

- #### get_from_flag
  🇫 Function --> `( string ) -> Vector( RecipeRaw )`

- #### get_from_skill_used
  🇫 Function --> `( SkillId ) -> Vector( RecipeRaw )`

- #### has_flag
  🇲 Method --> `( string ) -> bool`

- #### ident
  🇲 Method --> `() -> RecipeId`

- #### learn_by_disassembly
  🇻 Variable --> `Map( SkillId, int )`

- #### required_skills
  🇻 Variable --> `Map( SkillId, int )`

- #### result
  🇲 Method --> `() -> ItypeId`

- #### result_name
  🇲 Method --> `() -> string`

- #### skill_used
  🇻 Variable --> `SkillId`

- #### subcategory
  🇻 Variable --> `string`

- #### time
  🇻 Variable --> `int`

## Relic

### Bases

No base classes.

### Constructors

No constructors.

### Members

No members.

## SkillId

### Bases

No base classes.

### Constructors

- #### `SkillId.new()`
- #### `SkillId.new( SkillId )`
- #### `SkillId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> SkillId`

- #### obj
  🇲 Method --> `() -> SkillRaw`

- #### str
  🇲 Method --> `() -> string`

## SkillLevel

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### can_train
  🇲 Method --> `() -> bool`

- #### highest_level
  🇲 Method --> `() -> int`

- #### is_training
  🇲 Method --> `() -> bool`

- #### level
  🇲 Method --> `() -> int`

- #### train
  🇲 Method --> `( int, bool )`

## SkillLevelMap

### Bases

- `Map( SkillId, SkillLevel )`

### Constructors

No constructors.

### Members

- #### get_skill_level
  🇲 Method --> `( SkillId ) -> int`

- #### get_skill_level_object
  🇲 Method --> `( SkillId ) -> SkillLevel`

- #### mod_skill_level
  🇲 Method --> `( SkillId, int )`

## SpeciesTypeId

### Bases

No base classes.

### Constructors

- #### `SpeciesTypeId.new()`
- #### `SpeciesTypeId.new( SpeciesTypeId )`
- #### `SpeciesTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> SpeciesTypeId`

- #### obj
  🇲 Method --> `() -> SpeciesTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## Spell

The class used for spells that _a player_ knows, casts, and gains experience for using. If a given spell is not supposed to be directly cast by a player, consider using SpellSimple instead.

### Bases

No base classes.

### Constructors

- #### `Spell.new( SpellTypeId, int )`

### Members

- #### cast
  🇲 Method --> `( Creature, Tripoint )`
  > Cast this spell, as well as any sub-spells.

- #### cast_single_effect
  🇲 Method --> `( Creature, Tripoint )`
  > Cast _only_ this spell's main effects. Generally, cast() should be used instead.

- #### desc
  🇲 Method --> `() -> string`

- #### gain_exp
  🇲 Method --> `( int )`

- #### gain_levels
  🇲 Method --> `( int )`

- #### get_level
  🇲 Method --> `() -> int`

- #### id
  🇻 Variable --> `SpellTypeId`

- #### name
  🇲 Method --> `() -> string`

- #### set_exp
  🇲 Method --> `( int )`

- #### set_level
  🇲 Method --> `( int )`

- #### xp
  🇲 Method --> `() -> int`

## SpellSimple

The type for basic spells. If you don't need to track XP from casting (e.g., if a spell is intended to be cast by anything _other than_ a player), this is likely the appropriate type. Otherwise, see the Spell type.

### Bases

No base classes.

### Constructors

- #### `SpellSimple.new( SpellTypeId, bool )`
- #### `SpellSimple.new( SpellTypeId, bool, int )`

### Members

- #### cast
  🇲 Method --> `( Creature, Tripoint, Opt( int ) )`

- #### force_target_source
  🇻 Variable --> `bool`
  > Whether or not the target point is _locked_ to the source's location.

- #### id
  🇻 Variable --> `SpellTypeId`

- #### level
  🇻 Variable --> `int`

- #### max_level
  🇲 Method --> `() -> int`
  > Returns the defined maximum level of this SpellSimple instance, if defined. Otherwise, returns 0.

- #### prompt_cast
  🇫 Function --> `( SpellTypeId, Tripoint, Opt( int ) ) -> SpellSimple`
  > Static function: Creates and immediately casts a SimpleSpell, then returns the new spell for potential reuse. If the given tripoint is the player's location, the spell will be locked to the player. (This does not necessarily cause friendly fire!) If an integer is specified, the spell will be cast at that level.

- #### trigger_once_in
  🇻 Variable --> `int`
  > Used for enchantments; the spell's _chance_ to trigger every turn.

## SpellTypeId

### Bases

No base classes.

### Constructors

- #### `SpellTypeId.new()`
- #### `SpellTypeId.new( SpellTypeId )`
- #### `SpellTypeId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> SpellTypeId`

- #### obj
  🇲 Method --> `() -> SpellTypeRaw`

- #### str
  🇲 Method --> `() -> string`

## SpellTypeRaw

The 'raw' type for storing the information defining every spell in the game. It's not possible to cast directly from this type; check SpellSimple and Spell.

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### additional_spells
  🇲 Method --> `() -> Vector( SpellSimple )`
  > Other spells cast by this spell.

- #### aoe_increment
  🇻 Variable --> `double`

- #### base_casting_time
  🇻 Variable --> `int`

- #### base_energy_cost
  🇻 Variable --> `int`

- #### casting_time_increment
  🇻 Variable --> `double`

- #### damage_increment
  🇻 Variable --> `double`

- #### difficulty
  🇻 Variable --> `int`

- #### dot_increment
  🇻 Variable --> `double`

- #### duration_increment
  🇻 Variable --> `int`

- #### effect_name
  🇻 Variable --> `string`
  > The name of the primary effect this spell will enact.

- #### effect_str
  🇻 Variable --> `string`
  > Specifics about the effect this spell will enact.

- #### energy_increment
  🇻 Variable --> `double`

- #### field_chance
  🇻 Variable --> `int`

- #### field_intensity_increment
  🇻 Variable --> `double`

- #### field_intensity_variance
  🇻 Variable --> `double`

- #### final_casting_time
  🇻 Variable --> `int`

- #### final_energy_cost
  🇻 Variable --> `int`

- #### get_all
  🇫 Function --> `() -> <cppval: const std::vector<spell_type> & >`
  > Returns a (long) list of every spell in the game.

- #### id
  🇻 Variable --> `SpellTypeId`

- #### max_aoe
  🇻 Variable --> `int`

- #### max_damage
  🇻 Variable --> `int`

- #### max_dot
  🇻 Variable --> `int`

- #### max_duration
  🇻 Variable --> `int`

- #### max_field_intensity
  🇻 Variable --> `int`

- #### max_level
  🇻 Variable --> `int`

- #### max_range
  🇻 Variable --> `int`

- #### min_aoe
  🇻 Variable --> `int`

- #### min_damage
  🇻 Variable --> `int`

- #### min_dot
  🇻 Variable --> `int`

- #### min_duration
  🇻 Variable --> `int`

- #### min_field_intensity
  🇻 Variable --> `int`

- #### min_range
  🇻 Variable --> `int`

- #### range_increment
  🇻 Variable --> `double`

## TerId

### Bases

No base classes.

### Constructors

- #### `TerId.new()`
- #### `TerId.new( TerId )`
- #### `TerId.new( TerIntId )`
- #### `TerId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### int_id
  🇲 Method --> `() -> TerIntId`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> TerId`

- #### obj
  🇲 Method --> `() -> TerRaw`

- #### str
  🇲 Method --> `() -> string`

## TerIntId

### Bases

No base classes.

### Constructors

- #### `TerIntId.new()`
- #### `TerIntId.new( TerIntId )`
- #### `TerIntId.new( TerId )`

### Members

- #### is_valid
  🇲 Method --> `() -> bool`

- #### obj
  🇲 Method --> `() -> TerRaw`

- #### str_id
  🇲 Method --> `() -> TerId`

## TerRaw

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### close
  🇻 Variable --> `TerId`

- #### get_coverage
  🇲 Method --> `() -> int`

- #### get_flags
  🇲 Method --> `() -> <cppval: const std::set<std::basic_string<char>> & >`

- #### get_light_emitted
  🇲 Method --> `() -> int`

- #### get_max_volume
  🇲 Method --> `() -> Volume`

- #### get_movecost
  🇲 Method --> `() -> int`

- #### has_flag
  🇲 Method --> `( string ) -> bool`

- #### heat_radiation
  🇻 Variable --> `int`

- #### int_id
  🇲 Method --> `() -> TerIntId`

- #### name
  🇲 Method --> `() -> string`

- #### open
  🇻 Variable --> `TerId`

- #### roof
  🇻 Variable --> `TerId`

- #### set_coverage
  🇲 Method --> `( int )`

- #### set_flag
  🇲 Method --> `( string )`

- #### set_light_emitted
  🇲 Method --> `( int )`

- #### set_max_volume
  🇲 Method --> `( Volume )`

- #### set_movecost
  🇲 Method --> `( int )`

- #### str_id
  🇲 Method --> `() -> TerId`

- #### transforms_into
  🇻 Variable --> `TerId`

- #### trap_id_str
  🇻 Variable --> `string`

## TimeDuration

Represent duration between 2 fixed points in time

### Bases

No base classes.

### Constructors

- #### `TimeDuration.new()`

### Members

- #### from_days
  🇫 Function --> `( int ) -> TimeDuration`

- #### from_hours
  🇫 Function --> `( int ) -> TimeDuration`

- #### from_minutes
  🇫 Function --> `( int ) -> TimeDuration`

- #### from_seconds
  🇫 Function --> `( int ) -> TimeDuration`

- #### from_turns
  🇫 Function --> `( int ) -> TimeDuration`

- #### from_weeks
  🇫 Function --> `( int ) -> TimeDuration`

- #### make_random
  🇲 Method --> `( TimeDuration ) -> TimeDuration`

- #### to_days
  🇲 Method --> `() -> int`

- #### to_hours
  🇲 Method --> `() -> int`

- #### to_minutes
  🇲 Method --> `() -> int`

- #### to_seconds
  🇲 Method --> `() -> int`

- #### to_turns
  🇲 Method --> `() -> int`

- #### to_weeks
  🇲 Method --> `() -> int`

## TimePoint

Represent fixed point in time

### Bases

No base classes.

### Constructors

- #### `TimePoint.new()`

### Members

- #### from_turn
  🇫 Function --> `( int ) -> TimePoint`

- #### hour_of_day
  🇲 Method --> `() -> int`

- #### is_dawn
  🇲 Method --> `() -> bool`

- #### is_day
  🇲 Method --> `() -> bool`

- #### is_dusk
  🇲 Method --> `() -> bool`

- #### is_night
  🇲 Method --> `() -> bool`

- #### minute_of_hour
  🇲 Method --> `() -> int`

- #### second_of_minute
  🇲 Method --> `() -> int`

- #### to_string_time_of_day
  🇲 Method --> `() -> string`

- #### to_turn
  🇲 Method --> `() -> int`

## Tinymap

### Bases

- `Map`

### Constructors

No constructors.

### Members

No members.

## TrapId

### Bases

No base classes.

### Constructors

- #### `TrapId.new()`
- #### `TrapId.new( TrapId )`
- #### `TrapId.new( TrapIntId )`
- #### `TrapId.new( string )`

### Members

- #### implements_int_id
  🇫 Function --> `() -> bool`

- #### int_id
  🇲 Method --> `() -> TrapIntId`

- #### is_null
  🇲 Method --> `() -> bool`

- #### is_valid
  🇲 Method --> `() -> bool`

- #### NULL_ID
  🇫 Function --> `() -> TrapId`

- #### obj
  🇲 Method --> `() -> TrapRaw`

- #### str
  🇲 Method --> `() -> string`

## TrapIntId

### Bases

No base classes.

### Constructors

- #### `TrapIntId.new()`
- #### `TrapIntId.new( TrapIntId )`
- #### `TrapIntId.new( TrapId )`

### Members

- #### is_valid
  🇲 Method --> `() -> bool`

- #### obj
  🇲 Method --> `() -> TrapRaw`

- #### str_id
  🇲 Method --> `() -> TrapId`

## Tripoint

### Bases

No base classes.

### Constructors

- #### `Tripoint.new()`
- #### `Tripoint.new( Point, int )`
- #### `Tripoint.new( Tripoint )`
- #### `Tripoint.new( int, int, int )`

### Members

- #### abs
  🇲 Method --> `() -> Tripoint`

- #### rotate_2d
  🇲 Method --> `( int, Point ) -> Tripoint`

- #### x
  🇻 Variable --> `int`

- #### xy
  🇲 Method --> `() -> Point`

- #### y
  🇻 Variable --> `int`

- #### z
  🇻 Variable --> `int`

## UiList

### Bases

No base classes.

### Constructors

- #### `UiList.new()`

### Members

- #### add
  🇲 Method --> `( int, string )`
  > Adds an entry. `string` is its name, and `int` is what it returns. If `int` is `-1`, the number is decided orderly.

- #### add_w_col
  🇲 Method --> `( int, string, string, string )`
  > Adds an entry with desc and col(third `string`). col is additional text on the right of the entry name.

- #### add_w_desc
  🇲 Method --> `( int, string, string )`
  > Adds an entry with desc(second `string`). `desc_enabled(true)` is required for showing desc.

- #### border_color
  🇲 Method --> `( Color )`
  > Changes the color. Default color is `c_magenta`.

- #### desc_enabled
  🇲 Method --> `( bool )`
  > Puts a lower box. Footer or entry desc appears on it.

- #### entries
  🇻 Variable --> `Vector( UiListEntry )`
  > Entries from uilist. Remember, in lua, the first element of vector is `entries[1]`, not `entries[0]`.

- #### footer
  🇲 Method --> `( string )`
  > Sets footer text which is in lower box. It overwrites descs of entries unless is empty.

- #### hilight_color
  🇲 Method --> `( Color )`
  > Changes the color. Default color is `h_white`.

- #### hotkey_color
  🇲 Method --> `( Color )`
  > Changes the color. Default color is `c_light_green`.

- #### query
  🇲 Method --> `() -> int`
  > Returns retval for selected entry, or a negative number on fail/cancel

- #### text
  🇲 Method --> `( string )`
  > Sets text which is in upper box.

- #### text_color
  🇲 Method --> `( Color )`
  > Changes the color. Default color is `c_light_gray`.

- #### title
  🇲 Method --> `( string )`
  > Sets title which is on the top line.

- #### title_color
  🇲 Method --> `( Color )`
  > Changes the color. Default color is `c_green`.

## UiListEntry

This type came from UiList.

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### ctxt
  🇻 Variable --> `string`
  > Entry text of column.

- #### desc
  🇻 Variable --> `string`
  > Entry description

- #### enable
  🇻 Variable --> `bool`
  > Entry whether it's enabled or not. Default is `true`.

- #### txt
  🇻 Variable --> `string`
  > Entry text

- #### txt_color
  🇲 Method --> `( Color )`
  > Entry text color. Its default color is `c_red_red`, which makes color of the entry same as what `uilist` decides. So if you want to make color different, choose one except `c_red_red`.

## Volume

### Bases

No base classes.

### Constructors

No constructors.

### Members

- #### from_liter
  🇫 Function --> `( int ) -> Volume`

- #### from_milliliter
  🇫 Function --> `( int ) -> Volume`

- #### to_liter
  🇲 Method --> `() -> double`

- #### to_milliliter
  🇲 Method --> `() -> int`

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

## ArtifactCharge

### Entries

- `ARTC_NULL` = `0`
- `ARTC_TIME` = `1`
- `ARTC_SOLAR` = `2`
- `ARTC_PAIN` = `3`
- `ARTC_HP` = `4`
- `ARTC_FATIGUE` = `5`
- `ARTC_PORTAL` = `6`

## ArtifactChargeReq

### Entries

- `ACR_NULL` = `0`
- `ACR_EQUIP` = `1`
- `ACR_SKIN` = `2`
- `ACR_SLEEP` = `3`
- `ACR_RAD` = `4`
- `ACR_WET` = `5`
- `ACR_SKY` = `6`

## ArtifactEffectActive

### Entries

- `AEP_NULL` = `0`
- `AEP_STR_UP` = `1`
- `AEP_DEX_UP` = `2`
- `AEP_PER_UP` = `3`
- `AEP_INT_UP` = `4`
- `AEP_ALL_UP` = `5`
- `AEP_SPEED_UP` = `6`
- `AEP_PBLUE` = `7`
- `AEP_SNAKES` = `8`
- `AEP_INVISIBLE` = `9`
- `AEP_CLAIRVOYANCE` = `10`
- `AEP_SUPER_CLAIRVOYANCE` = `11`
- `AEP_STEALTH` = `12`
- `AEP_EXTINGUISH` = `13`
- `AEP_GLOW` = `14`
- `AEP_PSYSHIELD` = `15`
- `AEP_RESIST_ELECTRICITY` = `16`
- `AEP_CARRY_MORE` = `17`
- `AEP_SAP_LIFE` = `18`
- `AEP_FUN` = `19`
- `AEP_SPLIT` = `20`
- `AEP_HUNGER` = `21`
- `AEP_THIRST` = `22`
- `AEP_SMOKE` = `23`
- `AEP_EVIL` = `24`
- `AEP_SCHIZO` = `25`
- `AEP_RADIOACTIVE` = `26`
- `AEP_MUTAGENIC` = `27`
- `AEP_ATTENTION` = `28`
- `AEP_STR_DOWN` = `29`
- `AEP_DEX_DOWN` = `30`
- `AEP_PER_DOWN` = `31`
- `AEP_INT_DOWN` = `32`
- `AEP_ALL_DOWN` = `33`
- `AEP_SPEED_DOWN` = `34`
- `AEP_FORCE_TELEPORT` = `35`
- `AEP_MOVEMENT_NOISE` = `36`
- `AEP_BAD_WEATHER` = `37`
- `AEP_SICK` = `38`
- `AEP_CLAIRVOYANCE_PLUS` = `39`

## ArtifactEffectPassive

### Entries

- `AEA_NULL` = `0`
- `AEA_STORM` = `1`
- `AEA_FIREBALL` = `2`
- `AEA_ADRENALINE` = `3`
- `AEA_MAP` = `4`
- `AEA_BLOOD` = `5`
- `AEA_FATIGUE` = `6`
- `AEA_ACIDBALL` = `7`
- `AEA_PULSE` = `8`
- `AEA_HEAL` = `9`
- `AEA_CONFUSED` = `10`
- `AEA_ENTRANCE` = `11`
- `AEA_BUGS` = `12`
- `AEA_TELEPORT` = `13`
- `AEA_LIGHT` = `14`
- `AEA_GROWTH` = `15`
- `AEA_HURTALL` = `16`
- `AEA_FUN` = `17`
- `AEA_SPLIT` = `18`
- `AEA_RADIATION` = `19`
- `AEA_PAIN` = `20`
- `AEA_MUTATE` = `21`
- `AEA_PARALYZE` = `22`
- `AEA_FIRESTORM` = `23`
- `AEA_ATTENTION` = `24`
- `AEA_TELEGLOW` = `25`
- `AEA_NOISE` = `26`
- `AEA_SCREAM` = `27`
- `AEA_DIM` = `28`
- `AEA_FLASH` = `29`
- `AEA_VOMIT` = `30`
- `AEA_SHADOWS` = `31`
- `AEA_STAMINA_EMPTY` = `32`

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
- `DT_DARK` = `9`
- `DT_LIGHT` = `10`
- `DT_PSI` = `11`
- `DT_ELECTRIC` = `12`
- `DT_BULLET` = `13`

## MissionGoal

### Entries

- `MGOAL_NULL` = `0`
- `MGOAL_GO_TO` = `1`
- `MGOAL_GO_TO_TYPE` = `2`
- `MGOAL_FIND_ITEM` = `3`
- `MGOAL_FIND_ANY_ITEM` = `4`
- `MGOAL_FIND_ITEM_GROUP` = `5`
- `MGOAL_FIND_MONSTER` = `6`
- `MGOAL_FIND_NPC` = `7`
- `MGOAL_ASSASSINATE` = `8`
- `MGOAL_KILL_MONSTER` = `9`
- `MGOAL_KILL_MONSTER_TYPE` = `10`
- `MGOAL_RECRUIT_NPC` = `11`
- `MGOAL_RECRUIT_NPC_CLASS` = `12`
- `MGOAL_COMPUTER_TOGGLE` = `13`
- `MGOAL_KILL_MONSTER_SPEC` = `14`
- `MGOAL_TALK_TO_NPC` = `15`
- `MGOAL_CONDITION` = `16`

## MissionOrigin

### Entries

- `ORIGIN_NULL` = `0`
- `ORIGIN_GAME_START` = `1`
- `ORIGIN_OPENER_NPC` = `2`
- `ORIGIN_ANY_NPC` = `3`
- `ORIGIN_SECONDARY` = `4`
- `ORIGIN_COMPUTER` = `5`

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
- `DARKPROOF` = `37`
- `LIGHTPROOF` = `38`
- `PSIPROOF` = `39`
- `FIREY` = `40`
- `QUEEN` = `41`
- `ELECTRONIC` = `42`
- `FUR` = `43`
- `LEATHER` = `44`
- `WOOL` = `45`
- `FEATHER` = `46`
- `BONES` = `47`
- `FAT` = `48`
- `CONSOLE_DESPAWN` = `49`
- `IMMOBILE` = `50`
- `ID_CARD_DESPAWN` = `51`
- `RIDEABLE_MECH` = `52`
- `CARD_OVERRIDE` = `53`
- `MILITARY_MECH` = `54`
- `MECH_RECON_VISION` = `55`
- `MECH_DEFENSIVE` = `56`
- `HIT_AND_RUN` = `57`
- `GUILT` = `58`
- `PAY_BOT` = `59`
- `HUMAN` = `60`
- `NO_BREATHE` = `61`
- `FLAMMABLE` = `62`
- `REVIVES` = `63`
- `CHITIN` = `64`
- `VERMIN` = `65`
- `NOGIB` = `66`
- `LARVA` = `67`
- `ARTHROPOD_BLOOD` = `68`
- `ACID_BLOOD` = `69`
- `BILE_BLOOD` = `70`
- `ABSORBS` = `71`
- `ABSORBS_SPLITS` = `72`
- `CBM_CIV` = `73`
- `CBM_POWER` = `74`
- `CBM_SCI` = `75`
- `CBM_OP` = `76`
- `CBM_TECH` = `77`
- `CBM_SUBS` = `78`
- `UNUSED_76` = `79`
- `FISHABLE` = `80`
- `GROUP_BASH` = `81`
- `SWARMS` = `82`
- `GROUP_MORALE` = `83`
- `INTERIOR_AMMO` = `84`
- `CLIMBS` = `85`
- `PACIFIST` = `86`
- `PUSH_MON` = `87`
- `PUSH_VEH` = `88`
- `NIGHT_INVISIBILITY` = `89`
- `REVIVES_HEALTHY` = `90`
- `NO_NECRO` = `91`
- `PATH_AVOID_DANGER_1` = `92`
- `PATH_AVOID_DANGER_2` = `93`
- `PATH_AVOID_FIRE` = `94`
- `PATH_AVOID_FALL` = `95`
- `PRIORITIZE_TARGETS` = `96`
- `NOT_HALLUCINATION` = `97`
- `CANPLAY` = `98`
- `PET_MOUNTABLE` = `99`
- `PET_HARNESSABLE` = `100`
- `DOGFOOD` = `101`
- `MILKABLE` = `102`
- `SHEARABLE` = `103`
- `NO_BREED` = `104`
- `NO_FUNG_DMG` = `105`
- `PET_WONT_FOLLOW` = `106`
- `DRIPS_NAPALM` = `107`
- `DRIPS_GASOLINE` = `108`
- `ELECTRIC_FIELD` = `109`
- `LOUDMOVES` = `110`
- `CAN_OPEN_DOORS` = `111`
- `STUN_IMMUNE` = `112`
- `DROPS_AMMO` = `113`
- `CAN_BE_ORDERED` = `114`
- `SMALL_HEAD` = `115`
- `TINY_HEAD` = `116`
- `NO_HEAD_BONUS_CRIT` = `117`
- `HEAD_BONUS_CRIT_1` = `118`
- `HEAD_BONUS_CRIT_2` = `119`
- `TORSO_BONUS_CRIT_1` = `120`
- `TORSO_BONUS_CRIT_2` = `121`
- `PROJECTILE_RESISTANT_1` = `122`
- `PROJECTILE_RESISTANT_2` = `123`
- `PROJECTILE_RESISTANT_3` = `124`
- `PROJECTILE_RESISTANT_4` = `125`
- `VOLATILE` = `126`

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

- #### OM_MS_SIZE
  🇨 Constant --> `int` = `4320`

- #### OM_OMT_SIZE
  🇨 Constant --> `int` = `180`

- #### OM_SM_SIZE
  🇨 Constant --> `int` = `360`

- #### OMT_MS_SIZE
  🇨 Constant --> `int` = `24`

- #### OMT_SM_SIZE
  🇨 Constant --> `int` = `2`

- #### SM_MS_SIZE
  🇨 Constant --> `int` = `12`

## coords

Methods for manipulating coord systems and calculating distance

### Members

- #### ms_to_om
  🇫 Function --> `( Tripoint ) -> ( Point, Tripoint )`

- #### ms_to_omt
  🇫 Function --> `( Tripoint ) -> ( Tripoint, Point )`

- #### ms_to_sm
  🇫 Function --> `( Tripoint ) -> ( Tripoint, Point )`

- #### om_to_ms
  🇫 Function --> `( Point, Opt( Tripoint ) ) -> Tripoint`

- #### omt_to_ms
  🇫 Function --> `( Tripoint, Opt( Point ) ) -> Tripoint`

- #### rl_dist
  🇫 Function --> `( Tripoint, Tripoint ) -> int`\
  🇫 Function --> `( Point, Point ) -> int`

- #### sm_to_ms
  🇫 Function --> `( Tripoint, Opt( Point ) ) -> Tripoint`

- #### square_dist
  🇫 Function --> `( Tripoint, Tripoint ) -> int`\
  🇫 Function --> `( Point, Point ) -> int`

- #### trig_dist
  🇫 Function --> `( Tripoint, Tripoint ) -> double`\
  🇫 Function --> `( Point, Point ) -> double`

## gapi

Global game methods

### Members

- #### add_msg
  🇫 Function --> `( MsgType, ... )`\
  🇫 Function --> `( ... )`

- #### add_npc_follower
  🇫 Function --> `( Npc )`

- #### add_on_every_x_hook
  🇫 Function --> `( TimeDuration, function )`

- #### before_time_starts
  🇫 Function --> `() -> TimePoint`

- #### choose_adjacent
  🇫 Function --> `( string, Opt( bool ) ) -> Opt( Tripoint )`

- #### choose_direction
  🇫 Function --> `( string, Opt( bool ) ) -> Opt( Tripoint )`

- #### create_item
  🇫 Function --> `( ItypeId, int ) -> <cppval: std::unique_ptr<item> >`

- #### current_turn
  🇫 Function --> `() -> TimePoint`

- #### get_avatar
  🇫 Function --> `() -> Avatar`

- #### get_character_at
  🇫 Function --> `( Tripoint, Opt( bool ) ) -> Character`

- #### get_creature_at
  🇫 Function --> `( Tripoint, Opt( bool ) ) -> Creature`

- #### get_distribution_grid_tracker
  🇫 Function --> `() -> DistributionGridTracker`

- #### get_map
  🇫 Function --> `() -> Map`

- #### get_monster_at
  🇫 Function --> `( Tripoint, Opt( bool ) ) -> Monster`

- #### get_npc_at
  🇫 Function --> `( Tripoint, Opt( bool ) ) -> Npc`

- #### look_around
  🇫 Function --> `() -> Opt( Tripoint )`

- #### place_monster_around
  🇫 Function --> `( MtypeId, Tripoint, int ) -> Monster`

- #### place_monster_at
  🇫 Function --> `( MtypeId, Tripoint ) -> Monster`

- #### place_player_overmap_at
  🇫 Function --> `( Tripoint )`

- #### play_ambient_variant_sound
  🇫 Function --> `( string, string, int, SfxChannel, int, double, int )`

- #### play_variant_sound
  🇫 Function --> `( string, string, int )`\
  🇫 Function --> `( string, string, int, Angle, double, double )`

- #### remove_npc_follower
  🇫 Function --> `( Npc )`

- #### rng
  🇫 Function --> `( int, int ) -> int`

- #### turn_zero
  🇫 Function --> `() -> TimePoint`

## gdebug

Debugging and logging API.

### Members

- #### clear_lua_log
  🇫 Function --> `()`

- #### debugmsg
  🇫 Function --> `( ... )`

- #### log_error
  🇫 Function --> `( ... )`

- #### log_info
  🇫 Function --> `( ... )`

- #### log_warn
  🇫 Function --> `( ... )`

- #### reload_lua_code
  🇫 Function --> `()`

- #### save_game
  🇫 Function --> `() -> bool`

- #### set_log_capacity
  🇫 Function --> `( int )`

## hooks

Documentation for hooks

### Members

- #### on_character_reset_stats
  🇫 Function --> `()`
  > Called when character stat gets reset

- #### on_char_death
  🇫 Function --> `()`
  > Called when a character is dead

- #### on_creature_blocked
  🇫 Function --> `()`
  > Called when a character successfully blocks

- #### on_creature_dodged
  🇫 Function --> `()`
  > Called when a character successfully dodges

- #### on_creature_melee_attacked
  🇫 Function --> `()`
  > Called after a character has attacked in melee

- #### on_creature_performed_technique
  🇫 Function --> `()`
  > Called when a character has performed technique

- #### on_every_x
  🇫 Function --> `()`
  > Called every in-game period

- #### on_game_load
  🇫 Function --> `()`
  > Called right after game has loaded

- #### on_game_save
  🇫 Function --> `()`
  > Called when game is about to save

- #### on_game_started
  🇫 Function --> `()`
  > Called when the game has first started

- #### on_mapgen_postprocess
  🇫 Function --> `( Map, Tripoint, TimePoint )`
  > Called right after mapgen has completed. Map argument is the tinymap that represents 24x24 area (2x2 submaps, or 1x1 omt), tripoint is the absolute omt pos, and time_point is the current time (for time-based effects).

- #### on_mon_death
  🇫 Function --> `()`
  > Called when a monster is dead

## locale

Localization API.

### Members

- #### gettext
  🇫 Function --> `( string ) -> string`
  > Expects english source string, returns translated string.

- #### pgettext
  🇫 Function --> `( string, string ) -> string`
  > First is context string. Second is english source string.

- #### vgettext
  🇫 Function --> `( string, string, <cppval: unsigned long > ) -> string`
  > First is english singular string, second is english plural string. Number is amount to translate for.

- #### vpgettext
  🇫 Function --> `( string, string, string, <cppval: unsigned long > ) -> string`
  > First is context string. Second is english singular string. third is english plural. Number is amount to translate for.

## tests_lib

Library for testing purposes

### Members

- #### my_awesome_lambda_1
  🇫 Function --> `() -> int`

- #### my_awesome_lambda_2
  🇫 Function --> `() -> int`
