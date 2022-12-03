#include "locations.h"

#include "character.h"
#include "coordinates.h"
#include "item.h"
#include "map.h"
#include "monster.h"
#include "player.h"
#include "submap.h"
#include "vehicle.h"
#include "vpart_position.h"

static const item *cost_split_helper( const item *it, int qty )
{
    if( !it->count_by_charges() || qty <= 0 || qty >= it->charges ) {
        return it;
    }
    item *split = item_spawn_temporary( *it );
    split->charges = qty;
    return split;
}

template<class T>
void location<T>::detach_for_destroy( T *n )
{
    debugmsg( "Attempted to destroy something with a location" );
    detach( n );
}

void template_item_location::detach( item * )
{
    debugmsg( "Attempted to detach a template item" );
}

bool template_item_location::is_loaded( const item * ) const
{
    return false; //Loaded means in the reality bubble so no
}

void template_item_location::detach_for_destroy( item * )
{
    //Just suppressing the errors here, there's nothing to do to detach
}

tripoint template_item_location::position( const item * ) const
{
    debugmsg( "Attempted to find the position of a template item" );
    return tripoint_zero;
}

item_location_type template_item_location::where() const
{
    debugmsg( "Attempted to get the where of a template item" );
    return item_location_type::invalid;
}

int template_item_location::obtain_cost( const Character &, int, const item * ) const
{
    debugmsg( "Attempted to get the obtain cost of a template item" );
    return 0;
}

std::string template_item_location::describe( const Character *, const item * ) const
{
    debugmsg( "Tried to desribe the location of a template item" );
    return "Nowhere";
}

bool template_item_location::check_for_corruption( const item * ) const
{
    return true;
}

void character_item_location::detach( item *it )
{
    holder->i_rem( it );
}

bool character_item_location::is_loaded( const item * ) const
{
    return holder->is_loaded();
}

tripoint character_item_location::position( const item * ) const
{
    return holder->pos();
}

item_location_type character_item_location::where() const
{
    return item_location_type::character;
}

int character_item_location::obtain_cost( const Character &ch, int qty, const item *it ) const
{
    const item *split_stack = cost_split_helper( it, qty );
    return dynamic_cast<const player *>( &ch )->item_handling_cost( *split_stack, true,
            INVENTORY_HANDLING_PENALTY );
}

std::string character_item_location::describe( const Character *ch, const item *it ) const
{
    if( ch == holder ) {
        auto parents = holder->parents( *it );
        if( !parents.empty() && holder->is_worn( *parents.back() ) ) {
            return parents.back()->type_name();

        } else if( holder->is_worn( *it ) ) {
            return _( "worn" );

        } else {
            return _( "inventory" );
        }
    } else {
        return holder->name;
    }
}

bool character_item_location::check_for_corruption( const item *it ) const
{
    if( !holder->has_item_directly( *it ) ) {
        return false;
    }
    return holder->get_item_position( it ) >= 0;
}

void wield_item_location::detach( item * )
{
    holder->remove_weapon();
}
int wield_item_location::obtain_cost( const Character &ch, int qty, const item *it ) const
{
    const item *split_stack = cost_split_helper( it, qty );
    return  dynamic_cast<const player *>( &ch )->item_handling_cost( *split_stack, false, 0 );
}

std::string wield_item_location::describe( const Character *ch, const item * ) const
{
    if( ch == holder ) {
        return _( "wield" );
    }
    return holder->name;
}

bool wield_item_location::check_for_corruption( const item *it ) const
{
    return &( holder->get_weapon() ) == it;
}

void worn_item_location::detach( item *it )
{
    holder->remove_worn_items_with( [&it]( item & ch ) {
        return &ch == it;
    } );
}

int worn_item_location::obtain_cost( const Character &ch, int qty, const item *it ) const
{
    const item *split_stack = cost_split_helper( it, qty );
    return dynamic_cast<const player *>( &ch )->item_handling_cost( *split_stack, false,
            INVENTORY_HANDLING_PENALTY / 2 );
}

