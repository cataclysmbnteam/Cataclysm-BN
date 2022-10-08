#pragma once
#ifndef CATA_SRC_ENUM_BITSET_H
#define CATA_SRC_ENUM_BITSET_H

#include <bitset>
#include <type_traits>

#include "enum_traits.h"

template<typename E>
class enum_bitset
{
        static_assert( std::is_enum<E>::value, "the template argument is not an enum." );
        static_assert( has_enum_traits<E>::value,
                       "a specialization of 'enum_traits<E>' template containing 'last' element of the enum must be defined somewhere.  "
                       "The `last` constant must be of the same type as the enum iteslf."
                     );

    public:
        enum_bitset() = default;
        enum_bitset( const enum_bitset & ) = default;
        auto operator=( const enum_bitset & ) -> enum_bitset & = default;

        auto operator==( const enum_bitset &rhs ) const noexcept -> bool {
            return bits == rhs.bits;
        }

        auto operator!=( const enum_bitset &rhs ) const noexcept -> bool {
            return !( *this == rhs );
        }

        auto operator&=( const enum_bitset &rhs ) noexcept -> enum_bitset & {
            bits &= rhs.bits;
            return *this;
        }

        auto operator|=( const enum_bitset &rhs ) noexcept -> enum_bitset & {
            bits |= rhs.bits;
            return *this;
        }

        auto operator[]( E e ) const -> bool {
            return bits[ get_pos( e ) ];
        }

        auto operator[]( E e ) -> enum_bitset & {
            return bits[ get_pos( e ) ];
        }

        auto set( E e, bool val = true ) -> enum_bitset & {
            bits.set( get_pos( e ), val );
            return *this;
        }

        auto set_all() -> enum_bitset & {
            bits.set();
            return *this;
        }

        auto clear( E e ) -> enum_bitset & {
            bits.reset( get_pos( e ) );
            return *this;
        }

        auto clear_all() -> enum_bitset & {
            bits.reset();
            return *this;
        }

        auto test( E e ) const -> bool {
            return bits.test( get_pos( e ) );
        }

        auto test_all() const -> bool {
            return bits.all();
        }

        auto test_any() const -> bool {
            return bits.any();
        }

        auto count() const -> size_t {
            return bits.count();
        }

        static constexpr auto size() noexcept -> size_t {
            return get_pos( enum_traits<E>::last );
        }

    private:
        static constexpr auto get_pos( E e ) noexcept -> size_t {
            return static_cast<size_t>( static_cast<typename std::underlying_type<E>::type>( e ) );
        }

        std::bitset<enum_bitset<E>::size()> bits;
};

#endif // CATA_SRC_ENUM_BITSET_H
