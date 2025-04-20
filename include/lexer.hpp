#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace ParaCL
{
	enum class TokenType
	{
		OP_PLUS,		// +		4
		OP_MINUS,		// -		4
		OP_REM,			// %		5
		OP_MULT,		// *		5
		OP_DIV,			// /		5
		OP_LT,			// <		3
		OP_LTEQ,		// <=		3
		OP_GT,			// >		3
		OP_GTEQ,		// >=		3
		OP_EQUAL,		// ==		2
		OP_NEQUAL,		// !=		2

		OP_PLUS_SET,	// +=		1
		OP_MINUS_SET,	// -=		1
		OP_REM_SET,		// %=		1
		OP_MULT_SET,	// *=		1
		OP_DIV_SET,		// /=		1
		OP_SET,			// =		1

		UNOP_OPPNUM,	// !		6
		UNOP_INC,		// ++		6
		UNOP_DEC,		// --		6
		UNOP_MINUS,		// -		6

		KEYWORD_WHILE,
		KEYWORD_IF,
		KEYWORD_ELSE,
		KEYWORD_PRINT,
		
		OPEN_PAREN,		// (
		CLOSE_PAREN,	// )
		LSCOPE,			// { 
		RSCOPE,			// } 
		ENDCOLOM,		// ;

		VAR,
		NUMBER,
		GETNUM,
		NONE
	};

	enum class Priority
	{
		NONE,	// 0
		MIN,	// 1
		LOW,	// 2
		MED,	// 3
		AVER,	// 4	
		MAX,	// 5
		OVER	// 6
	};

	struct Token
	{
		std::string value;
		TokenType type;
		Priority priority = Priority::NONE;
	};

	// std::stoi !! 
	class Lexer
	{
	private:
		std::shared_ptr<std::vector<Token>> tokens;
		std::string code;
		size_t code_length;

	private: // helpful methods
		void strTokenize(size_t& i);
		void intTokenize(size_t& i);

	public:
		explicit Lexer(const std::string& code);
		void tokenize();

		std::shared_ptr<std::vector<Token>> gettokens();
	};
}