#include "catalua_bindings.h"

#include "catalua_bindings_utils.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

#include "itype.h"
#include "mtype.h"
#include "material.h"
#include "faction.h"
#include "character.h"
#include "martialarts.h"
#include "relic.h"
#include "ammo_effect.h"
#include "mongroup.h"
#include "disease.h"
#include "skill.h"
#include "ammo.h"

static void reg_islot( sol::state &lua );
static void reg_itype( sol::state &lua );
static void reg_item( sol::state &lua );

void cata::detail::reg_item( sol::state &lua )
{
    ::reg_itype( lua );
    ::reg_islot( lua );
    ::reg_item( lua );
}

void reg_item( sol::state &lua )
{
    {
        sol::usertype<item> ut = luna::new_usertype<item>( lua, luna::no_bases, luna::no_constructor );

        luna::set_fx( ut, "get_type", &item::typeId );
        DOC( "Almost for a corpse." );
        luna::set_fx( ut, "get_mtype",
        []( const item & it ) { return it.get_mtype() ? it.get_mtype()->id : mtype_id::NULL_ID(); } );

        DOC( "Translated item name with prefixes" );
        luna::set_fx( ut, "tname", &item::tname );

        DOC( "Display name with all bells and whistles like ammo and prefixes" );
        luna::set_fx( ut, "display_name", &item::display_name );

        DOC( "Weight of the item. The first `bool` is whether including contents, second `bool` is whether it is `integral_weight`." );
        luna::set_fx( ut, "weight", [](
                          item & it,
                          sol::optional<bool> include_contents,
                          sol::optional<bool> integral
        ) { return it.weight( include_contents.value_or( true ), integral.value_or( false ) ); } );

        DOC( "Volume of the item. `bool` is whether it is `integral_volume`." );
        luna::set_fx( ut, "volume",
        []( item & it, sol::optional<bool> integral ) { return it.volume( integral.value_or( false ) ); } );

        DOC( "Cents of the item. `bool` is whether it is a post-cataclysm value." );
        luna::set_fx( ut, "price", &item::price );

        DOC( "Check for variable of any type" );
        luna::set_fx( ut, "has_var", &item::has_var );
        DOC( "Erase variable" );
        luna::set_fx( ut, "erase_var", &item::erase_var );
        DOC( "Erase all variables" );
        luna::set_fx( ut, "clear_vars", &item::clear_vars );

        luna::set_fx( ut, "is_null", &item::is_null );
        luna::set_fx( ut, "is_unarmed_weapon", &item::is_unarmed_weapon );
        luna::set_fx( ut, "is_sided", &item::is_sided );
        luna::set_fx( ut, "is_power_armor", &item::is_power_armor );
        luna::set_fx( ut, "is_money", &item::is_money );
        luna::set_fx( ut, "is_gun", &item::is_gun );
        luna::set_fx( ut, "is_firearm", &item::is_firearm );
        luna::set_fx( ut, "is_silent", &item::is_silent );
        luna::set_fx( ut, "is_gunmod", &item::is_gunmod );
        luna::set_fx( ut, "is_bionic", &item::is_bionic );
        luna::set_fx( ut, "is_ammo_belt", &item::is_ammo_belt );
        luna::set_fx( ut, "is_bandolier", &item::is_bandolier );
        luna::set_fx( ut, "is_holster", &item::is_holster );
        luna::set_fx( ut, "is_ammo", &item::is_ammo );
        luna::set_fx( ut, "is_comestible", &item::is_comestible );
        luna::set_fx( ut, "is_food", &item::is_food );
        luna::set_fx( ut, "is_medication", &item::is_medication );
        luna::set_fx( ut, "is_brewable", &item::is_brewable );
        luna::set_fx( ut, "is_food_container", &item::is_food_container );
        luna::set_fx( ut, "is_med_container", &item::is_med_container );
        luna::set_fx( ut, "is_corpse", &item::is_corpse );
        luna::set_fx( ut, "is_ammo_container", &item::is_ammo_container );
        luna::set_fx( ut, "is_armor", &item::is_armor );
        luna::set_fx( ut, "is_book", &item::is_book );
        luna::set_fx( ut, "is_map", &item::is_map );
        luna::set_fx( ut, "is_container", &item::is_container );
        luna::set_fx( ut, "is_watertight_container", &item::is_watertight_container );
        luna::set_fx( ut, "is_non_resealable_container", &item::is_non_resealable_container );
        luna::set_fx( ut, "is_bucket", &item::is_bucket );
        luna::set_fx( ut, "is_bucket_nonempty", &item::is_bucket_nonempty );
        luna::set_fx( ut, "is_engine", &item::is_engine );
        luna::set_fx( ut, "is_wheel", &item::is_wheel );
        luna::set_fx( ut, "is_fuel", &item::is_fuel );
        luna::set_fx( ut, "is_toolmod", &item::is_toolmod );
        luna::set_fx( ut, "is_faulty", &item::is_faulty );
        luna::set_fx( ut, "is_irremovable", &item::is_irremovable );
        luna::set_fx( ut, "is_container_empty", &item::is_container_empty );
        luna::set_fx( ut, "is_salvageable", &item::is_salvageable );
        luna::set_fx( ut, "is_craft", &item::is_craft );
        luna::set_fx( ut, "is_emissive", &item::is_emissive );
        luna::set_fx( ut, "is_deployable", &item::is_deployable );
        luna::set_fx( ut, "is_tool", &item::is_tool );
        luna::set_fx( ut, "is_transformable", &item::is_transformable );
        luna::set_fx( ut, "is_artifact", &item::is_artifact );
        luna::set_fx( ut, "is_relic", &item::is_relic );
        luna::set_fx( ut, "is_seed", &item::is_seed );
        luna::set_fx( ut, "is_dangerous", &item::is_dangerous );
        luna::set_fx( ut, "is_tainted", &item::is_tainted );
        luna::set_fx( ut, "is_soft", &item::is_soft );
        luna::set_fx( ut, "is_reloadable", &item::is_reloadable );
        DOC( "DEPRECATED: Items are no longer filthy" );
        luna::set_fx( ut, "is_filthy", []() { return false; } );
        luna::set_fx( ut, "is_active", &item::is_active );
        luna::set_fx( ut, "is_upgrade", &item::is_upgrade );

        luna::set_fx( ut, "activate", &item::activate );
        luna::set_fx( ut, "deactivate", &item::deactivate );
        luna::set_fx( ut, "set_charges", &item::set_charges );
        luna::set_fx( ut, "set_countdown", &item::set_countdown );

        DOC( "Is this item an effective melee weapon for the given damage type?" );
        luna::set_fx( ut, "is_melee", sol::resolve<bool( damage_type ) const>
                      ( &item::is_melee ) );

        DOC( "Is this a magazine? (batteries are magazines)" );
        luna::set_fx( ut, "is_magazine", &item::is_magazine );

        DOC( "DEPRECATED: Is this a battery? (spoiler: it isn't)" );
        luna::set_fx( ut, "is_battery", &item::is_battery );

        luna::set_fx( ut, "conductive", &item::conductive );

        luna::set_fx( ut, "is_stackable", sol::resolve<bool() const> ( &item::count_by_charges ) );

        luna::set( ut, "charges", &item::charges );

        luna::set_fx( ut, "energy_remaining", &item::energy_remaining );

        luna::set_fx( ut, "has_infinite_charges", &item::has_infinite_charges );

        luna::set_fx( ut, "mod_charges", &item::mod_charges );

        luna::set_fx( ut, "made_of", []( item & it ) {
            return it.made_of();
        } );

        luna::set_fx( ut, "is_made_of",
                      sol::resolve < auto( const material_id & ) const -> bool > ( &item::made_of ) );

        luna::set_fx( ut, "get_kcal", []( item & it ) -> int { return it.is_comestible() ? it.get_comestible()->default_nutrition.kcal : 0; } );
        luna::set_fx( ut, "get_quench", []( item & it ) -> int { return it.is_comestible() ? it.get_comestible()->quench : 0; } );
        luna::set_fx( ut, "get_comestible_fun", []( item & it ) -> int { return it.get_comestible_fun(); } );
        DOC( "Gets the TimeDuration until this item rots" );
        luna::set_fx( ut, "get_rot", &item::get_rot );

        DOC( "Gets the category id this item is in" );
        luna::set_fx( ut, "get_category_id", &item::get_category_id );

        DOC( "Gets the faction id that owns this item" );
        luna::set_fx( ut, "get_owner", &item::get_owner );

        DOC( "Sets the ownership of this item to a faction" );
        luna::set_fx( ut, "set_owner",
                      sol::resolve<void( const faction_id & )>
                      ( &item::set_owner ) );

        DOC( "Sets the ownership of this item to a character" );
        luna::set_fx( ut, "set_owner",
                      sol::resolve<void( const Character & )>
                      ( &item::set_owner ) );

        luna::set_fx( ut, "get_owner_name", &item::get_owner_name );

        DOC( "Checks if this item owned by a character" );
        luna::set_fx( ut, "is_owned_by", &item::is_owned_by );

        DOC( "Checks if this item has the technique as an addition. Doesn't check original techniques." );
        luna::set_fx( ut, "has_technique",
                      sol::resolve<bool( const matec_id & ) const> ( &item::has_technique ) );
        DOC( "Gets all techniques. Including original techniques." );
        luna::set_fx( ut, "get_techniques",
                      sol::resolve<std::set<matec_id>() const> ( &item::get_techniques ) );
        DOC( "Adds the technique. It isn't treated original, but additional." );
        luna::set_fx( ut, "add_technique",
                      sol::resolve<void( const matec_id & )> ( &item::add_technique ) );
        DOC( "Removes the additional technique. Doesn't affect originial techniques." );
        luna::set_fx( ut, "remove_technique",
                      sol::resolve<void( const matec_id & )> ( &item::remove_technique ) );

        DOC( "Checks if this item can contain another" );
        luna::set_fx( ut, "can_contain",
                      sol::resolve<bool( const item & ) const>
                      ( &item::can_contain ) );

        DOC( "Gets the remaining space available for a type of liquid" );
        luna::set_fx( ut, "remaining_capacity_for_id", &item::get_remaining_capacity_for_id );

        DOC( "Gets maximum volume this item can hold (liquids, ammo, etc)" );
        luna::set_fx( ut, "total_capacity", &item::get_total_capacity );

        DOC( "Gets the current magazine" );
        luna::set_fx( ut, "current_magazine",
                      sol::resolve<const item*() const> ( &item::magazine_current ) );

        DOC( "Gets the maximum capacity of a magazine" );
        luna::set_fx( ut, "ammo_capacity",
                      sol::resolve<int( const bool ) const>
                      ( &item::ammo_capacity ) );

        DOC( "Get remaining ammo, works with batteries & stuff too" );
        luna::set_fx( ut, "ammo_remaining", &item::ammo_remaining );

        luna::set_fx( ut, "ammo_data", &item::ammo_data );
        luna::set_fx( ut, "ammo_required", &item::ammo_required );
        luna::set_fx( ut, "ammo_current", &item::ammo_current );

        luna::set_fx( ut, "ammo_consume", &item::ammo_consume );
        luna::set_fx( ut, "ammo_set", &item::ammo_set );
        luna::set_fx( ut, "ammo_unset", &item::ammo_unset );

        luna::set_fx( ut, "get_reload_time", &item::get_reload_time );

        DOC( "Adds an item(s) to contents" );
        luna::set_fx( ut, "add_item_with_id", &item::add_item_with_id );

        DOC( "Checks item contents for a given item id" );
        luna::set_fx( ut, "has_item_with_id", &item::has_item_with_id );

        DOC( "Checks if the item covers a bodypart" );
        luna::set_fx( ut, "covers", &item::covers );

        luna::set_fx( ut, "set_flag", &item::set_flag );
        luna::set_fx( ut, "unset_flag", &item::unset_flag );
        luna::set_fx( ut, "has_flag", &item::has_flag );
        luna::set_fx( ut, "has_own_flag", &item::has_own_flag );
        luna::set_fx( ut, "set_flag_recursive", &item::set_flag_recursive );
        luna::set_fx( ut, "unset_flags", &item::unset_flags );

        DOC( "Converts the item as given `ItypeId`." );
        luna::set_fx( ut, "convert", &item::convert );

        DOC( "Get variable as string" );
        luna::set_fx( ut, "get_var_str",
                      sol::resolve<std::string( const std::string &, const std::string & ) const>
                      ( &item::get_var ) );
        DOC( "Get variable as float number" );
        luna::set_fx( ut, "get_var_num",
                      sol::resolve<double( const std::string &, double ) const>( &item::get_var ) );
        DOC( "Get variable as tripoint" );
        luna::set_fx( ut, "get_var_tri",
                      sol::resolve<tripoint( const std::string &, const tripoint & ) const>
                      ( &item::get_var ) );

        luna::set_fx( ut, "set_var_str", sol::resolve<void( const std::string &, const std::string & )>
                      ( &item::set_var ) );
        luna::set_fx( ut, "set_var_num",
                      sol::resolve<void( const std::string &, double )>( &item::set_var ) );
        luna::set_fx( ut, "set_var_tri",
                      sol::resolve<void( const std::string &, const tripoint & )>( &item::set_var ) );

        luna::set_fx( ut, "attack_cost", &item::attack_cost );
        luna::set_fx( ut, "stamina_cost", &item::stamina_cost );
    }
}

