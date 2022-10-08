#pragma once
#ifndef CATA_SRC_FLAT_SET_H
#define CATA_SRC_FLAT_SET_H

#include <algorithm>
#include <vector>

namespace cata
{

struct transparent_less_than {
    using is_transparent = void;

    template<typename T, typename U>
    constexpr auto operator()( T &&lhs, U &&rhs ) const noexcept
    // NOLINTNEXTLINE(cata-use-localized-sorting)
    -> decltype( std::forward<T>( lhs ) < std::forward<U>( rhs ) ) {
        // NOLINTNEXTLINE(cata-use-localized-sorting)
        return std::forward<T>( lhs ) < std::forward<U>( rhs );
    }
};

/**
 * @brief A SimpleAssociativeContainer implemented as a sorted vector.
 *
 * O(n) insertion, O(log(n)) lookup, only one allocation at any given time.
 */
template<typename T, typename Compare = transparent_less_than, typename Data = std::vector<T>>
class flat_set : private Compare, Data
{
    private:
        template<typename Cmp, typename Sfinae, typename = void>
        struct has_is_transparent {};
        template<typename Cmp, typename Sfinae>
        struct has_is_transparent<Cmp, Sfinae, typename Cmp::is_transparent> {
            using type = void;
        };

    public:
        using typename Data::value_type;
        using typename Data::const_reference;
        using typename Data::const_pointer;
        using key_type = value_type;
        using key_compare = Compare;
        using value_compare = Compare;
        using typename Data::size_type;
        using typename Data::difference_type;
        using typename Data::const_iterator;
        using iterator = const_iterator;
        using typename Data::const_reverse_iterator;
        using reverse_iterator = const_reverse_iterator;

        flat_set() = default;
        flat_set( const key_compare &kc ) : Compare( kc ) {}
        template<typename InputIt>
        flat_set( InputIt first, InputIt last ) : Data( first, last ) {
            sort_data();
        }
        template<typename InputIt>
        flat_set( InputIt first, InputIt last, const key_compare &kc ) :
            Compare( kc ), Data( first, last ) {
            sort_data();
        }
        flat_set( std::initializer_list<value_type> init ) : Data( init ) {
            sort_data();
        }

        auto key_comp() const -> const key_compare & {
            return *this;
        }
        auto value_comp() const -> const value_compare & {
            return *this;
        }

        using Data::size;
        using Data::max_size;
        using Data::empty;

        using Data::reserve;
        using Data::capacity;
        using Data::shrink_to_fit;

        auto begin() const -> iterator {
            return cbegin();
        }
        auto end() const -> iterator {
            return cend();
        }
        auto rbegin() const -> reverse_iterator {
            return crbegin();
        }
        auto rend() const -> reverse_iterator {
            return crend();
        }
        using Data::cbegin;
        using Data::cend;
        using Data::crbegin;
        using Data::crend;

        auto operator[]( size_type i ) const -> const_reference {
            return Data::operator[]( i );
        }

        auto lower_bound( const T &t ) const -> const_iterator {
            return std::lower_bound( begin(), end(), t, key_comp() );
        }
        template<typename K, typename = typename has_is_transparent<Compare, K>::type>
        auto lower_bound( const K &k ) const -> const_iterator {
            return std::lower_bound( begin(), end(), k, key_comp() );
        }
        auto upper_bound( const T &t ) const -> const_iterator {
            return std::upper_bound( begin(), end(), t, key_comp() );
        }
        template<typename K, typename = typename has_is_transparent<Compare, K>::type>
        auto upper_bound( const K &k ) const -> const_iterator {
            return std::upper_bound( begin(), end(), k, key_comp() );
        }
        auto equal_range( const T &t ) const -> std::pair<const_iterator, const_iterator> {
            return { lower_bound( t ), upper_bound( t ) };
        }
        template<typename K, typename = typename has_is_transparent<Compare, K>::type>
        auto equal_range( const K &k ) const -> std::pair<const_iterator, const_iterator> {
            return { lower_bound( k ), upper_bound( k ) };
        }

        auto find( const value_type &value ) const -> const_iterator {
            auto at = lower_bound( value );
            if( at != end() && *at == value ) {
                return at;
            }
            return end();
        }
        template<typename K, typename = typename has_is_transparent<Compare, K>::type>
        auto find( const K &k ) const -> const_iterator {
            auto at = lower_bound( k );
            if( at != end() && *at == k ) {
                return at;
            }
            return end();
        }
        auto count( const T &t ) const -> size_type {
            auto at = lower_bound( t );
            return at != end() && *at == t;
        }
        template<typename K, typename = typename has_is_transparent<Compare, K>::type>
        auto count( const K &k ) const -> size_type {
            auto at = lower_bound( k );
            return at != end() && *at == k;
        }

        auto insert( iterator, const value_type &value ) -> iterator {
            /// TODO: Use insertion hint
            return insert( value ).first;
        }
        auto insert( iterator, value_type &&value ) -> iterator {
            /// TODO: Use insertion hint
            return insert( std::move( value ) ).first;
        }
        auto insert( const value_type &value ) -> std::pair<iterator, bool> {
            auto at = lower_bound( value );
            if( at != end() && *at == value ) {
                return { at, false };
            }
            return { Data::insert( at, value ), true };
        }
        auto insert( value_type &&value ) -> std::pair<iterator, bool> {
            auto at = lower_bound( value );
            if( at != end() && *at == value ) {
                return { at, false };
            }
            return { Data::insert( at, std::move( value ) ), true };
        }

        template<typename InputIt>
        void insert( InputIt first, InputIt last ) {
            /// TODO: could be faster when inserting only a few elements
            Data::insert( end(), first, last );
            sort_data();
        }

        using Data::clear;
        using Data::erase;
        auto erase( const value_type &value ) -> size_type {
            auto at = find( value );
            if( at != end() ) {
                erase( at );
                return 1;
            }
            return 0;
        }

        friend void swap( flat_set &l, flat_set &r ) {
            using std::swap;
            swap( static_cast<Compare &>( l ), static_cast<Compare &>( r ) );
            swap( static_cast<Data &>( l ), static_cast<Data &>( r ) );
        }
#define FLAT_SET_OPERATOR( op ) \
    friend bool operator op( const flat_set &l, const flat_set &r ) { \
        return l.data() op r.data(); \
    }
        FLAT_SET_OPERATOR( == )
        FLAT_SET_OPERATOR( != )
        FLAT_SET_OPERATOR( < )
        FLAT_SET_OPERATOR( <= )
        FLAT_SET_OPERATOR( > )
        FLAT_SET_OPERATOR( >= )
#undef FLAT_SET_OPERATOR
    private:
        auto data() const -> const Data & {
            return *this;
        }
        void sort_data() {
            std::sort( Data::begin(), Data::end(), key_comp() );
            auto new_end = std::unique( Data::begin(), Data::end() );
            Data::erase( new_end, end() );
        }
};

} // namespace cata

#endif // CATA_SRC_FLAT_SET_H
