#include <lexer.hpp>

void Lexer::strTokenize(size_t& i)
{
	std::string VAR_NAME{ code[i++] };
	while (i < code_lenght && (std::isalpha(code[i]) || std::isdigit(code[i]) || code[i] == '_'))
		VAR_NAME += code[i++];

	--i;

	if (VAR_NAME == "while")
		tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_WHILE });

	else if (VAR_NAME == "if")
		tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_IF });

	else if (VAR_NAME == "print")
		tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_PRINT });

	else
		tokens->emplace_back(Token{ VAR_NAME, TokenType::VAR });
}

void Lexer::intTokenize(size_t& i)
{
	std::string NUMBER{ code[i++] };
	while (i < code_lenght && std::isdigit(code[i]))
		NUMBER += code[i++];

	--i;

	tokens->emplace_back(Token{ NUMBER, TokenType::NUMBER });
}

Lexer::Lexer(const std::string& code) 
	: code(code), code_lenght(code.length()), tokens(std::make_shared<std::vector<Token>>()) 
{ }

void Lexer::tokenize()
{
	for (size_t i = 0; i < code_lenght; ++i)
	{
		if (code[i] == '/' && i + 1 < code_lenght && code[i + 1] == '/')
		{
			while (i < code_lenght && code[i] != '\n')
			{
				++i;
				continue;
			}
		}

		else if (code[i] == ' ' || code[i] == '\t' || code[i] == '\n')
			continue;

		else if (std::isalpha(code[i]))
			strTokenize(i);

		else if (std::isdigit(code[i]))
			intTokenize(i);

		else
		{
			switch (code[i])
			{
			case '+':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "+=", TokenType::OP_PLUS_SET, Priority::MIN });
					++i;
				}
				else if (i + 1 < code_lenght && code[i + 1] == '+')
				{
					tokens->emplace_back(Token{ "++", TokenType::UNOP_INC, Priority::OVER });
					++i;
				}
				else
					tokens->emplace_back(Token{ "+", TokenType::OP_PLUS, Priority::AVER });
				break;

			case '-':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "-=", TokenType::OP_MINUS_SET, Priority::MAX });
					++i;
				}
				else if (i + 1 < code_lenght && code[i + 1] == '-')
				{
					tokens->emplace_back(Token{ "--", TokenType::UNOP_DEC, Priority::OVER });
					++i;
				}
				else
					tokens->emplace_back(Token{ "-", TokenType::OP_MINUS, Priority::AVER });
				break;
				
			case '*':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "*=", TokenType::OP_MULT_SET, Priority::MAX });
					++i;
				}
				else
					tokens->emplace_back(Token{ "*", TokenType::OP_MULT, Priority::MAX });
				break;

			case '/':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "/=", TokenType::OP_DIV_SET, Priority::MAX });
					++i;
				}
				else
					tokens->emplace_back(Token{ "/", TokenType::OP_DIV, Priority::MAX });
				break;

			case '%':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "%=", TokenType::OP_REM_SET, Priority::MAX });
					++i;
				}
				else
					tokens->emplace_back(Token{ "%", TokenType::OP_REM, Priority::MAX });
				break;

			case '<':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "<=", TokenType::OP_LTEQ, Priority::MED });
					++i;
				}
				else
					tokens->emplace_back(Token{ "<", TokenType::OP_LT, Priority::MED });

				break;
				
			case '>':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ ">=", TokenType::OP_GTEQ, Priority::MED });
					++i;
				}
				else
					tokens->emplace_back(Token{ ">", TokenType::OP_GT, Priority::MED });

				break;

			case '=':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "==", TokenType::OP_EQUAL, Priority::LOW });
					++i;
				}
				else
					tokens->emplace_back(Token{ "=", TokenType::OP_SET, Priority::MIN });

				break;

			case '!':
				if (i + 1 < code_lenght && code[i + 1] == '=')
				{
					tokens->emplace_back(Token{ "!=", TokenType::OP_NEQUAL, Priority::LOW });
					++i;
				}
				else
				{
					tokens->emplace_back(Token{ "!=", TokenType::UNOP_OPPNUM, Priority::OVER });
				}
				break;

			case '?':
				tokens->emplace_back(Token{ "? ", TokenType::GETNUM });
				break;
				
			case '(':
				tokens->emplace_back(Token{ "(", TokenType::OPEN_PAREN });
				break;

			case ')':
				tokens->emplace_back(Token{ ")", TokenType::CLOSE_PAREN });
				break;
				
			case '{':
				tokens->emplace_back(Token{ "{", TokenType::LSCOPE });
				break;

			case '}':
				tokens->emplace_back(Token{ "}", TokenType::RSCOPE });
				break;


			case ';':
				tokens->emplace_back(Token{ ";", TokenType::ENDCOLOM });
				break;

			default:
				throw std::invalid_argument("invalid type");
			}
		}
	}
}

std::shared_ptr<std::vector<Token>> Lexer::gettokens()
{
	return tokens;
}