#define VALUE_PTR_MEMB(prop_name) luna::set_fx( ut, #prop_name, [](const UT_CLASS& c) { return c.prop_name.get(); } )
#define VALUE_PTR_MEMB_N(prop_name, lua_name_str) luna::set_fx( ut, lua_name_str, [](const UT_CLASS& c) { return c.prop_name.get(); } )

void reg_itype( sol::state &lua )
{
#define UT_CLASS itype
    {
        DOC("Slots for various item type properties. Each slot may contain a valid value or nil");
        sol::usertype<itype> ut = luna::new_usertype<itype>( lua, luna::no_bases, luna::no_constructor );

        VALUE_PTR_MEMB( container );
        VALUE_PTR_MEMB( tool );
        VALUE_PTR_MEMB( comestible );
        VALUE_PTR_MEMB( brewable );
        VALUE_PTR_MEMB( armor );
        VALUE_PTR_MEMB( pet_armor );
        VALUE_PTR_MEMB( book );
        VALUE_PTR_MEMB( mod );
        VALUE_PTR_MEMB( engine );
        VALUE_PTR_MEMB( wheel );
        VALUE_PTR_MEMB( fuel );
        VALUE_PTR_MEMB( gun );
        VALUE_PTR_MEMB( gunmod );
        VALUE_PTR_MEMB( magazine );
        VALUE_PTR_MEMB( battery );
        VALUE_PTR_MEMB( bionic );
        VALUE_PTR_MEMB( ammo );
        VALUE_PTR_MEMB( seed );
        VALUE_PTR_MEMB( artifact );
        VALUE_PTR_MEMB_N( relic_data, "relic" );
        VALUE_PTR_MEMB_N( milling_data, "milling" );

        DOC("Returns the name of the item type in the correct language and with respect to its grammatical number");
        SET_FX_N( nname, "get_name" );

        //TODO: Add rest of Fields/Functions
    }
#undef UT_CLASS
}

