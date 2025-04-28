#pragma once
#include <exception>
#include <string>
#include <unordered_map>

namespace ParaCL
{
	namespace Errors
	{
		class Syntax : public std::exception
		{
		public:
			explicit Syntax(const std::string& message, unsigned short line);
			char const* what() const override;

		private:
			std::string message;
		};
		
		class Type : public std::exception
		{
		public:
			explicit Type(const std::string& message, unsigned short line);
			char const* what() const override;

		private:
			std::string message;
		};

		class Runtime : public std::exception
		{
		public:
			explicit Runtime(const std::string& message, unsigned short line);
			char const* what() const override;

		private:
			std::string message;
		};

	}
}