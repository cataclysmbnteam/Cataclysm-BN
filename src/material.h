#pragma once
#ifndef CATA_SRC_MATERIAL_H
#define CATA_SRC_MATERIAL_H

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "fire.h"
#include "optional.h"
#include "type_id.h"

class material_type;

enum damage_type : int;
class JsonObject;

using mat_burn_products = std::vector<std::pair<itype_id, float>>;
using mat_compacts_into = std::vector<itype_id>;
using material_list = std::vector<material_type>;
using material_id_list = std::vector<material_id>;

class material_type
{
    public:
        material_id id;
        bool was_loaded = false;

    private:
        std::string _name;
        cata::optional<itype_id> _salvaged_into; // this material turns into this item when salvaged
        itype_id _repaired_with = itype_id( "null" ); // this material can be repaired with this item
        int _bash_resist = 0;                         // negative integers means susceptibility
        int _cut_resist = 0;
        int _acid_resist = 0;
        int _elec_resist = 0;
        int _fire_resist = 0;
        int _bullet_resist = 0;
        int _chip_resist = 0;                         // Resistance to physical damage of the item itself
        int _density = 1;                             // relative to "powder", which is 1
        float _warmth_when_wet = 0.2f;                // Percentage of warmth kept when fully drenched
        float _specific_heat_liquid = 4.186;
        // How resistant this material is to wind as a percentage - 0 to 100
        cata::optional<int> _wind_resist;
        float _specific_heat_solid = 2.108;
        float _latent_heat = 334;
        int _freeze_point = 32; // Fahrenheit
        bool _edible = false;
        bool _rotting = false;
        bool _soft = false;
        bool _reinforces = false;

        std::string _bash_dmg_verb;
        std::string _cut_dmg_verb;
        std::vector<std::string> _dmg_adj;

        std::map<vitamin_id, double> _vitamins;

        std::vector<mat_burn_data> _burn_data;

        //Burn products defined in JSON as "burn_products": [ [ "X", float efficiency ], [ "Y", float efficiency ] ]
        mat_burn_products _burn_products;

        material_id_list _compact_accepts;
        mat_compacts_into _compacts_into;

    public:
        material_type();

        void load( const JsonObject &jsobj, const std::string &src );
        void check() const;

        auto ident() const -> material_id;
        auto name() const -> std::string;
        /**
         * @returns An empty optional if this material can not be
         * salvaged into any items (e.g. for powder, liquids).
         * Or a valid id of the item type that this can be salvaged
         * into (e.g. clothes made of material leather can be salvaged
         * into lather patches).
         */
        auto salvaged_into() const -> cata::optional<itype_id>;
        auto repaired_with() const -> itype_id;
        auto bash_resist() const -> int;
        auto cut_resist() const -> int;
        auto bullet_resist() const -> int;
        auto bash_dmg_verb() const -> std::string;
        auto cut_dmg_verb() const -> std::string;
        auto dmg_adj( int damage ) const -> std::string;
        auto acid_resist() const -> int;
        auto elec_resist() const -> int;
        auto fire_resist() const -> int;
        auto chip_resist() const -> int;
        auto warmth_when_wet() const -> float {
            return _warmth_when_wet;
        }
        auto specific_heat_liquid() const -> float;
        auto specific_heat_solid() const -> float;
        auto latent_heat() const -> float;
        auto freeze_point() const -> int;
        auto density() const -> int;
        auto wind_resist() const -> cata::optional<int>;
        auto edible() const -> bool;
        auto rotting() const -> bool;
        auto soft() const -> bool;
        auto reinforces() const -> bool;

        auto vitamin( const vitamin_id &id ) const -> double {
            const auto iter = _vitamins.find( id );
            return iter != _vitamins.end() ? iter->second : 0;
        }

        auto burn_data( size_t intensity ) const -> const mat_burn_data &;
        auto burn_products() const -> const mat_burn_products &;
        auto compact_accepts() const -> const material_id_list &;
        auto compacts_into() const -> const mat_compacts_into &;
};

namespace materials
{

void load( const JsonObject &jo, const std::string &src );
void check();
void reset();

auto get_all() -> material_list;
auto get_compactable() -> material_list;
auto get_rotting() -> std::set<material_id>;

} // namespace materials

#endif // CATA_SRC_MATERIAL_H