void reg_islot( sol::state &lua )
{
#define UT_CLASS islot_container
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Inner volume of the container");
        SET_MEMB_RO( contains );

        DOC("Contents do not spoil");
        SET_MEMB_RO( preserves );

        DOC("Can be resealed");
        SET_MEMB_RO( seals );

        DOC("If this is set to anything but \"null\", changing this container's contents in any way will turn this item into that type");
        SET_MEMB_RO( unseals_into );

        DOC("Can hold liquids");
        SET_MEMB_RO( watertight );
    }
#undef UT_CLASS

#define UT_CLASS islot_tool
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        SET_MEMB_RO( charge_factor );
        SET_MEMB_RO( ammo_id );
        SET_MEMB_RO( charges_per_use );
        SET_MEMB_RO( def_charges );
        SET_MEMB_RO( default_ammo );
        SET_MEMB_RO( max_charges );
        SET_MEMB_RO( power_draw );
        SET_MEMB_RO( rand_charges );
        SET_MEMB_RO( revert_msg );
        SET_MEMB_RO( revert_to );
        SET_MEMB_RO( subtype );
        SET_MEMB_RO( turns_active );
        SET_MEMB_RO( turns_per_charge );
        SET_MEMB_RO( ups_eff_mult );
        SET_MEMB_RO( ups_recharge_rate );
    }
