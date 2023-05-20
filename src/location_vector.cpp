#include "location_vector.h"
#include "item.h"
#include "locations.h"

template<typename T>
location_vector<T>::location_vector( location<T> *loc ) : loc( loc ) {};

template<typename T>
location_vector<T>::location_vector( location<T> *loc,
                                     std::vector<detached_ptr<T>> &from ) : loc( loc )
{
    for( detached_ptr<T> &obj : from ) {
        if( !obj ) {
            continue;
        }
        obj->set_location( &*loc );
        contents.push_back( obj.release() );
    }
    from.clear();
};

template<typename T>
location_vector<T>::~location_vector()
{
    on_destroy();
}

template<typename T>
void location_vector<T>::push_back( detached_ptr<T> &&obj )
{
    if( !obj ) {
        return;
    }

    T *raw = obj.release();

    //Skip adding it if it's already here
    if( &*loc != raw->saved_loc ) {
        raw->resolve_saved_loc();
        contents.push_back( raw );
        if( destroyed ) {
            raw->destroy_in_place();
        }
    } else {
        raw->saved_loc = nullptr;
    }
    raw->set_location( &*loc );
}

template<typename T>
size_t location_vector<T>::size() const
{
    return contents.size();
}

template<typename T>
bool location_vector<T>::empty() const
{
    return contents.empty();
}

template<typename T>
T *location_vector<T>::back() const
{
    return contents.back();
}

template<typename T>
detached_ptr<T> location_vector<T>::remove( T *obj )
{
    if( destroyed ) {
        debugmsg( "Attempted to remove something from a destroyed location." );
        return detached_ptr<T>();
    }

    for( auto iter = contents.begin(); iter != contents.end(); iter++ ) {
        if( *iter == obj ) {
            detached_ptr<T> ret;
            erase( iter, &ret );
            return ret;
        }
    }
    debugmsg( "Attempted to remove something from a location that wasn't there." );
    return detached_ptr<T>();
}

template<typename T>
const std::vector<T *> &location_vector<T>::as_vector() const
{
    return contents;
}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::erase( typename
        std::vector<T *>::const_iterator
        it,
        detached_ptr<T> *out )
{
    if( destroyed && out ) {
        debugmsg( "Attempted to erase something from a destroyed location." );
        return contents.end();
    }
    T *subject = *it;
    typename std::vector<T *>::iterator ret = contents.erase( it );
    subject->remove_location();

    detached_ptr<T> local;
    detached_ptr<T> *used = out ? out : &local;

    *used = detached_ptr<T>( subject );
    return ret;
}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::insert( typename std::vector<T *>::iterator
        it,
        detached_ptr<T> &&obj )
{
    if( !obj ) {
        return it;
    }
    T *raw = obj.release();
    //Insert it if it's not already here, find it otherwise
    if( &*loc != raw->saved_loc ) {
        raw->resolve_saved_loc();
        raw->set_location( &*loc );
        if( destroyed ) {
            raw->destroy_in_place();
        }
        return contents.insert( it, raw );
    } else {
        raw->saved_loc = nullptr;
        raw->set_location( &*loc );
        return std::find( contents.begin(), contents.end(), raw );
    }
}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::insert( typename std::vector<T *>::iterator
        it,
        typename std::vector<detached_ptr<T>>::iterator start,
        typename std::vector<detached_ptr<T>>::iterator end )
{

    for( auto iter = start; iter != end; iter++ ) {
        if( !*iter ) {
            continue;
        }
        T *raw = iter->release();
        //Only insert it if it's not already here
        if( &*loc != raw->saved_loc ) {
            raw->saved_loc = nullptr;
            raw->set_location( &*loc );
            if( destroyed ) {
                raw->destroy_in_place();
            }
            it = contents.insert( it, raw );
        } else {
            raw->resolve_saved_loc();
            raw->set_location( &*loc );
        }
    }
    return it;
}

template<typename T>
typename std::vector<T *>::const_iterator location_vector<T>::begin() const
{
    return contents.begin();
}

template<typename T>
typename std::vector<T *>::const_iterator location_vector<T>::end() const
{
    return contents.end();
}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::begin()
{
    return contents.begin();
}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::end()
{
    return contents.end();
}

template<typename T>
typename std::vector<T *>::const_reverse_iterator location_vector<T>::rbegin() const
{
    return contents.rbegin();
}

template<typename T>
typename std::vector<T *>::const_reverse_iterator location_vector<T>::rend() const
{
    return contents.rend();
}

template<typename T>
typename std::vector<T *>::reverse_iterator location_vector<T>::rbegin()
{
    return contents.rbegin();
}

template<typename T>
typename std::vector<T *>::reverse_iterator location_vector<T>::rend()
{
    return contents.rend();
}

template<typename T>
typename std::vector<T *>::const_iterator location_vector<T>::cbegin() const
{
    return contents.cbegin();
}

template<typename T>
typename std::vector<T *>::const_iterator location_vector<T>::cend() const
{
    return contents.cend();
}

template<typename T>
typename std::vector<T *>::const_reverse_iterator location_vector<T>::crbegin() const
{
    return contents.crbegin();
}

template<typename T>
typename std::vector<T *>::const_reverse_iterator location_vector<T>::crend() const
{
    return contents.crend();
}

template<typename T>
T *location_vector<T>::front() const
{
    return contents.front();
}

template<typename T>
location<T> *location_vector<T>::get_location() const
{
    return &*loc;
}

template<typename T>
typename std::vector<detached_ptr<T>> location_vector<T>::clear()
{
    if( destroyed ) {
        debugmsg( "Attempted to clear a destroyed location." );
        return std::vector<detached_ptr<T>>();
    }
    std::vector<detached_ptr<T>> ret;
    for( item * const &i : contents ) {
        i->remove_location();
        ret.push_back( detached_ptr( i ) );
    }
    contents.clear();
    return ret;
}

template<typename T>
void location_vector<T>::remove_with( std::function < detached_ptr<T>( detached_ptr<T> && ) > cb )
{
    if( destroyed ) {
        debugmsg( "Attempted to remove_with from a destroyed location." );
        return;
    }
    if( locked ) {
        debugmsg( "Recursive removal in location_vector" );
        return;
    }
    locked = true;
    for( auto it = contents.begin(); it != contents.end(); ) {
        location<T> *saved_loc = ( *it )->loc;
        ( *it )->remove_location();
        detached_ptr<T> original( *it );
        detached_ptr<T> n = cb( std::move( original ) );
        if( n ) {
            if( &*n == *it ) {
                ( *it )->loc = saved_loc;
                n.release();
            } else {
                debugmsg( "Returning a different item in remove_with is not currently supported" );
            }
            it++;
        } else {
            it = contents.erase( it );
        }
    }
    locked = false;
}

template<typename T>
void location_vector<T>::move_by( tripoint offset )
{
    auto tile_loc = dynamic_cast<tile_item_location *>( &*loc );
    if( !tile_loc ) {
        debugmsg( "Tried to move_by a non-tile location" );
        return;
    }
    tile_loc->move_by( offset );
}

template<typename T>
void location_vector<T>::set_loc_hack( location<T> *new_loc )
{
    loc = std::unique_ptr<location<T>>( new_loc );
    for( item *&it : contents ) {
        it->remove_location();
        it->set_location( &*loc );
    }
}

template<typename T>
void location_vector<T>::on_destroy()
{
    if( destroyed ) {
        return;
    }
    for( item *&it : contents ) {
        it->destroy_in_place();
    }
    destroyed = true;
}


template
class location_vector<item>;
