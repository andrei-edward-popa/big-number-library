#pragma once

#include <string>
#include <exception>

class ArithmeticException : std::exception {
private:
	std::string message_;
public:
	ArithmeticException() = default;

	explicit ArithmeticException(const std::string& message) {
        message_ = message;
	}

	[[nodiscard]] const char* what() const noexcept override {
		return message_.c_str();
	}
};