std::string worn_item_location::describe( const Character *ch, const item * ) const
{
    if( ch == holder ) {
        return _( "worn" );
    }
    return holder->name;
}

bool worn_item_location::check_for_corruption( const item *it ) const
{
    if( !holder->has_item_directly( *it ) ) {
        return false;
    }
    return holder->get_item_position( it ) < -1;
}

tile_item_location::tile_item_location( tripoint position )
{
    pos = get_map().getabs( position );
}

void tile_item_location::detach( item *it )
{
    map &here = get_map();
    tripoint local = here.getlocal( pos );
    map_stack items = here.i_at( local );//TODO!: is this updating location properly?
    std::remove( items.begin(), items.end(), it );
}

bool tile_item_location::is_loaded( const item * ) const
{
    map &here = get_map();
    return here.inbounds( here.getlocal( pos ) );
}

tripoint tile_item_location::position( const item * ) const
{
    return get_map().getlocal( pos );
}

item_location_type tile_item_location::where() const
{
    return item_location_type::map;
}

int tile_item_location::obtain_cost( const Character &ch, int qty, const item *it ) const
{
    const item *split_stack = cost_split_helper( it, qty );
    int mv = dynamic_cast<const player *>( &ch )->item_handling_cost( *split_stack, true,
             MAP_HANDLING_PENALTY );
    mv += 100 * rl_dist( ch.pos(), get_map().getlocal( pos ) );
    return mv;
}

std::string tile_item_location::describe( const Character *ch, const item * ) const
{
    map &here = get_map();
    tripoint local = here.getlocal( pos );
    std::string res = here.name( local );
    if( ch ) {
        res += std::string( " " ) += direction_suffix( ch->pos(), local );
    }
    return res;
}

bool tile_item_location::check_for_corruption( const item *it ) const
{
    map &here = get_map();
    tripoint local = here.getlocal( pos );
    if( !here.inbounds( local ) ) {
        return true;
    }
    map_stack items = here.i_at( local );
    auto iter = std::find( items.begin(), items.end(), it );
    return iter != items.end();
}

bool monster_item_location::is_loaded( const item * ) const
{
    return on->is_loaded();
}

tripoint monster_item_location::position( const item * ) const
{
    return on->pos();
}

item_location_type monster_item_location::where() const
{
    return item_location_type::character;//I guess? Monster is not on the list and I want to delete that list not use it more
}

int monster_item_location::obtain_cost( const Character &, int, const item * ) const
{
    debugmsg( "Tried to find the obtain cost of an item on a monster" );
    return 0;
}

std::string monster_item_location::describe( const Character *, const item * ) const
{
    debugmsg( "Tried to describe the location of an item on a monster" );
    return "Error";
}

void monster_item_location::detach( item *it )
{
    on->remove_item( it );
}

bool monster_item_location::check_for_corruption( const item *it ) const
{
    std::vector<item *> &items = on->get_items();
    auto iter = std::find( items.begin(), items.end(), it );
    return iter != items.end();
}

void monster_tied_item_location::detach( item * )
{
    on->set_tied_item( nullptr );
}

bool monster_tied_item_location::check_for_corruption( const item *it ) const
{
    return on->get_tied_item() == it;
}

void monster_tack_item_location::detach( item * )
{
    on->set_tack_item( nullptr );
}

bool monster_tack_item_location::check_for_corruption( const item *it ) const
{
    return on->get_tack_item() == it;
}

void monster_armor_item_location::detach( item * )
{
    on->set_armor_item( nullptr );
}

bool monster_armor_item_location::check_for_corruption( const item *it ) const
{
    return on->get_armor_item() == it;
}

void monster_storage_item_location::detach( item * )
{
    on->set_storage_item( nullptr );
}

bool monster_storage_item_location::check_for_corruption( const item *it ) const
{
    return on->get_storage_item() == it;
}

void monster_battery_item_location::detach( item * )
{
    on->set_battery_item( nullptr );
}

bool monster_battery_item_location::check_for_corruption( const item *it ) const
{
    return on->get_battery_item() == it;
}

