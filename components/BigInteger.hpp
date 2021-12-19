#pragma once

#include <regex>
#include <string>
#include <limits>
#include <climits>
#include <compare>
#include <algorithm>
#include <type_traits>

#include <NumberFormatException.hpp>
#include <ArithmeticException.hpp>
#include <BigNumber.hpp>
#include <Traits.hpp>

class BigInteger : public BigNumber {

#define DECLARE_ASSIGNMENT_OPERATOR(op)									\
	constexpr BigInteger& operator op##=(const BigInteger& other) {		\
		(*this) = (*this) op other;										\
		return *this;													\
	}
	
    void check_number(const std::string& number) const override {
		std::regex integer_regex("-?\\d+");
		if (!std::regex_match(number, integer_regex)) {
			throw NumberFormatException(number);
		}
	}
	
	static constexpr BigInteger divide_by_2(const BigInteger& number) {
		BigInteger result = number;
		uint8_t prev_carry = 0;
		uint8_t next_carry;
		
		for (auto & unit : result.integer_storage()) {
            next_carry = unit % 2;
            unit = (unit / 2) + (prev_carry * half_overflow_unit_of_storage);
			prev_carry = next_carry;
		}
		
		if (result.integer_storage().size() >= 2 && result.integer_storage().front() == 0) {
			result.integer_storage().erase(result.integer_storage().begin());
		}
		
		return result;
	}
	
	constexpr void create_binary_from_decimal() {
		this->binary_storage().clear();
		this->binary_storage() = BigInteger::dec2bin(*this);
	}
	
	constexpr void create_decimal_from_binary() {
		this->integer_storage().clear();
		*this = BigInteger::bin2dec(this->binary_storage());
	}


