#pragma once
#ifndef CATA_SRC_LOCATIONS_H
#define CATA_SRC_LOCATIONS_H

#include "point.h"
#include "type_id.h"

class item;
enum class item_location_type : int;
class Character;
class Creature;
class submap;
class vehicle;
class monster;
class npc;
struct tripoint;
template<typename T>
class detached_ptr;

template<class T>
class location
{
    public:
        virtual auto detach( T *obj ) -> detached_ptr<T> = 0;
        virtual void attach( detached_ptr<T> &&obj ) = 0;
        virtual auto is_loaded( const T *obj ) const -> bool = 0;
        virtual auto position( const T *obj ) const -> tripoint = 0;
        virtual auto describe( const Character *ch, const T *obj ) const -> std::string = 0;
        virtual auto check_for_corruption( const T *it ) const -> bool = 0;
        virtual ~location() = default;
};

class item_location : public location<item>
{
    public:
        virtual auto where() const -> item_location_type = 0;
        virtual auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int = 0;
};

class character_item_location : public item_location
{
    protected:
        Character *holder;
    public:
        character_item_location( Character *h ): holder( h ) {};
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item *it ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class npc_mission_item_location : public character_item_location
{
    public:
        npc_mission_item_location( npc *h );
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class wield_item_location :  public item_location
{
    protected:
        Creature *holder;
    public:
        wield_item_location( Creature *h ): item_location(), holder( h ) {};
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item *it ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};


class worn_item_location :  public character_item_location
{
    public:
        worn_item_location( Character *h ): character_item_location( h ) {};
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class tile_item_location : public item_location
{
    protected:
        tripoint pos;//abs coords
    public:
        tile_item_location( tripoint position );
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item *it ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
        void move_by( tripoint offset );
};

class partial_con_item_location : public tile_item_location
{
    public:
        partial_con_item_location( tripoint position );
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class monster_item_location : public item_location
{
    protected:
        monster *on;
    public:
        monster_item_location( monster *on ) : on( on ) {}
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item *it ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class monster_component_item_location : public monster_item_location
{
    public:
        monster_component_item_location( monster *on ) : monster_item_location( on ) { }
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class monster_tied_item_location :  public monster_item_location
{
    public:
        monster_tied_item_location( monster *on ) : monster_item_location( on ) { }
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};
class monster_tack_item_location :  public monster_item_location
{
    public:
        monster_tack_item_location( monster *on ) : monster_item_location( on ) { }
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};
class monster_armor_item_location :  public monster_item_location
{
    public:
        monster_armor_item_location( monster *on ) : monster_item_location( on ) { }
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};
class monster_storage_item_location :  public monster_item_location
{
    public:
        monster_storage_item_location( monster *on ) : monster_item_location( on ) { }
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class monster_battery_item_location :  public monster_item_location
{
    public:
        monster_battery_item_location( monster *on ) : monster_item_location( on ) { }
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class vehicle_item_location : public item_location
{
    protected:
        vehicle *veh;
        int hack_id;
    public:
        vehicle_item_location( vehicle *veh, int hack_id ) : veh( veh ), hack_id( hack_id ) {}
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item *it ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class vehicle_base_item_location : public vehicle_item_location
{
    public:
        vehicle_base_item_location( vehicle *veh, int hack_id ) : vehicle_item_location( veh, hack_id ) {}
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class contents_item_location :  public item_location
{
    protected:
        item *container;
    public:
        contents_item_location( item *cont ) : container( cont ) {}
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item * ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;

        auto parent() const -> item *;
};

class component_item_location : public contents_item_location
{
    public:
        component_item_location( item *cont ) : contents_item_location( cont ) {}
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

class fake_item_location : public item_location
{
    public:
        fake_item_location() = default;
        auto detach( item *it ) -> detached_ptr<item> override;
        void attach( detached_ptr<item> &&obj ) override;
        auto is_loaded( const item *it ) const -> bool override;
        auto position( const item *it ) const -> tripoint override;
        auto where() const -> item_location_type override;
        auto obtain_cost( const Character &ch, int qty, const item *it ) const -> int override;
        auto describe( const Character *ch, const item *it ) const -> std::string override;
        auto check_for_corruption( const item *it ) const -> bool override;
};

#endif // CATA_SRC_LOCATIONS_H
