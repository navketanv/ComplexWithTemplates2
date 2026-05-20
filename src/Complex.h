#pragma once
#include <source_location>
#include <iostream>
#include "Memory/ObjectStorage.h"
#include <concepts>
#include <type_traits>
#include <utility>

template<typename T1, typename T2>
class Complex;

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const Complex<T1, T2>& rhs) noexcept;

template<typename T1, typename T2>
class Complex
{
private:
    struct ComplexData {
        T1 m_real;
        T2 m_imaginary;
    };

    using Storage = ObjectStorage<ComplexData>;

public:
    template<typename U1 = T1, typename U2 = T2>
        requires(
            std::constructible_from<T1, U1&&> && std::constructible_from<T2, U2&&> &&
            !(std::derived_from<std::remove_cvref_t<U1>, Complex<T1, T2>> ||
              std::derived_from<std::remove_cvref_t<U2>, Complex<T1, T2>>)
            )
    Complex(U1&& real, U2&& imaginary);

    template<typename U1 = T1>
        requires(
            std::constructible_from<T1, U1&&> &&
            !std::derived_from<std::remove_cvref_t<U1>, Complex<T1, T2>>
            )
    Complex(U1&& real);

    Complex();

    Complex(const Complex& rhs);
    Complex(Complex&& rhs) noexcept(std::is_nothrow_move_constructible_v<Storage>);
    Complex& operator=(const Complex& rhs);
    Complex& operator=(Complex&& rhs) noexcept(std::is_nothrow_move_assignable_v<Storage>);
    virtual ~Complex() noexcept;

    Complex& operator+=(const Complex& rhs);
    Complex& operator-=(const Complex& rhs);
    Complex& operator*=(const Complex& rhs);
    Complex& operator/=(const Complex& rhs);

    const T1& real() const noexcept;
    T1& real() noexcept;
    const T2& imaginary() const noexcept;
    T2& imaginary() noexcept;

    void real(T1 real) noexcept(std::is_nothrow_move_assignable_v<T1>);
    void imaginary(T2 imaginary) noexcept(std::is_nothrow_move_assignable_v<T2>);

    static void* operator new(size_t size) noexcept(false);
    static void operator delete(void* pMem) noexcept;

protected:
    virtual std::ostream& print(std::ostream& os, const std::string& prefix = std::string{}) const noexcept;
    static void printLocation(const std::source_location& location = std::source_location::current(), bool bDetailedLocation = false) noexcept;

private:
    const ComplexData& value() const noexcept;
    ComplexData& value() noexcept;
    static void myHandler();

/*
    template<typename... Args>
    static typename Complex<T1, T2>::Data* createData(Args&&... args);
    static typename Complex<T1, T2>::Data* createDataImpl(T1&& real, T2&& imaginary);
    static void destroyData(typename Complex<T1, T2>::Data*& pData) noexcept;
*/

private:
//    typename Complex<T1, T2>::Data* m_pData{nullptr};
    Storage m_storage{};
    static inline std::new_handler m_pHandler{nullptr};
    friend std::ostream& operator<< <> (std::ostream& os, const Complex<T1, T2>& rhs) noexcept;
};

template<typename U1, typename U2>
Complex(U1, U2) -> Complex<U1, U2>;

template<typename U1>
Complex(U1) -> Complex<U1, U1>;

#include "Complex.tpp"