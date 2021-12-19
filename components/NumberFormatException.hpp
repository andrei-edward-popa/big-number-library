#pragma once

#include <string>
#include <exception>

class NumberFormatException : std::exception {
private:
	std::string message_;
public:
	NumberFormatException() = default;

	explicit NumberFormatException(const std::string& value) {
        message_ = std::string("Value ") + value + std::string(" is not a valid number.");
	}

	[[nodiscard]] const char* what() const noexcept override {
		return message_.c_str();
	}
};

