#pragma once

#include <exception>
#include <string>

namespace Renderer
{
	class InvalidOperationException : public std::exception
	{
		public:
			std::string message;
			InvalidOperationException(std::string _msg) : message(_msg) {};

			const char* what() const noexcept override { return message.c_str(); }
	};

	class OutOfRangeException : public std::exception
	{
		public:
			std::string message;
			OutOfRangeException(std::string _msg) : message(_msg) {};

			const char* what() const noexcept override { return message.c_str(); }
	};
}