public:

	[[maybe_unused]] static constexpr BigInteger ZERO() {
        return 0;
    }

    [[maybe_unused]] static constexpr BigInteger ONE() {
        return 1;
    }

    [[maybe_unused]] static constexpr BigInteger TEN() {
        return 10;
    }

	explicit BigInteger(const std::string& number) : BigNumber(number) {
		this->check_number(number);
		this->parse(number);
	}
	
	constexpr BigInteger() {
        this->integer_storage().push_back(0);
        this->binary_storage().push_back(0);
    };
	constexpr BigInteger(const BigInteger&) = default;
	constexpr BigInteger(BigInteger&&) = default;
	constexpr BigInteger& operator=(const BigInteger&) = default;
	constexpr BigInteger& operator=(BigInteger&&) = default;
	
	template<Integer T>
	constexpr BigInteger(T value) : BigNumber() {
		if (value < 0) {
			this->state().is_negative = 1;
		}
		this->integer_storage().push_back(static_cast<unit_type>(std::abs(static_cast<int64_t>(value))));
		this->binary_storage().push_back(value);
	}
	
	[[nodiscard]] static constexpr BigInteger abs(const BigInteger& number) {
		BigInteger abs_number = number;
		abs_number.state().is_negative = 0;
		return abs_number;
	} 

	constexpr BigInteger operator+(const BigInteger& other) const {
		BigInteger result;
		unit_type num1, num2, res;
		uint64_t index1, index2, res_index;
		
		bool is_one_negative = (this->state().is_negative && !other.state().is_negative) ||
							   (!this->state().is_negative && other.state().is_negative);
		result.state().is_negative = (this->state().is_negative && BigInteger::abs(*this) > BigInteger::abs(other)) ||
									 (other.state().is_negative && BigInteger::abs(*this) < BigInteger::abs(other)) ||
									 (this->state().is_negative && other.state().is_negative);
				
		index1 = this->integer_storage().size();
		index2 = other.integer_storage().size();
		res_index = std::max(this->integer_storage().size(), other.integer_storage().size()) + 1;
		result.integer_storage() = std::vector<unit_type>(res_index);
		
		do {
            index1 -= 1;
            index2 -= 1;
            res_index -= 1;
			if (is_one_negative) {
				num1 = BigInteger::abs(other) >= BigInteger::abs(*this) ? other.integer_storage().at(index2) : this->integer_storage().at(index1);
				num2 = BigInteger::abs(other) >= BigInteger::abs(*this) ? this->integer_storage().at(index1) : other.integer_storage().at(index2);
				res = num1 < num2 ? overflow_unit_of_storage + num1 - num2 : num1 - num2;
				result.integer_storage().at(res_index) = res - result.integer_storage().at(res_index);
				result.integer_storage().at(res_index - 1) += (num1 < num2 ? 1 : 0);
			} else {
				num1 = this->integer_storage().at(index1);
				num2 = other.integer_storage().at(index2);
				res = num1 + num2;
				result.integer_storage().at(res_index) += res % overflow_unit_of_storage;
				result.integer_storage().at(res_index - 1) += res / overflow_unit_of_storage;
			}
		} while (index1 != 0 && index2 != 0);
		
		auto add_remaining_digits = [is_one_negative, &result, &res_index](uint64_t index, const std::vector<unit_type>& vec) {
		    if (index == 0) return;
			do {
                index -= 1;
                res_index -= 1;
                if (is_one_negative) {
                    result.integer_storage().at(res_index) = vec[index] - result.integer_storage().at(res_index);
                } else {
                    result.integer_storage().at(res_index) += vec[index];
                }
			} while (index != 0);
		};
		
		add_remaining_digits(index1, this->integer_storage());
		add_remaining_digits(index2, other.integer_storage());

		while (result.integer_storage().front() == 0 && result.integer_storage().size() > 1) {
			result.integer_storage().erase(result.integer_storage().begin());
		}
		
		if (result.integer_storage().front() == 0 && result.integer_storage().size() == 1) {
			result.state().is_negative = 0;
		}
		
		return result;
	}

	constexpr BigInteger operator-() const {
		BigInteger result = *this;
		result.state().is_negative = !result.state().is_negative;
		return result;
	}
	
	constexpr bool operator!() const {
		return (*this) == 0;
	}
	
	constexpr BigInteger operator-(const BigInteger& other) const {
		return (*this) + (-other);
	}

	constexpr BigInteger operator*(const BigInteger& other) const {
		using next_type = next_integer_type_t<unit_type>;

		std::vector<std::vector<next_type>> parts(other.integer_storage().size(),
													std::vector<next_type>(this->integer_storage().size()));
		next_type num1, num2, res, carry;
		unit_type res_low, res_high;
		for (uint64_t ii = other.integer_storage().size(); ii > 0; ii--) {
            uint64_t i = ii - 1;
			num1 = static_cast<next_type>(other.integer_storage().at(i));
			carry = 0;
			for (uint64_t jj = this->integer_storage().size(); jj > 0; jj--) {
                uint64_t j = jj - 1;
				num2 = static_cast<next_type>(this->integer_storage().at(j));
				res = num1 * num2 + carry;
				carry = res / overflow_unit_of_storage;
				if (j != 0)
					res %= overflow_unit_of_storage;
				parts.at(i).at(j) = res;
			}
		}
		
		std::vector<BigInteger> partialSums(other.integer_storage().size());
		BigInteger finalSum;
		
		for (uint64_t ii = other.integer_storage().size(); ii > 0; ii--) {
            uint64_t i = ii - 1;
			for (uint64_t jj = 0; jj < this->integer_storage().size(); jj++) {
                uint64_t j = jj;
				num1 = parts.at(i).at(j);
				res_high = static_cast<unit_type>(num1 / overflow_unit_of_storage);
				res_low = static_cast<unit_type>(num1 % overflow_unit_of_storage);
				if (res_high != 0) {
					partialSums.at(i).integer_storage().push_back(res_high);
				}
				partialSums.at(i).integer_storage().push_back(res_low);
			}
		}
		
		for (uint64_t i = 0; i < parts.size(); i++) {
			for (uint64_t j = 1; j < parts.size() - i; j++) {
					partialSums.at(i).integer_storage().push_back(0);
			}
			finalSum += partialSums.at(i);
		}
		
		finalSum.state().is_negative = this->state().is_negative + other.state().is_negative;
		
		return finalSum;
	}

	constexpr BigInteger operator/(const BigInteger& other) const {
		if (other == 0) {
			throw ArithmeticException("Division by 0 is not allowed.");
		}
		
		BigInteger remainder = BigInteger::abs(*this);
		BigInteger abs_other = BigInteger::abs(other);
		BigInteger quotient = 0;
		BigInteger pwr_two;
	    BigInteger double_divisor;
	    BigInteger double_divisor_storage;

		while (remainder >= abs_other) {
		    pwr_two = 1;
		    double_divisor = abs_other;

		    while (double_divisor_storage = double_divisor + double_divisor, double_divisor_storage < remainder) {
		        double_divisor = double_divisor_storage;
		        pwr_two += pwr_two;
		    }

		    quotient += pwr_two;
		    remainder -= double_divisor;
		}
		
		quotient.state().is_negative = this->state().is_negative + other.state().is_negative;

		return quotient;
	}

	constexpr BigInteger operator%(const BigInteger& other) const {
		BigInteger quotient = (*this) / other;
		return (*this) - quotient * other;
	}
	
	constexpr BigInteger operator<<(const BigInteger& other) const {
		return (*this) * pow(2, other);
	}
	
	constexpr BigInteger operator>>(const BigInteger& other) const {
		return (*this) / pow(2, other);
	}
	
	constexpr BigInteger operator&(const BigInteger& other) const {
		auto this_bin = BigInteger::dec2bin(*this);
		auto other_bin = BigInteger::dec2bin(other);

		uint64_t index1, index2, res_index;
		
		index1 = this_bin.size();
		index2 = other_bin.size();
		res_index = std::min(this_bin.size(), other_bin.size());
		auto result = std::vector<unit_type>(res_index);

		do {
			index1 -= 1;
			index2 -= 1;
			res_index -= 1;
            result[res_index] = this_bin[index1] & other_bin[index2];
		} while (index1 != 0 && index2 != 0);
		
		return BigInteger::bin2dec(result);
	}
	
	constexpr BigInteger operator|(const BigInteger& other) const {
		auto this_bin = BigInteger::dec2bin(*this);
		auto other_bin = BigInteger::dec2bin(other);

		uint64_t index1, index2, res_index;
		
		index1 = this_bin.size();
		index2 = other_bin.size();
		res_index = std::max(this_bin.size(), other_bin.size());
		auto result = std::vector<unit_type>(res_index);

        do {
            index1 -= 1;
            index2 -= 1;
            res_index -= 1;
            result[res_index] = this_bin[index1] | other_bin[index2];
        } while (index1 != 0 && index2 != 0);
		
		auto add_remaining_digits = [&result, &res_index](uint64_t index, const std::vector<unit_type>& vec) {
		    if (index == 0) return;
			do {
				index -= 1;
				res_index -= 1;
                result.at(res_index) = vec[index];
			} while (index != 0);
		};

		add_remaining_digits(index1, this_bin);
		add_remaining_digits(index2, other_bin);
		
		return BigInteger::bin2dec(result);
	}
	
	constexpr BigInteger operator^(const BigInteger& other) const {
        auto this_bin = BigInteger::dec2bin(*this);
        auto other_bin = BigInteger::dec2bin(other);

        uint64_t index1, index2, res_index;

        index1 = this_bin.size();
        index2 = other_bin.size();
        res_index = std::max(this_bin.size(), other_bin.size());
        auto result = std::vector<unit_type>(res_index);

        do {
            index1 -= 1;
            index2 -= 1;
            res_index -= 1;
            result[res_index] = this_bin[index1] ^ other_bin[index2];
        } while (index1 != 0 && index2 != 0);

        auto add_remaining_digits = [&result, &res_index](uint64_t index, const std::vector<unit_type>& vec) {
            if (index == 0) return;
            do {
                index -= 1;
                res_index -= 1;
                result.at(res_index) = vec[index];
            } while (index != 0);
        };

        add_remaining_digits(index1, this_bin);
        add_remaining_digits(index2, other_bin);

        return BigInteger::bin2dec(result);
	}
	
	constexpr BigInteger operator~() const {
		auto this_bin = BigInteger::dec2bin(*this);

		uint64_t index;
		
		index = this_bin.size();
		auto result = std::vector<unit_type>(index);

		do {
            index -= 1;
			result[index] = ~this_bin[index];
		} while (index != 0);
		
		return BigInteger::bin2dec(result);
	}

	constexpr BigInteger& operator++() {
		(*this) += 1;
		return *this;
	}


	constexpr BigInteger& operator--() {
		(*this) -= 1;
		return *this;
	}

	constexpr BigInteger operator++(int32_t) {
		BigInteger copy = *this;
		(*this) += 1;
		return copy;
	}

	constexpr BigInteger operator--(int32_t) {
		BigInteger copy = *this;
		(*this) -= 1;
		return copy;
	}
	
	constexpr bool operator==(const BigInteger& other) const {
		if (this->integer_storage().size() != other.integer_storage().size()) {
			return false;
		}
		
		if (this->state().is_negative == !other.state().is_negative) {
			return false;
		}
		
		for (uint64_t i = 0; i < this->integer_storage().size(); i++) {
			if (this->integer_storage().at(i) != other.integer_storage().at(i)) {
				return false;
			}
		}
		
		return true;
	}

	constexpr std::strong_ordering operator<=>(const BigInteger& other) const {
		if (this->state().is_negative == 1 && other.state().is_negative == 0) {
			return std::strong_ordering::less;
		}
		
		if (this->state().is_negative == 0 && other.state().is_negative == 1) {
			return std::strong_ordering::greater;
		}
	
		uint64_t counter = 0;
		unit_type first, second;
		auto so1 = this->state().is_negative && other.state().is_negative ? std::strong_ordering::greater :
																			std::strong_ordering::less;
		auto so2 = this->state().is_negative && other.state().is_negative ? std::strong_ordering::less :
																			std::strong_ordering::greater;
		
		if (this->integer_storage().size() == other.integer_storage().size()) {
			do {
				first = *std::next(this->integer_storage().begin(), static_cast<int64_t>(counter));
				second = *std::next(other.integer_storage().begin(), static_cast<int64_t>(counter));
				counter += 1;
			} while (counter != this->integer_storage().size() && first == second);
			
			return counter == this->integer_storage().size() && first == second ? std::strong_ordering::equal :
				   first < second ? so1 : so2;
		}
		
		return this->integer_storage().size() == other.integer_storage().size() ? std::strong_ordering::equal :
				   this->integer_storage().size() < other.integer_storage().size() ? so1 : so2;
	}
	
	static constexpr BigInteger pow(const BigInteger& base, const BigInteger& exponent) {
		if (exponent == 0) {
		    return 1;
		}
		
		if (exponent.integer_storage().back() & 1U) {
		    return base * pow(base, exponent / 2) * pow(base, exponent / 2);
		}
		
		return pow(base, exponent / 2) * pow(base, exponent / 2);
	}
	
	static constexpr std::vector<unit_type> dec2bin(const BigInteger& number) {
		std::vector<unit_type> result;
		BigInteger copy = BigInteger::abs(number);
		unit_type binary = 0;
		uint64_t index = 0;
		
		while (copy != 0) {
			binary |= (copy.integer_storage().back() % 2) << index;
			copy = BigInteger::divide_by_2(copy);
			index += 1;
			
			if (copy == 0 || index == CHAR_BIT * sizeof(unit_type)) {
				result.push_back(binary);
				binary = 0;
				index = 0;
			}
		}
		
		std::reverse(result.begin(), result.end());
		
		if (number.state().is_negative) {
			for (auto & unit : result) {
				unit = ~unit;
			}

			result.back() += 1;
		}

		return result;
	}

	void print_binary() const {
		auto bits = BigInteger::dec2bin(*this);
		for (auto unit : bits) {
			std::bitset<CHAR_BIT * sizeof(unit_type)> binary_unit(unit);
			std::cout << binary_unit;
		}
		std::cout << '\n';
	}
	
	static constexpr BigInteger bin2dec(const std::vector<unit_type>& binary) {
		BigInteger result = 0;
		uint64_t bit_index = CHAR_BIT * sizeof(unit_type) * binary.size();
		uint64_t unit_index = CHAR_BIT * sizeof(unit_type);
		uint64_t index = 0;
		
		do {
            unit_index -= 1;
            bit_index -= 1;

			if ((binary.at(index) >> unit_index) & 1U) {
				result += BigInteger::pow(2, bit_index);
			}
			
			if (unit_index == 0) {
				unit_index = CHAR_BIT * sizeof(unit_type);
				index += 1;
			}
		} while (bit_index != 0);
		
		return result;
	}

	DECLARE_ASSIGNMENT_OPERATOR(+)
	DECLARE_ASSIGNMENT_OPERATOR(-)
	DECLARE_ASSIGNMENT_OPERATOR(*)
	DECLARE_ASSIGNMENT_OPERATOR(/)
	DECLARE_ASSIGNMENT_OPERATOR(%)
	DECLARE_ASSIGNMENT_OPERATOR(>>)
	DECLARE_ASSIGNMENT_OPERATOR(<<)
	DECLARE_ASSIGNMENT_OPERATOR(&)
	DECLARE_ASSIGNMENT_OPERATOR(|)
	DECLARE_ASSIGNMENT_OPERATOR(^)
};

