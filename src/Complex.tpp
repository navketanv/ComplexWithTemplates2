#include <concepts>
#include <type_traits>
#include <utility>
#include <sstream>
#include <cmath>
#include <limits>
#include <stdexcept>
#include "Memory/NewHandlerGuard.h"

template<Arithmetic T1, Arithmetic T2>
template<Arithmetic U1, Arithmetic U2>
    requires(
        std::constructible_from<T1, U1&&> &&
        std::constructible_from<T2, U2&&> &&
        !(IsComplex_v<U1> || IsComplex_v<U2>)
    )
Complex<T1, T2>::Complex(U1&& real, U2&& imaginary)
    : m_storage(std::in_place, std::forward<U1>(real), std::forward<U2>(imaginary))
{
    printLocation();
    std::cout << "2-arg Forwarding Constructor : " << *this;
}

template<Arithmetic T1, Arithmetic T2>
template<Arithmetic U1>
    requires(
        std::constructible_from<T1, U1&&> && !IsComplex_v<U1>
    )
Complex<T1, T2>::Complex(U1&& real)
    : Complex(std::forward<U1>(real), T2{}) {}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>::Complex()
    : Complex(T1{}, T2{}) {}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>::Complex(const Complex<T1, T2>& rhs)
    : m_storage(std::in_place, rhs.value())
{
    printLocation();
    std::cout << "Copy Constructor Complex : " << *this;
}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>::Complex(Complex<T1, T2>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Storage>)
    : m_storage(std::move(rhs.m_storage))
{
    printLocation();
    std::cout << "Move Constructor Complex : " << *this;
}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>& Complex<T1, T2>::operator=(const Complex<T1, T2>& rhs) {
    if (this != &rhs) {
        m_storage = Storage(std::in_place, rhs.value());
    }
    printLocation();
    std::cout << "Copy Assignment Operator Complex : " << *this;
    return *this;
}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>& Complex<T1, T2>::operator=(Complex<T1, T2>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Storage>) {
    if (this != &rhs) {
        m_storage = std::move(rhs.m_storage);
    }
    printLocation();
    std::cout << "Move Assignment Operator Complex : " << *this;
    return *this;
}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>::~Complex() noexcept {
    printLocation();
}

template<Arithmetic T1, Arithmetic T2>
template<Arithmetic U1, Arithmetic U2>
Complex<T1, T2>& Complex<T1, T2>::operator+=(const Complex<U1, U2>& rhs) {
    printLocation();
    using T = std::common_type_t<T1, T2, U1, U2>;
    T realVal = static_cast<T>(real()) + static_cast<T>(rhs.real());
    T imgVal = static_cast<T>(imaginary()) + static_cast<T>(rhs.imaginary());
    real(std::move(static_cast<T1>(realVal)));
    imaginary(std::move(static_cast<T2>(imgVal)));
    std::cout << *this;
    return *this;
}

template<Arithmetic T1, Arithmetic T2>
template<Arithmetic U1, Arithmetic U2>
Complex<T1, T2>& Complex<T1, T2>::operator-=(const Complex<U1, U2>& rhs) {
    printLocation();
    using T = std::common_type_t<T1, T2, U1, U2>;
    T realVal = static_cast<T>(real()) - static_cast<T>(rhs.real());
    T imgVal = static_cast<T>(imaginary()) - static_cast<T>(rhs.imaginary());
    real(std::move(static_cast<T1>(realVal)));
    imaginary(std::move(static_cast<T2>(imgVal)));
    std::cout << *this;
    return *this;
}

template<Arithmetic T1, Arithmetic T2>
template<Arithmetic U1, Arithmetic U2>
Complex<T1, T2>& Complex<T1, T2>::operator*=(const Complex<U1, U2>& rhs) {
    printLocation();
    using T = std::common_type_t<T1, T2, U1, U2>;
    T a = static_cast<T>(real());
    T b = static_cast<T>(imaginary());
    T c = static_cast<T>(rhs.real());
    T d = static_cast<T>(rhs.imaginary());
    T realVal = T{};
    T imgVal = T{};
    if constexpr (std::floating_point<T>) {
        using std::fma;
        realVal = fma(a, c, -fma(b, d, T{}));
        imgVal = fma(b, c, fma(a, d, T{}));
    } else {
        realVal = a * c - b * d;
        imgVal = b * c + a * d;
    }

    real(std::move(static_cast<T1>(realVal)));
    imaginary(std::move(static_cast<T2>(imgVal)));
    std::cout << *this;
    return *this;
}

template<Arithmetic T1, Arithmetic T2>
template<Arithmetic U1, Arithmetic U2>
    requires(std::floating_point<T1> && std::floating_point<T2>)
Complex<T1, T2>& Complex<T1, T2>::operator/=(const Complex<U1, U2>& rhs) {
    printLocation();
    using T = std::common_type_t<T1, T2, U1, U2>;
    using std::abs;
    T a = static_cast<T>(real());
    T b = static_cast<T>(imaginary());
    T c = static_cast<T>(rhs.real());
    T d = static_cast<T>(rhs.imaginary());

    if constexpr (std::floating_point<T>) {
        if ((abs(c) <= std::numeric_limits<T>::epsilon()) &&
            (abs(d) <= std::numeric_limits<T>::epsilon())) {
            throw std::domain_error("Division by Zero Error");
        }
    } else if ((c == 0) && (d == 0)) {
        throw std::domain_error("Division by Zero Error");
    }

    if (abs(c) >= abs(d)) {
        T r = d / c;
        T denom = (c + r * d);
        T realVal = (a + b * r) / denom;
        T imgVal = (b - r * a) / denom;
        real(std::move(realVal));
        imaginary(std::move(imgVal));
    } else {
        T r = c / d;
        T denom = (c * r + d);
        T realVal = (b + a * r) / denom;
        T imgVal = (b * r - a) / denom;
        real(std::move(realVal));
        imaginary(std::move(imgVal));
    }
    std::cout << *this;
    return *this;
}

