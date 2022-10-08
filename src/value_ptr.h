#pragma once
#ifndef CATA_SRC_VALUE_PTR_H
#define CATA_SRC_VALUE_PTR_H

#include <memory>

class JsonIn;
class JsonOut;

namespace cata
{

/**
 * This class is essentially a copyable unique pointer. Its purpose is to allow
 * for sparse storage of data without wasting memory in classes such as itype.
 */
template <class T>
class value_ptr : public std::unique_ptr<T>
{
    public:
        value_ptr() = default;
        value_ptr( value_ptr && ) = default;
        value_ptr( T *value ) : std::unique_ptr<T>( value ) {}
        value_ptr( const value_ptr<T> &other ) :
            std::unique_ptr<T>( other ? new T( *other ) : nullptr ) {}
        auto operator=( value_ptr<T> other ) -> value_ptr & {
            std::unique_ptr<T>::operator=( std::move( other ) );
            return *this;
        }

        template<typename Stream = JsonOut>
        void serialize( Stream &jsout ) const {
            if( this->get() ) {
                this->get()->serialize( jsout );
            } else {
                jsout.write_null();
            }
        }
        template<typename Stream = JsonIn>
        void deserialize( Stream &jsin ) {
            if( jsin.test_null() ) {
                this->reset();
            } else {
                this->reset( new T() );
                this->get()->deserialize( jsin );
            }
        }
};

template <class T, class... Args>
auto make_value( Args &&...args ) -> value_ptr<T>
{
    return value_ptr<T>( new T( std::forward<Args>( args )... ) );
}

template <class T>
auto value_ptr_equals( const value_ptr<T> &lhs, const value_ptr<T> &rhs ) -> bool
{
    return ( !lhs && !rhs ) || ( lhs && rhs && *lhs == *rhs );
}

} // namespace cata

#endif // CATA_SRC_VALUE_PTR_H
