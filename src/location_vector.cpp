#include "location_vector.h"
#include "item.h"

template<typename T>
location_vector<T>::location_vector( location<T> *loc ) : loc( loc ) {};

template<typename T>
location_vector<T>::location_vector( location<T> *loc,
                                     std::vector<detached_ptr<T>> &from ) : loc( loc )
{
    for( detached_ptr<T> &obj : from ) {
        obj->set_location( &*loc );
        contents.push_back( &*obj );
        obj.ptr = nullptr;
    }
    from.clear();
};

template<typename T>
void location_vector<T>::push_back( detached_ptr<T> &&obj )
{
    T *raw = obj.ptr;
    obj.ptr = nullptr;
    raw->set_location( &*loc );
    contents.push_back( raw );
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
    T *subject = *it;
    typename std::vector<T *>::iterator ret = contents.erase( it );
    subject->remove_location();

    detached_ptr<T> local;
    detached_ptr<T> *used = out ? out : &local;

    *used = detached_ptr<T>( subject );

}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::insert( typename std::vector<T *>::iterator
        it,
        detached_ptr<T> &&obj )
{
    if( !obj ) {
        return;
    }
    T *raw = obj->release();
    raw->set_location( &*loc );
    return contents.insert( it, raw );
}

template<typename T>
typename std::vector<T *>::iterator location_vector<T>::insert( typename std::vector<T *>::iterator
        it,
        typename std::vector<detached_ptr<T>>::iterator start,
        typename std::vector<detached_ptr<T>>::iterator end )
{

    for( auto iter = start; iter != end; iter++ ) {
        T *raw = ( *iter )->release();
        raw->set_location( &*loc );
        it = contents.insert( it, raw );
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
typename std::vector<detached_ptr<T>> location_vector<T>::clear() const
{
    std::vector<detached_ptr<T>> ret;
    for( item *&i : contents ) {
        i->remove_location();
        ret.push_back( detached_ptr( i ) );
    }
    contents.clear();
    return ret;
}

template<typename T>
void location_vector<T>::remove_with( std::function < detached_ptr<T>( detached_ptr<T> && ) > cb )
{
    for( auto it = contents.begin(); it != contents.end(); ) {
        location<T> *saved_loc = ( *it )->loc;
        ( *it )->remove_location();
        detached_ptr<T> original( *it );
        detached_ptr<T> n = cb( std::move( original ) );
        if( n ) {
            if( &*n == *it ) {
                ( *it )->loc = saved_loc;
                return;
            } else {
                debugmsg( "Returning a different item in remove_with is not currently supported" );
                return;
            }
        }
    }
}
