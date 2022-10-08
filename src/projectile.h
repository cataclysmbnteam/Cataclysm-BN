#pragma once
#ifndef CATA_SRC_PROJECTILE_H
#define CATA_SRC_PROJECTILE_H

#include <memory>
#include <set>
#include <string>

#include "damage.h"
#include "point.h"
#include "string_id.h"

class Creature;
struct explosion_data;
class item;
struct ammo_effect;

using ammo_effect_str_id = string_id<ammo_effect>;

struct projectile {
        damage_instance impact;
        // how hard is it to dodge? essentially rolls to-hit,
        // bullets have arbitrarily high values but thrown objects have dodgeable values.
        // TODO: Get rid of this, replace with something sane (or just get rid)
        int speed = 0;
        int range = 0;

        /**
         * Returns an item that should be dropped or an item for which is_null() is true
         *  when item to drop is unset.
         */
        auto get_drop() const -> const item &;
        /** Copies item `it` as a drop for this projectile. */
        void set_drop( const item &it );
        void set_drop( item &&it );
        void unset_drop();

        auto get_custom_explosion() const -> const explosion_data &;
        void set_custom_explosion( const explosion_data &ex );
        void unset_custom_explosion();

        auto get_ammo_effects() -> const std::set<ammo_effect_str_id> & {
            return proj_effects;
        }

        projectile();
        projectile( const projectile & );
        projectile( projectile && );
        auto operator=( const projectile & ) -> projectile &;
        ~projectile();

        void deserialize( JsonIn &jsin );
        void load( JsonObject &jo );

        auto has_effect( const ammo_effect_str_id &id ) const -> bool {
            return proj_effects.count( id ) > 0;
        }
        void add_effect( const ammo_effect_str_id &id ) {
            proj_effects.insert( id );
        }

    private:
        // Actual item used (to drop contents etc.).
        // Null in case of bullets (they aren't "made of cartridges").
        std::unique_ptr<item> drop;
        std::unique_ptr<explosion_data> custom_explosion;
        std::set<ammo_effect_str_id> proj_effects;
};

struct dealt_projectile_attack {
    projectile proj; // What we used to deal the attack
    Creature *hit_critter; // The critter that stopped the projectile or null
    dealt_damage_instance dealt_dam; // If hit_critter isn't null, hit data is written here
    tripoint end_point; // Last hit tile (is hit_critter is null, drops should spawn here)
    double missed_by; // Accuracy of dealt attack
};

void apply_ammo_effects( const tripoint &p, const std::set<ammo_effect_str_id> &effects );
// Legacy. TODO: Remove
void apply_ammo_effects( const tripoint &p, const std::set<std::string> &effects );
auto max_aoe_size( const std::set<ammo_effect_str_id> &tags ) -> int;
auto max_aoe_size( const std::set<std::string> &tags ) -> int;

#endif // CATA_SRC_PROJECTILE_H
