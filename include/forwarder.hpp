// ================================ FORWARDER =============================== //
// Project:         Forwarder
// Name:            forwarder.hpp
// Description:     A C++ forwarder class
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2020-]
// License:         BSD 3-Clause License
// ========================================================================== //
#ifndef _FORWARDER_HPP_INCLUDED
#define _FORWARDER_HPP_INCLUDED
// ========================================================================== //



// ============================== PREAMBLE ================================== //
// C++ standard library
#include <utility>
#include <type_traits>
// Project sources
// Third-party libraries
// Miscellaneous
namespace fwd {
// ========================================================================== //



// ================================= SIMILAR ================================ //
// Checks if two types are similar: cv-qualified types
template <class T, class U>
struct is_similar
: std::conditional_t<
    std::is_const_v<T> || std::is_volatile_v<T> ||
    std::is_const_v<U> || std::is_volatile_v<U>,
    is_similar<std::remove_cv_t<T>, std::remove_cv_t<U>>,
    std::is_same<T, U>
> {};

// Checks if two types are similar: pointer types
template <class T, class U>
struct is_similar<T*, U*>: is_similar<T, U> {};

// Checks if two types are similar: pointer to member types
template <class T, class U, class C>
struct is_similar<T C::*, U C::*>: is_similar<T, U> {};

// Checks if two types are similar: arrays
template <class T, class U, std::size_t N>
struct is_similar<T[N], U[N]>: is_similar<T, U> {};

// Checks if two types are similar: unknown bound arrays
template <class T, class U>
struct is_similar<T[], U[]>: is_similar<T, U> {};

// Checks if two types are similar: left-hand side unknown bound array
template <class T, class U, std::size_t N>
struct is_similar<T[], U[N]>: is_similar<T, U> {};

// Checks if two types are similar: right-hand side unknown bound array
template <class T, class U, std::size_t N>
struct is_similar<T[N], U[]>: is_similar<T, U> {};

// Variable template
template <class T, class U>
inline constexpr bool is_similar_v = is_similar<T, U>::value;

// Concept implementation
template <class T, class U>
concept concept_similar = is_similar_v<T, U>;

// Concept
template <class T, class U>
concept similar = concept_similar<T, U> && concept_similar<U, T>;
// ========================================================================== //



// ================================== ALIKE ================================= //
// Checks if two types are alike: their decayed types are similar
template <class T, class U>
struct is_alike: is_similar<std::decay_t<T>, std::decay_t<U>> {};

// Variable template
template <class T, class U>
inline constexpr bool is_alike_v = is_alike<T, U>::value;

// Concept implementation
template <class T, class U>
concept concept_alike = is_alike_v<T, U>;

// Concept
template <class T, class U>
concept alike = concept_alike<T, U> && concept_alike<U, T>;
// ========================================================================== //



// ================================ FORWARDER =============================== //
// A class that holds an entity and forwards it 
template <class T>
class forwarder
{
    // Friendship
    template <class U>
    friend class forwarder;
    
    // Types
    public:
    using type = T&&;

    // Lifecycle
    public:
    template <alike<T> U>
    constexpr forwarder(U&& arg) noexcept(
        std::is_nothrow_constructible_v<T, U&&>
    ): _arg(std::forward<U>(arg)) {
    }
    constexpr forwarder(const forwarder<T>& other) noexcept(
        std::is_nothrow_constructible_v<T, T>
    ) = default;
    constexpr forwarder(forwarder<T>&& other) noexcept(
        std::is_nothrow_constructible_v<T, T&&>
    ): _arg(std::forward<T>(other._arg)) {
    }
    template <alike<T> U>
    constexpr forwarder(const forwarder<U>& other) noexcept(
        std::is_nothrow_constructible_v<T, U>
    ): _arg(other._arg) {
    }
    template <alike<T> U>
    constexpr forwarder(forwarder<U>&& other) noexcept(
        std::is_nothrow_constructible_v<T, U&&>
    ): _arg(std::forward<U>(other._arg)) {
    }

    // Forwarding
    public:
    constexpr T&& operator()() noexcept {
        return std::forward<T>(_arg);
    }
    constexpr T&& operator()() const noexcept {
        return std::forward<T>(_arg);
    }

    // Implementation details: data members
    private:
    T _arg;
};

// Checks if a type is a forwarder: default
template <class T>
struct is_forwarder: std::false_type {};

// Checks if a type is a forwarder: forwarder specialization
template <class T>
struct is_forwarder<forwarder<T>>: std::true_type {};

// Variable template
template <class T>
inline constexpr bool is_forwarder_v = is_forwarder<T>::value;

// Deduction guide
template <class T>
requires !is_forwarder_v<std::remove_cvref_t<T>>
forwarder(T&&) -> forwarder<T>;
// ========================================================================== //



// ========================================================================== //
} // namespace fwd
#endif // _FORWARDER_HPP_INCLUDED
// ========================================================================== //
