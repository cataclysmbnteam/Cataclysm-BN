#include "safe_reference.h"
#include "item.h"
#include "json.h"
#include "character.h"
#include "map.h"
#include "map_selector.h"
#include "avatar.h"
#include "game.h"
#include "vpart_position.h"
#include "vehicle.h"
#include "vehicle_selector.h"

uint64_t save_id_prefix = 0;
bool save_and_quit = false;

template<typename T>
void safe_reference<T>::serialize_global( JsonOut &json )
{
    json.member( "safe_references" );
    json.start_array();
    for( rbi_it &it : records_by_id ) {
        //TODO!: better format
        safe_reference<T>::id_type id = it->second->id;
        uint32_t count = it->second->json_count;
        json.write( id );
        json.write( count );
    }
    json.end_array();
}

template<typename T>
void safe_reference<T>::deserialize_global( JsonIn &jsin )
{
    jsin.start_array();
    while( !jsin.end_array() ) {
        safe_reference<T>::id_type id;
        jsin.read( id );
        uint32_t count;
        jsin.read( count );
        record *rec = new record( id );
        rec->json_count = count;
        records_by_id.insert( {id, rec} );
    }
}

void reset_save_ids( uint32_t prefix, bool quitting )
{
    save_id_prefix = prefix;
    save_id_prefix <<= 32;
    save_and_quit = quitting;
    safe_reference<item>::next_id = 1;
}

template<>
void deserialize<item>( safe_reference<item> &out, JsonIn &js )
{
    if( !js.test_number() ) {
        //legacy item_location support.


        auto obj = js.get_object();
        auto type = obj.get_string( "type" );
        int idx = -1;
        tripoint pos = tripoint_min;

        obj.read( "idx", idx );
        obj.read( "pos", pos );

        item *it = nullptr;

        auto find_index = [&idx, &it]( const item * e ) {
            if( idx-- == 0 ) {
                it = const_cast<item *>( e );
                return VisitResponse::ABORT;
            }
            return VisitResponse::NEXT;
        };

        if( type == "character" ) {
            character_id who_id;
            if( obj.has_member( "character" ) ) {
                obj.read( "character", who_id );
            } else {
                // This is for migrating saves before npc item locations were supported and all
                // character item locations were assumed to be on g->u
                who_id = get_avatar().getID();
            }

            Character *who = g->critter_by_id<Character>( who_id );
            if( !who ) {
                debugmsg( "Could not find character for item location.  May not have been loaded yet." );
                return;
            }
            who->visit_items( find_index );
            out = safe_reference<item>( it );
        } else if( type == "map" ) {
            map_cursor map_cur( pos );
            map_cur.visit_items( find_index );
            out = safe_reference<item>( it );
        } else if( type == "vehicle" ) {
            vehicle *const veh = veh_pointer_or_null( get_map().veh_at( pos ) );
            int part = obj.get_int( "part" );
            if( veh && part >= 0 && part < veh->part_count() ) {
                vehicle_cursor veh_cur( *veh, part );
                veh_cur.visit_items( find_index );
                out = safe_reference<item>( it );
            }
        } else if( type == "in_container" ) {
            safe_reference<item> parent;
            obj.read( "parent", parent );
            if( !parent ) {
                return;
            }
            const std::vector<item *> parent_contents = parent->contents.all_items_top();
            auto iter = parent_contents.begin();
            std::advance( iter, idx );
            out = safe_reference<item>( *iter );
        }
        return;
    }
    uint64_t id = 0;
    if( !js.read( id, false ) ) {
        return;
    }
    out = safe_reference<item>( id );
}

template<typename T>
void deserialize( safe_reference<T> &out, JsonIn &js )
{
    uint64_t id = 0;
    if( !js.read( id, false ) ) {
        return;
    }
    out = safe_reference<T>( id );
}

template<typename T>
void serialize( const safe_reference<T> &val, JsonOut &js )
{
    js.write( val.serialize() );
}

template
void serialize<item>( const safe_reference<item> &val, JsonOut &js );