#undef UT_CLASS

#define UT_CLASS islot_comestible
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("comestible subtype - eg. FOOD, DRINK, MED");
        SET_MEMB_N_RO(comesttype, "comest_type");

        DOC("tool needed to consume (e.g. lighter for cigarettes)");
        SET_MEMB_RO(tool);

        DOC("Defaults # of charges (drugs, loaf of bread? etc)");
        SET_MEMB_RO(def_charges);

        DOC("effect on character thirst (may be negative)");
        SET_MEMB_RO(quench);

        DOC("Nutrition values to use for this type when they aren't calculated from components");
        SET_MEMB_RO(default_nutrition);

        DOC("Time until becomes rotten at standard temperature, or zero if never spoils");
        SET_MEMB_RO(spoils);

        DOC("addiction potential");
        SET_MEMB_N_RO(addict, "addict_value");

        DOC("effects of addiction");
        SET_MEMB_N_RO(add, "addict_type");

        DOC("stimulant effect");
        SET_MEMB_N_RO(stim, "stimulant_type");

        DOC("fatigue altering effect");
        SET_MEMB_RO(fatigue_mod);

        DOC("Reference to other item that replaces this one as a component in recipe results");
        SET_MEMB_RO(cooks_like);

        DOC("Reference to item that will be received after smoking current item");
        SET_MEMB_RO(smoking_result);

        //DOC("TODO: add documentation");
        SET_MEMB_RO(healthy);

        DOC("chance (odds) of becoming parasitised when eating (zero if never occurs)");
        SET_MEMB_RO(parasites);

        DOC("Amount of radiation you get from this comestible");
        SET_MEMB_RO(radiation);

        DOC("pet food category");
        SET_MEMB_RO(petfood);

        DOC("freezing point in degrees Fahrenheit, below this temperature item can freeze");
        SET_MEMB_RO(freeze_point);

        DOC("List of diseases carried by this comestible and their associated probability");
        SET_MEMB_RO(contamination);

        DOC("specific heats in J/(g K) and latent heat in J/g");
        SET_MEMB_RO(specific_heat_liquid);
        SET_MEMB_RO(specific_heat_solid);
        SET_MEMB_RO(latent_heat);

        DOC("A penalty applied to fun for every time this food has been eaten in the last 48 hours");
        SET_MEMB_RO(monotony_penalty);

        SET_FX(has_calories);

        SET_FX(get_default_nutr);

        DOC("The monster group that is drawn from when the item rots away");
        SET_MEMB_RO(rot_spawn);

        DOC("Chance the above monster group spawns");
        SET_MEMB_RO(rot_spawn_chance);

    }
