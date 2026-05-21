#pragma once
#include <source_location>
#include <iostream>
#include "Memory/ObjectStorage.h"
#include <concepts>
#include <type_traits>

template<typename T>
concept Arithmetic = (std::integral<std::remove_cvref_t<T>> ||
                      std::floating_point<std::remove_cvref_t<T>>) &&
                     !std::same_as<std::remove_cvref_t<T>, bool>;

template<Arithmetic T1, Arithmetic T2>
class Complex;

template<typename T>
struct IsComplex : std::false_type{};

template<Arithmetic T1, Arithmetic T2>
struct IsComplex<Complex<T1, T2>> : std::true_type{};

template<typename T>
inline constexpr bool IsComplex_v = IsComplex<std::remove_cvref_t<T>>::value;

template<Arithmetic T1, Arithmetic T2>
std::ostream& operator<<(std::ostream& os, const Complex<T1, T2>& rhs) noexcept;

template<Arithmetic T1, Arithmetic T2>
class Complex
{
private:
    struct ComplexData {
        T1 m_real;
        T2 m_imaginary;
    };

    using Storage = ObjectStorage<ComplexData>;

public:
    template<Arithmetic U1 = T1, Arithmetic U2 = T2>
        requires(
            std::constructible_from<T1, U1&&> &&
            std::constructible_from<T2, U2&&> &&
            !(IsComplex_v<U1> || IsComplex_v<U2>)
        )
    explicit Complex(U1&& real, U2&& imaginary);

    template<Arithmetic U1 = T1>
        requires(
            std::constructible_from<T1, U1&&> &&
            !IsComplex_v<U1>
        )
    explicit Complex(U1&& real);

    Complex();

    Complex(const Complex& rhs);
    Complex(Complex&& rhs) noexcept(std::is_nothrow_move_constructible_v<Storage>);
    Complex& operator=(const Complex& rhs);
    Complex& operator=(Complex&& rhs) noexcept(std::is_nothrow_move_assignable_v<Storage>);
    virtual ~Complex() noexcept;

    template<Arithmetic U1, Arithmetic U2>
    Complex& operator+=(const Complex<U1, U2>& rhs);
    template<Arithmetic U1, Arithmetic U2>
    Complex& operator-=(const Complex<U1, U2>& rhs);
    template<Arithmetic U1, Arithmetic U2>
    Complex& operator*=(const Complex<U1, U2>& rhs);
    template<Arithmetic U1, Arithmetic U2>
        requires(std::floating_point<T1> && std::floating_point<T2>)
    Complex& operator/=(const Complex<U1, U2>& rhs);

    const T1& real() const noexcept;
    const T2& imaginary() const noexcept;

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
    T1& real() noexcept;
    T2& imaginary() noexcept;
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

template<Arithmetic U1, Arithmetic U2>
Complex(U1, U2) -> Complex<U1, U2>;

template<Arithmetic U1>
Complex(U1) -> Complex<U1, U1>;

#include "Complex.tpp"