bool vehicle_item_location::is_loaded( const item * ) const
{
    if( !veh->is_loaded() ) {
        return false;
    }
    //Have to check the bounds, the vehicle might be half outside the bubble
    return get_map().inbounds( veh->global_part_pos3( part_id ) );
}

tripoint vehicle_item_location::position( const item * ) const
{
    return veh->global_part_pos3( part_id );
}

item_location_type vehicle_item_location::where() const
{
    return item_location_type::vehicle;
}

void vehicle_item_location::detach( item *it )
{
    vehicle_stack items = veh->get_items( part_id );
    std::remove( items.begin(), items.end(), it );
}

int vehicle_item_location::obtain_cost( const Character &ch, int qty, const item *it ) const
{
    const item *obj = cost_split_helper( it, qty );
    int mv = dynamic_cast<const player *>( &ch )->item_handling_cost( *obj, true,
             VEHICLE_HANDLING_PENALTY );
    mv += 100 * rl_dist( ch.pos(), veh->global_part_pos3( part_id ) );
    return mv;
}

std::string vehicle_item_location::describe( const Character *ch, const item * ) const
{
    vpart_position part_pos( *veh, part_id );
    std::string res;
    if( auto label = part_pos.get_label() ) {
        res = colorize( *label, c_light_blue ) + " ";
    }
    if( auto cargo_part = part_pos.part_with_feature( "CARGO", true ) ) {
        res += cargo_part->part().name();
    } else {
        debugmsg( "item in vehicle part without cargo storage" );
    }
    if( ch ) {
        res += " " + direction_suffix( ch->pos(), part_pos.pos() );
    }
    return res;
}

bool vehicle_item_location::check_for_corruption( const item *it ) const
{
    vehicle_stack items = veh->get_items( part_id );
    auto iter = std::find( items.begin(), items.end(), it );
    return iter != items.end();
}

void vehicle_base_item_location::detach( item * )
{
    debugmsg( "Attempted to detach a vehicle base part" );
}
int vehicle_base_item_location::obtain_cost( const Character &, int, const item * ) const
{
    debugmsg( "Attempted to find the obtain cost of a vehicle part's base item" );
    return 0;
}

std::string vehicle_base_item_location::describe( const Character *, const item * ) const
{
    debugmsg( "Attempted to find the location of an item being used as a base part" );
    return "Error";
}

bool vehicle_base_item_location::check_for_corruption( const item *it ) const
{
    return veh->part( part_id ).base == it;
}

void contents_item_location::detach( item *it )
{
    container->contents.remove_top( it );
}

bool contents_item_location::is_loaded( const item *it ) const
{
    return it->is_loaded();
}

item_location_type contents_item_location::where() const
{
    return item_location_type::container;
}

int contents_item_location::obtain_cost( const Character &ch, int qty, const item * ) const
{
    return INVENTORY_HANDLING_PENALTY + container->obtain_cost( ch, qty );

    /* TODO!: decode this and make holsters work again
    if( parents.back()->can_holster( obj, true ) ) {
        auto ptr = dynamic_cast<const holster_actor *>
                   ( parents.back()->type->get_use( "holster" )->get_actor_ptr() );
        mv += dynamic_cast<player *>( who )->item_handling_cost( obj, false, ptr->draw_cost );

    } else if( parents.back()->is_bandolier() ) {
        auto ptr = dynamic_cast<const bandolier_actor *>
                   ( parents.back()->type->get_use( "bandolier" )->get_actor_ptr() );
        mv += dynamic_cast<player *>( who )->item_handling_cost( obj, false, ptr->draw_cost );

    } else {
        mv +=
    }*/
}

tripoint contents_item_location::position( const item *it ) const
{
    return it->position();
}

std::string contents_item_location::describe( const Character *, const item * ) const
{
    return string_format( _( "inside %s" ), container->tname() );
}

bool contents_item_location::check_for_corruption( const item *it ) const
{
    return container->has_item_directly( *it );
}

item *contents_item_location::parent() const
{
    return container;
}