#undef UT_CLASS

#define UT_CLASS islot_brewable
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("What are the results of fermenting this item");
        SET_MEMB_RO( results );

        DOC("How long for this brew to ferment");
        SET_MEMB_RO( time );
    }
#undef UT_CLASS

#define UT_CLASS islot_armor
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Layer, encumbrance and coverage information");
        SET_MEMB_N_RO( data, "layer_data" );
        // TODO: add armor_portion_data binding

        DOC("Resistance to environmental effects");
        SET_MEMB_RO( env_resist );

        DOC("Environmental protection of a gas mask with installed filter");
        SET_MEMB_RO( env_resist_w_filter );

        DOC("Damage negated by this armor. Usually calculated from materials+thickness");
        SET_MEMB_RO( resistance );
        // TODO: add resistances binding

        DOC("Whether this item can be worn on either side of the body");
        SET_MEMB_RO( sided );

        DOC("How much storage this items provides when worn");
        SET_MEMB_RO( storage );

        DOC("Multiplier on resistances provided by armor's materials.  \nDamaged armors have lower effective thickness, low capped at 1.  \nNote: 1 thickness means item retains full resistance when damaged.");
        SET_MEMB_RO( thickness );

        DOC("Whitelisted clothing mods.\nRestricted clothing mods must be listed here by id to be compatible.");
        SET_MEMB_RO( valid_mods );

        DOC("How much warmth this item provides");
        SET_MEMB_RO( warmth );

        DOC("Bonus to weight capacity");
        SET_MEMB_RO( weight_capacity_bonus );

        DOC("Factor modifying weight capacity");
        SET_MEMB_RO( weight_capacity_modifier );
    }
#undef UT_CLASS

