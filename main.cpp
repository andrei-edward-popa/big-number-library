#include <iostream>

#include <BigInteger.hpp>

int main() {
	try {
		BigInteger num1("-59832563298473298659832743284483294732984733");
		BigInteger num2("59832563298473298659832743284483294732984732");
		BigInteger num3("57564636357843758437584375843");
		BigInteger num4("5756463635784375843758437584357564636357843758437584375843575646363578437584375843758435756463635784375843758437584357564636357843758437584375843"
						"5756463635784375843758437584357564636357843758437584375843575646363578437584375843758435756463635784375843758437584357564636357843758437584375843"
						"5756463635784375843758437584357564636357843758437584375843575646363578437584375843758435756463635784375843758437584357564636357843758437584375843");
		std::cout << "num1         = " << num1 << '\n';
		std::cout << "num2         = " << num2 << '\n';
		std::cout << "num3         = " << num3 << '\n';
		std::cout << "num4         = " << num4 << '\n';
		std::cout << "num1 + num2  = " << num1 + num2 << '\n';
		std::cout << "num2 - num1  = " << num2 - num1 << '\n';
		std::cout << "num1 * num2  = " << num1 * num2 << '\n';
		std::cout << "num2 / num3  = " << (num2 / num3) << '\n';
		std::cout << "num2 % num3  = " << (num2 % num3) << '\n';
		std::cout << "num2 & num3  = " << (num2 & num3) << '\n';
		std::cout << "num2 | num3  = " << (num2 | num3) << '\n';
		std::cout << "num2 ^ num3  = " << (num2 ^ num3) << '\n';
		std::cout << "num3 >> 10   = " << (num3 >> 10) << '\n';
		std::cout << "num3 << 2    = " << (num3 << 2) << '\n';
		std::cout << std::boolalpha;
		std::cout << "num1 == num2 ? " << (num1 == num2) << '\n';
		std::cout << "num1 != num2 ? " << (num1 != num2) << '\n';
		std::cout << "num1 < num2  ? " << (num1 < num2) << '\n';
		std::cout << "num1 > num2  ? " << (num1 > num2) << '\n';
		std::cout << "num1 <= num2 ? " << (num1 <= num2) << '\n';
		std::cout << "num1 >= num2 ? " << (num1 >= num2) << '\n';
		std::cout << "~num3        = " << (~num3) << '\n';
		std::cout << "Bin num3     = "; num3.print_binary();
		std::cout << "Bin -num3    = "; (-num3).print_binary();
		std::cout << "num4 / num2  = " << (num4 / num2) << '\n';
	} catch (NumberFormatException& ex) {
		std::cout << ex.what() << '\n';
	}
}

