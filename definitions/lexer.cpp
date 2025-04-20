#include <lexer.hpp>
namespace ParaCL
{
	void Lexer::strTokenize(size_t& i)
	{
		if (!tokens->empty() && tokens->back().type == TokenType::NUMBER)
		{
			throw std::logic_error("before operator= should be VAR\n");
		}

		std::string VAR_NAME{ code[i++] };
		while (i < code_length && (std::isalpha(code[i]) || std::isdigit(code[i]) || code[i] == '_'))
			VAR_NAME += code[i++];

		--i;

		if (VAR_NAME == "while")
			tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_WHILE });

		else if (VAR_NAME == "if")
			tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_IF });

		else if (VAR_NAME == "else")
			tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_ELSE });

		else if (VAR_NAME == "print")
			tokens->emplace_back(Token{ VAR_NAME, TokenType::KEYWORD_PRINT });
		else
			tokens->emplace_back(Token{ VAR_NAME, TokenType::VAR });
	}

	void Lexer::intTokenize(size_t& i)
	{
		std::string NUMBER{ code[i++] };
		while (i < code_length && std::isdigit(code[i]))
			NUMBER += code[i++];

		--i;

		tokens->emplace_back(Token{ NUMBER, TokenType::NUMBER });
	}

	Lexer::Lexer(const std::string& code)
		: code(code), code_length(code.length()), tokens(std::make_shared<std::vector<Token>>())
	{
	}

	void Lexer::tokenize()
	{
		for (size_t i = 0; i < code_length; ++i)
		{
			if (code[i] == ' ' || code[i] == '\t' || code[i] == '\n')
				continue;

			else if (std::isalpha(code[i]))
				strTokenize(i);

			else if (std::isdigit(code[i]))
				intTokenize(i);

			else
			{
				bool next_i_ex = i + 1 < code_length;
				switch (code[i])
				{
				case '+':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "+=", TokenType::OP_PLUS_SET, Priority::MIN });
						++i;
					}
					else if (next_i_ex && code[i + 1] == '+')
					{
						tokens->emplace_back(Token{ "++", TokenType::UNOP_INC, Priority::OVER });
						++i;
					}
					else
						tokens->emplace_back(Token{ "+", TokenType::OP_PLUS, Priority::AVER });
					break;

				case '-':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "-=", TokenType::OP_MINUS_SET, Priority::MAX });
						++i;
					}
					else if (next_i_ex && code[i + 1] == '-')
					{
						tokens->emplace_back(Token{ "--", TokenType::UNOP_DEC, Priority::OVER });
						++i;
					}
					else
						tokens->emplace_back(Token{ "-", TokenType::OP_MINUS, Priority::AVER });
					break;

				case '*':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "*=", TokenType::OP_MULT_SET, Priority::MAX });
						++i;
					}
					else
						tokens->emplace_back(Token{ "*", TokenType::OP_MULT, Priority::MAX });
					break;

				case '/':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "/=", TokenType::OP_DIV_SET, Priority::MAX });
						++i;
					}
					else if (next_i_ex && code[i + 1] == '/')
					{
						++i;
						while (i < code_length && code[i] != '\n')
						{
							++i;
						}
					}
					else if (next_i_ex && code[i + 1] == '*')
					{
						i += 2;
						while (code[i] != '*' && i + 1 < code_length && code[i + 1] != '/')
						{
							++i;
						}
						++i;
					}
					else
						tokens->emplace_back(Token{ "/", TokenType::OP_DIV, Priority::MAX });
					break;

				case '%':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "%=", TokenType::OP_REM_SET, Priority::MAX });
						++i;
					}
					else
						tokens->emplace_back(Token{ "%", TokenType::OP_REM, Priority::MAX });
					break;

				case '<':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "<=", TokenType::OP_LTEQ, Priority::MED });
						++i;
					}
					else
						tokens->emplace_back(Token{ "<", TokenType::OP_LT, Priority::MED });

					break;

				case '>':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ ">=", TokenType::OP_GTEQ, Priority::MED });
						++i;
					}
					else
						tokens->emplace_back(Token{ ">", TokenType::OP_GT, Priority::MED });

					break;

				case '=':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "==", TokenType::OP_EQUAL, Priority::LOW });
						++i;
					}
					else
					{
						tokens->emplace_back(Token{ "=", TokenType::OP_SET, Priority::MIN });
					}
					break;

				case '!':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ "!=", TokenType::OP_NEQUAL, Priority::LOW });
						++i;
					}
					else
						tokens->emplace_back(Token{ "!", TokenType::UNOP_OPPNUM, Priority::OVER });
					break;

				case '?':
					tokens->emplace_back(Token{ "?", TokenType::GETNUM });
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

}