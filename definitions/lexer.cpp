#include <lexer.hpp>

void Lexer::strTokenize(size_t i)
{
	std::string VAR_NAME{ code[i] };
	++i;
	while (i < code_lenght && (std::isalpha(code[i]) || std::isdigit(code[i]) || code[i] == '_'))
		VAR_NAME += code[i++];

	if (VAR_NAME == "while")
		tokens.emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_WHILE });

	else if (VAR_NAME == "if")
		tokens.emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_IF });

	else if (VAR_NAME == "PRINT")
		tokens.emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_PRINT });

	else
		tokens.emplace_back(Token{ VAR_NAME, TokenType::VAR });
}

void Lexer::intTokenize(size_t i)
{
	std::string NUMBER{ code[i] };
	++i;
	while (i < code_lenght && std::isdigit(code[i]))
		NUMBER += code[i++];

	tokens.emplace_back(Token{ NUMBER, TokenType::VAR });
}

Lexer::Lexer(std::string_view code) : code(code), code_lenght(code.length()) { }

void Lexer::tokenize()
{
	for (size_t i = 0; i < code_lenght; ++i)
	{
		if (std::isalpha(code[i]))
		{
			strTokenize(i);
		}
		else if (std::isdigit(code[i]))
		{
			intTokenize(i);
		}
		else
		{
			switch (code[i])
			{
			case '+':
				tokens.emplace_back(Token{ "+", TokenType::OP_PLUS});
				break;

			case '-':
				tokens.emplace_back(Token{ "-", TokenType::OP_MINUS });
				break;
				
			case '*':
				tokens.emplace_back(Token{ "*", TokenType::OP_MULT });
				break;

			case '/':
				tokens.emplace_back(Token{ "/", TokenType::OP_DIV });
				break;

			case '<':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens.emplace_back(Token{ "<=", TokenType::OP_LTEQ });
					++i;
				}
				else
					tokens.emplace_back(Token{ "<", TokenType::OP_LT });

				break;
				
			case '>':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens.emplace_back(Token{ ">=", TokenType::OP_GTEQ });
					++i;
				}
				else
					tokens.emplace_back(Token{ ">", TokenType::OP_GT });

				break;

			case '=':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens.emplace_back(Token{ "==", TokenType::OP_EQUAL });
					++i;
				}
				else
					tokens.emplace_back(Token{ "=", TokenType::OP_SET });

				break;

			case '!':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens.emplace_back(Token{ "!=", TokenType::OP_NEQUAL });
					++i;
				}
				else
					tokens.emplace_back(Token{ "!", TokenType::UOPER_NOT });

				break;

			case '?':
				int tvalue;
				std::cin >> tvalue;
				if (!std::cin.good())
					throw std::invalid_argument("invalid input");

				tokens.emplace_back(Token{ std::to_string(tvalue), TokenType::NUMBER });
				break;
				
			case '(':
				tokens.emplace_back(Token{ "(", TokenType::OPEN_PAREN });
				break;

			case ')':
				tokens.emplace_back(Token{ ")", TokenType::CLOSE_PAREN });
				break;
				
			case '{':
				tokens.emplace_back(Token{ "{", TokenType::LSCOPE });
				break;

			case '}':
				tokens.emplace_back(Token{ "}", TokenType::RSCOPE });
				break;

			case ';':
				tokens.emplace_back(Token{ ";", TokenType::ENDCOLOM });
				break;

			default:
				throw std::invalid_argument("invalid type");
			}
		}
	}
}


const std::vector<Token>& Lexer::gettokes() &
{
	return tokens;
}