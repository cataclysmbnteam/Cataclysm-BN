#include "location_vector.h"
location_vector::location_vector( location<T> *loc ) : loc( loc ) {};
location_vector::location_vector( location<T> *loc,
                                  std::vector<detached_ptr<T>> &from ) : loc( loc )
{
    for( detached_ptr<T> &obj : from ) {
        obj->set_location( &*loc );
        contents.push_back( &*obj );
        obj.ptr = nullptr;
    }
    from.clear();
};

void location_vector::push_back( detached_ptr<T> &&obj )
{
    T *raw = obj.ptr;
    obj.ptr = nullptr;
    raw->set_location( &*loc );
    contents.push_back( raw );
}

size_t location_vector::size() const
{
    return contents.size();
}
bool location_vector::empty() const
{
    return contents.empty();
}

T *location_vector::back() const
{
    return contents.back();
}

const std::vector<T *> &location_vector::as_vector() const
{
    return contents;
}

typename std::vector<T *>::iterator location_vector::erase( typename std::vector<T *>::iterator it,
        detached_ptr<T> *out = nullptr );

typename std::vector<T *>::iterator location_vector::insert( typename std::vector<T *>::iterator it,
        detached_ptr<T> &&obj );

typename std::vector<T *>::iterator location_vector::insert( typename std::vector<T *>::iterator it,
        typename std::vector<detached_ptr<T>>::const_iterator start,
        typename std::vector<detached_ptr<T>>::const_iterator end );

typename std::vector<T *>::const_iterator location_vector::begin() const
{
    return contents.begin();
}

typename std::vector<T *>::const_iterator location_vector::end() const
{
    return contents.end();
}

typename std::vector<T *>::iterator location_vector::begin()
{
    return contents.begin();
}

typename std::vector<T *>::iterator location_vector::end()
{
    return contents.end();
}

typename std::vector<T *>::const_reverse_iterator location_vector::rbegin() const
{
    return contents.rbegin();
}

typename std::vector<T *>::const_reverse_iterator location_vector::rend() const
{
    return contents.rend();
}

typename std::vector<T *>::reverse_iterator location_vector::rbegin()
{
    return contents.rbegin();
}

typename std::vector<T *>::reverse_iterator location_vector::rend()
{
    return contents.rend();
}

typename std::vector<T *>::const_iterator location_vector::cbegin() const
{
    return contents.cbegin();
}

typename std::vector<T *>::const_iterator location_vector::cend() const
{
    return contents.cend();
}

typename std::vector<detached_ptr<T>> location_vector::clear() const
{
    std::vector<detached_ptr<T>> ret;
    for( item *&i : contents ) {
        i->remove_location();
        ret.push_back( detached_ptr( i ) );
    }
    contents.clear();
    return ret;
}
