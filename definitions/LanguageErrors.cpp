#include <LanguageErrors.hpp>

namespace ParaCL
{
	namespace Errors
	{
		//
		// --- Syntax errors ---
		//
		Syntax::Syntax(const std::string& message, unsigned short line)
			: message("ParaCL error | Syntax error | Line ")
		{
			this->message += std::to_string(line) + "\n";
			this->message += "Detals: " + message + '.' + "\n";
		}

		char const* Syntax::what() const noexcept
		{
			return message.c_str();
		}
		  

		//
		// --- Type errors ---
		//
		Type::Type(const std::string& message, unsigned short line)
			: message("ParaCL error | Type error | Line ")
		{
			this->message += std::to_string(line) + "\n";
			this->message += "Detals: " + message + '.' + "\n";
		}

		char const* Type::what() const noexcept
		{
			return message.c_str();
		}


		//
		// --- Runtime errors ---
		//
		Runtime::Runtime(const std::string& message, unsigned short line)
			: message("ParaCL error | Runtime error | Line ")
		{
			this->message += std::to_string(line) + "\n";
			this->message += "Detals: " + message + '.' + "\n";
		}

		char const* Runtime::what() const noexcept
		{
			return message.c_str();
		}
	}
}