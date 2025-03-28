#pragma once

#include <vector>
#include <string>
#include <iostream>

enum class TokenType
{
	OP_PLUS,		// +
	OP_MINUS,		// -
	OP_MULT,		// *
	OP_DIV,			// /
	OP_LT,			// <
	OP_LTEQ,		// <=
	OP_GT,			// >
	OP_GTEQ,		// >=
	OP_EQUAL,		// == 
	OP_NEQUAL,		// !=
	OP_SET,			// =

	UOPER_NOT,		// !

	KEYWORD_WHILE,
	KEYWORD_IF,
	KEYWORD_PRINT,

	OPEN_PAREN,		// (
	CLOSE_PAREN,	// )
	LSCOPE,			// { 
	RSCOPE,			// } 
	ENDCOLOM,		// ;

	VAR,
	NUMBER
};

struct Token
{
	std::string value;
	TokenType type;
};

// std::stoi !! 
class Lexer
{
private:
	std::vector<Token> tokens;
	std::string_view code;
	size_t code_lenght;

private: // helpful methods
	void strTokenize(size_t i);
	void intTokenize(size_t i);

public:
	explicit Lexer(std::string_view code);
	void tokenize();

	const std::vector<Token>& gettokes() &;
};