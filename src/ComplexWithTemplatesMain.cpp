#include <iostream>
#include "Complex.h"

int main()
{
    std::cout << "Hello World!" << std::endl;
    Complex<int, int> C1(1,2);
    int a = 2; double b = -3;
    Complex<int, double> C2(a, b);
    Complex<int, double> C3 = C2;
    Complex C4 = Complex(a, 3.7);
    Complex C5(std::move(Complex(4.5, 6.3)));
    std::cout << "C5 = " << C5 << '\n';
    C3 = C4;
    C4.real(27);
    C4.imaginary(3.19);
    C3.imaginary(b);
    Complex C6(27);

    return 0;
}
