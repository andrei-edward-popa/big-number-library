#include <gtest/gtest.h>

#include <BigInteger.hpp>

TEST(Add, BigInteger) {
	BigInteger num1("-59832563298473298659832743284483294732984733");
	BigInteger num2("59832563298473298659832743284483294732984732");
	BigInteger result = num1 + num2;
	BigInteger expected_result("-1");
	ASSERT_EQ(result, expected_result);
}

