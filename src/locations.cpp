#include "locations.h"

#include "character.h"
#include "coordinates.h"
#include "item.h"
#include "location_ptr.h"
#include "map.h"
#include "monster.h"
#include "npc.h"
#include "player.h"
#include "submap.h"
#include "vehicle.h"
#include "vpart_position.h"
#include "vpart_range.h"
#include "veh_type.h"

static const item *cost_split_helper( const item *it, int qty )
{
    if( !it->count_by_charges() || qty <= 0 || qty >= it->charges ) {
        return it;
    }
    item *split = item::spawn_temporary( *it );
    split->charges = qty;
    return split;
}

detached_ptr<item> fake_item_location::detach( item * )
{
    debugmsg( "Attempted to detach a fake item" );
    return detached_ptr<item>();
}

void fake_item_location::attach( detached_ptr<item> && )
{
    debugmsg( "Attempted to attach to a fake location" );
}

bool fake_item_location::is_loaded( const item * ) const
{
    return false; //Loaded means in the reality bubble so no
}

tripoint fake_item_location::position( const item * ) const
{
    debugmsg( "Attempted to find the position of a fake item" );
    return tripoint_zero;
}

item_location_type fake_item_location::where() const
{
    debugmsg( "Attempted to get the where of a fake item" );
    return item_location_type::invalid;
}

int fake_item_location::obtain_cost( const Character &, int, const item * ) const
{
    debugmsg( "Attempted to get the obtain cost of a fake item" );
    return 0;
}

std::string fake_item_location::describe( const Character *, const item * ) const
{
    return "Error: Nowhere";
}

bool fake_item_location::check_for_corruption( const item * ) const
{
    return true;
}

detached_ptr<item> character_item_location::detach( item *it )
{
    return holder->i_rem( it );
}

