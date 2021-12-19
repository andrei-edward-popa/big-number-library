#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include <Traits.hpp>

class BigNumber {
public:
	using unit_type = uint64_t;
private:

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	union {
		struct {
			uint8_t is_negative : 1 = 0;
			[[maybe_unused]] uint8_t reserved : 7;
		};
        [[maybe_unused]] uint8_t value;
	} state_;
	#pragma GCC diagnostic pop
	
	std::vector<unit_type> integer_storage_;
    [[maybe_unused]] std::vector<unit_type> fractional_storage_;
    std::vector<unit_type> binary_storage_;

protected:

    [[maybe_unused]] static constexpr unit_type digits_to_store = static_cast<unit_type>(([]<typename T>(){
		if constexpr (std::is_same_v<T, uint64_t>) {
			return 18;
		}

		if constexpr (std::is_same_v<T, uint32_t>) {
			return 9;
		}

		throw std::runtime_error("This unit type is not supported!");
	}).operator()<unit_type>());

    [[maybe_unused]] static constexpr unit_type overflow_unit_of_storage = static_cast<unit_type>(([]<typename T>(){
		if constexpr (std::is_same_v<T, uint64_t>) {
			return 1e18;
		}

		if constexpr (std::is_same_v<T, uint32_t>) {
			return 1e9;
		}

		throw std::runtime_error("This unit type is not supported!");
	}).operator()<unit_type>());

    [[maybe_unused]] static constexpr unit_type half_overflow_unit_of_storage = overflow_unit_of_storage / 2;
	[[maybe_unused]] static constexpr unit_type max_unit_of_storage = overflow_unit_of_storage - 1;
	
	constexpr explicit BigNumber(const std::string&) {
		/* Nothing yet */
	};

	constexpr BigNumber() = default;
	constexpr BigNumber(const BigNumber&) = default;
	constexpr BigNumber(BigNumber&&) = default;
	constexpr BigNumber& operator=(const BigNumber&) = default;
	constexpr BigNumber& operator=(BigNumber&&) = default;

	void parse(const std::string& number) noexcept {
		uint64_t len = number.size();
        uint64_t counter = 0, index;
        uint64_t start = len % digits_to_store;
		state_.is_negative = number.front() == '-';
		if (start - state_.is_negative != 0) {
			this->integer_storage().push_back(std::stoull(number.substr(state_.is_negative, start - state_.is_negative)));
		}
		while (counter < len / static_cast<int64_t>(digits_to_store)) {
			index = start + counter * digits_to_store;
			this->integer_storage().push_back(std::stoull(number.substr(index, digits_to_store)));
			counter += 1;
		}
	}

	constexpr std::vector<unit_type>& integer_storage() {
		return integer_storage_;
	}
	
	[[nodiscard]] constexpr const std::vector<unit_type>& integer_storage() const {
		return integer_storage_;
	}
	
	constexpr std::vector<unit_type>& binary_storage() {
		return binary_storage_;
	}
	
	[[nodiscard]] constexpr const std::vector<unit_type>& binary_storage() const {
		return binary_storage_;
	}
	
	constexpr auto& state() {
		return state_;
	}

    [[nodiscard]] constexpr const auto& state() const {
		return state_;
	}
	
	virtual void check_number(const std::string& number) const = 0;
	
public:

	friend std::ostream& operator<<(std::ostream& os, const BigNumber& number) {
		if (number.state_.is_negative) {
			std::cout << '-';
		}

		for (auto unit : number.integer_storage()) {
			os << unit;
			os << std::setfill('0') << std::setw(digits_to_store);
		}

		os << std::setfill(' ') << std::setw(0);

		return os;
	}
	
	virtual ~BigNumber() = default;
};

