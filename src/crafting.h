#pragma once

#include <list>
#include <set>
#include <vector>

#include "point.h"
#include "mapdata.h"
#include "ret_val.h"
#include "type_id.h"
#include "veh_type.h"

class avatar;
class Character;
class inventory;
class item;
class player;
class recipe;
struct iuse_location;
struct tool_comp;

using metric = std::pair<units::mass, units::volume>;

enum class cost_adjustment : int;

enum class bench_type : int {
    ground = 0,
    hands,
    furniture,
    vehicle
};

struct workbench_info_wrapper {
    // Base multiplier applied for crafting here
    float multiplier = 1.0f;
    float multiplier_adjusted = multiplier;
    // Mass/volume allowed before a crafting speed penalty is applied
    units::mass allowed_mass = 0_gram;
    units::volume allowed_volume = 0_ml;
    bench_type type = bench_type::ground;
    workbench_info_wrapper( furn_workbench_info f_info ) : multiplier( f_info.multiplier ),
        allowed_mass( f_info.allowed_mass ),
        allowed_volume( f_info.allowed_volume ), type( bench_type::furniture ) {
    }
    workbench_info_wrapper( vpslot_workbench v_info ) : multiplier( v_info.multiplier ),
        allowed_mass( v_info.allowed_mass ),
        allowed_volume( v_info.allowed_volume ), type( bench_type::vehicle ) {
    }
    workbench_info_wrapper( float multiplier, const units::mass &allowed_mass,
                            const units::volume &allowed_volume, const bench_type &type )
        : multiplier( multiplier ), allowed_mass( allowed_mass ), allowed_volume( allowed_volume ),
          type( type ) {
    }

    void adjust_multiplier( const metric &metrics );
};

struct bench_location {
    workbench_info_wrapper wb_info;
    tripoint position;

    explicit bench_location( workbench_info_wrapper info, tripoint position )
        : wb_info( info ), position( position ) {
    }
};

template<typename Type>
struct comp_selection;

/**
 * @brief Removes any (removable) ammo and stores it in character's inventory.
 */
void remove_ammo( item &dis_item, Character &who );
/**
 * @brief Removes any (removable) ammo from each item and stores it in character's inventory.
 */
void remove_ammo( std::vector<item *> &dis_items, Character &who );

std::optional<workbench_info_wrapper> make_workbench_info( item craft, bench_type type,
        tripoint &location );
void complete_craft( Character &p, item &craft );

namespace crafting
{

std::optional<workbench_info_wrapper> best_bench_loc( const item &craft, const tripoint &loc );
workbench_info_wrapper best_bench_here( const item &craft, const tripoint &loc,
                                        bool can_lift );
/**
* Returns the set of book types in crafting_inv that provide the
* given recipe.
* @param c Character whose skills are used to limit the available recipes
* @param crafting_inv Current available items that may contain readable books
* @param r Recipe to search for in the available books
*/
std::set<itype_id> get_books_for_recipe( const Character &c, const inventory &crafting_inv,
        const recipe *r );

/**
 * Returns the set of book types that provide the given recipe.
 */
std::set<itype_id> get_books_for_recipe( const recipe *r );

int charges_for_complete( int full_charges );
int charges_for_starting( int full_charges );
int charges_for_continuing( int full_charges );

/**
 * Returns selected tool component that matches one of the expected ones.
 * @param tools tools to match
 * @param batch size of batch to craft, multiplier on expected charges
 * @param map_inv map inventory to select from
 * @param player_with_inv if not null, character who provides additional inventory
 * @param hotkeys hotkeys available to the menu
 * @param can_cancel can the selection be aborted with no result
 * @param adjustment affects required charges, see @ref cost_adjustment
 */
comp_selection<tool_comp>
select_tool_component( const std::vector<tool_comp> &tools, int batch, const inventory &map_inv,
                       const Character *player_with_inv,
                       bool can_cancel,
                       const std::string &hotkeys,
                       cost_adjustment adjustment );

comp_selection<tool_comp>
select_tool_component( const std::vector<tool_comp> &tools, int batch, const inventory &map_inv,
                       const Character *player_with_inv,
                       bool can_cancel = false );

/** Check if character can disassemble an item using the given crafting inventory. */
ret_val<bool> can_disassemble( const Character &who, const item &obj, const inventory &inv );

/**
 * Prompt for an item to disassemble, then start activity.
 */
bool disassemble( avatar &you );

/**
 * Prompt to disassemble given item, then start activity.
 */
bool disassemble( avatar &you, item &target );

/**
 * Start an activity to disassemble all items in avatar's square.
 */
bool disassemble_all( avatar &you, bool recursively );

/**
 * Complete disassembly of target item.
 */
void complete_disassemble( Character &who, const iuse_location &target, const tripoint &pos );

} // namespace crafting