#define UT_CLASS islot_pet_armor
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("The minimum volume a pet can be and wear this armor");
        SET_MEMB_RO( min_vol );

        DOC("The maximum volume a pet can be and wear this armor");
        SET_MEMB_RO( max_vol );

        DOC("Resistance to environmental effects");
        SET_MEMB_RO( env_resist );

        DOC("Environmental protection of a gas mask with installed filter");
        SET_MEMB_RO( env_resist_w_filter );

        DOC(" How much storage this items provides when worn");
        SET_MEMB_RO( storage );

        DOC("Multiplier on resistances provided by this armor");
        SET_MEMB_RO( thickness );

        DOC("What animal bodytype can wear this armor");
        SET_MEMB_RO( bodytype );
    }
#undef UT_CLASS

#define UT_CLASS islot_book
    {
        auto ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("How long in minutes it takes to read.  \n\"To read\" means getting 1 skill point, not all of them.");
        SET_MEMB_RO( time );

        DOC("Fun books have chapters; after all are read, the book is less fun.");
        SET_MEMB_RO( chapters );

        DOC("Which martial art it teaches.  Can be MartialArtsId.NULL_ID");
        SET_MEMB_RO( martial_art );

        DOC("How fun reading this is, can be negative");
        SET_MEMB_RO( fun );

        DOC("Intelligence required to read it");
        SET_MEMB_N_RO( intel, "intelligence" );

        DOC("Which skill it upgrades, if any. Can be SkillId.NULL_ID");
        SET_MEMB_RO( skill );

        DOC("The skill level required to understand it");
        SET_MEMB_N_RO( req, "skill_min" );

        DOC("The skill level the book provides");
        SET_MEMB_N_RO( level, "skill_max" );

        DOC("Recipes contained in this book");
        SET_MEMB_RO( recipes );
    }
#undef UT_CLASS

#define UT_CLASS islot_mod
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("If non-empty restrict mod to items with those base (before modifiers) ammo types");
        SET_MEMB_RO( acceptable_ammo );

        DOC("If set modifies parent ammo to this type");
        SET_MEMB_RO( ammo_modifier );

        DOC("Proportional adjustment of parent item ammo capacity");
        SET_MEMB_RO( capacity_multiplier );

        DOC("If non-empty replaces the compatible magazines for the parent item");
        SET_MEMB_RO( magazine_adaptor );
    }
#undef UT_CLASS

#define UT_CLASS islot_engine
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("For combustion engines, the displacement");
        SET_MEMB_RO( displacement );
    }
#undef UT_CLASS

#define UT_CLASS islot_wheel
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Diameter of wheel in inches");
        SET_MEMB_RO( diameter );

        DOC("Width of wheel in inches");
        SET_MEMB_RO( width );
    }
#undef UT_CLASS

#define UT_CLASS islot_fuel
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Energy of the fuel (kilojoules per charge)");
        SET_MEMB_RO( energy );

        SET_MEMB_RO( explosion_data );
        SET_MEMB_N_RO( has_explode_data, "has_explosion_data" );
        SET_MEMB_RO( pump_terrain );
    }
#undef UT_CLASS

#define UT_CLASS islot_gun
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::bases<common_ranged_data>(), luna::no_constructor );
    }
#undef UT_CLASS

#define UT_CLASS islot_gunmod
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::bases<common_ranged_data>(), luna::no_constructor );
    }
#undef UT_CLASS

#define UT_CLASS islot_magazine
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Default type of ammo contained by a magazine (often set for ammo belts)");
        SET_MEMB_RO( default_ammo );

        DOC("Capacity of magazine (in equivalent units to ammo charges)");
        SET_MEMB_RO( capacity );

        DOC("Default amount of ammo contained by a magazine (often set for ammo belts)");
        SET_MEMB_RO( count );

        DOC("For ammo belts one linkage (of given type) is dropped for each unit of ammo consumed");
        SET_MEMB_RO( linkage );

        DOC("If false, ammo will cook off if this mag is affected by fire");
        SET_MEMB_RO( protects_contents );

        DOC("How reliable this magazine on a range of 0 to 10?");
        SET_MEMB_RO( reliability );

        DOC("How long it takes to load each unit of ammo into the magazine");
        SET_MEMB_RO( reload_time );

        DOC("What type of ammo this magazine can be loaded with");
        SET_MEMB_N_RO( type, "ammo_type" );
    }
