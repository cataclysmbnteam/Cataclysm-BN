#pragma once
#ifndef CATA_SRC_LOCATIONS_H
#define CATA_SRC_LOCATIONS_H

#include "point.h"

class item;
enum class item_location_type : int;
class Character;
class submap;
class vehicle;
class monster;
struct tripoint;

template<class T>
class location
{
    public:
        virtual void detach( T *obj ) = 0;
        virtual void detach_for_destroy( T *obj );
        virtual bool is_loaded( const T *obj ) const = 0;
        virtual tripoint position( const T *obj ) const = 0;
        virtual std::string describe( const Character *ch, const T *obj ) const = 0;
        virtual ~location() {};
        virtual bool check_for_corruption( const T *it ) const = 0;
};

//There is already a class in distribution_grid.h called tile_location. This all needs namespacing really.
//TODO!: namespace all this
class go_tile_location
{
    protected:
        tripoint pos;//abs coords
    public:
        void move_to( const tripoint &p ); //abs coords
        void move_by( const tripoint &offset ); //relative to current pos
};

class item_location : public location<item>
{
    public:
        virtual item_location_type where() const = 0;
        virtual int obtain_cost( const Character &ch, int qty, const item *it ) const = 0;
};

class character_item_location : public item_location
{
    protected:
        Character *holder;
    public:
        character_item_location( Character *h ): holder( h ) {};
        void detach( item *it ) override;
        bool is_loaded( const item *it ) const override;
        tripoint position( const item *it ) const override;
        item_location_type where() const override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class wield_item_location :  public character_item_location
{
    public:
        wield_item_location( Character *h ): character_item_location( h ) {};
        void detach( item *it ) override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class worn_item_location :  public character_item_location
{
    public:
        worn_item_location( Character *h ): character_item_location( h ) {};
        void detach( item *it ) override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class tile_item_location : public item_location, public go_tile_location
{
    public:
        tile_item_location( tripoint position );
        void detach( item *it ) override;
        bool is_loaded( const item *it ) const override;
        tripoint position( const item *it ) const override;
        item_location_type where() const override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class monster_item_location : public item_location
{
    protected:
        monster *on;
    public:
        monster_item_location( monster *on ) : on( on ) {}
        void detach( item *it ) override;
        bool is_loaded( const item *it ) const override;
        tripoint position( const item *it ) const override;
        item_location_type where() const override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class monster_component_item_location : public monster_item_location
{
    public:
        monster_component_item_location( monster *on ) : monster_item_location( on ) { }
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};

class monster_tied_item_location :  public monster_item_location
{
    public:
        monster_tied_item_location( monster *on ) : monster_item_location( on ) { }
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};
class monster_tack_item_location :  public monster_item_location
{
    public:
        monster_tack_item_location( monster *on ) : monster_item_location( on ) { }
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};
class monster_armor_item_location :  public monster_item_location
{
    public:
        monster_armor_item_location( monster *on ) : monster_item_location( on ) { }
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};
class monster_storage_item_location :  public monster_item_location
{
    public:
        monster_storage_item_location( monster *on ) : monster_item_location( on ) { }
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};

class monster_battery_item_location :  public monster_item_location
{
    public:
        monster_battery_item_location( monster *on ) : monster_item_location( on ) { }
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};

class vehicle_item_location : public item_location
{
    protected:
        vehicle *veh;
        int part_id;//TODO!: check how this works
    public:
        vehicle_item_location( vehicle *veh, int part_id ) : veh( veh ), part_id( part_id ) {}
        void detach( item *it ) override;
        bool is_loaded( const item *it ) const override;
        tripoint position( const item *it ) const override;
        item_location_type where() const override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class vehicle_base_item_location : public vehicle_item_location
{
    public:
        vehicle_base_item_location( vehicle *veh, int part_id ) : vehicle_item_location( veh, part_id ) {}
        void detach( item *it ) override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

class contents_item_location :  public item_location
{
    protected:
        item *container;
    public:
        contents_item_location( item *cont ) : container( cont ) {}
        void detach( item *it ) override;
        bool is_loaded( const item *it ) const override;
        tripoint position( const item *it ) const override;
        item_location_type where() const override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;

        item *parent() const;
};

class component_item_location : public contents_item_location
{
    public:
        component_item_location( item *cont ) : contents_item_location( cont ) {}
        void detach( item *it ) override;
        bool check_for_corruption( const item *it ) const override;
};

class template_item_location : public item_location
{
    public:
        template_item_location() {};
        void detach( item *it ) override;
        bool is_loaded( const item *it ) const override;
        void detach_for_destroy( item *it ) override;
        tripoint position( const item *it ) const override;
        item_location_type where() const override;
        int obtain_cost( const Character &ch, int qty, const item *it ) const override;
        std::string describe( const Character *ch, const item *it ) const override;
        bool check_for_corruption( const item *it ) const override;
};

#endif // CATA_SRC_LOCATIONS_H
