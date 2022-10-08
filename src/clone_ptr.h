#ifndef CATA_SRC_CLONE_PTR_H
#define CATA_SRC_CLONE_PTR_H

#include <memory>

namespace cata
{

template<typename T>
class clone_ptr
{
    public:
        clone_ptr() = default;
        clone_ptr( std::nullptr_t ) {}
        clone_ptr( const clone_ptr &other ) : p_( other.p_ ? other.p_->clone() : nullptr ) {}
        clone_ptr( clone_ptr && ) = default;
        auto operator=( const clone_ptr &other ) -> clone_ptr & {
            p_ = other.p_ ? other.p_->clone() : nullptr;
            return *this;
        }
        auto operator=( clone_ptr && ) -> clone_ptr & = default;

        // implicit conversion from unique_ptr
        template<typename U>
        clone_ptr( std::unique_ptr<U> p ) : p_( std::move( p ) ) {}

        auto operator*() -> T & {
            return *p_;
        }
        auto operator*() const -> const T & {
            return *p_;
        }
        auto operator->() -> T * {
            return p_.get();
        }
        auto operator->() const -> const T * {
            return p_.get();
        }
        auto get() -> T * {
            return p_.get();
        }
        auto get() const -> const T * {
            return p_.get();
        }

        explicit operator bool() const {
            return !!*this;
        }
        auto operator!() const -> bool {
            return !p_;
        }

        friend auto operator==( const clone_ptr &l, const clone_ptr &r ) -> bool {
            return l.p_ == r.p_;
        }
        friend auto operator!=( const clone_ptr &l, const clone_ptr &r ) -> bool {
            return l.p_ != r.p_;
        }
    private:
        std::unique_ptr<T> p_;
};

} // namespace cata

#endif // CATA_SRC_CLONE_PTR_H
