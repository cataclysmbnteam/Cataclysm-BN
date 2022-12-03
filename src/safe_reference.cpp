#include "safe_reference.h"
#include "item.h"
#include "json.h"


uint64_t save_id_prefix;
bool save_and_quit;

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