#undef UT_CLASS

#define UT_CLASS islot_battery
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Maximum energy the battery can store");
        SET_MEMB_RO( max_capacity );
    }
#undef UT_CLASS

#define UT_CLASS islot_bionic
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Id of the bionic");
        SET_MEMB_N_RO( id, "bionic_id" );

        DOC("Arbitrary difficulty scale");
        SET_MEMB_RO( difficulty );

        DOC("Item with installation data that can be used to provide almost guaranteed successful install of corresponding bionic");
        SET_MEMB_RO( installation_data );

        DOC("Whether this CBM is an upgrade of another");
        SET_MEMB_RO( is_upgrade );
    }
#undef UT_CLASS

#define UT_CLASS islot_ammo
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::bases<common_ranged_data>(), luna::no_constructor );

        DOC("Default charges");
        SET_MEMB_RO( def_charges );

        DOC("Ammo type, basically the \"form\" of the ammo that fits into the gun/tool");
        SET_MEMB_N_RO( type, "ammo_id" );

        SET_MEMB_RO(ammo_effects);

        DOC("Type id of casings, if any");
        SET_MEMB_N_RO( casing, "casing_id" );

        DOC("Should this ammo explode in fire?");
        SET_MEMB_RO( cookoff );

        DOC("Chance to fail to recover the ammo used.");
        SET_MEMB_RO( dont_recover_one_in );

        SET_MEMB_RO( drop );
        SET_MEMB_RO( drop_count );
        SET_MEMB_RO( drop_active );

        SET_MEMB_RO( force_stat_display );

        DOC("Base loudness of ammo (possibly modified by gun/gunmods)");
        SET_MEMB_RO( loudness );

        DOC("Recoil (per shot), roughly equivalent to kinetic energy (in Joules)");
        SET_MEMB_RO( recoil );

        DOC("AoE shape or null if it's a projectile");
        SET_MEMB_RO( shape );

        DOC("Should this ammo apply a special explosion effect when in fire?");
        SET_MEMB_RO( special_cookoff );
    }
#undef UT_CLASS

#define UT_CLASS islot_seed
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        DOC("Name of the plant.");
        luna::set_fx( ut, "get_plant_name", []( const islot_seed & s, int num ) { return s.plant_name.translated( num ); } );

        DOC("Type id of the fruit item.");
        SET_MEMB_RO( fruit_id );

        DOC("Time it takes for a seed to grow (based of off a season length of 91 days).");
        SET_MEMB_RO( grow );

        DOC("Additionally items (a list of their item ids) that will spawn when harvesting the plant.");
        SET_MEMB_RO( byproducts );

        DOC("Amount of harvested charges of fruits is divided by this number.");
        SET_MEMB_RO( fruit_div );
    }
#undef UT_CLASS

#define UT_CLASS islot_artifact
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        SET_MEMB_RO( charge_req );
        SET_MEMB_RO( charge_type );
        SET_MEMB_RO( dream_freq_met );
        SET_MEMB_RO( dream_freq_unmet );
        SET_MEMB_RO( dream_msg_met );
        SET_MEMB_RO( dream_msg_unmet );

        SET_MEMB_RO( effects_activated );
        SET_MEMB_RO( effects_carried );
        SET_MEMB_RO( effects_wielded );
        SET_MEMB_RO( effects_worn );
    }
#undef UT_CLASS

#define UT_CLASS islot_milling
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        SET_MEMB_N_RO( conversion_rate_, "conversion_rate" );
        SET_MEMB_N_RO( into_, "converts_into" );
    }
#undef UT_CLASS

#define UT_CLASS common_ranged_data
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );

        SET_MEMB_N_RO( aimedcritbonus, "aimed_crit_bonus" );
        SET_MEMB_N_RO( aimedcritmaxbonus, "aimed_crit_max_bonus" );
        SET_MEMB_RO( damage );
        SET_MEMB_RO( dispersion );
        SET_MEMB_RO( range );
        SET_MEMB_RO( speed );
    }
#undef UT_CLASS

#define UT_CLASS relic
    {
        sol::usertype<UT_CLASS> ut = luna::new_usertype<UT_CLASS>( lua, luna::no_bases, luna::no_constructor );


    }
#undef UT_CLASS
}
