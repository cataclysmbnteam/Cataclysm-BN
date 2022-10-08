#pragma once
#ifndef CATA_SRC_SAFE_REFERENCE_H
#define CATA_SRC_SAFE_REFERENCE_H

/**
A pair of classes to provide safe references to objects.

A safe_reference_anchor is made a member of some object, and a safe_reference
to that object (or any other object with the same lifetime) can be obtained
from the safe_reference_anchor.

When the safe_reference is destroyed or assigned-to, all safe_references
derived from it are invalidated.  When one attempts to fetch the referenced
object from an invalidated safe_reference, it returns nullptr.

The motivating use case is to store references to items in item_locations in a
way that is safe if that item is moved or destroyed.

*/

#include <memory>

template<typename T>
class safe_reference
{
    public:
        safe_reference() = default;

        auto get() const -> T * {
            return impl.lock().get();
        }

        explicit operator bool() const {
            return !!*this;
        }

        auto operator!() const -> bool {
            return impl.expired();
        }

        auto operator*() const -> T & {
            return *get();
        }

        auto operator->() const -> T * {
            return get();
        }
    private:
        friend class safe_reference_anchor;

        safe_reference( const std::shared_ptr<T> &p ) : impl( p ) {}

        std::weak_ptr<T> impl;
};

class safe_reference_anchor
{
    public:
        safe_reference_anchor();
        safe_reference_anchor( const safe_reference_anchor & );
        auto operator=( const safe_reference_anchor & ) -> safe_reference_anchor &;

        template<typename T>
        auto reference_to( T *object ) -> safe_reference<T> {
            // Using the shared_ptr aliasing constructor
            return safe_reference<T>( std::shared_ptr<T>( impl, object ) );
        }
    private:
        struct empty {};
        std::shared_ptr<empty> impl;
};

#endif // CATA_SRC_SAFE_REFERENCE_H