void character_item_location::attach( detached_ptr<item> &&it )
{
    holder->i_add( std::move( it ) );
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

npc_mission_item_location::npc_mission_item_location( npc *h ) : character_item_location( h ) {};

detached_ptr<item> npc_mission_item_location::detach( item *it )
{
    npc *as_npc = static_cast<npc *>( holder );
    return as_npc->companion_mission_inv.remove_item( it );
}

void npc_mission_item_location::attach( detached_ptr<item> &&it )
{

    npc *as_npc = static_cast<npc *>( holder );
    as_npc->companion_mission_inv.add_item( std::move( it ) );
}

bool npc_mission_item_location::check_for_corruption( const item *it ) const
{
    npc *as_npc = static_cast<npc *>( holder );
    return as_npc->companion_mission_inv.position_by_item( it ) >= 0;
}

detached_ptr<item> wield_item_location::detach( item *it )
{
    for( std::pair<const bodypart_str_id, bodypart> &part : holder->get_body() ) {
        if( &*part.second.wielding.wielded == it ) {
            detached_ptr<item> d = part.second.wielding.wielded.release();
            return d;
        }
    }
    debugmsg( "Could not find wielded item for detach" );
    return detached_ptr<item>();
}

void wield_item_location::attach( detached_ptr<item> &&it )
{
    auto &body = holder->get_body();
    auto iter = body.find( body_part_arm_r );
    if( iter != body.end() ) {
        bodypart &part = holder->get_part( body_part_arm_r );
        part.wielding.wielded = std::move( it );
    }
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
    return holder->get_name();
}

bool wield_item_location::is_loaded( const item * ) const
{
    return holder->is_loaded();
}

tripoint wield_item_location::position( const item * ) const
{
    return holder->pos();
}

item_location_type wield_item_location::where( ) const
{
    return item_location_type::character;
}

bool wield_item_location::check_for_corruption( const item * ) const
{
    return false;
}

detached_ptr<item> worn_item_location::detach( item *it )
{
    detached_ptr<item> res;
    holder->remove_worn_items_with( [&it, &res]( detached_ptr<item> &&ch ) {
        if( &*ch == it ) {
            res = std::move( ch );
        }
        return std::move( ch );
    } );
    if( !res ) {
        debugmsg( "Failed to find worn item in detach" );
    }
    return res;
}


void worn_item_location::attach( detached_ptr<item> &&it )
{
    holder->add_worn( std::move( it ) );
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
    pos = position;
}

detached_ptr<item> tile_item_location::detach( item *it )
{
    map &here = get_map();
    tripoint local = here.getlocal( pos );
    map_stack items = here.i_at( local );
    for( auto iter = items.begin(); iter != items.end(); iter++ ) {
        if( *iter == it ) {
            detached_ptr<item> res;
            items.erase( iter, &res );
            return res;
        }
    }
    debugmsg( "Could not find item in tile detach" );
    return detached_ptr<item>();
}

void tile_item_location::attach( detached_ptr<item> &&it )
{
    map &here = get_map();
    tripoint local = here.getlocal( pos );
    map_stack items = here.i_at( local );
    items.insert( std::move( it ) );
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
    if( iter == items.end() ) {
        tripoint p( 0, 0, local.z );
        while( p.x < 11 * 12 ) {
            p.y = 0;
            while( p.y < 11 * 12 ) {
                map_stack si = here.i_at( p );
                if( std::find( si.begin(), si.end(), it ) != si.end() ) {
                    debugmsg( "Found upcoming at %d,%d,%d", p.x, p.y, p.z );
                    return false;
                }
                p.y++;
            }
            p.x++;
        }
        debugmsg( "Couldn't find upcoming at all" );
        return false;
    }
    return true;
}

void tile_item_location::move_by( tripoint offset )
{
    pos += offset;
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
    return item_location_type::monster;
}

int monster_item_location::obtain_cost( const Character &, int, const item * ) const
{
    debugmsg( "Tried to find the obtain cost of an item on a monster" );
    return 0;
}

std::string monster_item_location::describe( const Character *, const item * ) const
{
    return "on monster";
}

detached_ptr<item> monster_item_location::detach( item *it )
{
    return on->remove_item( it );
}

void monster_item_location::attach( detached_ptr<item> &&it )
{
    on->add_item( std::move( it ) );
}


bool monster_item_location::check_for_corruption( const item *it ) const
{
    const std::vector<item *> &items = on->get_items();
    auto iter = std::find( items.begin(), items.end(), it );
    return iter != items.end();
}

detached_ptr<item> monster_component_item_location::detach( item *it )
{
    return on->remove_corpse_component( *it );
}

void monster_component_item_location::attach( detached_ptr<item> &&it )
{
    on->add_corpse_component( std::move( it ) );
}

bool monster_component_item_location::check_for_corruption( const item * ) const
{
    return true;
    //std::vector<item *> &items = on->get_corpse_components();
    //auto search = std::find( items.begin(), items.end(), it );
    //return search != items.end();
}

detached_ptr<item> monster_tied_item_location::detach( item * )
{
    return on->remove_tied_item();
}

void monster_tied_item_location::attach( detached_ptr<item> &&it )
{
    on->set_tied_item( std::move( it ) );
}

bool monster_tied_item_location::check_for_corruption( const item *it ) const
{
    return on->get_tied_item() == it;
}

detached_ptr<item> monster_tack_item_location::detach( item * )
{
    return on->remove_tack_item( );
}

void monster_tack_item_location::attach( detached_ptr<item> &&it )
{
    on->set_tack_item( std::move( it ) );
}

bool monster_tack_item_location::check_for_corruption( const item *it ) const
{
    return on->get_tack_item() == it;
}

detached_ptr<item> monster_armor_item_location::detach( item * )
{
    return on->remove_armor_item( );
}

void monster_armor_item_location::attach( detached_ptr<item> &&it )
{
    on->set_armor_item( std::move( it ) );
}

bool monster_armor_item_location::check_for_corruption( const item *it ) const
{
    return on->get_armor_item() == it;
}

detached_ptr<item> monster_storage_item_location::detach( item * )
{
    return on->remove_storage_item( );
}

void monster_storage_item_location::attach( detached_ptr<item> &&it )
{
    on->set_storage_item( std::move( it ) );
}

bool monster_storage_item_location::check_for_corruption( const item *it ) const
{
    return on->get_storage_item() == it;
}

detached_ptr<item> monster_battery_item_location::detach( item * )
{
    return on->remove_battery_item( );
}

void monster_battery_item_location::attach( detached_ptr<item> &&it )
{
    on->set_battery_item( std::move( it ) );
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
    return get_map().inbounds( veh->mount_to_tripoint( veh->get_part_hack( hack_id ).mount ) );
}

tripoint vehicle_item_location::position( const item * ) const
{
    return veh->mount_to_tripoint( veh->get_part_hack( hack_id ).mount );
}

item_location_type vehicle_item_location::where() const
{
    return item_location_type::vehicle;
}

detached_ptr<item> vehicle_item_location::detach( item *it )
{
    return veh->get_part_hack( hack_id ).remove_item( *it );
}

void vehicle_item_location::attach( detached_ptr<item> &&it )
{
    veh->get_part_hack( hack_id ).add_item( std::move( it ) );
}

int vehicle_item_location::obtain_cost( const Character &ch, int qty, const item *it ) const
{
    const item *obj = cost_split_helper( it, qty );
    int mv = dynamic_cast<const player *>( &ch )->item_handling_cost( *obj, true,
             VEHICLE_HANDLING_PENALTY );
    mv += 100 * rl_dist( ch.pos(), veh->mount_to_tripoint( veh->get_part_hack( hack_id ).mount ) );
    return mv;
}

std::string vehicle_item_location::describe( const Character *ch, const item * ) const
{
    vpart_position part_pos( *veh, veh->get_part_id_hack( hack_id ) );
    std::string res;
    if( auto label = part_pos.get_label() ) {
        res = colorize( *label, c_light_blue ) + " ";
    }
    if( auto cargo_part = part_pos.part_with_feature( "CARGO", true ) ) {
        res += cargo_part->part().name();
    } else {
        return "Error: vehicle part without storage";
    }
    if( ch ) {
        res += " " + direction_suffix( ch->pos(), part_pos.pos() );
    }
    return res;
}

bool vehicle_item_location::check_for_corruption( const item *it ) const
{
    vehicle_part &part = veh->get_part_hack( hack_id );
    std::vector<item *> items = part.get_items();
    auto iter = std::find( items.begin(), items.end(), it );
    return iter != items.end();
}

detached_ptr<item> vehicle_base_item_location::detach( item * )
{
    debugmsg( "Attempted to detach a vehicle base part" );
    return detached_ptr<item>();
}

void vehicle_base_item_location::attach( detached_ptr<item> && )
{
    debugmsg( "Tried to attach to a vehicle base location" );
}

int vehicle_base_item_location::obtain_cost( const Character &, int, const item * ) const
{
    debugmsg( "Attempted to find the obtain cost of a vehicle part's base item" );
    return 0;
}

std::string vehicle_base_item_location::describe( const Character *, const item * ) const
{
    return "Error: Vehicle base part";
}

bool vehicle_base_item_location::check_for_corruption( const item *it ) const
{
    return veh->get_part_hack( hack_id ).base == it;
}

detached_ptr<item> contents_item_location::detach( item *it )
{
    return container->contents.remove_top( it );
}

void contents_item_location::attach( detached_ptr<item> &&it )
{
    container->contents.insert_item( std::move( it ) );
}

bool contents_item_location::is_loaded( const item * ) const
{
    return container->is_loaded();
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
    const std::vector<item *> &items = container->contents.all_items_top();
    auto search = std::find( items.begin(), items.end(), it );
    return search != items.end();
}

item *contents_item_location::parent() const
{
    return container;
}

detached_ptr<item> component_item_location::detach( item *it )
{
    return container->remove_component( *it );
}

void component_item_location::attach( detached_ptr<item> &&it )
{
    return container->add_component( std::move( it ) );
}

bool component_item_location::check_for_corruption( const item *it ) const
{
    const location_vector<item> &items = container->get_components();
    auto search = std::find( items.begin(), items.end(), it );
    return search != items.end();
}


partial_con_item_location::partial_con_item_location( tripoint position ) : tile_item_location(
        position ) {}
detached_ptr<item> partial_con_item_location::detach( item * )
{
    debugmsg( "Tried to detach an item from a partial construction" );
    return detached_ptr<item>();
}

void partial_con_item_location::attach( detached_ptr<item> && )
{
    debugmsg( "Tried to attach an item to a partial construction" );
}

bool partial_con_item_location::check_for_corruption( const item * ) const
{
    return true;
}