template<Arithmetic T1, Arithmetic T2>
const T1& Complex<T1, T2>::real() const noexcept {
    return m_storage->m_real;
}

template<Arithmetic T1, Arithmetic T2>
T1& Complex<T1, T2>::real() noexcept {
    return m_storage->m_real;
}

template<Arithmetic T1, Arithmetic T2>
const T2& Complex<T1, T2>::imaginary() const noexcept {
    return m_storage->m_imaginary;
}

template<Arithmetic T1, Arithmetic T2>
T2& Complex<T1, T2>::imaginary() noexcept {
    return m_storage->m_imaginary;
}

template<Arithmetic T1, Arithmetic T2>
void Complex<T1, T2>::real(T1 real) noexcept(std::is_nothrow_move_assignable_v<T1>) {
    printLocation();
    m_storage->m_real = std::move(real);
}

template<Arithmetic T1, Arithmetic T2>
void Complex<T1, T2>::imaginary(T2 imaginary) noexcept(std::is_nothrow_move_assignable_v<T2>) {
    printLocation();
    m_storage->m_imaginary = std::move(imaginary);
}

template<Arithmetic T1, Arithmetic T2>
void* Complex<T1, T2>::operator new(size_t size) noexcept(false) {
    std::cout << "Overloaded Operator New To Create Instance Of Complex\n";
    NewHandlerGuard guard(myHandler);
    return ::operator new(size);
}

template<Arithmetic T1, Arithmetic T2>
void Complex<T1, T2>::operator delete(void* pMem) noexcept {
    std::cout << "Overloaded Operator delete To delete instance of class Complex\n";
    if (pMem != nullptr) {
        ::operator delete(pMem);
        pMem = nullptr;
    }
}

template<Arithmetic T1, Arithmetic T2>
std::ostream& Complex<T1, T2>::print(std::ostream& os, const std::string& prefix) const noexcept {
    using std::abs;
    char plus = (imaginary() < 0) ? '-' : '+';
    return os << prefix << std::string("Complex Number is : ") << real() << plus << abs(imaginary()) << "i\n";
}

template<Arithmetic T1, Arithmetic T2>
void Complex<T1, T2>::printLocation(const std::source_location& location, bool bDetailedLocation) noexcept {
    std::stringstream sstr;
    if (bDetailedLocation) {
        sstr << location.file_name() << ':'
             << location.line() << " = "
             << location.function_name() << '\n';
    } else {
        sstr << location.function_name() << '\n';
    }
    std::puts(sstr.str().c_str());
}

template<Arithmetic T1, Arithmetic T2>
const Complex<T1, T2>::ComplexData& Complex<T1, T2>::value() const noexcept {
    return m_storage.value();
}

template<Arithmetic T1, Arithmetic T2>
Complex<T1, T2>::ComplexData& Complex<T1, T2>::value() noexcept {
    return m_storage.value();
}

template<Arithmetic T1, Arithmetic T2>
void Complex<T1, T2>::myHandler() {
    std::cout << "Failed To Allocate Memory For Instance of Class Complex\n";
    std::set_new_handler(m_pHandler);
    throw std::bad_alloc();
}

template<Arithmetic T1, Arithmetic T2>
std::ostream& operator<<(std::ostream& os, const Complex<T1, T2>& rhs) noexcept {
    return rhs.print(os);
}

/*
template<Arithmetic T1, Arithmetic T2>
template<Arithmetic... Args>
typename Complex<T1, T2>::Data* Complex<T1, T2>::createData(Args&&... args) {
    printLocation();
    static_assert((sizeof...(args) == 2), "Complex Number Requires Exactly 2 arguments");
    return createDataImpl(std::forward<Args>(args)...);
}

template<Arithmetic T1, Arithmetic T2>
typename Complex<T1, T2>::Data* Complex<T1, T2>::createDataImpl(T1&& real, T2&& imaginary) {
    typename Complex<T1, T2>::Data* pData = nullptr;
    bool bRealConstructed{false};
    bool bImgConstructed{false};

    try {
        pData = static_cast<Complex<T1, T2>::Data*>(::operator new(sizeof(Complex<T1, T2>::Data)));
        if (pData) {
            new (&pData->m_real) T1(std::move(real));
            bRealConstructed = true;
            new (&pData->m_imaginary) T2(std::move(imaginary));
            bImgConstructed = true;
        }
    } catch(...) {
        if (pData != nullptr) {
            if (bImgConstructed) {
                pData->m_imaginary.~T2();
            }
            if (bRealConstructed) {
                pData->m_real.~T1();
            }
            ::operator delete(pData);
            pData = nullptr;
        }
        throw;
    }
    return pData;
}

template<Arithmetic T1, Arithmetic T2>
void Complex<T1, T2>::destroyData(typename Complex<T1, T2>::Data*& pData) noexcept {
    if (pData != nullptr) {
        pData->m_imaginary.~T2();
        pData->m_real.~T1();
        ::operator delete(pData);
        pData = nullptr;
    }
}
*/
