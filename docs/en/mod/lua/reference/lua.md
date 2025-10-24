---
edit: false
---

# Lua API reference

> [!NOTE]
>
> This page is auto-generated from [`data/raw/generate_docs.lua`][generate_docs]
> and should not be edited directly.

> [!WARNING]
>
> In Lua, functions can be called with a `:` and pass the object itself as the first argument, eg:
>
> Members where this behaviour is intended to be used are marked as 🇲 Methods<br/>
> Their signature documentation hides the first argument to reflect that
>
> - Call 🇫 Function members with a `.`
> - Call 🇲 Method members with a `:`
>
> Alternatively, you can still call 🇲 Methods with a `.`, from the class type or the variable itself
> but a value of the given type must be passed as the first parameter (that is hidden)
>
> All of these do the same thing:
>
> - ```
>   print(Angle.from_radians(3):to_degrees(a))
>   ```
> - ```
>   print(Angle.to_degrees(Angle.from_radians(3)))
>   ```
> - ```
>   local a = Angle.from_radians(3)
>   print(a:to_degrees())
>   ```
> - ```
>   local a = Angle.from_radians(3)
>   print(a.to_degrees(a))
>   ```
> - ```
>   local a = Angle.from_radians(3)
>   print(Angle.to_degrees(a))
>   ```

[generate_docs]: https://github.com/cataclysmbnteam/Cataclysm-BN/blob/main/data/raw/generate_docs.lua

## ActivityTypeId {#sol::ActivityTypeId}

### Bases {#sol::ActivityTypeId::@bases}

No base classes.

### Constructors {#sol::ActivityTypeId::@ctors}

