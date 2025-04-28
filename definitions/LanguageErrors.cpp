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
			this->message += "Detals: " + message + '.';
		}

		char const* Syntax::what() const
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
			this->message += "Detals: " + message + '.';
		}

		char const* Type::what() const
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
			this->message += "Detals: " + message + '.';
		}

		char const* Runtime::what() const
		{
			return message.c_str();
		}
	}
}