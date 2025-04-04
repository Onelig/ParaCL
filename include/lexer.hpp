#pragma once

#include <vector>
#include <string>
#include <iostream>

enum class TokenType
{
	OP_PLUS,		// +	4
	OP_MINUS,		// -	4
	OP_MULT,		// *	5
	OP_DIV,			// /	5
	OP_LT,			// <	3
	OP_LTEQ,		// <=	3
	OP_GT,			// >	3
	OP_GTEQ,		// >=	3
	OP_EQUAL,		// ==	2
	OP_NEQUAL,		// !=	2

	OP_SET,			// =	1

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

enum class Priority
{
	NONE,	// 0
	MIN,	// 1
	LOW,	// 2
	MED,	// 3
	AVER,	// 4	
	MAX,	// 5
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
	std::vector<Token> tokens;
	std::string code;
	size_t code_lenght;

private: // helpful methods
	void strTokenize(size_t& i);
	void intTokenize(size_t& i);

public:
	explicit Lexer(std::string_view code);
	void tokenize();

	const std::vector<Token>& gettokens() &;
};