- ActivityTypeId.new( )
- ActivityTypeId.new( [ActivityTypeId](#sol::ActivityTypeId) )
- ActivityTypeId.new( string )

### Members {#sol::ActivityTypeId::@members}

#### NULL_ID {#sol::ActivityTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [ActivityTypeId](#sol::ActivityTypeId)</code>

#### obj {#sol::ActivityTypeId::obj}

🇲 Method --> <code>( ) -> ActivityTypeRaw</code>

#### is_valid {#sol::ActivityTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::ActivityTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::ActivityTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::ActivityTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## Angle {#sol::Angle}

### Bases {#sol::Angle::@bases}

No base classes.

### Constructors {#sol::Angle::@ctors}

No constructors.

### Members {#sol::Angle::@members}

#### from_radians {#sol::Angle::from_radians}

🇫 Function --> <code>( number ) -> [Angle](#sol::Angle)</code>

#### to_arcmin {#sol::Angle::to_arcmin}

🇲 Method --> <code>( ) -> number</code>

#### to_degrees {#sol::Angle::to_degrees}

🇲 Method --> <code>( ) -> number</code>

#### from_arcmin {#sol::Angle::from_arcmin}

🇫 Function --> <code>( number ) -> [Angle](#sol::Angle)</code>

#### from_degrees {#sol::Angle::from_degrees}

🇫 Function --> <code>( number ) -> [Angle](#sol::Angle)</code>

#### to_radians {#sol::Angle::to_radians}

🇲 Method --> <code>( ) -> number</code>

## Avatar {#sol::Avatar}

### Bases {#sol::Avatar::@bases}

- `Player`
- `Character`
- `Creature`

### Constructors {#sol::Avatar::@ctors}

No constructors.

### Members {#sol::Avatar::@members}

#### get_active_missions {#sol::Avatar::get_active_missions}

🇲 Method --> <code>( ) -> [Mission](#sol::Mission)[]</code>

#### get_completed_missions {#sol::Avatar::get_completed_missions}

🇲 Method --> <code>( ) -> [Mission](#sol::Mission)[]</code>

#### get_failed_missions {#sol::Avatar::get_failed_missions}

🇲 Method --> <code>( ) -> [Mission](#sol::Mission)[]</code>

## BionicDataId {#sol::BionicDataId}

### Bases {#sol::BionicDataId::@bases}

No base classes.

### Constructors {#sol::BionicDataId::@ctors}

- BionicDataId.new( )
- BionicDataId.new( [BionicDataId](#sol::BionicDataId) )
- BionicDataId.new( string )

### Members {#sol::BionicDataId::@members}

#### NULL_ID {#sol::BionicDataId::NULL_ID}

🇫 Function --> <code>( ) -> [BionicDataId](#sol::BionicDataId)</code>

#### obj {#sol::BionicDataId::obj}

🇲 Method --> <code>( ) -> BionicDataRaw</code>

#### is_valid {#sol::BionicDataId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::BionicDataId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::BionicDataId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::BionicDataId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## BodyPartTypeId {#sol::BodyPartTypeId}

### Bases {#sol::BodyPartTypeId::@bases}

No base classes.

### Constructors {#sol::BodyPartTypeId::@ctors}

- BodyPartTypeId.new( )
- BodyPartTypeId.new( [BodyPartTypeId](#sol::BodyPartTypeId) )
- BodyPartTypeId.new( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) )
- BodyPartTypeId.new( string )

### Members {#sol::BodyPartTypeId::@members}

#### NULL_ID {#sol::BodyPartTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [BodyPartTypeId](#sol::BodyPartTypeId)</code>

#### str {#sol::BodyPartTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### obj {#sol::BodyPartTypeId::obj}

🇲 Method --> <code>( ) -> BodyPartTypeRaw</code>

#### is_null {#sol::BodyPartTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### is_valid {#sol::BodyPartTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::BodyPartTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

#### int_id {#sol::BodyPartTypeId::int_id}

🇲 Method --> <code>( ) -> [BodyPartTypeIntId](#sol::BodyPartTypeIntId)</code>

## BodyPartTypeIntId {#sol::BodyPartTypeIntId}

### Bases {#sol::BodyPartTypeIntId::@bases}

No base classes.

### Constructors {#sol::BodyPartTypeIntId::@ctors}

- BodyPartTypeIntId.new( )
- BodyPartTypeIntId.new( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) )
- BodyPartTypeIntId.new( [BodyPartTypeId](#sol::BodyPartTypeId) )

### Members {#sol::BodyPartTypeIntId::@members}

#### obj {#sol::BodyPartTypeIntId::obj}

🇲 Method --> <code>( ) -> BodyPartTypeRaw</code>

#### is_valid {#sol::BodyPartTypeIntId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str_id {#sol::BodyPartTypeIntId::str_id}

🇲 Method --> <code>( ) -> [BodyPartTypeId](#sol::BodyPartTypeId)</code>

## Character {#sol::Character}

### Bases {#sol::Character::@bases}

- `Creature`

### Constructors {#sol::Character::@ctors}

No constructors.

### Members {#sol::Character::@members}

#### name {#sol::Character::name}

🇻 Variable --> <code>string</code>

#### follower_ids {#sol::Character::follower_ids}

🇻 Variable --> <code>Set( [CharacterId](#sol::CharacterId) )</code>

#### cash {#sol::Character::cash}

🇻 Variable --> <code>int</code>

#### mutation_category_level {#sol::Character::mutation_category_level}

🇻 Variable --> <code>Dict( [MutationCategoryTraitId](#sol::MutationCategoryTraitId), int )</code>

#### male {#sol::Character::male}

🇻 Variable --> <code>bool</code>

#### focus_pool {#sol::Character::focus_pool}

🇻 Variable --> <code>int</code>

#### rust_rate {#sol::Character::rust_rate}

🇲 Method --> <code>( ) -> integer</code>

#### mod_skill_level {#sol::Character::mod_skill_level}

🇲 Method --> <code>( [SkillId](#sol::SkillId), integer )</code>

#### practice {#sol::Character::practice}

🇲 Method --> <code>( [SkillId](#sol::SkillId), integer, integer, boolean )</code>

#### set_skill_level {#sol::Character::set_skill_level}

🇲 Method --> <code>( [SkillId](#sol::SkillId), integer )</code>

#### read_speed {#sol::Character::read_speed}

🇲 Method --> <code>( boolean ) -> integer</code>

#### get_skill_level_object {#sol::Character::get_skill_level_object}

🇲 Method --> <code>( [SkillId](#sol::SkillId) ) -> [SkillLevel](#sol::SkillLevel)</code>

#### item_worn_with_id {#sol::Character::item_worn_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> [Item](#sol::Item)</code>

#### get_skill_level {#sol::Character::get_skill_level}

🇲 Method --> <code>( [SkillId](#sol::SkillId) ) -> integer</code>

#### worn_with_flag {#sol::Character::worn_with_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId), [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

#### get_all_skills {#sol::Character::get_all_skills}

🇲 Method --> <code>( ) -> [SkillLevelMap](#sol::SkillLevelMap)</code>

#### worn_with_id {#sol::Character::worn_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

#### get_time_died {#sol::Character::get_time_died}

🇲 Method --> <code>( ) -> [TimePoint](#sol::TimePoint)</code>

#### item_worn_with_flag {#sol::Character::item_worn_with_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId), [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> [Item](#sol::Item)</code>

#### is_rad_immune {#sol::Character::is_rad_immune}

🇲 Method --> <code>( ) -> boolean</code>

#### healing_rate {#sol::Character::healing_rate}

🇲 Method --> <code>( number ) -> number</code>

#### rest_quality {#sol::Character::rest_quality}

🇲 Method --> <code>( ) -> number</code>

#### add_addiction {#sol::Character::add_addiction}

🇲 Method --> <code>( [AddictionType](#sol::AddictionType), integer )</code>

#### crossed_threshold {#sol::Character::crossed_threshold}

🇲 Method --> <code>( ) -> boolean</code>

#### rem_addiction {#sol::Character::rem_addiction}

🇲 Method --> <code>( [AddictionType](#sol::AddictionType) )</code>

#### addiction_level {#sol::Character::addiction_level}

🇲 Method --> <code>( [AddictionType](#sol::AddictionType) ) -> integer</code>

#### has_addiction {#sol::Character::has_addiction}

🇲 Method --> <code>( [AddictionType](#sol::AddictionType) ) -> boolean</code>

#### is_throw_immune {#sol::Character::is_throw_immune}

🇲 Method --> <code>( ) -> boolean</code>

#### clear_mutations {#sol::Character::clear_mutations}

🇲 Method --> <code>( )</code>

#### get_mutations {#sol::Character::get_mutations}

🇲 Method --> <code>( boolean ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

#### is_wearing_on_bp {#sol::Character::is_wearing_on_bp}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

#### clear_skills {#sol::Character::clear_skills}

🇲 Method --> <code>( )</code>

#### healing_rate_medicine {#sol::Character::healing_rate_medicine}

🇲 Method --> <code>( number, [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> number</code>

#### get_base_traits {#sol::Character::get_base_traits}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

#### mutation_value {#sol::Character::mutation_value}

🇲 Method --> <code>( string ) -> number</code>

#### is_wearing {#sol::Character::is_wearing}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

#### can_unwield {#sol::Character::can_unwield}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

#### unwield {#sol::Character::unwield}

🇲 Method --> <code>( ) -> boolean</code>

#### add_bionic {#sol::Character::add_bionic}

🇲 Method --> <code>( [BionicDataId](#sol::BionicDataId) )</code>

#### remove_bionic {#sol::Character::remove_bionic}

🇲 Method --> <code>( [BionicDataId](#sol::BionicDataId) )</code>

#### get_power_level {#sol::Character::get_power_level}

🇲 Method --> <code>( ) -> [Energy](#sol::Energy)</code>

#### mod_power_level {#sol::Character::mod_power_level}

🇲 Method --> <code>( [Energy](#sol::Energy) )</code>

#### get_max_power_level {#sol::Character::get_max_power_level}

🇲 Method --> <code>( ) -> [Energy](#sol::Energy)</code>

#### mod_max_power_level {#sol::Character::mod_max_power_level}

🇲 Method --> <code>( [Energy](#sol::Energy) )</code>

#### get_free_bionics_slots {#sol::Character::get_free_bionics_slots}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_used_bionics_slots {#sol::Character::get_used_bionics_slots}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### has_active_bionic {#sol::Character::has_active_bionic}

🇲 Method --> <code>( [BionicDataId](#sol::BionicDataId) ) -> boolean</code>

#### get_total_bionics_slots {#sol::Character::get_total_bionics_slots}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### has_any_bionic {#sol::Character::has_any_bionic}

🇲 Method --> <code>( ) -> boolean</code>

#### clear_bionics {#sol::Character::clear_bionics}

🇲 Method --> <code>( )</code>

#### has_bionics {#sol::Character::has_bionics}

🇲 Method --> <code>( ) -> boolean</code>

#### is_wielding {#sol::Character::is_wielding}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

#### set_power_level {#sol::Character::set_power_level}

🇲 Method --> <code>( [Energy](#sol::Energy) )</code>

#### is_max_power {#sol::Character::is_max_power}

🇲 Method --> <code>( ) -> boolean</code>

#### is_armed {#sol::Character::is_armed}

🇲 Method --> <code>( ) -> boolean</code>

#### can_pick_weight {#sol::Character::can_pick_weight}

🇲 Method --> <code>( [Mass](#sol::Mass), boolean ) -> boolean</code>

#### can_wield {#sol::Character::can_wield}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

#### is_hauling {#sol::Character::is_hauling}

🇲 Method --> <code>( ) -> boolean</code>

#### wield {#sol::Character::wield}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

#### set_max_power_level {#sol::Character::set_max_power_level}

🇲 Method --> <code>( [Energy](#sol::Energy) )</code>

#### can_pick_volume {#sol::Character::can_pick_volume}

🇲 Method --> <code>( [Volume](#sol::Volume) ) -> boolean</code>

#### volume_carried {#sol::Character::volume_carried}

🇲 Method --> <code>( ) -> [Volume](#sol::Volume)</code>

#### has_power {#sol::Character::has_power}

🇲 Method --> <code>( ) -> boolean</code>

#### volume_capacity {#sol::Character::volume_capacity}

🇲 Method --> <code>( ) -> [Volume](#sol::Volume)</code>

#### has_max_power {#sol::Character::has_max_power}

🇲 Method --> <code>( ) -> boolean</code>

#### weight_carried {#sol::Character::weight_carried}

🇲 Method --> <code>( ) -> [Mass](#sol::Mass)</code>

#### is_worn {#sol::Character::is_worn}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

#### add_item_with_id {#sol::Character::add_item_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), integer )</code>

> Adds an item with the given id and amount

#### get_item_with_id {#sol::Character::get_item_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), boolean ) -> [Item](#sol::Item)</code>

> Gets the first occurrence of an item with the given id

#### has_bionic {#sol::Character::has_bionic}

🇲 Method --> <code>( [BionicDataId](#sol::BionicDataId) ) -> boolean</code>

#### rooted {#sol::Character::rooted}

🇲 Method --> <code>( )</code>

#### blossoms {#sol::Character::blossoms}

🇲 Method --> <code>( )</code>

#### fall_asleep {#sol::Character::fall_asleep}

🇲 Method --> <code>( )</code>\
🇲 Method --> <code>( [TimeDuration](#sol::TimeDuration) )</code>

#### get_visible_creatures {#sol::Character::get_visible_creatures}

🇲 Method --> <code>( integer ) -> [Creature](#sol::Creature)[]</code>

#### get_hostile_creatures {#sol::Character::get_hostile_creatures}

🇲 Method --> <code>( integer ) -> [Creature](#sol::Creature)[]</code>

#### wearing_something_on {#sol::Character::wearing_something_on}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

#### spores {#sol::Character::spores}

🇲 Method --> <code>( )</code>

#### set_painkiller {#sol::Character::set_painkiller}

🇲 Method --> <code>( integer )</code>

#### shout {#sol::Character::shout}

🇲 Method --> <code>( string, boolean )</code>

#### get_painkiller {#sol::Character::get_painkiller}

🇲 Method --> <code>( ) -> integer</code>

#### vomit {#sol::Character::vomit}

🇲 Method --> <code>( )</code>

#### mod_painkiller {#sol::Character::mod_painkiller}

🇲 Method --> <code>( integer )</code>

#### restore_scent {#sol::Character::restore_scent}

🇲 Method --> <code>( )</code>

#### get_shout_volume {#sol::Character::get_shout_volume}

🇲 Method --> <code>( ) -> integer</code>

#### is_wearing_helmet {#sol::Character::is_wearing_helmet}

🇲 Method --> <code>( ) -> boolean</code>

#### add_morale {#sol::Character::add_morale}

🇲 Method --> <code>( [MoraleTypeDataId](#sol::MoraleTypeDataId), integer, integer, [TimeDuration](#sol::TimeDuration), [TimeDuration](#sol::TimeDuration), boolean, ItypeRaw )</code>

#### suffer {#sol::Character::suffer}

🇲 Method --> <code>( )</code>

#### learn_recipe {#sol::Character::learn_recipe}

🇲 Method --> <code>( [RecipeId](#sol::RecipeId) )</code>

#### irradiate {#sol::Character::irradiate}

🇲 Method --> <code>( number, boolean ) -> boolean</code>

#### hearing_ability {#sol::Character::hearing_ability}

🇲 Method --> <code>( ) -> number</code>

#### can_hear {#sol::Character::can_hear}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), integer ) -> boolean</code>

#### get_morale_level {#sol::Character::get_morale_level}

🇲 Method --> <code>( ) -> integer</code>

#### knows_recipe {#sol::Character::knows_recipe}

🇲 Method --> <code>( [RecipeId](#sol::RecipeId) ) -> boolean</code>

#### has_morale_to_read {#sol::Character::has_morale_to_read}

🇲 Method --> <code>( ) -> boolean</code>

#### has_morale {#sol::Character::has_morale}

🇲 Method --> <code>( [MoraleTypeDataId](#sol::MoraleTypeDataId) ) -> boolean</code>

#### has_morale_to_craft {#sol::Character::has_morale_to_craft}

🇲 Method --> <code>( ) -> boolean</code>

#### get_morale {#sol::Character::get_morale}

🇲 Method --> <code>( [MoraleTypeDataId](#sol::MoraleTypeDataId) ) -> integer</code>

#### clear_morale {#sol::Character::clear_morale}

🇲 Method --> <code>( )</code>

#### rem_morale {#sol::Character::rem_morale}

🇲 Method --> <code>( [MoraleTypeDataId](#sol::MoraleTypeDataId) )</code>

#### has_item_with_id {#sol::Character::has_item_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), boolean ) -> boolean</code>

> Checks for an item with the given id

#### wake_up {#sol::Character::wake_up}

🇲 Method --> <code>( )</code>

#### set_stamina {#sol::Character::set_stamina}

🇲 Method --> <code>( integer )</code>

#### base_age {#sol::Character::base_age}

🇲 Method --> <code>( ) -> integer</code>

#### metabolic_rate {#sol::Character::metabolic_rate}

🇲 Method --> <code>( ) -> number</code>

#### set_base_age {#sol::Character::set_base_age}

🇲 Method --> <code>( integer )</code>

#### age {#sol::Character::age}

🇲 Method --> <code>( ) -> integer</code>

#### mod_base_age {#sol::Character::mod_base_age}

🇲 Method --> <code>( integer )</code>

#### base_height {#sol::Character::base_height}

🇲 Method --> <code>( ) -> integer</code>

#### cancel_activity {#sol::Character::cancel_activity}

🇲 Method --> <code>( )</code>

#### assign_activity {#sol::Character::assign_activity}

🇲 Method --> <code>( [ActivityTypeId](#sol::ActivityTypeId), integer, integer, integer, string )</code>

#### has_item_with_flag {#sol::Character::has_item_with_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId), boolean ) -> boolean</code>

> Checks for an item with the given flag

#### has_activity {#sol::Character::has_activity}

🇲 Method --> <code>( [ActivityTypeId](#sol::ActivityTypeId) ) -> boolean</code>

#### all_items_with_flag {#sol::Character::all_items_with_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId), boolean ) -> [Item](#sol::Item)[]</code>

> Gets all items with the given flag

#### inv_remove_item {#sol::Character::inv_remove_item}

🇲 Method --> <code>( [Item](#sol::Item) )</code>

> Removes given `Item` from character's inventory. The `Item` must be in the inventory, neither wielded nor worn.

#### all_items {#sol::Character::all_items}

🇲 Method --> <code>( boolean ) -> [Item](#sol::Item)[]</code>

> Gets all items

#### mod_stamina {#sol::Character::mod_stamina}

🇲 Method --> <code>( integer )</code>

#### set_base_height {#sol::Character::set_base_height}

🇲 Method --> <code>( integer )</code>

#### height {#sol::Character::height}

🇲 Method --> <code>( ) -> integer</code>

#### set_rad {#sol::Character::set_rad}

🇲 Method --> <code>( integer )</code>

#### get_rad {#sol::Character::get_rad}

🇲 Method --> <code>( ) -> integer</code>

#### mod_rad {#sol::Character::mod_rad}

🇲 Method --> <code>( integer )</code>

#### get_stamina_max {#sol::Character::get_stamina_max}

🇲 Method --> <code>( ) -> integer</code>

#### get_stamina {#sol::Character::get_stamina}

🇲 Method --> <code>( ) -> integer</code>

#### mod_base_height {#sol::Character::mod_base_height}

🇲 Method --> <code>( integer )</code>

#### mod_stim {#sol::Character::mod_stim}

🇲 Method --> <code>( integer )</code>

#### get_stim {#sol::Character::get_stim}

🇲 Method --> <code>( ) -> integer</code>

#### bodyweight {#sol::Character::bodyweight}

🇲 Method --> <code>( ) -> [Mass](#sol::Mass)</code>

#### set_stim {#sol::Character::set_stim}

🇲 Method --> <code>( integer )</code>

#### bionics_weight {#sol::Character::bionics_weight}

🇲 Method --> <code>( ) -> [Mass](#sol::Mass)</code>

#### get_armor_acid {#sol::Character::get_armor_acid}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_bionics {#sol::Character::get_bionics}

🇲 Method --> <code>( ) -> [BionicDataId](#sol::BionicDataId)[]</code>

#### get_highest_category {#sol::Character::get_highest_category}

🇲 Method --> <code>( ) -> [MutationCategoryTraitId](#sol::MutationCategoryTraitId)</code>

#### is_weak_to_water {#sol::Character::is_weak_to_water}

🇲 Method --> <code>( ) -> boolean</code>

#### mod_fatigue {#sol::Character::mod_fatigue}

🇲 Method --> <code>( integer )</code>

#### mod_thirst {#sol::Character::mod_thirst}

🇲 Method --> <code>( integer )</code>

#### mod_sleep_deprivation {#sol::Character::mod_sleep_deprivation}

🇲 Method --> <code>( integer )</code>

#### set_thirst {#sol::Character::set_thirst}

🇲 Method --> <code>( integer )</code>

#### set_stored_kcal {#sol::Character::set_stored_kcal}

🇲 Method --> <code>( integer )</code>

#### set_fatigue {#sol::Character::set_fatigue}

🇲 Method --> <code>( integer )</code>

#### mod_stored_kcal {#sol::Character::mod_stored_kcal}

🇲 Method --> <code>( integer )</code>

#### get_fatigue {#sol::Character::get_fatigue}

🇲 Method --> <code>( ) -> integer</code>

#### get_stored_kcal {#sol::Character::get_stored_kcal}

🇲 Method --> <code>( ) -> integer</code>

#### get_sleep_deprivation {#sol::Character::get_sleep_deprivation}

🇲 Method --> <code>( ) -> integer</code>

#### max_stored_kcal {#sol::Character::max_stored_kcal}

🇲 Method --> <code>( ) -> integer</code>

#### get_thirst {#sol::Character::get_thirst}

🇲 Method --> <code>( ) -> integer</code>

#### get_kcal_percent {#sol::Character::get_kcal_percent}

🇲 Method --> <code>( ) -> number</code>

#### set_healthy_mod {#sol::Character::set_healthy_mod}

🇲 Method --> <code>( number )</code>

#### set_sleep_deprivation {#sol::Character::set_sleep_deprivation}

🇲 Method --> <code>( integer )</code>

#### set_faction_id {#sol::Character::set_faction_id}

🇲 Method --> <code>( [FactionId](#sol::FactionId) )</code>

#### get_part_encumbrance {#sol::Character::get_part_encumbrance}

🇲 Method --> <code>( [BodyPartTypeId](#sol::BodyPartTypeId) ) -> integer</code>

#### blood_loss {#sol::Character::blood_loss}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### is_wearing_power_armor {#sol::Character::is_wearing_power_armor}

🇲 Method --> <code>( boolean ) -> boolean</code>

#### is_wearing_active_optcloak {#sol::Character::is_wearing_active_optcloak}

🇲 Method --> <code>( ) -> boolean</code>

#### is_wearing_active_power_armor {#sol::Character::is_wearing_active_power_armor}

🇲 Method --> <code>( ) -> boolean</code>

#### get_faction_id {#sol::Character::get_faction_id}

🇲 Method --> <code>( ) -> [FactionId](#sol::FactionId)</code>

#### set_temp_btu {#sol::Character::set_temp_btu}

🇲 Method --> <code>( integer )</code>

> Sets ALL body parts on a creature to the given temperature (in Body Temperature Units).

#### set_part_temp_btu {#sol::Character::set_part_temp_btu}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer )</code>

> Sets a specific body part to a given temperature (in Body Temperature Units).

#### sight_impaired {#sol::Character::sight_impaired}

🇲 Method --> <code>( ) -> boolean</code>

#### get_temp_btu {#sol::Character::get_temp_btu}

🇲 Method --> <code>( ) -> table<[BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer></code>

> Gets all bodyparts and their associated temperatures (in Body Temperature Units).

#### has_alarm_clock {#sol::Character::has_alarm_clock}

🇲 Method --> <code>( ) -> boolean</code>

#### get_part_temp_btu {#sol::Character::get_part_temp_btu}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

> Gets the current temperature of a specific body part (in Body Temperature Units).

#### has_watch {#sol::Character::has_watch}

🇲 Method --> <code>( ) -> boolean</code>

#### in_climate_control {#sol::Character::in_climate_control}

🇲 Method --> <code>( ) -> boolean</code>

#### set_healthy {#sol::Character::set_healthy}

🇲 Method --> <code>( number )</code>

#### mod_healthy {#sol::Character::mod_healthy}

🇲 Method --> <code>( number )</code>

#### get_str_base {#sol::Character::get_str_base}

🇲 Method --> <code>( ) -> integer</code>

#### get_int {#sol::Character::get_int}

🇲 Method --> <code>( ) -> integer</code>

#### get_dex_base {#sol::Character::get_dex_base}

🇲 Method --> <code>( ) -> integer</code>

#### get_int_base {#sol::Character::get_int_base}

🇲 Method --> <code>( ) -> integer</code>

#### get_per_base {#sol::Character::get_per_base}

🇲 Method --> <code>( ) -> integer</code>

#### get_str_bonus {#sol::Character::get_str_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_per {#sol::Character::get_per}

🇲 Method --> <code>( ) -> integer</code>

#### get_str {#sol::Character::get_str}

🇲 Method --> <code>( ) -> integer</code>

#### getID {#sol::Character::getID}

🇲 Method --> <code>( ) -> [CharacterId](#sol::CharacterId)</code>

#### get_dex {#sol::Character::get_dex}

🇲 Method --> <code>( ) -> integer</code>

#### setID {#sol::Character::setID}

🇲 Method --> <code>( [CharacterId](#sol::CharacterId), boolean )</code>

#### reset_encumbrance {#sol::Character::reset_encumbrance}

🇲 Method --> <code>( )</code>

#### reset {#sol::Character::reset}

🇲 Method --> <code>( )</code>

#### mod_healthy_mod {#sol::Character::mod_healthy_mod}

🇲 Method --> <code>( number, number )</code>

#### get_dex_bonus {#sol::Character::get_dex_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_int_bonus {#sol::Character::get_int_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### mod_speed_bonus {#sol::Character::mod_speed_bonus}

🇲 Method --> <code>( integer )</code>

#### mod_int_bonus {#sol::Character::mod_int_bonus}

🇲 Method --> <code>( integer )</code>

#### set_speed_bonus {#sol::Character::set_speed_bonus}

🇲 Method --> <code>( integer )</code>

#### get_healthy_mod {#sol::Character::get_healthy_mod}

🇲 Method --> <code>( ) -> number</code>

#### get_healthy {#sol::Character::get_healthy}

🇲 Method --> <code>( ) -> number</code>

#### get_per_bonus {#sol::Character::get_per_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### mod_per_bonus {#sol::Character::mod_per_bonus}

🇲 Method --> <code>( integer )</code>

#### mod_str_bonus {#sol::Character::mod_str_bonus}

🇲 Method --> <code>( integer )</code>

#### set_str_bonus {#sol::Character::set_str_bonus}

🇲 Method --> <code>( integer )</code>

#### mod_dex_bonus {#sol::Character::mod_dex_bonus}

🇲 Method --> <code>( integer )</code>

#### set_dex_bonus {#sol::Character::set_dex_bonus}

🇲 Method --> <code>( integer )</code>

#### set_int_bonus {#sol::Character::set_int_bonus}

🇲 Method --> <code>( integer )</code>

#### set_per_bonus {#sol::Character::set_per_bonus}

🇲 Method --> <code>( integer )</code>

#### mutation_armor {#sol::Character::mutation_armor}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), [DamageType](#sol::DamageType) ) -> number</code>

#### is_blind {#sol::Character::is_blind}

🇲 Method --> <code>( ) -> boolean</code>

#### get_movement_mode {#sol::Character::get_movement_mode}

🇲 Method --> <code>( ) -> [CharacterMoveMode](#sol::CharacterMoveMode)</code>

#### mabuff_block_bonus {#sol::Character::mabuff_block_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### mabuff_dodge_bonus {#sol::Character::mabuff_dodge_bonus}

🇲 Method --> <code>( ) -> number</code>

#### mabuff_speed_bonus {#sol::Character::mabuff_speed_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### mabuff_damage_mult {#sol::Character::mabuff_damage_mult}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> number</code>

#### mabuff_arpen_bonus {#sol::Character::mabuff_arpen_bonus}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> integer</code>

#### mabuff_damage_bonus {#sol::Character::mabuff_damage_bonus}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> integer</code>

#### mabuff_tohit_bonus {#sol::Character::mabuff_tohit_bonus}

🇲 Method --> <code>( ) -> number</code>

#### global_sm_location {#sol::Character::global_sm_location}

🇲 Method --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

#### hitall {#sol::Character::hitall}

🇲 Method --> <code>( integer, integer, [Creature](#sol::Creature) ) -> integer</code>

#### has_mabuff {#sol::Character::has_mabuff}

🇲 Method --> <code>( [MartialArtsBuffId](#sol::MartialArtsBuffId) ) -> boolean</code>

#### heal {#sol::Character::heal}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer )</code>

#### global_square_location {#sol::Character::global_square_location}

🇲 Method --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

#### healall {#sol::Character::healall}

🇲 Method --> <code>( integer )</code>

#### hurtall {#sol::Character::hurtall}

🇲 Method --> <code>( integer, [Creature](#sol::Creature), boolean )</code>

#### mabuff_attack_cost_penalty {#sol::Character::mabuff_attack_cost_penalty}

🇲 Method --> <code>( ) -> integer</code>

#### mutation_effect {#sol::Character::mutation_effect}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### remove_mutation {#sol::Character::remove_mutation}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId), boolean )</code>

#### mutate_towards {#sol::Character::mutate_towards}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId)[], integer ) -> boolean</code>\
🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### has_child_flag {#sol::Character::has_child_flag}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### get_lowest_hp {#sol::Character::get_lowest_hp}

🇲 Method --> <code>( ) -> integer</code>

#### remove_child_flag {#sol::Character::remove_child_flag}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### mabuff_attack_cost_mult {#sol::Character::mabuff_attack_cost_mult}

🇲 Method --> <code>( ) -> number</code>

#### mutate_towards {#sol::Character::mutate_towards}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### mutate_category {#sol::Character::mutate_category}

🇲 Method --> <code>( [MutationCategoryTraitId](#sol::MutationCategoryTraitId) )</code>

#### mutation_loss_effect {#sol::Character::mutation_loss_effect}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### mutate_towards {#sol::Character::mutate_towards}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId)[], integer ) -> boolean</code>

#### has_active_mutation {#sol::Character::has_active_mutation}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### mutation_ok {#sol::Character::mutation_ok}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId), boolean, boolean ) -> boolean</code>

#### mutate {#sol::Character::mutate}

🇲 Method --> <code>( )</code>

#### is_invisible {#sol::Character::is_invisible}

🇲 Method --> <code>( ) -> boolean</code>

#### can_run {#sol::Character::can_run}

🇲 Method --> <code>( ) -> boolean</code>

#### is_limb_disabled {#sol::Character::is_limb_disabled}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

#### mabuff_armor_bonus {#sol::Character::mabuff_armor_bonus}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> integer</code>

#### bionic_armor_bonus {#sol::Character::bionic_armor_bonus}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), [DamageType](#sol::DamageType) ) -> number</code>

#### has_base_trait {#sol::Character::has_base_trait}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### has_opposite_trait {#sol::Character::has_opposite_trait}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### has_trait_flag {#sol::Character::has_trait_flag}

🇲 Method --> <code>( [JsonTraitFlagId](#sol::JsonTraitFlagId) ) -> boolean</code>

#### set_mutation {#sol::Character::set_mutation}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### cough {#sol::Character::cough}

🇲 Method --> <code>( boolean, integer )</code>

#### uncanny_dodge {#sol::Character::uncanny_dodge}

🇲 Method --> <code>( ) -> boolean</code>

#### set_movement_mode {#sol::Character::set_movement_mode}

🇲 Method --> <code>( [CharacterMoveMode](#sol::CharacterMoveMode) )</code>

#### get_melee_stamina_cost {#sol::Character::get_melee_stamina_cost}

🇲 Method --> <code>( [Item](#sol::Item) ) -> integer</code>

#### expose_to_disease {#sol::Character::expose_to_disease}

🇲 Method --> <code>( [DiseaseTypeId](#sol::DiseaseTypeId) )</code>

#### is_stealthy {#sol::Character::is_stealthy}

🇲 Method --> <code>( ) -> boolean</code>

#### is_quiet {#sol::Character::is_quiet}

🇲 Method --> <code>( ) -> boolean</code>

#### is_limb_broken {#sol::Character::is_limb_broken}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

#### unset_mutation {#sol::Character::unset_mutation}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### deactivate_mutation {#sol::Character::deactivate_mutation}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### is_deaf {#sol::Character::is_deaf}

🇲 Method --> <code>( ) -> boolean</code>

#### forced_dismount {#sol::Character::forced_dismount}

🇲 Method --> <code>( )</code>

#### has_two_arms {#sol::Character::has_two_arms}

🇲 Method --> <code>( ) -> boolean</code>

#### get_working_leg_count {#sol::Character::get_working_leg_count}

🇲 Method --> <code>( ) -> integer</code>

#### get_working_arm_count {#sol::Character::get_working_arm_count}

🇲 Method --> <code>( ) -> integer</code>

#### activate_mutation {#sol::Character::activate_mutation}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) )</code>

#### dismount {#sol::Character::dismount}

🇲 Method --> <code>( )</code>

#### check_mount_will_move {#sol::Character::check_mount_will_move}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> boolean</code>

#### can_mount {#sol::Character::can_mount}

🇲 Method --> <code>( [Monster](#sol::Monster) ) -> boolean</code>

#### check_mount_is_spooked {#sol::Character::check_mount_is_spooked}

🇲 Method --> <code>( ) -> boolean</code>

#### mount_creature {#sol::Character::mount_creature}

🇲 Method --> <code>( [Monster](#sol::Monster) )</code>

#### is_mounted {#sol::Character::is_mounted}

🇲 Method --> <code>( ) -> boolean</code>

#### bodypart_exposure {#sol::Character::bodypart_exposure}

🇲 Method --> <code>( ) -> table<[BodyPartTypeIntId](#sol::BodyPartTypeIntId), number></code>

## CharacterId {#sol::CharacterId}

### Bases {#sol::CharacterId::@bases}

No base classes.

### Constructors {#sol::CharacterId::@ctors}

- CharacterId.new( )
- CharacterId.new( int )

### Members {#sol::CharacterId::@members}

#### is_valid {#sol::CharacterId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### get_value {#sol::CharacterId::get_value}

🇲 Method --> <code>( ) -> integer</code>

## Creature {#sol::Creature}

### Bases {#sol::Creature::@bases}

No base classes.

### Constructors {#sol::Creature::@ctors}

No constructors.

### Members {#sol::Creature::@members}

#### get_name {#sol::Creature::get_name}

🇲 Method --> <code>( ) -> string</code>

#### get_armor_cut_base {#sol::Creature::get_armor_cut_base}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_armor_bullet_base {#sol::Creature::get_armor_bullet_base}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_armor_bash_base {#sol::Creature::get_armor_bash_base}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_armor_bullet {#sol::Creature::get_armor_bullet}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_armor_cut {#sol::Creature::get_armor_cut}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_armor_bash_bonus {#sol::Creature::get_armor_bash_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_armor_bullet_bonus {#sol::Creature::get_armor_bullet_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_armor_cut_bonus {#sol::Creature::get_armor_cut_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_dodge {#sol::Creature::get_dodge}

🇲 Method --> <code>( ) -> number</code>

#### get_armor_type {#sol::Creature::get_armor_type}

🇲 Method --> <code>( [DamageType](#sol::DamageType), [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_melee {#sol::Creature::get_melee}

🇲 Method --> <code>( ) -> number</code>

#### get_armor_bash {#sol::Creature::get_armor_bash}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_num_dodges {#sol::Creature::get_num_dodges}

🇲 Method --> <code>( ) -> integer</code>

#### get_pain {#sol::Creature::get_pain}

🇲 Method --> <code>( ) -> integer</code>

#### set_pain {#sol::Creature::set_pain}

🇲 Method --> <code>( integer )</code>

#### mod_pain_noresist {#sol::Creature::mod_pain_noresist}

🇲 Method --> <code>( integer )</code>

#### get_env_resist {#sol::Creature::get_env_resist}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_perceived_pain {#sol::Creature::get_perceived_pain}

🇲 Method --> <code>( ) -> integer</code>

#### mod_moves {#sol::Creature::mod_moves}

🇲 Method --> <code>( integer )</code>

#### get_moves {#sol::Creature::get_moves}

🇲 Method --> <code>( ) -> integer</code>

#### get_num_blocks {#sol::Creature::get_num_blocks}

🇲 Method --> <code>( ) -> integer</code>

#### set_moves {#sol::Creature::set_moves}

🇲 Method --> <code>( integer )</code>

#### mod_pain {#sol::Creature::mod_pain}

🇲 Method --> <code>( integer )</code>

#### get_hit {#sol::Creature::get_hit}

🇲 Method --> <code>( ) -> number</code>

#### get_size {#sol::Creature::get_size}

🇲 Method --> <code>( ) -> [MonsterSize](#sol::MonsterSize)</code>

#### get_speed_bonus {#sol::Creature::get_speed_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_speed_mult {#sol::Creature::get_speed_mult}

🇲 Method --> <code>( ) -> number</code>

#### get_speed_base {#sol::Creature::get_speed_base}

🇲 Method --> <code>( ) -> integer</code>

#### set_all_parts_hp_to_max {#sol::Creature::set_all_parts_hp_to_max}

🇲 Method --> <code>( )</code>

#### set_all_parts_hp_cur {#sol::Creature::set_all_parts_hp_cur}

🇲 Method --> <code>( integer )</code>

#### get_block_bonus {#sol::Creature::get_block_bonus}

🇲 Method --> <code>( ) -> integer</code>

#### get_hit_base {#sol::Creature::get_hit_base}

🇲 Method --> <code>( ) -> number</code>

#### get_dodge_base {#sol::Creature::get_dodge_base}

🇲 Method --> <code>( ) -> number</code>

#### get_hit_bonus {#sol::Creature::get_hit_bonus}

🇲 Method --> <code>( ) -> number</code>

#### get_dodge_bonus {#sol::Creature::get_dodge_bonus}

🇲 Method --> <code>( ) -> number</code>

#### get_speed {#sol::Creature::get_speed}

🇲 Method --> <code>( ) -> integer</code>

#### mod_part_hp_max {#sol::Creature::mod_part_hp_max}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer )</code>

#### set_part_hp_max {#sol::Creature::set_part_hp_max}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer )</code>

#### hp_percentage {#sol::Creature::hp_percentage}

🇲 Method --> <code>( ) -> integer</code>

#### get_hp_max {#sol::Creature::get_hp_max}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_hp {#sol::Creature::get_hp}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### mod_part_hp_cur {#sol::Creature::mod_part_hp_cur}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer )</code>

#### has_flag {#sol::Creature::has_flag}

🇲 Method --> <code>( [MonsterFlag](#sol::MonsterFlag) ) -> boolean</code>

#### get_part_hp_max {#sol::Creature::get_part_hp_max}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### get_part_hp_cur {#sol::Creature::get_part_hp_cur}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### set_part_hp_cur {#sol::Creature::set_part_hp_cur}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer )</code>

#### get_part_healed_total {#sol::Creature::get_part_healed_total}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> integer</code>

#### has_grab_break_tec {#sol::Creature::has_grab_break_tec}

🇲 Method --> <code>( ) -> boolean</code>

#### has_trait {#sol::Creature::has_trait}

🇲 Method --> <code>( [MutationBranchId](#sol::MutationBranchId) ) -> boolean</code>

#### get_value {#sol::Creature::get_value}

🇲 Method --> <code>( string ) -> string</code>

#### sight_range {#sol::Creature::sight_range}

🇲 Method --> <code>( integer ) -> integer</code>

#### power_rating {#sol::Creature::power_rating}

🇲 Method --> <code>( ) -> number</code>

#### sees {#sol::Creature::sees}

🇲 Method --> <code>( [Creature](#sol::Creature) ) -> boolean</code>

#### attitude_to {#sol::Creature::attitude_to}

🇲 Method --> <code>( [Creature](#sol::Creature) ) -> [Attitude](#sol::Attitude)</code>

#### stability_roll {#sol::Creature::stability_roll}

🇲 Method --> <code>( ) -> number</code>

#### speed_rating {#sol::Creature::speed_rating}

🇲 Method --> <code>( ) -> number</code>

#### knock_back_to {#sol::Creature::knock_back_to}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) )</code>

#### ranged_target_size {#sol::Creature::ranged_target_size}

🇲 Method --> <code>( ) -> number</code>

#### apply_damage {#sol::Creature::apply_damage}

🇲 Method --> <code>( [Creature](#sol::Creature), [BodyPartTypeIntId](#sol::BodyPartTypeIntId), integer, boolean )</code>

#### deal_damage {#sol::Creature::deal_damage}

🇲 Method --> <code>( [Creature](#sol::Creature), [BodyPartTypeIntId](#sol::BodyPartTypeIntId), [DamageInstance](#sol::DamageInstance) ) -> [DealtDamageInstance](#sol::DealtDamageInstance)</code>

#### size_melee_penalty {#sol::Creature::size_melee_penalty}

🇲 Method --> <code>( ) -> integer</code>

#### dodge_roll {#sol::Creature::dodge_roll}

🇲 Method --> <code>( ) -> number</code>

#### as_character {#sol::Creature::as_character}

🇲 Method --> <code>( ) -> [Character](#sol::Character)</code>

#### get_grammatical_genders {#sol::Creature::get_grammatical_genders}

🇲 Method --> <code>( ) -> string[]</code>

#### skin_name {#sol::Creature::skin_name}

🇲 Method --> <code>( ) -> string</code>

#### disp_name {#sol::Creature::disp_name}

🇲 Method --> <code>( possessive: boolean, capitalize_first: boolean ) -> string</code>

#### as_avatar {#sol::Creature::as_avatar}

🇲 Method --> <code>( ) -> [Avatar](#sol::Avatar)</code>

#### is_avatar {#sol::Creature::is_avatar}

🇲 Method --> <code>( ) -> boolean</code>

#### is_monster {#sol::Creature::is_monster}

🇲 Method --> <code>( ) -> boolean</code>

#### is_npc {#sol::Creature::is_npc}

🇲 Method --> <code>( ) -> boolean</code>

#### as_npc {#sol::Creature::as_npc}

🇲 Method --> <code>( ) -> [Npc](#sol::Npc)</code>

#### as_monster {#sol::Creature::as_monster}

🇲 Method --> <code>( ) -> [Monster](#sol::Monster)</code>

#### get_weight {#sol::Creature::get_weight}

🇲 Method --> <code>( ) -> [Mass](#sol::Mass)</code>

#### digging {#sol::Creature::digging}

🇲 Method --> <code>( ) -> boolean</code>

#### is_underwater {#sol::Creature::is_underwater}

🇲 Method --> <code>( ) -> boolean</code>

#### get_effect_int {#sol::Creature::get_effect_int}

🇲 Method --> <code>( [EffectTypeId](#sol::EffectTypeId), [BodyPartTypeId](#sol::BodyPartTypeId) ) -> integer</code>

#### get_effect_dur {#sol::Creature::get_effect_dur}

🇲 Method --> <code>( [EffectTypeId](#sol::EffectTypeId), [BodyPartTypeId](#sol::BodyPartTypeId) ) -> [TimeDuration](#sol::TimeDuration)</code>

#### has_effect_with_flag {#sol::Creature::has_effect_with_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId), [BodyPartTypeId](#sol::BodyPartTypeId) ) -> boolean</code>

#### has_effect {#sol::Creature::has_effect}

🇲 Method --> <code>( [EffectTypeId](#sol::EffectTypeId), [BodyPartTypeId](#sol::BodyPartTypeId) ) -> boolean</code>

#### add_effect {#sol::Creature::add_effect}

🇲 Method --> <code>( [EffectTypeId](#sol::EffectTypeId), [TimeDuration](#sol::TimeDuration), [BodyPartTypeId](#sol::BodyPartTypeId), integer )</code>

> Effect type, duration, bodypart and intensity

#### clear_effects {#sol::Creature::clear_effects}

🇲 Method --> <code>( )</code>

#### remove_effect {#sol::Creature::remove_effect}

🇲 Method --> <code>( [EffectTypeId](#sol::EffectTypeId), [BodyPartTypeId](#sol::BodyPartTypeId) ) -> boolean</code>

#### remove_value {#sol::Creature::remove_value}

🇲 Method --> <code>( string )</code>

#### set_value {#sol::Creature::set_value}

🇲 Method --> <code>( string, string )</code>

#### is_on_ground {#sol::Creature::is_on_ground}

🇲 Method --> <code>( ) -> boolean</code>

#### set_pos_ms {#sol::Creature::set_pos_ms}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) )</code>

#### is_immune_damage {#sol::Creature::is_immune_damage}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> boolean</code>

#### in_species {#sol::Creature::in_species}

🇲 Method --> <code>( [SpeciesTypeId](#sol::SpeciesTypeId) ) -> boolean</code>

#### is_warm {#sol::Creature::is_warm}

🇲 Method --> <code>( ) -> boolean</code>

#### set_underwater {#sol::Creature::set_underwater}

🇲 Method --> <code>( boolean )</code>

#### get_pos_ms {#sol::Creature::get_pos_ms}

🇲 Method --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

#### has_weapon {#sol::Creature::has_weapon}

🇲 Method --> <code>( ) -> boolean</code>

#### is_dead {#sol::Creature::is_dead}

🇲 Method --> <code>( ) -> boolean</code>

#### is_hallucination {#sol::Creature::is_hallucination}

🇲 Method --> <code>( ) -> boolean</code>

#### is_immune_effect {#sol::Creature::is_immune_effect}

🇲 Method --> <code>( [EffectTypeId](#sol::EffectTypeId) ) -> boolean</code>

#### is_elec_immune {#sol::Creature::is_elec_immune}

🇲 Method --> <code>( ) -> boolean</code>

#### get_weight_capacity {#sol::Creature::get_weight_capacity}

🇲 Method --> <code>( ) -> integer</code>

## DamageInstance {#sol::DamageInstance}

> Represents a bunch of damage amounts\
> Constructors are:\
> new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)

### Bases {#sol::DamageInstance::@bases}

No base classes.

### Constructors {#sol::DamageInstance::@ctors}

- DamageInstance.new( )
- DamageInstance.new( [DamageType](#sol::DamageType), double, double, double, double )

### Members {#sol::DamageInstance::@members}

#### damage_units {#sol::DamageInstance::damage_units}

🇻 Variable --> <code>Vector( [DamageUnit](#sol::DamageUnit) )</code>

#### empty {#sol::DamageInstance::empty}

🇲 Method --> <code>( ) -> boolean</code>

#### add_damage {#sol::DamageInstance::add_damage}

🇲 Method --> <code>( [DamageType](#sol::DamageType), number, number, number, number )</code>

#### add {#sol::DamageInstance::add}

🇲 Method --> <code>( [DamageUnit](#sol::DamageUnit) )</code>

#### clear {#sol::DamageInstance::clear}

🇲 Method --> <code>( )</code>

#### type_damage {#sol::DamageInstance::type_damage}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> number</code>

#### total_damage {#sol::DamageInstance::total_damage}

🇲 Method --> <code>( ) -> number</code>

#### mult_damage {#sol::DamageInstance::mult_damage}

🇲 Method --> <code>( number, boolean )</code>

## DamageUnit {#sol::DamageUnit}

> Represents a damage amount\
> Constructors are:\
> new()\
> new(damageType, amount, armorPen, remainingArmorMultiplier, damageMultiplier)

### Bases {#sol::DamageUnit::@bases}

No base classes.

### Constructors {#sol::DamageUnit::@ctors}

- DamageUnit.new( [DamageType](#sol::DamageType), double, double, double, double )

### Members {#sol::DamageUnit::@members}

#### type {#sol::DamageUnit::type}

🇻 Variable --> <code>[DamageType](#sol::DamageType)</code>

#### res_mult {#sol::DamageUnit::res_mult}

🇻 Variable --> <code>double</code>

#### damage_multiplier {#sol::DamageUnit::damage_multiplier}

🇻 Variable --> <code>double</code>

#### res_pen {#sol::DamageUnit::res_pen}

🇻 Variable --> <code>double</code>

#### amount {#sol::DamageUnit::amount}

🇻 Variable --> <code>double</code>

## DealtDamageInstance {#sol::DealtDamageInstance}

> Represents the final dealt damage

### Bases {#sol::DealtDamageInstance::@bases}

No base classes.

### Constructors {#sol::DealtDamageInstance::@ctors}

No constructors.

### Members {#sol::DealtDamageInstance::@members}

#### dealt_dams {#sol::DealtDamageInstance::dealt_dams}

🇻 Variable --> <code>Array( int, 14 )</code>

#### bp_hit {#sol::DealtDamageInstance::bp_hit}

🇻 Variable --> <code>[BodyPartTypeId](#sol::BodyPartTypeId)</code>

#### type_damage {#sol::DealtDamageInstance::type_damage}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> integer</code>

#### total_damage {#sol::DealtDamageInstance::total_damage}

🇲 Method --> <code>( ) -> integer</code>

## DiseaseTypeId {#sol::DiseaseTypeId}

### Bases {#sol::DiseaseTypeId::@bases}

No base classes.

### Constructors {#sol::DiseaseTypeId::@ctors}

- DiseaseTypeId.new( )
- DiseaseTypeId.new( [DiseaseTypeId](#sol::DiseaseTypeId) )
- DiseaseTypeId.new( string )

### Members {#sol::DiseaseTypeId::@members}

#### NULL_ID {#sol::DiseaseTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [DiseaseTypeId](#sol::DiseaseTypeId)</code>

#### obj {#sol::DiseaseTypeId::obj}

🇲 Method --> <code>( ) -> DiseaseTypeRaw</code>

#### is_valid {#sol::DiseaseTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::DiseaseTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::DiseaseTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::DiseaseTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## DistributionGrid {#sol::DistributionGrid}

### Bases {#sol::DistributionGrid::@bases}

No base classes.

### Constructors {#sol::DistributionGrid::@ctors}

No constructors.

### Members {#sol::DistributionGrid::@members}

#### get_resource {#sol::DistributionGrid::get_resource}

🇲 Method --> <code>( boolean ) -> integer</code>

> Boolean argument controls recursive behavior

#### mod_resource {#sol::DistributionGrid::mod_resource}

🇲 Method --> <code>( integer, boolean ) -> integer</code>

> Boolean argument controls recursive behavior

## DistributionGridTracker {#sol::DistributionGridTracker}

### Bases {#sol::DistributionGridTracker::@bases}

No base classes.

### Constructors {#sol::DistributionGridTracker::@ctors}

No constructors.

### Members {#sol::DistributionGridTracker::@members}

#### get_grid_at_abs_ms {#sol::DistributionGridTracker::get_grid_at_abs_ms}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> [DistributionGrid](#sol::DistributionGrid)</code>

## EffectTypeId {#sol::EffectTypeId}

### Bases {#sol::EffectTypeId::@bases}

No base classes.

### Constructors {#sol::EffectTypeId::@ctors}

- EffectTypeId.new( )
- EffectTypeId.new( [EffectTypeId](#sol::EffectTypeId) )
- EffectTypeId.new( string )

### Members {#sol::EffectTypeId::@members}

#### NULL_ID {#sol::EffectTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [EffectTypeId](#sol::EffectTypeId)</code>

#### obj {#sol::EffectTypeId::obj}

🇲 Method --> <code>( ) -> EffectTypeRaw</code>

#### is_valid {#sol::EffectTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::EffectTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::EffectTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::EffectTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## Energy {#sol::Energy}

### Bases {#sol::Energy::@bases}

No base classes.

### Constructors {#sol::Energy::@ctors}

No constructors.

### Members {#sol::Energy::@members}

#### from_joule {#sol::Energy::from_joule}

🇫 Function --> <code>( integer ) -> [Energy](#sol::Energy)</code>

#### from_kilojoule {#sol::Energy::from_kilojoule}

🇫 Function --> <code>( integer ) -> [Energy](#sol::Energy)</code>

#### to_kilojoule {#sol::Energy::to_kilojoule}

🇲 Method --> <code>( ) -> integer</code>

#### to_joule {#sol::Energy::to_joule}

🇲 Method --> <code>( ) -> integer</code>

## FactionId {#sol::FactionId}

### Bases {#sol::FactionId::@bases}

No base classes.

### Constructors {#sol::FactionId::@ctors}

- FactionId.new( )
- FactionId.new( [FactionId](#sol::FactionId) )
- FactionId.new( string )

### Members {#sol::FactionId::@members}

#### NULL_ID {#sol::FactionId::NULL_ID}

🇫 Function --> <code>( ) -> [FactionId](#sol::FactionId)</code>

#### obj {#sol::FactionId::obj}

🇲 Method --> <code>( ) -> [FactionRaw](#sol::FactionRaw)</code>

#### is_valid {#sol::FactionId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::FactionId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::FactionId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::FactionId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## FactionRaw {#sol::FactionRaw}

### Bases {#sol::FactionRaw::@bases}

No base classes.

### Constructors {#sol::FactionRaw::@ctors}

No constructors.

### Members {#sol::FactionRaw::@members}

#### str_id {#sol::FactionRaw::str_id}

🇲 Method --> <code>( ) -> [FactionId](#sol::FactionId)</code>

## FieldTypeId {#sol::FieldTypeId}

### Bases {#sol::FieldTypeId::@bases}

No base classes.

### Constructors {#sol::FieldTypeId::@ctors}

- FieldTypeId.new( )
- FieldTypeId.new( [FieldTypeId](#sol::FieldTypeId) )
- FieldTypeId.new( [FieldTypeIntId](#sol::FieldTypeIntId) )
- FieldTypeId.new( string )

### Members {#sol::FieldTypeId::@members}

#### NULL_ID {#sol::FieldTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [FieldTypeId](#sol::FieldTypeId)</code>

#### str {#sol::FieldTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### obj {#sol::FieldTypeId::obj}

🇲 Method --> <code>( ) -> FieldTypeRaw</code>

#### is_null {#sol::FieldTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### is_valid {#sol::FieldTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::FieldTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

#### int_id {#sol::FieldTypeId::int_id}

🇲 Method --> <code>( ) -> [FieldTypeIntId](#sol::FieldTypeIntId)</code>

## FieldTypeIntId {#sol::FieldTypeIntId}

### Bases {#sol::FieldTypeIntId::@bases}

No base classes.

### Constructors {#sol::FieldTypeIntId::@ctors}

- FieldTypeIntId.new( )
- FieldTypeIntId.new( [FieldTypeIntId](#sol::FieldTypeIntId) )
- FieldTypeIntId.new( [FieldTypeId](#sol::FieldTypeId) )

### Members {#sol::FieldTypeIntId::@members}

#### obj {#sol::FieldTypeIntId::obj}

🇲 Method --> <code>( ) -> FieldTypeRaw</code>

#### is_valid {#sol::FieldTypeIntId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str_id {#sol::FieldTypeIntId::str_id}

🇲 Method --> <code>( ) -> [FieldTypeId](#sol::FieldTypeId)</code>

## FurnId {#sol::FurnId}

### Bases {#sol::FurnId::@bases}

No base classes.

### Constructors {#sol::FurnId::@ctors}

- FurnId.new( )
- FurnId.new( [FurnId](#sol::FurnId) )
- FurnId.new( [FurnIntId](#sol::FurnIntId) )
- FurnId.new( string )

### Members {#sol::FurnId::@members}

#### NULL_ID {#sol::FurnId::NULL_ID}

🇫 Function --> <code>( ) -> [FurnId](#sol::FurnId)</code>

#### str {#sol::FurnId::str}

🇲 Method --> <code>( ) -> string</code>

#### obj {#sol::FurnId::obj}

🇲 Method --> <code>( ) -> [FurnRaw](#sol::FurnRaw)</code>

#### is_null {#sol::FurnId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### is_valid {#sol::FurnId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::FurnId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

#### int_id {#sol::FurnId::int_id}

🇲 Method --> <code>( ) -> [FurnIntId](#sol::FurnIntId)</code>

## FurnIntId {#sol::FurnIntId}

### Bases {#sol::FurnIntId::@bases}

No base classes.

### Constructors {#sol::FurnIntId::@ctors}

- FurnIntId.new( )
- FurnIntId.new( [FurnIntId](#sol::FurnIntId) )
- FurnIntId.new( [FurnId](#sol::FurnId) )

### Members {#sol::FurnIntId::@members}

#### obj {#sol::FurnIntId::obj}

🇲 Method --> <code>( ) -> [FurnRaw](#sol::FurnRaw)</code>

#### is_valid {#sol::FurnIntId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str_id {#sol::FurnIntId::str_id}

🇲 Method --> <code>( ) -> [FurnId](#sol::FurnId)</code>

## FurnRaw {#sol::FurnRaw}

### Bases {#sol::FurnRaw::@bases}

No base classes.

### Constructors {#sol::FurnRaw::@ctors}

No constructors.

### Members {#sol::FurnRaw::@members}

#### transforms_into {#sol::FurnRaw::transforms_into}

🇻 Variable --> <code>[FurnId](#sol::FurnId)</code>

#### open {#sol::FurnRaw::open}

🇻 Variable --> <code>[FurnId](#sol::FurnId)</code>

#### close {#sol::FurnRaw::close}

🇻 Variable --> <code>[FurnId](#sol::FurnId)</code>

#### set_movecost {#sol::FurnRaw::set_movecost}

🇲 Method --> <code>( integer )</code>

#### set_max_volume {#sol::FurnRaw::set_max_volume}

🇲 Method --> <code>( [Volume](#sol::Volume) )</code>

#### get_coverage {#sol::FurnRaw::get_coverage}

🇲 Method --> <code>( ) -> integer</code>

#### set_coverage {#sol::FurnRaw::set_coverage}

🇲 Method --> <code>( integer )</code>

#### get_max_volume {#sol::FurnRaw::get_max_volume}

🇲 Method --> <code>( ) -> [Volume](#sol::Volume)</code>

#### get_movecost {#sol::FurnRaw::get_movecost}

🇲 Method --> <code>( ) -> integer</code>

#### get_light_emitted {#sol::FurnRaw::get_light_emitted}

🇲 Method --> <code>( ) -> integer</code>

#### name {#sol::FurnRaw::name}

🇲 Method --> <code>( ) -> string</code>

#### int_id {#sol::FurnRaw::int_id}

🇲 Method --> <code>( ) -> [FurnIntId](#sol::FurnIntId)</code>

#### set_light_emitted {#sol::FurnRaw::set_light_emitted}

🇲 Method --> <code>( integer )</code>

#### get_flags {#sol::FurnRaw::get_flags}

🇲 Method --> <code>( ) -> string[]</code>

#### set_flag {#sol::FurnRaw::set_flag}

🇲 Method --> <code>( string )</code>

#### has_flag {#sol::FurnRaw::has_flag}

🇲 Method --> <code>( string ) -> boolean</code>

#### str_id {#sol::FurnRaw::str_id}

🇲 Method --> <code>( ) -> [FurnId](#sol::FurnId)</code>

## Item {#sol::Item}

### Bases {#sol::Item::@bases}

No base classes.

### Constructors {#sol::Item::@ctors}

No constructors.

### Members {#sol::Item::@members}

#### charges {#sol::Item::charges}

🇻 Variable --> <code>int</code>

#### get_type {#sol::Item::get_type}

🇲 Method --> <code>( ) -> [ItypeId](#sol::ItypeId)</code>

#### get_owner {#sol::Item::get_owner}

🇲 Method --> <code>( ) -> [FactionId](#sol::FactionId)</code>

> Gets the faction id that owns this item

#### get_category_id {#sol::Item::get_category_id}

🇲 Method --> <code>( ) -> string</code>

> Gets the category id this item is in

#### get_comestible_fun {#sol::Item::get_comestible_fun}

🇲 Method --> <code>( ) -> integer</code>

#### get_rot {#sol::Item::get_rot}

🇲 Method --> <code>( ) -> [TimeDuration](#sol::TimeDuration)</code>

> Gets the TimeDuration until this item rots

#### get_quench {#sol::Item::get_quench}

🇲 Method --> <code>( ) -> integer</code>

#### set_owner {#sol::Item::set_owner}

🇲 Method --> <code>( [FactionId](#sol::FactionId) )</code>

> Sets the ownership of this item to a faction

#### get_owner_name {#sol::Item::get_owner_name}

🇲 Method --> <code>( ) -> string</code>

#### add_technique {#sol::Item::add_technique}

🇲 Method --> <code>( [MartialArtsTechniqueId](#sol::MartialArtsTechniqueId) )</code>

> Adds the technique. It isn't treated original, but additional.

#### set_owner {#sol::Item::set_owner}

🇲 Method --> <code>( [Character](#sol::Character) )</code>

> Sets the ownership of this item to a character

#### get_techniques {#sol::Item::get_techniques}

🇲 Method --> <code>( ) -> [MartialArtsTechniqueId](#sol::MartialArtsTechniqueId)[]</code>

> Gets all techniques. Including original techniques.

#### is_owned_by {#sol::Item::is_owned_by}

🇲 Method --> <code>( [Character](#sol::Character), boolean ) -> boolean</code>

> Checks if this item owned by a character

#### has_technique {#sol::Item::has_technique}

🇲 Method --> <code>( [MartialArtsTechniqueId](#sol::MartialArtsTechniqueId) ) -> boolean</code>

> Checks if this item has the technique as an addition. Doesn't check original techniques.

#### get_kcal {#sol::Item::get_kcal}

🇲 Method --> <code>( ) -> integer</code>

#### is_made_of {#sol::Item::is_made_of}

🇲 Method --> <code>( [MaterialTypeId](#sol::MaterialTypeId) ) -> boolean</code>

#### made_of {#sol::Item::made_of}

🇲 Method --> <code>( ) -> [MaterialTypeId](#sol::MaterialTypeId)[]</code>

#### is_melee {#sol::Item::is_melee}

🇲 Method --> <code>( [DamageType](#sol::DamageType) ) -> boolean</code>

> Is this item an effective melee weapon for the given damage type?

#### is_magazine {#sol::Item::is_magazine}

🇲 Method --> <code>( ) -> boolean</code>

> Is this a magazine? (batteries are magazines)

#### deactivate {#sol::Item::deactivate}

🇲 Method --> <code>( )</code>

#### is_upgrade {#sol::Item::is_upgrade}

🇲 Method --> <code>( ) -> boolean</code>

#### activate {#sol::Item::activate}

🇲 Method --> <code>( )</code>

#### remove_technique {#sol::Item::remove_technique}

🇲 Method --> <code>( [MartialArtsTechniqueId](#sol::MartialArtsTechniqueId) )</code>

> Removes the additional technique. Doesn't affect originial techniques.

#### is_stackable {#sol::Item::is_stackable}

🇲 Method --> <code>( ) -> boolean</code>

#### mod_charges {#sol::Item::mod_charges}

🇲 Method --> <code>( integer )</code>

#### conductive {#sol::Item::conductive}

🇲 Method --> <code>( ) -> boolean</code>

#### has_infinite_charges {#sol::Item::has_infinite_charges}

🇲 Method --> <code>( ) -> boolean</code>

#### energy_remaining {#sol::Item::energy_remaining}

🇲 Method --> <code>( ) -> [Energy](#sol::Energy)</code>

#### is_active {#sol::Item::is_active}

🇲 Method --> <code>( ) -> boolean</code>

#### can_contain {#sol::Item::can_contain}

🇲 Method --> <code>( [Item](#sol::Item) ) -> boolean</code>

> Checks if this item can contain another

#### total_capacity {#sol::Item::total_capacity}

🇲 Method --> <code>( ) -> [Volume](#sol::Volume)</code>

> Gets maximum volume this item can hold (liquids, ammo, etc)

#### unset_flags {#sol::Item::unset_flags}

🇲 Method --> <code>( )</code>

#### convert {#sol::Item::convert}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId) )</code>

> Converts the item as given `ItypeId`.

#### set_flag_recursive {#sol::Item::set_flag_recursive}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId) )</code>

#### has_flag {#sol::Item::has_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId) ) -> boolean</code>

#### has_own_flag {#sol::Item::has_own_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId) ) -> boolean</code>

#### unset_flag {#sol::Item::unset_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId) )</code>

#### get_var_str {#sol::Item::get_var_str}

🇲 Method --> <code>( string, string ) -> string</code>

> Get variable as string

#### get_var_tri {#sol::Item::get_var_tri}

🇲 Method --> <code>( string, [Tripoint](#sol::Tripoint) ) -> [Tripoint](#sol::Tripoint)</code>

> Get variable as tripoint

#### set_var_tri {#sol::Item::set_var_tri}

🇲 Method --> <code>( string, [Tripoint](#sol::Tripoint) )</code>

#### get_var_num {#sol::Item::get_var_num}

🇲 Method --> <code>( string, number ) -> number</code>

> Get variable as float number

#### set_var_num {#sol::Item::set_var_num}

🇲 Method --> <code>( string, number )</code>

#### set_var_str {#sol::Item::set_var_str}

🇲 Method --> <code>( string, string )</code>

#### remaining_capacity_for_id {#sol::Item::remaining_capacity_for_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), boolean ) -> integer</code>

> Gets the remaining space available for a type of liquid

#### set_flag {#sol::Item::set_flag}

🇲 Method --> <code>( [JsonFlagId](#sol::JsonFlagId) )</code>

#### has_item_with_id {#sol::Item::has_item_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId) ) -> boolean</code>

> Checks item contents for a given item id

#### ammo_data {#sol::Item::ammo_data}

🇲 Method --> <code>( ) -> ItypeRaw</code>

#### ammo_required {#sol::Item::ammo_required}

🇲 Method --> <code>( ) -> integer</code>

#### ammo_remaining {#sol::Item::ammo_remaining}

🇲 Method --> <code>( ) -> integer</code>

> Get remaining ammo, works with batteries & stuff too

#### current_magazine {#sol::Item::current_magazine}

🇲 Method --> <code>( ) -> [Item](#sol::Item)</code>

> Gets the current magazine

#### ammo_capacity {#sol::Item::ammo_capacity}

🇲 Method --> <code>( boolean ) -> integer</code>

> Gets the maximum capacity of a magazine

#### covers {#sol::Item::covers}

🇲 Method --> <code>( [BodyPartTypeIntId](#sol::BodyPartTypeIntId) ) -> boolean</code>

> Checks if the item covers a bodypart

#### ammo_current {#sol::Item::ammo_current}

🇲 Method --> <code>( ) -> [ItypeId](#sol::ItypeId)</code>

#### ammo_set {#sol::Item::ammo_set}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), integer )</code>

#### add_item_with_id {#sol::Item::add_item_with_id}

🇲 Method --> <code>( [ItypeId](#sol::ItypeId), integer )</code>

> Adds an item(s) to contents

#### ammo_consume {#sol::Item::ammo_consume}

🇲 Method --> <code>( integer, [Tripoint](#sol::Tripoint) ) -> integer</code>

#### get_reload_time {#sol::Item::get_reload_time}

🇲 Method --> <code>( ) -> integer</code>

#### ammo_unset {#sol::Item::ammo_unset}

🇲 Method --> <code>( )</code>

#### attack_cost {#sol::Item::attack_cost}

🇲 Method --> <code>( ) -> integer</code>

#### is_soft {#sol::Item::is_soft}

🇲 Method --> <code>( ) -> boolean</code>

#### is_bionic {#sol::Item::is_bionic}

🇲 Method --> <code>( ) -> boolean</code>

#### is_ammo_belt {#sol::Item::is_ammo_belt}

🇲 Method --> <code>( ) -> boolean</code>

#### is_gunmod {#sol::Item::is_gunmod}

🇲 Method --> <code>( ) -> boolean</code>

#### is_firearm {#sol::Item::is_firearm}

🇲 Method --> <code>( ) -> boolean</code>

#### is_silent {#sol::Item::is_silent}

🇲 Method --> <code>( ) -> boolean</code>

#### is_gun {#sol::Item::is_gun}

🇲 Method --> <code>( ) -> boolean</code>

#### is_bandolier {#sol::Item::is_bandolier}

🇲 Method --> <code>( ) -> boolean</code>

#### is_ammo {#sol::Item::is_ammo}

🇲 Method --> <code>( ) -> boolean</code>

#### is_brewable {#sol::Item::is_brewable}

🇲 Method --> <code>( ) -> boolean</code>

#### is_holster {#sol::Item::is_holster}

🇲 Method --> <code>( ) -> boolean</code>

#### is_medication {#sol::Item::is_medication}

🇲 Method --> <code>( ) -> boolean</code>

#### is_comestible {#sol::Item::is_comestible}

🇲 Method --> <code>( ) -> boolean</code>

#### is_food {#sol::Item::is_food}

🇲 Method --> <code>( ) -> boolean</code>

#### is_food_container {#sol::Item::is_food_container}

🇲 Method --> <code>( ) -> boolean</code>

#### is_money {#sol::Item::is_money}

🇲 Method --> <code>( ) -> boolean</code>

#### is_sided {#sol::Item::is_sided}

🇲 Method --> <code>( ) -> boolean</code>

#### weight {#sol::Item::weight}

🇲 Method --> <code>( boolean, boolean ) -> [Mass](#sol::Mass)</code>

> Weight of the item. The first `bool` is whether including contents, second `bool` is whether it is `integral_weight`.

#### volume {#sol::Item::volume}

🇲 Method --> <code>( boolean ) -> [Volume](#sol::Volume)</code>

> Volume of the item. `bool` is whether it is `integral_volume`.

#### display_name {#sol::Item::display_name}

🇲 Method --> <code>( integer ) -> string</code>

> Display name with all bells and whistles like ammo and prefixes

#### get_mtype {#sol::Item::get_mtype}

🇲 Method --> <code>( ) -> [MtypeId](#sol::MtypeId)</code>

> Almost for a corpse.

#### tname {#sol::Item::tname}

🇲 Method --> <code>( integer, boolean, integer ) -> string</code>

> Translated item name with prefixes

#### is_power_armor {#sol::Item::is_power_armor}

🇲 Method --> <code>( ) -> boolean</code>

#### price {#sol::Item::price}

🇲 Method --> <code>( boolean ) -> number</code>

> Cents of the item. `bool` is whether it is a post-cataclysm value.

#### erase_var {#sol::Item::erase_var}

🇲 Method --> <code>( string )</code>

> Erase variable

#### is_unarmed_weapon {#sol::Item::is_unarmed_weapon}

🇲 Method --> <code>( ) -> boolean</code>

#### has_var {#sol::Item::has_var}

🇲 Method --> <code>( string ) -> boolean</code>

> Check for variable of any type

#### is_null {#sol::Item::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### clear_vars {#sol::Item::clear_vars}

🇲 Method --> <code>( )</code>

> Erase all variables

#### is_reloadable {#sol::Item::is_reloadable}

🇲 Method --> <code>( ) -> boolean</code>

#### is_med_container {#sol::Item::is_med_container}

🇲 Method --> <code>( ) -> boolean</code>

#### is_ammo_container {#sol::Item::is_ammo_container}

🇲 Method --> <code>( ) -> boolean</code>

#### is_deployable {#sol::Item::is_deployable}

🇲 Method --> <code>( ) -> boolean</code>

#### is_tool {#sol::Item::is_tool}

🇲 Method --> <code>( ) -> boolean</code>

#### is_emissive {#sol::Item::is_emissive}

🇲 Method --> <code>( ) -> boolean</code>

#### is_salvageable {#sol::Item::is_salvageable}

🇲 Method --> <code>( boolean ) -> boolean</code>

#### is_craft {#sol::Item::is_craft}

🇲 Method --> <code>( ) -> boolean</code>

#### is_container_empty {#sol::Item::is_container_empty}

🇲 Method --> <code>( ) -> boolean</code>

#### is_transformable {#sol::Item::is_transformable}

🇲 Method --> <code>( ) -> boolean</code>

#### is_relic {#sol::Item::is_relic}

🇲 Method --> <code>( ) -> boolean</code>

#### is_tainted {#sol::Item::is_tainted}

🇲 Method --> <code>( ) -> boolean</code>

#### is_artifact {#sol::Item::is_artifact}

🇲 Method --> <code>( ) -> boolean</code>

#### is_dangerous {#sol::Item::is_dangerous}

🇲 Method --> <code>( ) -> boolean</code>

#### is_seed {#sol::Item::is_seed}

🇲 Method --> <code>( ) -> boolean</code>

#### is_corpse {#sol::Item::is_corpse}

🇲 Method --> <code>( ) -> boolean</code>

#### is_irremovable {#sol::Item::is_irremovable}

🇲 Method --> <code>( ) -> boolean</code>

#### is_toolmod {#sol::Item::is_toolmod}

🇲 Method --> <code>( ) -> boolean</code>

#### is_container {#sol::Item::is_container}

🇲 Method --> <code>( ) -> boolean</code>

#### is_watertight_container {#sol::Item::is_watertight_container}

🇲 Method --> <code>( ) -> boolean</code>

#### is_map {#sol::Item::is_map}

🇲 Method --> <code>( ) -> boolean</code>

#### is_armor {#sol::Item::is_armor}

🇲 Method --> <code>( ) -> boolean</code>

#### is_book {#sol::Item::is_book}

🇲 Method --> <code>( ) -> boolean</code>

#### is_faulty {#sol::Item::is_faulty}

🇲 Method --> <code>( ) -> boolean</code>

#### is_non_resealable_container {#sol::Item::is_non_resealable_container}

🇲 Method --> <code>( ) -> boolean</code>

#### is_bucket_nonempty {#sol::Item::is_bucket_nonempty}

🇲 Method --> <code>( ) -> boolean</code>

#### is_fuel {#sol::Item::is_fuel}

🇲 Method --> <code>( ) -> boolean</code>

#### is_bucket {#sol::Item::is_bucket}

🇲 Method --> <code>( ) -> boolean</code>

#### is_wheel {#sol::Item::is_wheel}

🇲 Method --> <code>( ) -> boolean</code>

#### is_engine {#sol::Item::is_engine}

🇲 Method --> <code>( ) -> boolean</code>

#### stamina_cost {#sol::Item::stamina_cost}

🇲 Method --> <code>( ) -> integer</code>

## ItemStack {#sol::ItemStack}

> Iterate over this using pairs()

### Bases {#sol::ItemStack::@bases}

No base classes.

### Constructors {#sol::ItemStack::@ctors}

No constructors.

### Members {#sol::ItemStack::@members}

## ItypeId {#sol::ItypeId}

### Bases {#sol::ItypeId::@bases}

No base classes.

### Constructors {#sol::ItypeId::@ctors}

- ItypeId.new( )
- ItypeId.new( [ItypeId](#sol::ItypeId) )
- ItypeId.new( string )

### Members {#sol::ItypeId::@members}

#### NULL_ID {#sol::ItypeId::NULL_ID}

🇫 Function --> <code>( ) -> [ItypeId](#sol::ItypeId)</code>

#### obj {#sol::ItypeId::obj}

🇲 Method --> <code>( ) -> ItypeRaw</code>

#### is_valid {#sol::ItypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::ItypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::ItypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::ItypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## JsonFlagId {#sol::JsonFlagId}

### Bases {#sol::JsonFlagId::@bases}

No base classes.

### Constructors {#sol::JsonFlagId::@ctors}

- JsonFlagId.new( )
- JsonFlagId.new( [JsonFlagId](#sol::JsonFlagId) )
- JsonFlagId.new( string )

### Members {#sol::JsonFlagId::@members}

#### NULL_ID {#sol::JsonFlagId::NULL_ID}

🇫 Function --> <code>( ) -> [JsonFlagId](#sol::JsonFlagId)</code>

#### obj {#sol::JsonFlagId::obj}

🇲 Method --> <code>( ) -> JsonFlagRaw</code>

#### is_valid {#sol::JsonFlagId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::JsonFlagId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::JsonFlagId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::JsonFlagId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## JsonTraitFlagId {#sol::JsonTraitFlagId}

### Bases {#sol::JsonTraitFlagId::@bases}

No base classes.

### Constructors {#sol::JsonTraitFlagId::@ctors}

- JsonTraitFlagId.new( )
- JsonTraitFlagId.new( [JsonTraitFlagId](#sol::JsonTraitFlagId) )
- JsonTraitFlagId.new( string )

### Members {#sol::JsonTraitFlagId::@members}

#### NULL_ID {#sol::JsonTraitFlagId::NULL_ID}

🇫 Function --> <code>( ) -> [JsonTraitFlagId](#sol::JsonTraitFlagId)</code>

#### obj {#sol::JsonTraitFlagId::obj}

🇲 Method --> <code>( ) -> JsonTraitFlagRaw</code>

#### is_valid {#sol::JsonTraitFlagId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::JsonTraitFlagId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::JsonTraitFlagId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::JsonTraitFlagId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## Map {#sol::Map}

### Bases {#sol::Map::@bases}

No base classes.

### Constructors {#sol::Map::@ctors}

No constructors.

### Members {#sol::Map::@members}

#### get_abs_ms {#sol::Map::get_abs_ms}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> [Tripoint](#sol::Tripoint)</code>

> Convert local ms -> absolute ms

#### mod_field_int_at {#sol::Map::mod_field_int_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId), integer ) -> integer</code>

#### mod_field_age_at {#sol::Map::mod_field_age_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId), [TimeDuration](#sol::TimeDuration) ) -> [TimeDuration](#sol::TimeDuration)</code>

#### get_field_age_at {#sol::Map::get_field_age_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId) ) -> [TimeDuration](#sol::TimeDuration)</code>

#### get_field_int_at {#sol::Map::get_field_int_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId) ) -> integer</code>

#### has_field_at {#sol::Map::has_field_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId) ) -> boolean</code>

#### set_field_int_at {#sol::Map::set_field_int_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId), integer, boolean ) -> integer</code>

#### add_field_at {#sol::Map::add_field_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId), integer, [TimeDuration](#sol::TimeDuration) ) -> boolean</code>

#### set_trap_at {#sol::Map::set_trap_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [TrapIntId](#sol::TrapIntId) )</code>

> Set a trap at a position on the map. It can also replace existing trap, even with `trap_null`.

#### set_field_age_at {#sol::Map::set_field_age_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId), [TimeDuration](#sol::TimeDuration), boolean ) -> [TimeDuration](#sol::TimeDuration)</code>

#### get_trap_at {#sol::Map::get_trap_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> [TrapIntId](#sol::TrapIntId)</code>

#### remove_field_at {#sol::Map::remove_field_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FieldTypeIntId](#sol::FieldTypeIntId) )</code>

#### disarm_trap_at {#sol::Map::disarm_trap_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) )</code>

> Disarms a trap using your skills and stats, with consequences depending on success or failure.

#### set_furn_at {#sol::Map::set_furn_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [FurnIntId](#sol::FurnIntId) )</code>

#### set_ter_at {#sol::Map::set_ter_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [TerIntId](#sol::TerIntId) ) -> boolean</code>

#### get_map_size {#sol::Map::get_map_size}

🇲 Method --> <code>( ) -> integer</code>

> In map squares

#### create_item_at {#sol::Map::create_item_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [ItypeId](#sol::ItypeId), integer )</code>

> Creates a new item(s) at a position on the map.

#### get_map_size_in_submaps {#sol::Map::get_map_size_in_submaps}

🇲 Method --> <code>( ) -> integer</code>

#### get_local_ms {#sol::Map::get_local_ms}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> [Tripoint](#sol::Tripoint)</code>

> Convert absolute ms -> local ms

#### get_furn_at {#sol::Map::get_furn_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> [FurnIntId](#sol::FurnIntId)</code>

#### create_corpse_at {#sol::Map::create_corpse_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [MtypeId](#sol::MtypeId), [TimePoint](#sol::TimePoint), string, integer )</code>

> Creates a new corpse at a position on the map. You can skip `Opt` ones by omitting them or passing `nil`. `MtypeId` specifies which monster's body it is, `TimePoint` indicates when it died, `string` gives it a custom name, and `int` determines the revival time if the monster has the `REVIVES` flag.

#### get_items_at {#sol::Map::get_items_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> any</code>

#### get_ter_at {#sol::Map::get_ter_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> [TerIntId](#sol::TerIntId)</code>

#### has_items_at {#sol::Map::has_items_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) ) -> boolean</code>

#### clear_items_at {#sol::Map::clear_items_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) )</code>

#### remove_item_at {#sol::Map::remove_item_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), [Item](#sol::Item) )</code>

#### remove_trap_at {#sol::Map::remove_trap_at}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) )</code>

> Simpler version of `set_trap_at` with `trap_null`.

## MapStack {#sol::MapStack}

### Bases {#sol::MapStack::@bases}

- `ItemStack`

### Constructors {#sol::MapStack::@ctors}

No constructors.

### Members {#sol::MapStack::@members}

#### as_item_stack {#sol::MapStack::as_item_stack}

🇲 Method --> <code>( ) -> [ItemStack](#sol::ItemStack)</code>

## MartialArtsBuffId {#sol::MartialArtsBuffId}

### Bases {#sol::MartialArtsBuffId::@bases}

No base classes.

### Constructors {#sol::MartialArtsBuffId::@ctors}

- MartialArtsBuffId.new( )
- MartialArtsBuffId.new( [MartialArtsBuffId](#sol::MartialArtsBuffId) )
- MartialArtsBuffId.new( string )

### Members {#sol::MartialArtsBuffId::@members}

#### NULL_ID {#sol::MartialArtsBuffId::NULL_ID}

🇫 Function --> <code>( ) -> [MartialArtsBuffId](#sol::MartialArtsBuffId)</code>

#### obj {#sol::MartialArtsBuffId::obj}

🇲 Method --> <code>( ) -> MartialArtsBuffRaw</code>

#### is_valid {#sol::MartialArtsBuffId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MartialArtsBuffId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MartialArtsBuffId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MartialArtsBuffId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## MartialArtsTechniqueId {#sol::MartialArtsTechniqueId}

### Bases {#sol::MartialArtsTechniqueId::@bases}

No base classes.

### Constructors {#sol::MartialArtsTechniqueId::@ctors}

- MartialArtsTechniqueId.new( )
- MartialArtsTechniqueId.new( [MartialArtsTechniqueId](#sol::MartialArtsTechniqueId) )
- MartialArtsTechniqueId.new( string )

### Members {#sol::MartialArtsTechniqueId::@members}

#### NULL_ID {#sol::MartialArtsTechniqueId::NULL_ID}

🇫 Function --> <code>( ) -> [MartialArtsTechniqueId](#sol::MartialArtsTechniqueId)</code>

#### obj {#sol::MartialArtsTechniqueId::obj}

🇲 Method --> <code>( ) -> [MartialArtsTechniqueRaw](#sol::MartialArtsTechniqueRaw)</code>

#### is_valid {#sol::MartialArtsTechniqueId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MartialArtsTechniqueId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MartialArtsTechniqueId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MartialArtsTechniqueId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## MartialArtsTechniqueRaw {#sol::MartialArtsTechniqueRaw}

### Bases {#sol::MartialArtsTechniqueRaw::@bases}

No base classes.

### Constructors {#sol::MartialArtsTechniqueRaw::@ctors}

No constructors.

### Members {#sol::MartialArtsTechniqueRaw::@members}

#### name {#sol::MartialArtsTechniqueRaw::name}

🇫 Function --> <code>( )</code>

#### knockback_follow {#sol::MartialArtsTechniqueRaw::knockback_follow}

🇫 Function --> <code>( )</code>

#### crit_ok {#sol::MartialArtsTechniqueRaw::crit_ok}

🇫 Function --> <code>( )</code>

#### crit_tec {#sol::MartialArtsTechniqueRaw::crit_tec}

🇫 Function --> <code>( )</code>

#### powerful_knockback {#sol::MartialArtsTechniqueRaw::powerful_knockback}

🇫 Function --> <code>( )</code>

#### disarms {#sol::MartialArtsTechniqueRaw::disarms}

🇫 Function --> <code>( )</code>

#### dodge_counter {#sol::MartialArtsTechniqueRaw::dodge_counter}

🇫 Function --> <code>( )</code>

#### take_weapon {#sol::MartialArtsTechniqueRaw::take_weapon}

🇫 Function --> <code>( )</code>

#### block_counter {#sol::MartialArtsTechniqueRaw::block_counter}

🇫 Function --> <code>( )</code>

#### miss_recovery {#sol::MartialArtsTechniqueRaw::miss_recovery}

🇫 Function --> <code>( )</code>

#### knockback_spread {#sol::MartialArtsTechniqueRaw::knockback_spread}

🇫 Function --> <code>( )</code>

#### stun_dur {#sol::MartialArtsTechniqueRaw::stun_dur}

🇫 Function --> <code>( )</code>

#### avatar_message {#sol::MartialArtsTechniqueRaw::avatar_message}

🇫 Function --> <code>( )</code>

#### get_description {#sol::MartialArtsTechniqueRaw::get_description}

🇲 Method --> <code>( ) -> string</code>

#### knockback_dist {#sol::MartialArtsTechniqueRaw::knockback_dist}

🇫 Function --> <code>( )</code>

#### npc_message {#sol::MartialArtsTechniqueRaw::npc_message}

🇫 Function --> <code>( )</code>

#### side_switch {#sol::MartialArtsTechniqueRaw::side_switch}

🇫 Function --> <code>( )</code>

#### defensive {#sol::MartialArtsTechniqueRaw::defensive}

🇫 Function --> <code>( )</code>

#### down_dur {#sol::MartialArtsTechniqueRaw::down_dur}

🇫 Function --> <code>( )</code>

#### grab_break {#sol::MartialArtsTechniqueRaw::grab_break}

🇫 Function --> <code>( )</code>

## Mass {#sol::Mass}

### Bases {#sol::Mass::@bases}

No base classes.

### Constructors {#sol::Mass::@ctors}

No constructors.

### Members {#sol::Mass::@members}

#### from_milligram {#sol::Mass::from_milligram}

🇫 Function --> <code>( integer ) -> [Mass](#sol::Mass)</code>

#### from_newton {#sol::Mass::from_newton}

🇫 Function --> <code>( integer ) -> [Mass](#sol::Mass)</code>

#### to_newton {#sol::Mass::to_newton}

🇲 Method --> <code>( ) -> integer</code>

#### from_kilogram {#sol::Mass::from_kilogram}

🇫 Function --> <code>( integer ) -> [Mass](#sol::Mass)</code>

#### to_kilogram {#sol::Mass::to_kilogram}

🇲 Method --> <code>( ) -> integer</code>

#### to_milligram {#sol::Mass::to_milligram}

🇲 Method --> <code>( ) -> integer</code>

#### to_gram {#sol::Mass::to_gram}

🇲 Method --> <code>( ) -> integer</code>

#### from_gram {#sol::Mass::from_gram}

🇫 Function --> <code>( integer ) -> [Mass](#sol::Mass)</code>

## MaterialTypeId {#sol::MaterialTypeId}

### Bases {#sol::MaterialTypeId::@bases}

No base classes.

### Constructors {#sol::MaterialTypeId::@ctors}

- MaterialTypeId.new( )
- MaterialTypeId.new( [MaterialTypeId](#sol::MaterialTypeId) )
- MaterialTypeId.new( string )

### Members {#sol::MaterialTypeId::@members}

#### NULL_ID {#sol::MaterialTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [MaterialTypeId](#sol::MaterialTypeId)</code>

#### obj {#sol::MaterialTypeId::obj}

🇲 Method --> <code>( ) -> [MaterialTypeRaw](#sol::MaterialTypeRaw)</code>

#### is_valid {#sol::MaterialTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MaterialTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MaterialTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MaterialTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## MaterialTypeRaw {#sol::MaterialTypeRaw}

### Bases {#sol::MaterialTypeRaw::@bases}

No base classes.

### Constructors {#sol::MaterialTypeRaw::@ctors}

No constructors.

### Members {#sol::MaterialTypeRaw::@members}

#### str_id {#sol::MaterialTypeRaw::str_id}

🇲 Method --> <code>( ) -> [MaterialTypeId](#sol::MaterialTypeId)</code>

#### name {#sol::MaterialTypeRaw::name}

🇲 Method --> <code>( ) -> string</code>

## Mission {#sol::Mission}

### Bases {#sol::Mission::@bases}

No base classes.

### Constructors {#sol::Mission::@ctors}

- Mission.new( )

### Members {#sol::Mission::@members}

#### name {#sol::Mission::name}

🇲 Method --> <code>( ) -> string</code>

> Returns the mission's name as a string.

#### fail {#sol::Mission::fail}

🇲 Method --> <code>( )</code>

> Fails the mission.

#### wrap_up {#sol::Mission::wrap_up}

🇲 Method --> <code>( )</code>

> Wraps up the mission successfully.

#### is_assigned {#sol::Mission::is_assigned}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission is currently assigned.

#### has_generic_rewards {#sol::Mission::has_generic_rewards}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission has generic rewards.

#### get_likely_rewards {#sol::Mission::get_likely_rewards}

🇲 Method --> <code>( ) -> Vector(Pair(int,[ItypeId](#sol::ItypeId)))</code>

> Returns the likely rewards of the mission (vector of (int chance, itype_id) pairs).

#### has_failed {#sol::Mission::has_failed}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission has failed.

#### step_complete {#sol::Mission::step_complete}

🇲 Method --> <code>( integer )</code>

> Marks a mission step as complete, taking an integer step index.

#### reserve_random {#sol::Mission::reserve_random}

🇫 Function --> <code>( [MissionOrigin](#sol::MissionOrigin), [Tripoint](#sol::Tripoint), [CharacterId](#sol::CharacterId) ) -> [Mission](#sol::Mission)</code>

> Reserves a random mission at the specified origin and position for the given NPC. Returns the new mission.

#### in_progress {#sol::Mission::in_progress}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission is currently in progress.

#### reserve_new {#sol::Mission::reserve_new}

🇫 Function --> <code>( [MissionTypeIdRaw](#sol::MissionTypeIdRaw), [CharacterId](#sol::CharacterId) ) -> [Mission](#sol::Mission)</code>

> Reserves a new mission of the given type for the specified NPC. Returns the new mission.

#### assign {#sol::Mission::assign}

🇲 Method --> <code>( [Avatar](#sol::Avatar) )</code>

> Assigns this mission to the given avatar.

#### get_item_id {#sol::Mission::get_item_id}

🇲 Method --> <code>( ) -> [ItypeId](#sol::ItypeId)</code>

> Returns the item ID associated with the mission.

#### get_npc_id {#sol::Mission::get_npc_id}

🇲 Method --> <code>( ) -> [CharacterId](#sol::CharacterId)</code>

> Returns the NPC character ID associated with the mission.

#### get_id {#sol::Mission::get_id}

🇲 Method --> <code>( ) -> integer</code>

> Returns the mission's unique ID.

#### get_deadline {#sol::Mission::get_deadline}

🇲 Method --> <code>( ) -> [TimePoint](#sol::TimePoint)</code>

> Returns the mission's deadline as a time_point.

#### get_description {#sol::Mission::get_description}

🇲 Method --> <code>( ) -> string</code>

> Returns the mission description.

#### has_deadline {#sol::Mission::has_deadline}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission has a deadline.

#### mission_id {#sol::Mission::mission_id}

🇲 Method --> <code>( ) -> [MissionTypeIdRaw](#sol::MissionTypeIdRaw)</code>

> Returns the mission type ID of this mission.

#### get_target_point {#sol::Mission::get_target_point}

🇲 Method --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

> Returns the target of the mission (pointer to tripoint_abs_omt).

#### has_target {#sol::Mission::has_target}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission has a target.

#### get_type {#sol::Mission::get_type}

🇲 Method --> <code>( ) -> [MissionType](#sol::MissionType)</code>

> Returns the mission type of the target (pointer to mission_type).

#### get_value {#sol::Mission::get_value}

🇲 Method --> <code>( ) -> integer</code>

> Returns the mission's value as an integer.

#### get_follow_up {#sol::Mission::get_follow_up}

🇲 Method --> <code>( ) -> [MissionTypeIdRaw](#sol::MissionTypeIdRaw)</code>

> Returns the follow-up mission type ID.

#### has_follow_up {#sol::Mission::has_follow_up}

🇲 Method --> <code>( ) -> boolean</code>

> Returns true if the mission has a follow-up mission.

## MissionType {#sol::MissionType}

### Bases {#sol::MissionType::@bases}

No base classes.

### Constructors {#sol::MissionType::@ctors}

- MissionType.new( )

### Members {#sol::MissionType::@members}

#### description {#sol::MissionType::description}

🇻 Variable --> <code>CppVal&lt;translation&gt;</code>

> Returns the mission's description as a string.

#### target_npc_id {#sol::MissionType::target_npc_id}

🇻 Variable --> <code>[CharacterId](#sol::CharacterId)</code>

> Returns the ID of the target NPC for the mission, if any.

#### item_count {#sol::MissionType::item_count}

🇻 Variable --> <code>int</code>

> Returns the count of items involved in the mission.

#### empty_container {#sol::MissionType::empty_container}

🇻 Variable --> <code>[ItypeId](#sol::ItypeId)</code>

> Returns true if the mission requires the container to be empty.

#### remove_container {#sol::MissionType::remove_container}

🇻 Variable --> <code>bool</code>

> Returns true if the mission requires removing a container.

#### monster_type {#sol::MissionType::monster_type}

🇻 Variable --> <code>[MtypeId](#sol::MtypeId)</code>

> Returns the monster type associated with the mission, if any.

#### follow_up {#sol::MissionType::follow_up}

🇻 Variable --> <code>[MissionTypeIdRaw](#sol::MissionTypeIdRaw)</code>

> Returns any follow-up mission type ID.

#### monster_kill_goal {#sol::MissionType::monster_kill_goal}

🇻 Variable --> <code>int</code>

> Returns the number of monsters required to kill for this mission.

#### dialogue {#sol::MissionType::dialogue}

🇻 Variable --> <code>Dict( string, CppVal&lt;translation&gt; )</code>

> Returns any associated dialogue for the mission.

#### origins {#sol::MissionType::origins}

🇻 Variable --> <code>Vector( [MissionOrigin](#sol::MissionOrigin) )</code>

> Returns a list of origins from which this mission can be generated.

#### item_id {#sol::MissionType::item_id}

🇻 Variable --> <code>[ItypeId](#sol::ItypeId)</code>

> Returns the ID of the mission's main item target, if applicable.

#### likely_rewards {#sol::MissionType::likely_rewards}

🇻 Variable --> <code>Vector( Pair( int, [ItypeId](#sol::ItypeId) ) )</code>

> Returns a vector of likely rewards (chance, itype_id pairs).

#### difficulty {#sol::MissionType::difficulty}

🇻 Variable --> <code>int</code>

> Returns the mission's difficulty as an integer.

#### goal {#sol::MissionType::goal}

🇻 Variable --> <code>[MissionGoal](#sol::MissionGoal)</code>

> Returns the mission's goal text.

#### deadline_low {#sol::MissionType::deadline_low}

🇻 Variable --> <code>[TimeDuration](#sol::TimeDuration)</code>

> Returns the minimum allowed deadline for the mission.

#### value {#sol::MissionType::value}

🇻 Variable --> <code>int</code>

> Returns the mission's reward value as an integer.

#### has_generic_rewards {#sol::MissionType::has_generic_rewards}

🇻 Variable --> <code>bool</code>

> Returns true if the mission has generic rewards.

#### deadline_high {#sol::MissionType::deadline_high}

🇻 Variable --> <code>[TimeDuration](#sol::TimeDuration)</code>

> Returns the maximum allowed deadline for the mission.

#### urgent {#sol::MissionType::urgent}

🇻 Variable --> <code>bool</code>

> Returns true if the mission is marked as urgent.

#### get_all {#sol::MissionType::get_all}

🇫 Function --> <code>( ) -> [MissionType](#sol::MissionType)[]</code>

> Returns all available missions.

#### get_random_mission_id {#sol::MissionType::get_random_mission_id}

🇫 Function --> <code>( [MissionOrigin](#sol::MissionOrigin), [Tripoint](#sol::Tripoint) ) -> [MissionTypeIdRaw](#sol::MissionTypeIdRaw)</code>

> Returns a random mission type ID at the specified origin and overmap tile position.

#### tname {#sol::MissionType::tname}

🇲 Method --> <code>( ) -> string</code>

## MissionTypeIdRaw {#sol::MissionTypeIdRaw}

### Bases {#sol::MissionTypeIdRaw::@bases}

No base classes.

### Constructors {#sol::MissionTypeIdRaw::@ctors}

- MissionTypeIdRaw.new( string )

### Members {#sol::MissionTypeIdRaw::@members}

No members.

## Monster {#sol::Monster}

### Bases {#sol::Monster::@bases}

- `Creature`

### Constructors {#sol::Monster::@ctors}

No constructors.

### Members {#sol::Monster::@members}

#### friendly {#sol::Monster::friendly}

🇻 Variable --> <code>int</code>

#### death_drops {#sol::Monster::death_drops}

🇻 Variable --> <code>bool</code>

#### faction {#sol::Monster::faction}

🇻 Variable --> <code>[MonsterFactionIntId](#sol::MonsterFactionIntId)</code>

#### unique_name {#sol::Monster::unique_name}

🇻 Variable --> <code>string</code>

#### anger {#sol::Monster::anger}

🇻 Variable --> <code>int</code>

#### morale {#sol::Monster::morale}

🇻 Variable --> <code>int</code>

#### climbs {#sol::Monster::climbs}

🇲 Method --> <code>( ) -> boolean</code>

#### swims {#sol::Monster::swims}

🇲 Method --> <code>( ) -> boolean</code>

#### flies {#sol::Monster::flies}

🇲 Method --> <code>( ) -> boolean</code>

#### move_target {#sol::Monster::move_target}

🇲 Method --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

#### digs {#sol::Monster::digs}

🇲 Method --> <code>( ) -> boolean</code>

#### is_wandering {#sol::Monster::is_wandering}

🇲 Method --> <code>( ) -> boolean</code>

#### attitude {#sol::Monster::attitude}

🇲 Method --> <code>( [Character](#sol::Character) ) -> [MonsterAttitude](#sol::MonsterAttitude)</code>

#### move_to {#sol::Monster::move_to}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), boolean, boolean, number ) -> boolean</code>

#### make_fungus {#sol::Monster::make_fungus}

🇲 Method --> <code>( ) -> boolean</code>

#### wander_to {#sol::Monster::wander_to}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), integer )</code>

#### set_hp {#sol::Monster::set_hp}

🇲 Method --> <code>( integer )</code>

#### can_dig {#sol::Monster::can_dig}

🇲 Method --> <code>( ) -> boolean</code>

#### heal {#sol::Monster::heal}

🇲 Method --> <code>( integer, boolean ) -> integer</code>

#### can_climb {#sol::Monster::can_climb}

🇲 Method --> <code>( ) -> boolean</code>

#### can_hear {#sol::Monster::can_hear}

🇲 Method --> <code>( ) -> boolean</code>

#### can_submerge {#sol::Monster::can_submerge}

🇲 Method --> <code>( ) -> boolean</code>

#### get_upgrade_time {#sol::Monster::get_upgrade_time}

🇲 Method --> <code>( ) -> integer</code>

#### try_upgrade {#sol::Monster::try_upgrade}

🇲 Method --> <code>( boolean )</code>

#### hasten_upgrade {#sol::Monster::hasten_upgrade}

🇲 Method --> <code>( )</code>

#### get_type {#sol::Monster::get_type}

🇲 Method --> <code>( ) -> [MtypeId](#sol::MtypeId)</code>

#### can_upgrade {#sol::Monster::can_upgrade}

🇲 Method --> <code>( ) -> boolean</code>

#### can_drown {#sol::Monster::can_drown}

🇲 Method --> <code>( ) -> boolean</code>

#### try_reproduce {#sol::Monster::try_reproduce}

🇲 Method --> <code>( )</code>

#### spawn {#sol::Monster::spawn}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint) )</code>

#### make_friendly {#sol::Monster::make_friendly}

🇲 Method --> <code>( )</code>

#### refill_udders {#sol::Monster::refill_udders}

🇲 Method --> <code>( )</code>

#### can_see {#sol::Monster::can_see}

🇲 Method --> <code>( ) -> boolean</code>

#### name {#sol::Monster::name}

🇲 Method --> <code>( integer ) -> string</code>

#### name_with_armor {#sol::Monster::name_with_armor}

🇲 Method --> <code>( ) -> string</code>

#### make_ally {#sol::Monster::make_ally}

🇲 Method --> <code>( [Monster](#sol::Monster) )</code>

## MonsterFactionId {#sol::MonsterFactionId}

### Bases {#sol::MonsterFactionId::@bases}

No base classes.

### Constructors {#sol::MonsterFactionId::@ctors}

- MonsterFactionId.new( )
- MonsterFactionId.new( [MonsterFactionId](#sol::MonsterFactionId) )
- MonsterFactionId.new( [MonsterFactionIntId](#sol::MonsterFactionIntId) )
- MonsterFactionId.new( string )

### Members {#sol::MonsterFactionId::@members}

#### NULL_ID {#sol::MonsterFactionId::NULL_ID}

🇫 Function --> <code>( ) -> [MonsterFactionId](#sol::MonsterFactionId)</code>

#### str {#sol::MonsterFactionId::str}

🇲 Method --> <code>( ) -> string</code>

#### obj {#sol::MonsterFactionId::obj}

🇲 Method --> <code>( ) -> MonsterFactionRaw</code>

#### is_null {#sol::MonsterFactionId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### is_valid {#sol::MonsterFactionId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MonsterFactionId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

#### int_id {#sol::MonsterFactionId::int_id}

🇲 Method --> <code>( ) -> [MonsterFactionIntId](#sol::MonsterFactionIntId)</code>

## MonsterFactionIntId {#sol::MonsterFactionIntId}

### Bases {#sol::MonsterFactionIntId::@bases}

No base classes.

### Constructors {#sol::MonsterFactionIntId::@ctors}

- MonsterFactionIntId.new( )
- MonsterFactionIntId.new( [MonsterFactionIntId](#sol::MonsterFactionIntId) )
- MonsterFactionIntId.new( [MonsterFactionId](#sol::MonsterFactionId) )

### Members {#sol::MonsterFactionIntId::@members}

#### obj {#sol::MonsterFactionIntId::obj}

🇲 Method --> <code>( ) -> MonsterFactionRaw</code>

#### is_valid {#sol::MonsterFactionIntId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str_id {#sol::MonsterFactionIntId::str_id}

🇲 Method --> <code>( ) -> [MonsterFactionId](#sol::MonsterFactionId)</code>

## MoraleTypeDataId {#sol::MoraleTypeDataId}

### Bases {#sol::MoraleTypeDataId::@bases}

No base classes.

### Constructors {#sol::MoraleTypeDataId::@ctors}

- MoraleTypeDataId.new( )
- MoraleTypeDataId.new( [MoraleTypeDataId](#sol::MoraleTypeDataId) )
- MoraleTypeDataId.new( string )

### Members {#sol::MoraleTypeDataId::@members}

#### NULL_ID {#sol::MoraleTypeDataId::NULL_ID}

🇫 Function --> <code>( ) -> [MoraleTypeDataId](#sol::MoraleTypeDataId)</code>

#### obj {#sol::MoraleTypeDataId::obj}

🇲 Method --> <code>( ) -> MoraleTypeDataRaw</code>

#### is_valid {#sol::MoraleTypeDataId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MoraleTypeDataId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MoraleTypeDataId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MoraleTypeDataId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## MtypeId {#sol::MtypeId}

### Bases {#sol::MtypeId::@bases}

No base classes.

### Constructors {#sol::MtypeId::@ctors}

- MtypeId.new( )
- MtypeId.new( [MtypeId](#sol::MtypeId) )
- MtypeId.new( string )

### Members {#sol::MtypeId::@members}

#### NULL_ID {#sol::MtypeId::NULL_ID}

🇫 Function --> <code>( ) -> [MtypeId](#sol::MtypeId)</code>

#### obj {#sol::MtypeId::obj}

🇲 Method --> <code>( ) -> MtypeRaw</code>

#### is_valid {#sol::MtypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MtypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MtypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MtypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## MutationBranchId {#sol::MutationBranchId}

### Bases {#sol::MutationBranchId::@bases}

No base classes.

### Constructors {#sol::MutationBranchId::@ctors}

- MutationBranchId.new( )
- MutationBranchId.new( [MutationBranchId](#sol::MutationBranchId) )
- MutationBranchId.new( string )

### Members {#sol::MutationBranchId::@members}

#### NULL_ID {#sol::MutationBranchId::NULL_ID}

🇫 Function --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)</code>

#### obj {#sol::MutationBranchId::obj}

🇲 Method --> <code>( ) -> [MutationBranchRaw](#sol::MutationBranchRaw)</code>

#### is_valid {#sol::MutationBranchId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MutationBranchId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MutationBranchId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MutationBranchId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## MutationBranchRaw {#sol::MutationBranchRaw}

### Bases {#sol::MutationBranchRaw::@bases}

No base classes.

### Constructors {#sol::MutationBranchRaw::@ctors}

No constructors.

### Members {#sol::MutationBranchRaw::@members}

#### id {#sol::MutationBranchRaw::id}

🇻 Variable --> <code>[MutationBranchId](#sol::MutationBranchId)</code>

#### max_stamina_modifier {#sol::MutationBranchRaw::max_stamina_modifier}

🇻 Variable --> <code>double</code>

#### falling_damage_multiplier {#sol::MutationBranchRaw::falling_damage_multiplier}

🇻 Variable --> <code>double</code>

#### weight_capacity_modifier {#sol::MutationBranchRaw::weight_capacity_modifier}

🇻 Variable --> <code>double</code>

#### movecost_swim_modifier {#sol::MutationBranchRaw::movecost_swim_modifier}

🇻 Variable --> <code>double</code>

#### hearing_modifier {#sol::MutationBranchRaw::hearing_modifier}

🇻 Variable --> <code>double</code>

#### noise_modifier {#sol::MutationBranchRaw::noise_modifier}

🇻 Variable --> <code>double</code>

#### attackcost_modifier {#sol::MutationBranchRaw::attackcost_modifier}

🇻 Variable --> <code>double</code>

#### movecost_flatground_modifier {#sol::MutationBranchRaw::movecost_flatground_modifier}

🇻 Variable --> <code>double</code>

#### hp_adjustment {#sol::MutationBranchRaw::hp_adjustment}

🇻 Variable --> <code>double</code>

> Flat adjustment to HP.

#### movecost_obstacle_modifier {#sol::MutationBranchRaw::movecost_obstacle_modifier}

🇻 Variable --> <code>double</code>

#### str_modifier {#sol::MutationBranchRaw::str_modifier}

🇻 Variable --> <code>double</code>

> Adjustment to Strength that doesn't affect HP.

#### speed_modifier {#sol::MutationBranchRaw::speed_modifier}

🇻 Variable --> <code>double</code>

#### dodge_modifier {#sol::MutationBranchRaw::dodge_modifier}

🇻 Variable --> <code>double</code>

#### hp_modifier_secondary {#sol::MutationBranchRaw::hp_modifier_secondary}

🇻 Variable --> <code>double</code>

> Secondary HP multiplier; stacks with the other one. 1.0 doubles HP; -0.5 halves it.

#### scent_modifier {#sol::MutationBranchRaw::scent_modifier}

🇻 Variable --> <code>double</code>

#### healthy_rate {#sol::MutationBranchRaw::healthy_rate}

🇻 Variable --> <code>double</code>

> How quickly health (not HP) trends toward healthy_mod.

#### overmap_sight {#sol::MutationBranchRaw::overmap_sight}

🇻 Variable --> <code>double</code>

#### stamina_regen_modifier {#sol::MutationBranchRaw::stamina_regen_modifier}

🇻 Variable --> <code>double</code>

#### overmap_multiplier {#sol::MutationBranchRaw::overmap_multiplier}

🇻 Variable --> <code>double</code>

#### skill_rust_multiplier {#sol::MutationBranchRaw::skill_rust_multiplier}

🇻 Variable --> <code>double</code>

#### reading_speed_multiplier {#sol::MutationBranchRaw::reading_speed_multiplier}

🇻 Variable --> <code>double</code>

#### bleed_resist {#sol::MutationBranchRaw::bleed_resist}

🇻 Variable --> <code>double</code>

#### fatigue_regen_modifier {#sol::MutationBranchRaw::fatigue_regen_modifier}

🇻 Variable --> <code>double</code>

#### thirst_modifier {#sol::MutationBranchRaw::thirst_modifier}

🇻 Variable --> <code>double</code>

#### stealth_modifier {#sol::MutationBranchRaw::stealth_modifier}

🇻 Variable --> <code>double</code>

#### fatigue_modifier {#sol::MutationBranchRaw::fatigue_modifier}

🇻 Variable --> <code>double</code>

#### night_vision_range {#sol::MutationBranchRaw::night_vision_range}

🇻 Variable --> <code>double</code>

#### metabolism_modifier {#sol::MutationBranchRaw::metabolism_modifier}

🇻 Variable --> <code>double</code>

#### temperature_speed_modifier {#sol::MutationBranchRaw::temperature_speed_modifier}

🇻 Variable --> <code>double</code>

#### construction_speed_modifier {#sol::MutationBranchRaw::construction_speed_modifier}

🇻 Variable --> <code>double</code>

> Construction speed multiplier. 2.0 doubles construction speed; 0.5 halves it.

#### movecost_modifier {#sol::MutationBranchRaw::movecost_modifier}

🇻 Variable --> <code>double</code>

#### packmule_modifier {#sol::MutationBranchRaw::packmule_modifier}

🇻 Variable --> <code>double</code>

> Packmule multiplier. 2.0 doubles backpack/container volume; 0.5 halves it.

#### crafting_speed_modifier {#sol::MutationBranchRaw::crafting_speed_modifier}

🇻 Variable --> <code>double</code>

> Crafting speed multiplier. 2.0 doubles crafting speed; 0.5 halves it.

#### starting_trait {#sol::MutationBranchRaw::starting_trait}

🇻 Variable --> <code>bool</code>

> Whether this trait can normally be taken during character generation.

#### starts_active {#sol::MutationBranchRaw::starts_active}

🇻 Variable --> <code>bool</code>

> Whether a mutation activates when granted.

#### fatigue {#sol::MutationBranchRaw::fatigue}

🇻 Variable --> <code>bool</code>

> Mutation causes fatigue when used.

#### allow_soft_gear {#sol::MutationBranchRaw::allow_soft_gear}

🇻 Variable --> <code>bool</code>

> Mutation allows soft gear to be worn over otherwise-restricted parts.

#### hunger {#sol::MutationBranchRaw::hunger}

🇻 Variable --> <code>bool</code>

> Mutation deducts calories when used.

#### mixed_effect {#sol::MutationBranchRaw::mixed_effect}

🇻 Variable --> <code>bool</code>

> Whether this mutation has positive /and/ negative effects.

#### debug {#sol::MutationBranchRaw::debug}

🇻 Variable --> <code>bool</code>

> Whether or not this mutation is limited to debug use.

#### valid {#sol::MutationBranchRaw::valid}

🇻 Variable --> <code>bool</code>

> Whether this mutation is available through generic mutagen.

#### player_display {#sol::MutationBranchRaw::player_display}

🇻 Variable --> <code>bool</code>

> Whether or not this mutation shows up in the status (`@`) menu.

#### purifiable {#sol::MutationBranchRaw::purifiable}

🇻 Variable --> <code>bool</code>

> Whether this mutation is possible to remove through Purifier. False for 'special' mutations.

#### profession {#sol::MutationBranchRaw::profession}

🇻 Variable --> <code>bool</code>

> Whether this trait is ONLY gained through professional training/experience (and/or quests).

#### threshold {#sol::MutationBranchRaw::threshold}

🇻 Variable --> <code>bool</code>

> Whether this is a Threshold mutation, and thus especially difficult to mutate. One per character.

#### thirst {#sol::MutationBranchRaw::thirst}

🇻 Variable --> <code>bool</code>

> Mutation dehydrates when used.

#### activated {#sol::MutationBranchRaw::activated}

🇻 Variable --> <code>bool</code>

> Whether this mutation can be activated at will.

#### visibility {#sol::MutationBranchRaw::visibility}

🇻 Variable --> <code>int</code>

> How visible the mutation is to others.

#### healing_awake {#sol::MutationBranchRaw::healing_awake}

🇻 Variable --> <code>double</code>

> Healing per turn from mutation.

#### pain_recovery {#sol::MutationBranchRaw::pain_recovery}

🇻 Variable --> <code>double</code>

> Pain recovery per turn from mutation.

#### healing_resting {#sol::MutationBranchRaw::healing_resting}

🇻 Variable --> <code>double</code>

> Healing per turn from mutation, while asleep.

#### points {#sol::MutationBranchRaw::points}

🇻 Variable --> <code>int</code>

> Point cost in character creation(?).

#### mending_modifier {#sol::MutationBranchRaw::mending_modifier}

🇻 Variable --> <code>double</code>

> Multiplier applied to broken limb regeneration. Normally 0.25; clamped to 0.25..1.0.

#### bodytemp_sleep_btu {#sol::MutationBranchRaw::bodytemp_sleep_btu}

🇻 Variable --> <code>int</code>

#### hp_modifier {#sol::MutationBranchRaw::hp_modifier}

🇻 Variable --> <code>double</code>

> Bonus HP multiplier. 1.0 doubles HP; -0.5 halves it.

#### bodytemp_min_btu {#sol::MutationBranchRaw::bodytemp_min_btu}

🇻 Variable --> <code>int</code>

#### bodytemp_max_btu {#sol::MutationBranchRaw::bodytemp_max_btu}

🇻 Variable --> <code>int</code>

#### cost {#sol::MutationBranchRaw::cost}

🇻 Variable --> <code>int</code>

#### ugliness {#sol::MutationBranchRaw::ugliness}

🇻 Variable --> <code>int</code>

> How physically unappealing the mutation is. Can be negative.

#### cooldown {#sol::MutationBranchRaw::cooldown}

🇻 Variable --> <code>int</code>

> Costs are incurred every 'cooldown' turns.

#### thresh_requirements {#sol::MutationBranchRaw::thresh_requirements}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

> Lists the threshold mutation(s) required to gain this mutation.

#### replaced_by {#sol::MutationBranchRaw::replaced_by}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

> Lists mutations that replace (e.g. evolve from) this one.

#### mutation_types {#sol::MutationBranchRaw::mutation_types}

🇲 Method --> <code>( ) -> string[]</code>

> Lists the type(s) of this mutation. Mutations of a given type are mutually exclusive.

#### conflicts_with {#sol::MutationBranchRaw::conflicts_with}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

> Lists conflicting mutations.

#### other_prerequisites {#sol::MutationBranchRaw::other_prerequisites}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

> Lists the secondary mutation(s) needed to gain this mutation.

#### categories {#sol::MutationBranchRaw::categories}

🇲 Method --> <code>( ) -> [MutationCategoryTraitId](#sol::MutationCategoryTraitId)[]</code>

> Lists the categories this mutation belongs to.

#### get_all {#sol::MutationBranchRaw::get_all}

🇫 Function --> <code>( ) -> [MutationBranchRaw](#sol::MutationBranchRaw)[]</code>

> Returns a (long) list of every mutation in the game.

#### addition_mutations {#sol::MutationBranchRaw::addition_mutations}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

#### prerequisites {#sol::MutationBranchRaw::prerequisites}

🇲 Method --> <code>( ) -> [MutationBranchId](#sol::MutationBranchId)[]</code>

> Lists the primary mutation(s) needed to gain this mutation.

#### name {#sol::MutationBranchRaw::name}

🇲 Method --> <code>( ) -> string</code>

#### desc {#sol::MutationBranchRaw::desc}

🇲 Method --> <code>( ) -> string</code>

## MutationCategoryTraitId {#sol::MutationCategoryTraitId}

### Bases {#sol::MutationCategoryTraitId::@bases}

No base classes.

### Constructors {#sol::MutationCategoryTraitId::@ctors}

- MutationCategoryTraitId.new( )
- MutationCategoryTraitId.new( [MutationCategoryTraitId](#sol::MutationCategoryTraitId) )
- MutationCategoryTraitId.new( string )

### Members {#sol::MutationCategoryTraitId::@members}

#### NULL_ID {#sol::MutationCategoryTraitId::NULL_ID}

🇫 Function --> <code>( ) -> [MutationCategoryTraitId](#sol::MutationCategoryTraitId)</code>

#### obj {#sol::MutationCategoryTraitId::obj}

🇲 Method --> <code>( ) -> MutationCategoryTraitRaw</code>

#### is_valid {#sol::MutationCategoryTraitId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::MutationCategoryTraitId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::MutationCategoryTraitId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::MutationCategoryTraitId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## Npc {#sol::Npc}

### Bases {#sol::Npc::@bases}

- `Player`
- `Character`
- `Creature`

### Constructors {#sol::Npc::@ctors}

No constructors.

### Members {#sol::Npc::@members}

#### current_activity_id {#sol::Npc::current_activity_id}

🇻 Variable --> <code>[ActivityTypeId](#sol::ActivityTypeId)</code>

#### hit_by_player {#sol::Npc::hit_by_player}

🇻 Variable --> <code>bool</code>

#### needs {#sol::Npc::needs}

🇻 Variable --> <code>Vector( [NpcNeed](#sol::NpcNeed) )</code>

#### patience {#sol::Npc::patience}

🇻 Variable --> <code>int</code>

#### marked_for_death {#sol::Npc::marked_for_death}

🇻 Variable --> <code>bool</code>

#### op_of_u {#sol::Npc::op_of_u}

🇻 Variable --> <code>[NpcOpinion](#sol::NpcOpinion)</code>

#### personality {#sol::Npc::personality}

🇻 Variable --> <code>[NpcPersonality](#sol::NpcPersonality)</code>

#### danger_assessment {#sol::Npc::danger_assessment}

🇲 Method --> <code>( ) -> number</code>

#### say {#sol::Npc::say}

🇲 Method --> <code>( string )</code>

#### current_ally {#sol::Npc::current_ally}

🇲 Method --> <code>( ) -> [Creature](#sol::Creature)</code>

#### get_monster_faction {#sol::Npc::get_monster_faction}

🇲 Method --> <code>( ) -> [MonsterFactionIntId](#sol::MonsterFactionIntId)</code>

#### follow_distance {#sol::Npc::follow_distance}

🇲 Method --> <code>( ) -> integer</code>

#### current_target {#sol::Npc::current_target}

🇲 Method --> <code>( ) -> [Creature](#sol::Creature)</code>

#### smash_ability {#sol::Npc::smash_ability}

🇲 Method --> <code>( ) -> integer</code>

#### complain_about {#sol::Npc::complain_about}

🇲 Method --> <code>( string, [TimeDuration](#sol::TimeDuration), string, boolean ) -> boolean</code>

#### evaluate_enemy {#sol::Npc::evaluate_enemy}

🇲 Method --> <code>( [Creature](#sol::Creature) ) -> number</code>

#### complain {#sol::Npc::complain}

🇲 Method --> <code>( ) -> boolean</code>

#### has_omt_destination {#sol::Npc::has_omt_destination}

🇲 Method --> <code>( ) -> boolean</code>

#### get_attitude {#sol::Npc::get_attitude}

🇲 Method --> <code>( ) -> [NpcAttitude](#sol::NpcAttitude)</code>

#### warn_about {#sol::Npc::warn_about}

🇲 Method --> <code>( string, [TimeDuration](#sol::TimeDuration), string, integer, [Tripoint](#sol::Tripoint) )</code>

#### saw_player_recently {#sol::Npc::saw_player_recently}

🇲 Method --> <code>( ) -> boolean</code>

#### can_open_door {#sol::Npc::can_open_door}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), boolean ) -> boolean</code>

#### can_move_to {#sol::Npc::can_move_to}

🇲 Method --> <code>( [Tripoint](#sol::Tripoint), boolean ) -> boolean</code>

#### mutiny {#sol::Npc::mutiny}

🇲 Method --> <code>( )</code>

#### guaranteed_hostile {#sol::Npc::guaranteed_hostile}

🇲 Method --> <code>( ) -> boolean</code>

#### has_player_activity {#sol::Npc::has_player_activity}

🇲 Method --> <code>( ) -> boolean</code>

#### is_travelling {#sol::Npc::is_travelling}

🇲 Method --> <code>( ) -> boolean</code>

#### is_enemy {#sol::Npc::is_enemy}

🇲 Method --> <code>( ) -> boolean</code>

#### is_following {#sol::Npc::is_following}

🇲 Method --> <code>( ) -> boolean</code>

#### is_obeying {#sol::Npc::is_obeying}

🇲 Method --> <code>( [Character](#sol::Character) ) -> boolean</code>

#### make_angry {#sol::Npc::make_angry}

🇲 Method --> <code>( )</code>

#### turned_hostile {#sol::Npc::turned_hostile}

🇲 Method --> <code>( ) -> boolean</code>

#### hostile_anger_level {#sol::Npc::hostile_anger_level}

🇲 Method --> <code>( ) -> integer</code>

#### set_faction_id {#sol::Npc::set_faction_id}

🇲 Method --> <code>( [FactionId](#sol::FactionId) )</code>

#### is_minion {#sol::Npc::is_minion}

🇲 Method --> <code>( ) -> boolean</code>

#### is_friendly {#sol::Npc::is_friendly}

🇲 Method --> <code>( [Character](#sol::Character) ) -> boolean</code>

#### is_walking_with {#sol::Npc::is_walking_with}

🇲 Method --> <code>( ) -> boolean</code>

#### is_patrolling {#sol::Npc::is_patrolling}

🇲 Method --> <code>( ) -> boolean</code>

#### set_attitude {#sol::Npc::set_attitude}

🇲 Method --> <code>( [NpcAttitude](#sol::NpcAttitude) )</code>

#### is_leader {#sol::Npc::is_leader}

🇲 Method --> <code>( ) -> boolean</code>

#### is_guarding {#sol::Npc::is_guarding}

🇲 Method --> <code>( ) -> boolean</code>

#### is_player_ally {#sol::Npc::is_player_ally}

🇲 Method --> <code>( ) -> boolean</code>

#### is_stationary {#sol::Npc::is_stationary}

🇲 Method --> <code>( boolean ) -> boolean</code>

#### is_ally {#sol::Npc::is_ally}

🇲 Method --> <code>( [Character](#sol::Character) ) -> boolean</code>

#### has_activity {#sol::Npc::has_activity}

🇲 Method --> <code>( ) -> boolean</code>

## NpcOpinion {#sol::NpcOpinion}

### Bases {#sol::NpcOpinion::@bases}

No base classes.

### Constructors {#sol::NpcOpinion::@ctors}

- NpcOpinion.new( )
- NpcOpinion.new( int, int, int, int, int )

### Members {#sol::NpcOpinion::@members}

#### trust {#sol::NpcOpinion::trust}

🇻 Variable --> <code>int</code>

#### anger {#sol::NpcOpinion::anger}

🇻 Variable --> <code>int</code>

#### value {#sol::NpcOpinion::value}

🇻 Variable --> <code>int</code>

#### fear {#sol::NpcOpinion::fear}

🇻 Variable --> <code>int</code>

#### owed {#sol::NpcOpinion::owed}

🇻 Variable --> <code>int</code>

## NpcPersonality {#sol::NpcPersonality}

### Bases {#sol::NpcPersonality::@bases}

No base classes.

### Constructors {#sol::NpcPersonality::@ctors}

- NpcPersonality.new( )

### Members {#sol::NpcPersonality::@members}

#### aggression {#sol::NpcPersonality::aggression}

🇻 Variable --> <code>char</code>

#### collector {#sol::NpcPersonality::collector}

🇻 Variable --> <code>char</code>

#### bravery {#sol::NpcPersonality::bravery}

🇻 Variable --> <code>char</code>

#### altruism {#sol::NpcPersonality::altruism}

🇻 Variable --> <code>char</code>

## Player {#sol::Player}

### Bases {#sol::Player::@bases}

- `Character`
- `Creature`

### Constructors {#sol::Player::@ctors}

No constructors.

### Members {#sol::Player::@members}

No members.

## Point {#sol::Point}

### Bases {#sol::Point::@bases}

No base classes.

### Constructors {#sol::Point::@ctors}

- Point.new( )
- Point.new( [Point](#sol::Point) )
- Point.new( int, int )

### Members {#sol::Point::@members}

#### x {#sol::Point::x}

🇻 Variable --> <code>int</code>

#### y {#sol::Point::y}

🇻 Variable --> <code>int</code>

#### abs {#sol::Point::abs}

🇲 Method --> <code>( ) -> [Point](#sol::Point)</code>

#### rotate {#sol::Point::rotate}

🇲 Method --> <code>( integer, [Point](#sol::Point) ) -> [Point](#sol::Point)</code>

## PopupInputStr {#sol::PopupInputStr}

### Bases {#sol::PopupInputStr::@bases}

No base classes.

### Constructors {#sol::PopupInputStr::@ctors}

- PopupInputStr.new( )

### Members {#sol::PopupInputStr::@members}

#### title {#sol::PopupInputStr::title}

🇲 Method --> <code>( string )</code>

> `title` is on the left of input field.

#### query_str {#sol::PopupInputStr::query_str}

🇲 Method --> <code>( ) -> string</code>

> Returns your input.

#### desc {#sol::PopupInputStr::desc}

🇲 Method --> <code>( string )</code>

> `desc` is above input field.

#### query_int {#sol::PopupInputStr::query_int}

🇲 Method --> <code>( ) -> integer</code>

> Returns your input, but allows numbers only.

## QueryPopup {#sol::QueryPopup}

### Bases {#sol::QueryPopup::@bases}

No base classes.

### Constructors {#sol::QueryPopup::@ctors}

- QueryPopup.new( )

### Members {#sol::QueryPopup::@members}

#### message {#sol::QueryPopup::message}

🇲 Method --> <code>( any )</code>

#### query {#sol::QueryPopup::query}

🇲 Method --> <code>( ) -> string</code>

> Returns selected action

#### query_yn {#sol::QueryPopup::query_yn}

🇲 Method --> <code>( ) -> string</code>

> Returns `YES` or `NO`. If ESC pressed, returns `NO`.

#### allow_any_key {#sol::QueryPopup::allow_any_key}

🇲 Method --> <code>( boolean )</code>

> Set whether to allow any key

#### message_color {#sol::QueryPopup::message_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

#### query_ynq {#sol::QueryPopup::query_ynq}

🇲 Method --> <code>( ) -> string</code>

> Returns `YES`, `NO` or `QUIT`. If ESC pressed, returns `QUIT`.

## RecipeId {#sol::RecipeId}

### Bases {#sol::RecipeId::@bases}

No base classes.

### Constructors {#sol::RecipeId::@ctors}

- RecipeId.new( )
- RecipeId.new( [RecipeId](#sol::RecipeId) )
- RecipeId.new( string )

### Members {#sol::RecipeId::@members}

#### NULL_ID {#sol::RecipeId::NULL_ID}

🇫 Function --> <code>( ) -> [RecipeId](#sol::RecipeId)</code>

#### obj {#sol::RecipeId::obj}

🇲 Method --> <code>( ) -> [RecipeRaw](#sol::RecipeRaw)</code>

#### is_valid {#sol::RecipeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::RecipeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::RecipeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::RecipeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## RecipeRaw {#sol::RecipeRaw}

### Bases {#sol::RecipeRaw::@bases}

No base classes.

### Constructors {#sol::RecipeRaw::@ctors}

No constructors.

### Members {#sol::RecipeRaw::@members}

#### category {#sol::RecipeRaw::category}

🇻 Variable --> <code>string</code>

#### required_skills {#sol::RecipeRaw::required_skills}

🇻 Variable --> <code>Dict( [SkillId](#sol::SkillId), int )</code>

#### learn_by_disassembly {#sol::RecipeRaw::learn_by_disassembly}

🇻 Variable --> <code>Dict( [SkillId](#sol::SkillId), int )</code>

#### difficulty {#sol::RecipeRaw::difficulty}

🇻 Variable --> <code>int</code>

#### booksets {#sol::RecipeRaw::booksets}

🇻 Variable --> <code>Dict( [ItypeId](#sol::ItypeId), int )</code>

#### time {#sol::RecipeRaw::time}

🇻 Variable --> <code>int</code>

#### skill_used {#sol::RecipeRaw::skill_used}

🇻 Variable --> <code>[SkillId](#sol::SkillId)</code>

#### subcategory {#sol::RecipeRaw::subcategory}

🇻 Variable --> <code>string</code>

#### has_flag {#sol::RecipeRaw::has_flag}

🇲 Method --> <code>( string ) -> boolean</code>

#### get_from_skill_used {#sol::RecipeRaw::get_from_skill_used}

🇫 Function --> <code>( [SkillId](#sol::SkillId) ) -> [RecipeRaw](#sol::RecipeRaw)[]</code>

#### result_name {#sol::RecipeRaw::result_name}

🇲 Method --> <code>( ) -> string</code>

#### result {#sol::RecipeRaw::result}

🇲 Method --> <code>( ) -> [ItypeId](#sol::ItypeId)</code>

#### get_from_flag {#sol::RecipeRaw::get_from_flag}

🇫 Function --> <code>( string ) -> [RecipeRaw](#sol::RecipeRaw)[]</code>

#### ident {#sol::RecipeRaw::ident}

🇲 Method --> <code>( ) -> [RecipeId](#sol::RecipeId)</code>

#### get_all {#sol::RecipeRaw::get_all}

🇫 Function --> <code>( ) -> [RecipeRaw](#sol::RecipeRaw)[]</code>

## SkillId {#sol::SkillId}

### Bases {#sol::SkillId::@bases}

No base classes.

### Constructors {#sol::SkillId::@ctors}

- SkillId.new( )
- SkillId.new( [SkillId](#sol::SkillId) )
- SkillId.new( string )

### Members {#sol::SkillId::@members}

#### NULL_ID {#sol::SkillId::NULL_ID}

🇫 Function --> <code>( ) -> [SkillId](#sol::SkillId)</code>

#### obj {#sol::SkillId::obj}

🇲 Method --> <code>( ) -> SkillRaw</code>

#### is_valid {#sol::SkillId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::SkillId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::SkillId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::SkillId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## SkillLevel {#sol::SkillLevel}

### Bases {#sol::SkillLevel::@bases}

No base classes.

### Constructors {#sol::SkillLevel::@ctors}

No constructors.

### Members {#sol::SkillLevel::@members}

#### is_training {#sol::SkillLevel::is_training}

🇲 Method --> <code>( ) -> boolean</code>

#### train {#sol::SkillLevel::train}

🇲 Method --> <code>( integer, boolean )</code>

#### highest_level {#sol::SkillLevel::highest_level}

🇲 Method --> <code>( ) -> integer</code>

#### level {#sol::SkillLevel::level}

🇲 Method --> <code>( ) -> integer</code>

#### can_train {#sol::SkillLevel::can_train}

🇲 Method --> <code>( ) -> boolean</code>

## SkillLevelMap {#sol::SkillLevelMap}

### Bases {#sol::SkillLevelMap::@bases}

- `Dict( SkillId, SkillLevel )`

### Constructors {#sol::SkillLevelMap::@ctors}

No constructors.

### Members {#sol::SkillLevelMap::@members}

#### mod_skill_level {#sol::SkillLevelMap::mod_skill_level}

🇲 Method --> <code>( [SkillId](#sol::SkillId), integer )</code>

#### get_skill_level {#sol::SkillLevelMap::get_skill_level}

🇲 Method --> <code>( [SkillId](#sol::SkillId) ) -> integer</code>

#### get_skill_level_object {#sol::SkillLevelMap::get_skill_level_object}

🇲 Method --> <code>( [SkillId](#sol::SkillId) ) -> [SkillLevel](#sol::SkillLevel)</code>

## SpeciesTypeId {#sol::SpeciesTypeId}

### Bases {#sol::SpeciesTypeId::@bases}

No base classes.

### Constructors {#sol::SpeciesTypeId::@ctors}

- SpeciesTypeId.new( )
- SpeciesTypeId.new( [SpeciesTypeId](#sol::SpeciesTypeId) )
- SpeciesTypeId.new( string )

### Members {#sol::SpeciesTypeId::@members}

#### NULL_ID {#sol::SpeciesTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [SpeciesTypeId](#sol::SpeciesTypeId)</code>

#### obj {#sol::SpeciesTypeId::obj}

🇲 Method --> <code>( ) -> SpeciesTypeRaw</code>

#### is_valid {#sol::SpeciesTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::SpeciesTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::SpeciesTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::SpeciesTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## Spell {#sol::Spell}

> The class used for spells that _a player_ knows, casts, and gains experience for using. If a given spell is not supposed to be directly cast by a player, consider using SpellSimple instead.

### Bases {#sol::Spell::@bases}

No base classes.

### Constructors {#sol::Spell::@ctors}

- Spell.new( [SpellTypeId](#sol::SpellTypeId), int )

### Members {#sol::Spell::@members}

#### id {#sol::Spell::id}

🇻 Variable --> <code>[SpellTypeId](#sol::SpellTypeId)</code>

#### get_level {#sol::Spell::get_level}

🇲 Method --> <code>( ) -> integer</code>

#### name {#sol::Spell::name}

🇲 Method --> <code>( ) -> string</code>

#### desc {#sol::Spell::desc}

🇲 Method --> <code>( ) -> string</code>

#### cast {#sol::Spell::cast}

🇲 Method --> <code>( [Creature](#sol::Creature), [Tripoint](#sol::Tripoint) )</code>

> Cast this spell, as well as any sub-spells.

#### set_level {#sol::Spell::set_level}

🇲 Method --> <code>( integer )</code>

#### set_exp {#sol::Spell::set_exp}

🇲 Method --> <code>( integer )</code>

#### gain_levels {#sol::Spell::gain_levels}

🇲 Method --> <code>( integer )</code>

#### xp {#sol::Spell::xp}

🇲 Method --> <code>( ) -> integer</code>

#### gain_exp {#sol::Spell::gain_exp}

🇲 Method --> <code>( integer )</code>

#### cast_single_effect {#sol::Spell::cast_single_effect}

🇲 Method --> <code>( [Creature](#sol::Creature), [Tripoint](#sol::Tripoint) )</code>

> Cast _only_ this spell's main effects. Generally, cast() should be used instead.

## SpellSimple {#sol::SpellSimple}

> The type for basic spells. If you don't need to track XP from casting (e.g., if a spell is intended to be cast by anything _other than_ a player), this is likely the appropriate type. Otherwise, see the Spell type.

### Bases {#sol::SpellSimple::@bases}

No base classes.

### Constructors {#sol::SpellSimple::@ctors}

- SpellSimple.new( [SpellTypeId](#sol::SpellTypeId), bool )
- SpellSimple.new( [SpellTypeId](#sol::SpellTypeId), bool, int )

### Members {#sol::SpellSimple::@members}

#### level {#sol::SpellSimple::level}

🇻 Variable --> <code>int</code>

#### trigger_once_in {#sol::SpellSimple::trigger_once_in}

🇻 Variable --> <code>int</code>

> Used for enchantments; the spell's _chance_ to trigger every turn.

#### id {#sol::SpellSimple::id}

🇻 Variable --> <code>[SpellTypeId](#sol::SpellTypeId)</code>

#### force_target_source {#sol::SpellSimple::force_target_source}

🇻 Variable --> <code>bool</code>

> Whether or not the target point is _locked_ to the source's location.

#### prompt_cast {#sol::SpellSimple::prompt_cast}

🇫 Function --> <code>( [SpellTypeId](#sol::SpellTypeId), [Tripoint](#sol::Tripoint), integer ) -> [SpellSimple](#sol::SpellSimple)</code>

> Static function: Creates and immediately casts a SimpleSpell, then returns the new spell for potential reuse. If the given tripoint is the player's location, the spell will be locked to the player. (This does not necessarily cause friendly fire!) If an integer is specified, the spell will be cast at that level.

#### max_level {#sol::SpellSimple::max_level}

🇲 Method --> <code>( ) -> integer</code>

> Returns the defined maximum level of this SpellSimple instance, if defined. Otherwise, returns 0.

#### cast {#sol::SpellSimple::cast}

🇲 Method --> <code>( [Creature](#sol::Creature), [Tripoint](#sol::Tripoint), integer )</code>

## SpellTypeId {#sol::SpellTypeId}

### Bases {#sol::SpellTypeId::@bases}

No base classes.

### Constructors {#sol::SpellTypeId::@ctors}

- SpellTypeId.new( )
- SpellTypeId.new( [SpellTypeId](#sol::SpellTypeId) )
- SpellTypeId.new( string )

### Members {#sol::SpellTypeId::@members}

#### NULL_ID {#sol::SpellTypeId::NULL_ID}

🇫 Function --> <code>( ) -> [SpellTypeId](#sol::SpellTypeId)</code>

#### obj {#sol::SpellTypeId::obj}

🇲 Method --> <code>( ) -> [SpellTypeRaw](#sol::SpellTypeRaw)</code>

#### is_valid {#sol::SpellTypeId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str {#sol::SpellTypeId::str}

🇲 Method --> <code>( ) -> string</code>

#### is_null {#sol::SpellTypeId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::SpellTypeId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

## SpellTypeRaw {#sol::SpellTypeRaw}

> The 'raw' type for storing the information defining every spell in the game. It's not possible to cast directly from this type; check SpellSimple and Spell.

### Bases {#sol::SpellTypeRaw::@bases}

No base classes.

### Constructors {#sol::SpellTypeRaw::@ctors}

No constructors.

### Members {#sol::SpellTypeRaw::@members}

#### aoe_increment {#sol::SpellTypeRaw::aoe_increment}

🇻 Variable --> <code>double</code>

#### max_dot {#sol::SpellTypeRaw::max_dot}

🇻 Variable --> <code>int</code>

#### min_duration {#sol::SpellTypeRaw::min_duration}

🇻 Variable --> <code>int</code>

#### dot_increment {#sol::SpellTypeRaw::dot_increment}

🇻 Variable --> <code>double</code>

#### max_aoe {#sol::SpellTypeRaw::max_aoe}

🇻 Variable --> <code>int</code>

#### min_dot {#sol::SpellTypeRaw::min_dot}

🇻 Variable --> <code>int</code>

#### final_casting_time {#sol::SpellTypeRaw::final_casting_time}

🇻 Variable --> <code>int</code>

#### duration_increment {#sol::SpellTypeRaw::duration_increment}

🇻 Variable --> <code>int</code>

#### base_energy_cost {#sol::SpellTypeRaw::base_energy_cost}

🇻 Variable --> <code>int</code>

#### max_level {#sol::SpellTypeRaw::max_level}

🇻 Variable --> <code>int</code>

#### max_duration {#sol::SpellTypeRaw::max_duration}

🇻 Variable --> <code>int</code>

#### difficulty {#sol::SpellTypeRaw::difficulty}

🇻 Variable --> <code>int</code>

#### energy_increment {#sol::SpellTypeRaw::energy_increment}

🇻 Variable --> <code>double</code>

#### final_energy_cost {#sol::SpellTypeRaw::final_energy_cost}

🇻 Variable --> <code>int</code>

#### base_casting_time {#sol::SpellTypeRaw::base_casting_time}

🇻 Variable --> <code>int</code>

#### min_aoe {#sol::SpellTypeRaw::min_aoe}

🇻 Variable --> <code>int</code>

#### range_increment {#sol::SpellTypeRaw::range_increment}

🇻 Variable --> <code>double</code>

#### field_chance {#sol::SpellTypeRaw::field_chance}

🇻 Variable --> <code>int</code>

#### min_field_intensity {#sol::SpellTypeRaw::min_field_intensity}

🇻 Variable --> <code>int</code>

#### effect_str {#sol::SpellTypeRaw::effect_str}

🇻 Variable --> <code>string</code>

> Specifics about the effect this spell will enact.

#### id {#sol::SpellTypeRaw::id}

🇻 Variable --> <code>[SpellTypeId](#sol::SpellTypeId)</code>

#### effect_name {#sol::SpellTypeRaw::effect_name}

🇻 Variable --> <code>string</code>

> The name of the primary effect this spell will enact.

#### max_range {#sol::SpellTypeRaw::max_range}

🇻 Variable --> <code>int</code>

#### field_intensity_increment {#sol::SpellTypeRaw::field_intensity_increment}

🇻 Variable --> <code>double</code>

#### field_intensity_variance {#sol::SpellTypeRaw::field_intensity_variance}

🇻 Variable --> <code>double</code>

#### min_range {#sol::SpellTypeRaw::min_range}

🇻 Variable --> <code>int</code>

#### max_field_intensity {#sol::SpellTypeRaw::max_field_intensity}

🇻 Variable --> <code>int</code>

#### max_damage {#sol::SpellTypeRaw::max_damage}

🇻 Variable --> <code>int</code>

#### min_damage {#sol::SpellTypeRaw::min_damage}

🇻 Variable --> <code>int</code>

#### damage_increment {#sol::SpellTypeRaw::damage_increment}

🇻 Variable --> <code>double</code>

#### casting_time_increment {#sol::SpellTypeRaw::casting_time_increment}

🇻 Variable --> <code>double</code>

#### get_all {#sol::SpellTypeRaw::get_all}

🇫 Function --> <code>( ) -> [SpellTypeRaw](#sol::SpellTypeRaw)[]</code>

> Returns a (long) list of every spell in the game.

#### additional_spells {#sol::SpellTypeRaw::additional_spells}

🇲 Method --> <code>( ) -> [SpellSimple](#sol::SpellSimple)[]</code>

> Other spells cast by this spell.

## TerId {#sol::TerId}

### Bases {#sol::TerId::@bases}

No base classes.

### Constructors {#sol::TerId::@ctors}

- TerId.new( )
- TerId.new( [TerId](#sol::TerId) )
- TerId.new( [TerIntId](#sol::TerIntId) )
- TerId.new( string )

### Members {#sol::TerId::@members}

#### NULL_ID {#sol::TerId::NULL_ID}

🇫 Function --> <code>( ) -> [TerId](#sol::TerId)</code>

#### str {#sol::TerId::str}

🇲 Method --> <code>( ) -> string</code>

#### obj {#sol::TerId::obj}

🇲 Method --> <code>( ) -> [TerRaw](#sol::TerRaw)</code>

#### is_null {#sol::TerId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### is_valid {#sol::TerId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::TerId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

#### int_id {#sol::TerId::int_id}

🇲 Method --> <code>( ) -> [TerIntId](#sol::TerIntId)</code>

## TerIntId {#sol::TerIntId}

### Bases {#sol::TerIntId::@bases}

No base classes.

### Constructors {#sol::TerIntId::@ctors}

- TerIntId.new( )
- TerIntId.new( [TerIntId](#sol::TerIntId) )
- TerIntId.new( [TerId](#sol::TerId) )

### Members {#sol::TerIntId::@members}

#### obj {#sol::TerIntId::obj}

🇲 Method --> <code>( ) -> [TerRaw](#sol::TerRaw)</code>

#### is_valid {#sol::TerIntId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str_id {#sol::TerIntId::str_id}

🇲 Method --> <code>( ) -> [TerId](#sol::TerId)</code>

## TerRaw {#sol::TerRaw}

### Bases {#sol::TerRaw::@bases}

No base classes.

### Constructors {#sol::TerRaw::@ctors}

No constructors.

### Members {#sol::TerRaw::@members}

#### heat_radiation {#sol::TerRaw::heat_radiation}

🇻 Variable --> <code>int</code>

#### close {#sol::TerRaw::close}

🇻 Variable --> <code>[TerId](#sol::TerId)</code>

#### trap_id_str {#sol::TerRaw::trap_id_str}

🇻 Variable --> <code>string</code>

#### open {#sol::TerRaw::open}

🇻 Variable --> <code>[TerId](#sol::TerId)</code>

#### roof {#sol::TerRaw::roof}

🇻 Variable --> <code>[TerId](#sol::TerId)</code>

#### transforms_into {#sol::TerRaw::transforms_into}

🇻 Variable --> <code>[TerId](#sol::TerId)</code>

#### set_coverage {#sol::TerRaw::set_coverage}

🇲 Method --> <code>( integer )</code>

#### get_max_volume {#sol::TerRaw::get_max_volume}

🇲 Method --> <code>( ) -> [Volume](#sol::Volume)</code>

#### set_max_volume {#sol::TerRaw::set_max_volume}

🇲 Method --> <code>( [Volume](#sol::Volume) )</code>

#### get_coverage {#sol::TerRaw::get_coverage}

🇲 Method --> <code>( ) -> integer</code>

#### set_movecost {#sol::TerRaw::set_movecost}

🇲 Method --> <code>( integer )</code>

#### set_light_emitted {#sol::TerRaw::set_light_emitted}

🇲 Method --> <code>( integer )</code>

#### name {#sol::TerRaw::name}

🇲 Method --> <code>( ) -> string</code>

#### int_id {#sol::TerRaw::int_id}

🇲 Method --> <code>( ) -> [TerIntId](#sol::TerIntId)</code>

#### get_movecost {#sol::TerRaw::get_movecost}

🇲 Method --> <code>( ) -> integer</code>

#### get_flags {#sol::TerRaw::get_flags}

🇲 Method --> <code>( ) -> string[]</code>

#### set_flag {#sol::TerRaw::set_flag}

🇲 Method --> <code>( string )</code>

#### has_flag {#sol::TerRaw::has_flag}

🇲 Method --> <code>( string ) -> boolean</code>

#### get_light_emitted {#sol::TerRaw::get_light_emitted}

🇲 Method --> <code>( ) -> integer</code>

#### str_id {#sol::TerRaw::str_id}

🇲 Method --> <code>( ) -> [TerId](#sol::TerId)</code>

## TimeDuration {#sol::TimeDuration}

> Represent duration between 2 fixed points in time

### Bases {#sol::TimeDuration::@bases}

No base classes.

### Constructors {#sol::TimeDuration::@ctors}

- TimeDuration.new( )

### Members {#sol::TimeDuration::@members}

#### from_turns {#sol::TimeDuration::from_turns}

🇫 Function --> <code>( integer ) -> [TimeDuration](#sol::TimeDuration)</code>

#### to_seconds {#sol::TimeDuration::to_seconds}

🇲 Method --> <code>( ) -> integer</code>

#### to_turns {#sol::TimeDuration::to_turns}

🇲 Method --> <code>( ) -> integer</code>

#### to_minutes {#sol::TimeDuration::to_minutes}

🇲 Method --> <code>( ) -> integer</code>

#### to_weeks {#sol::TimeDuration::to_weeks}

🇲 Method --> <code>( ) -> integer</code>

#### to_days {#sol::TimeDuration::to_days}

🇲 Method --> <code>( ) -> integer</code>

#### make_random {#sol::TimeDuration::make_random}

🇲 Method --> <code>( [TimeDuration](#sol::TimeDuration) ) -> [TimeDuration](#sol::TimeDuration)</code>

#### to_hours {#sol::TimeDuration::to_hours}

🇲 Method --> <code>( ) -> integer</code>

#### from_days {#sol::TimeDuration::from_days}

🇫 Function --> <code>( integer ) -> [TimeDuration](#sol::TimeDuration)</code>

#### from_minutes {#sol::TimeDuration::from_minutes}

🇫 Function --> <code>( integer ) -> [TimeDuration](#sol::TimeDuration)</code>

#### from_seconds {#sol::TimeDuration::from_seconds}

🇫 Function --> <code>( integer ) -> [TimeDuration](#sol::TimeDuration)</code>

#### from_weeks {#sol::TimeDuration::from_weeks}

🇫 Function --> <code>( integer ) -> [TimeDuration](#sol::TimeDuration)</code>

#### from_hours {#sol::TimeDuration::from_hours}

🇫 Function --> <code>( integer ) -> [TimeDuration](#sol::TimeDuration)</code>

## TimePoint {#sol::TimePoint}

> Library for dealing with time primitives.\
> Represent fixed point in time

### Bases {#sol::TimePoint::@bases}

No base classes.

### Constructors {#sol::TimePoint::@ctors}

- TimePoint.new( )

### Members {#sol::TimePoint::@members}

#### from_turn {#sol::TimePoint::from_turn}

🇫 Function --> <code>( integer ) -> [TimePoint](#sol::TimePoint)</code>

#### second_of_minute {#sol::TimePoint::second_of_minute}

🇲 Method --> <code>( ) -> integer</code>

#### minute_of_hour {#sol::TimePoint::minute_of_hour}

🇲 Method --> <code>( ) -> integer</code>

#### to_string_time_of_day {#sol::TimePoint::to_string_time_of_day}

🇲 Method --> <code>( ) -> string</code>

#### is_dawn {#sol::TimePoint::is_dawn}

🇲 Method --> <code>( ) -> boolean</code>

#### hour_of_day {#sol::TimePoint::hour_of_day}

🇲 Method --> <code>( ) -> integer</code>

#### is_day {#sol::TimePoint::is_day}

🇲 Method --> <code>( ) -> boolean</code>

#### is_dusk {#sol::TimePoint::is_dusk}

🇲 Method --> <code>( ) -> boolean</code>

#### to_turn {#sol::TimePoint::to_turn}

🇲 Method --> <code>( ) -> integer</code>

#### is_night {#sol::TimePoint::is_night}

🇲 Method --> <code>( ) -> boolean</code>

## Tinymap {#sol::Tinymap}

### Bases {#sol::Tinymap::@bases}

- `Map`

### Constructors {#sol::Tinymap::@ctors}

No constructors.

### Members {#sol::Tinymap::@members}

No members.

## TrapId {#sol::TrapId}

### Bases {#sol::TrapId::@bases}

No base classes.

### Constructors {#sol::TrapId::@ctors}

- TrapId.new( )
- TrapId.new( [TrapId](#sol::TrapId) )
- TrapId.new( [TrapIntId](#sol::TrapIntId) )
- TrapId.new( string )

### Members {#sol::TrapId::@members}

#### NULL_ID {#sol::TrapId::NULL_ID}

🇫 Function --> <code>( ) -> [TrapId](#sol::TrapId)</code>

#### str {#sol::TrapId::str}

🇲 Method --> <code>( ) -> string</code>

#### obj {#sol::TrapId::obj}

🇲 Method --> <code>( ) -> TrapRaw</code>

#### is_null {#sol::TrapId::is_null}

🇲 Method --> <code>( ) -> boolean</code>

#### is_valid {#sol::TrapId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### implements_int_id {#sol::TrapId::implements_int_id}

🇫 Function --> <code>( ) -> boolean</code>

#### int_id {#sol::TrapId::int_id}

🇲 Method --> <code>( ) -> [TrapIntId](#sol::TrapIntId)</code>

## TrapIntId {#sol::TrapIntId}

### Bases {#sol::TrapIntId::@bases}

No base classes.

### Constructors {#sol::TrapIntId::@ctors}

- TrapIntId.new( )
- TrapIntId.new( [TrapIntId](#sol::TrapIntId) )
- TrapIntId.new( [TrapId](#sol::TrapId) )

### Members {#sol::TrapIntId::@members}

#### obj {#sol::TrapIntId::obj}

🇲 Method --> <code>( ) -> TrapRaw</code>

#### is_valid {#sol::TrapIntId::is_valid}

🇲 Method --> <code>( ) -> boolean</code>

#### str_id {#sol::TrapIntId::str_id}

🇲 Method --> <code>( ) -> [TrapId](#sol::TrapId)</code>

## Tripoint {#sol::Tripoint}

### Bases {#sol::Tripoint::@bases}

No base classes.

### Constructors {#sol::Tripoint::@ctors}

- Tripoint.new( )
- Tripoint.new( [Point](#sol::Point), int )
- Tripoint.new( [Tripoint](#sol::Tripoint) )
- Tripoint.new( int, int, int )

### Members {#sol::Tripoint::@members}

#### x {#sol::Tripoint::x}

🇻 Variable --> <code>int</code>

#### z {#sol::Tripoint::z}

🇻 Variable --> <code>int</code>

#### y {#sol::Tripoint::y}

🇻 Variable --> <code>int</code>

#### xy {#sol::Tripoint::xy}

🇲 Method --> <code>( ) -> [Point](#sol::Point)</code>

#### rotate_2d {#sol::Tripoint::rotate_2d}

🇲 Method --> <code>( integer, [Point](#sol::Point) ) -> [Tripoint](#sol::Tripoint)</code>

#### abs {#sol::Tripoint::abs}

🇲 Method --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

## UiList {#sol::UiList}

### Bases {#sol::UiList::@bases}

No base classes.

### Constructors {#sol::UiList::@ctors}

- UiList.new( )

### Members {#sol::UiList::@members}

#### entries {#sol::UiList::entries}

🇻 Variable --> <code>Vector( [UiListEntry](#sol::UiListEntry) )</code>

> Entries from uilist. Remember, in lua, the first element of vector is `entries[1]`, not `entries[0]`.

#### border_color {#sol::UiList::border_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

> Changes the color. Default color is `c_magenta`.

#### title {#sol::UiList::title}

🇲 Method --> <code>( string )</code>

> Sets title which is on the top line.

#### text_color {#sol::UiList::text_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

> Changes the color. Default color is `c_light_gray`.

#### hilight_color {#sol::UiList::hilight_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

> Changes the color. Default color is `h_white`.

#### title_color {#sol::UiList::title_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

> Changes the color. Default color is `c_green`.

#### hotkey_color {#sol::UiList::hotkey_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

> Changes the color. Default color is `c_light_green`.

#### add_w_col {#sol::UiList::add_w_col}

🇲 Method --> <code>( integer, string, string, string )</code>

> Adds an entry with desc and col(third `string`). col is additional text on the right of the entry name.

#### add {#sol::UiList::add}

🇲 Method --> <code>( integer, string )</code>

> Adds an entry. `string` is its name, and `int` is what it returns. If `int` is `-1`, the number is decided orderly.

#### text {#sol::UiList::text}

🇲 Method --> <code>( string )</code>

> Sets text which is in upper box.

#### add_w_desc {#sol::UiList::add_w_desc}

🇲 Method --> <code>( integer, string, string )</code>

> Adds an entry with desc(second `string`). `desc_enabled(true)` is required for showing desc.

#### footer {#sol::UiList::footer}

🇲 Method --> <code>( string )</code>

> Sets footer text which is in lower box. It overwrites descs of entries unless is empty.

#### desc_enabled {#sol::UiList::desc_enabled}

🇲 Method --> <code>( boolean )</code>

> Puts a lower box. Footer or entry desc appears on it.

#### query {#sol::UiList::query}

🇲 Method --> <code>( ) -> integer</code>

> Returns retval for selected entry, or a negative number on fail/cancel

## UiListEntry {#sol::UiListEntry}

> This type came from UiList.

### Bases {#sol::UiListEntry::@bases}

No base classes.

### Constructors {#sol::UiListEntry::@ctors}

No constructors.

### Members {#sol::UiListEntry::@members}

#### enable {#sol::UiListEntry::enable}

🇻 Variable --> <code>bool</code>

> Entry whether it's enabled or not. Default is `true`.

#### ctxt {#sol::UiListEntry::ctxt}

🇻 Variable --> <code>string</code>

> Entry text of column.

#### desc {#sol::UiListEntry::desc}

🇻 Variable --> <code>string</code>

> Entry description

#### txt {#sol::UiListEntry::txt}

🇻 Variable --> <code>string</code>

> Entry text

#### txt_color {#sol::UiListEntry::txt_color}

🇲 Method --> <code>( [Color](#sol::Color) )</code>

> Entry text color. Its default color is `c_red_red`, which makes color of the entry same as what `uilist` decides. So if you want to make color different, choose one except `c_red_red`.

## Volume {#sol::Volume}

### Bases {#sol::Volume::@bases}

No base classes.

### Constructors {#sol::Volume::@ctors}

No constructors.

### Members {#sol::Volume::@members}

#### from_milliliter {#sol::Volume::from_milliliter}

🇫 Function --> <code>( integer ) -> [Volume](#sol::Volume)</code>

#### to_milliliter {#sol::Volume::to_milliliter}

🇲 Method --> <code>( ) -> integer</code>

#### to_liter {#sol::Volume::to_liter}

🇲 Method --> <code>( ) -> number</code>

#### from_liter {#sol::Volume::from_liter}

🇫 Function --> <code>( integer ) -> [Volume](#sol::Volume)</code>

# Enums

## AddictionType {#sol::AddictionType}

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

## Attitude {#sol::Attitude}

### Entries

- `Hostile` = `0`
- `Neutral` = `1`
- `Friendly` = `2`
- `Any` = `3`

## BodyPart {#sol::BodyPart}

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

## CharacterMoveMode {#sol::CharacterMoveMode}

### Entries

- `walk` = `0`
- `run` = `1`
- `crouch` = `2`

## Color {#sol::Color}

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

## DamageType {#sol::DamageType}

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

## MissionGoal {#sol::MissionGoal}

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

## MissionOrigin {#sol::MissionOrigin}

### Entries

- `ORIGIN_NULL` = `0`
- `ORIGIN_GAME_START` = `1`
- `ORIGIN_OPENER_NPC` = `2`
- `ORIGIN_ANY_NPC` = `3`
- `ORIGIN_SECONDARY` = `4`
- `ORIGIN_COMPUTER` = `5`

## MonsterAttitude {#sol::MonsterAttitude}

### Entries

- `MATT_NULL` = `0`
- `MATT_FRIEND` = `1`
- `MATT_FPASSIVE` = `2`
- `MATT_FLEE` = `3`
- `MATT_IGNORE` = `4`
- `MATT_FOLLOW` = `5`
- `MATT_ATTACK` = `6`
- `MATT_ZLAVE` = `7`

## MonsterFactionAttitude {#sol::MonsterFactionAttitude}

### Entries

- `ByMood` = `0`
- `Neutral` = `1`
- `Friendly` = `2`
- `Hate` = `3`

## MonsterFlag {#sol::MonsterFlag}

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

## MonsterSize {#sol::MonsterSize}

### Entries

- `TINY` = `0`
- `SMALL` = `1`
- `MEDIUM` = `2`
- `LARGE` = `3`
- `HUGE` = `4`

## MsgType {#sol::MsgType}

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

## NpcAttitude {#sol::NpcAttitude}

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

## NpcNeed {#sol::NpcNeed}

### Entries

- `need_none` = `0`
- `need_ammo` = `1`
- `need_weapon` = `2`
- `need_gun` = `3`
- `need_food` = `4`
- `need_drink` = `5`
- `need_safety` = `6`

## SfxChannel {#sol::SfxChannel}

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

## const {#sol::const}

Various game constants

### Members

#### OM_OMT_SIZE {#sol::nil::OM_OMT_SIZE}

🇨 Constant --> <code>int</code> = `180`

#### OMT_SM_SIZE {#sol::nil::OMT_SM_SIZE}

🇨 Constant --> <code>int</code> = `2`

#### OMT_MS_SIZE {#sol::nil::OMT_MS_SIZE}

🇨 Constant --> <code>int</code> = `24`

#### OM_MS_SIZE {#sol::nil::OM_MS_SIZE}

🇨 Constant --> <code>int</code> = `4320`

#### OM_SM_SIZE {#sol::nil::OM_SM_SIZE}

🇨 Constant --> <code>int</code> = `360`

#### SM_MS_SIZE {#sol::nil::SM_MS_SIZE}

🇨 Constant --> <code>int</code> = `12`

## coords {#sol::coords}

Methods for manipulating coord systems and calculating distance

### Members

#### ms_to_sm {#sol::nil::ms_to_sm}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint) ) -> ([Tripoint](#sol::Tripoint),[Point](#sol::Point))</code>

#### om_to_ms {#sol::nil::om_to_ms}

🇫 Function --> <code>( [Point](#sol::Point), [Tripoint](#sol::Tripoint) ) -> [Tripoint](#sol::Tripoint)</code>

#### rl_dist {#sol::nil::rl_dist}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), [Tripoint](#sol::Tripoint) ) -> integer</code>\
🇫 Function --> <code>( [Point](#sol::Point), [Point](#sol::Point) ) -> integer</code>

#### trig_dist {#sol::nil::trig_dist}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), [Tripoint](#sol::Tripoint) ) -> number</code>\
🇫 Function --> <code>( [Point](#sol::Point), [Point](#sol::Point) ) -> number</code>

#### omt_to_ms {#sol::nil::omt_to_ms}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), [Point](#sol::Point) ) -> [Tripoint](#sol::Tripoint)</code>

#### ms_to_om {#sol::nil::ms_to_om}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint) ) -> ([Point](#sol::Point),[Tripoint](#sol::Tripoint))</code>

#### sm_to_ms {#sol::nil::sm_to_ms}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), [Point](#sol::Point) ) -> [Tripoint](#sol::Tripoint)</code>

#### ms_to_omt {#sol::nil::ms_to_omt}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint) ) -> ([Tripoint](#sol::Tripoint),[Point](#sol::Point))</code>

#### square_dist {#sol::nil::square_dist}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), [Tripoint](#sol::Tripoint) ) -> integer</code>\
🇫 Function --> <code>( [Point](#sol::Point), [Point](#sol::Point) ) -> integer</code>

## gapi {#sol::gapi}

Global game methods

### Members

#### get_avatar {#sol::nil::get_avatar}

🇫 Function --> <code>( ) -> [Avatar](#sol::Avatar)</code>

#### get_character_at {#sol::nil::get_character_at}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), boolean ) -> [Character](#sol::Character)</code>

#### get_npc_at {#sol::nil::get_npc_at}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), boolean ) -> [Npc](#sol::Npc)</code>

#### place_monster_around {#sol::nil::place_monster_around}

🇫 Function --> <code>( [MtypeId](#sol::MtypeId), [Tripoint](#sol::Tripoint), integer ) -> [Monster](#sol::Monster)</code>

#### place_monster_at {#sol::nil::place_monster_at}

🇫 Function --> <code>( [MtypeId](#sol::MtypeId), [Tripoint](#sol::Tripoint) ) -> [Monster](#sol::Monster)</code>

#### get_monster_at {#sol::nil::get_monster_at}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), boolean ) -> [Monster](#sol::Monster)</code>

#### choose_adjacent {#sol::nil::choose_adjacent}

🇫 Function --> <code>( string, boolean ) -> [Tripoint](#sol::Tripoint)</code>

#### look_around {#sol::nil::look_around}

🇫 Function --> <code>( ) -> [Tripoint](#sol::Tripoint)</code>

#### choose_direction {#sol::nil::choose_direction}

🇫 Function --> <code>( string, boolean ) -> [Tripoint](#sol::Tripoint)</code>

#### play_ambient_variant_sound {#sol::nil::play_ambient_variant_sound}

🇫 Function --> <code>( string, string, integer, [SfxChannel](#sol::SfxChannel), integer, number, integer )</code>

#### play_variant_sound {#sol::nil::play_variant_sound}

🇫 Function --> <code>( string, string, integer )</code>\
🇫 Function --> <code>( string, string, integer, [Angle](#sol::Angle), number, number )</code>

#### add_npc_follower {#sol::nil::add_npc_follower}

🇫 Function --> <code>( [Npc](#sol::Npc) )</code>

#### get_creature_at {#sol::nil::get_creature_at}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint), boolean ) -> [Creature](#sol::Creature)</code>

#### add_on_every_x_hook {#sol::nil::add_on_every_x_hook}

🇫 Function --> <code>( [TimeDuration](#sol::TimeDuration), function )</code>

#### add_msg {#sol::nil::add_msg}

🇫 Function --> <code>( [MsgType](#sol::MsgType), any )</code>\
🇫 Function --> <code>( any )</code>

#### get_distribution_grid_tracker {#sol::nil::get_distribution_grid_tracker}

🇫 Function --> <code>( ) -> [DistributionGridTracker](#sol::DistributionGridTracker)</code>

#### get_map {#sol::nil::get_map}

🇫 Function --> <code>( ) -> [Map](#sol::Map)</code>

#### create_item {#sol::nil::create_item}

🇫 Function --> <code>( [ItypeId](#sol::ItypeId), integer ) -> any</code>

#### place_player_overmap_at {#sol::nil::place_player_overmap_at}

🇫 Function --> <code>( [Tripoint](#sol::Tripoint) )</code>

#### turn_zero {#sol::nil::turn_zero}

🇫 Function --> <code>( ) -> [TimePoint](#sol::TimePoint)</code>

#### current_turn {#sol::nil::current_turn}

🇫 Function --> <code>( ) -> [TimePoint](#sol::TimePoint)</code>

#### rng {#sol::nil::rng}

🇫 Function --> <code>( integer, integer ) -> integer</code>

#### before_time_starts {#sol::nil::before_time_starts}

🇫 Function --> <code>( ) -> [TimePoint](#sol::TimePoint)</code>

#### remove_npc_follower {#sol::nil::remove_npc_follower}

🇫 Function --> <code>( [Npc](#sol::Npc) )</code>

## gdebug {#sol::gdebug}

Debugging and logging API.

### Members

#### log_info {#sol::nil::log_info}

🇫 Function --> <code>( any )</code>

#### clear_lua_log {#sol::nil::clear_lua_log}

🇫 Function --> <code>( )</code>

#### set_log_capacity {#sol::nil::set_log_capacity}

🇫 Function --> <code>( integer )</code>

#### reload_lua_code {#sol::nil::reload_lua_code}

🇫 Function --> <code>( )</code>

#### debugmsg {#sol::nil::debugmsg}

🇫 Function --> <code>( any )</code>

#### log_warn {#sol::nil::log_warn}

🇫 Function --> <code>( any )</code>

#### log_error {#sol::nil::log_error}

🇫 Function --> <code>( any )</code>

#### save_game {#sol::nil::save_game}

🇫 Function --> <code>( ) -> boolean</code>

## hooks {#sol::hooks}

Documentation for hooks

### Members

#### on_game_save {#sol::nil::on_game_save}

🇫 Function --> <code>( )</code>

> Called when game is about to save

#### on_character_reset_stats {#sol::nil::on_character_reset_stats}

🇫 Function --> <code>( )</code>

> Called when character stat gets reset

#### on_creature_melee_attacked {#sol::nil::on_creature_melee_attacked}

🇫 Function --> <code>( )</code>

> Called after a character has attacked in melee

#### on_char_death {#sol::nil::on_char_death}

🇫 Function --> <code>( )</code>

> Called when a character is dead

#### on_mon_death {#sol::nil::on_mon_death}

🇫 Function --> <code>( )</code>

> Called when a monster is dead

#### on_every_x {#sol::nil::on_every_x}

🇫 Function --> <code>( )</code>

> Called every in-game period

#### on_creature_performed_technique {#sol::nil::on_creature_performed_technique}

🇫 Function --> <code>( )</code>

> Called when a character has performed technique

#### on_creature_dodged {#sol::nil::on_creature_dodged}

🇫 Function --> <code>( )</code>

> Called when a character successfully dodges

#### on_creature_blocked {#sol::nil::on_creature_blocked}

🇫 Function --> <code>( )</code>

> Called when a character successfully blocks

#### on_game_load {#sol::nil::on_game_load}

🇫 Function --> <code>( )</code>

> Called right after game has loaded

#### on_game_started {#sol::nil::on_game_started}

🇫 Function --> <code>( )</code>

> Called when the game has first started

#### on_mapgen_postprocess {#sol::nil::on_mapgen_postprocess}

🇫 Function --> <code>( [Map](#sol::Map), [Tripoint](#sol::Tripoint), [TimePoint](#sol::TimePoint) )</code>

> Called right after mapgen has completed. Map argument is the tinymap that represents 24x24 area (2x2 submaps, or 1x1 omt), tripoint is the absolute omt pos, and time_point is the current time (for time-based effects).

## locale {#sol::locale}

Localization API.

### Members

#### gettext {#sol::nil::gettext}

🇫 Function --> <code>( string ) -> string</code>

> Expects english source string, returns translated string.

#### pgettext {#sol::nil::pgettext}

🇫 Function --> <code>( string, string ) -> string</code>

> First is context string. Second is english source string.

#### vgettext {#sol::nil::vgettext}

🇫 Function --> <code>( string, string, any ) -> string</code>

> First is english singular string, second is english plural string. Number is amount to translate for.

#### vpgettext {#sol::nil::vpgettext}

🇫 Function --> <code>( string, string, string, any ) -> string</code>

> First is context string. Second is english singular string. third is english plural. Number is amount to translate for.

## tests_lib {#sol::tests_lib}

Library for testing purposes

### Members

#### my_awesome_lambda_1 {#sol::nil::my_awesome_lambda_1}

🇫 Function --> <code>( ) -> integer</code>

#### my_awesome_lambda_2 {#sol::nil::my_awesome_lambda_2}

🇫 Function --> <code>( ) -> integer</code>
