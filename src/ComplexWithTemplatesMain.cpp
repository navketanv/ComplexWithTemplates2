#include <iostream>
#include "Complex.h"

int main()
{
    std::cout << "Hello World!" << std::endl;
    Complex<double, double> C1(1,2);
    float a = 2; double b = -3;
    Complex<float, double> C2(a, b);
    Complex<float, double> C3 = C2;
    Complex C4 = Complex(a, 3.7);
    Complex C5(std::move(Complex(4.5, 6.3)));
    std::cout << "C5 = " << C5 << '\n';
    C3 = C4;
    C4.real(27);
    C4.imaginary(3.19);
    C3.imaginary(b);
    Complex C6(39.f);
    Complex C7(b);
    Complex C9(2, 25.f);
    Complex C8 = C2 * C4;

    return 0;
}
