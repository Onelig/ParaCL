#include <lexer.hpp>
#include <LanguageErrors.hpp>
#include <stdexcept>


namespace ParaCL
{
	void Lexer::strTokenize(size_t& i)
	{
		if (!tokens->empty() && (tokens->back().type == TokenType::NUMBER || tokens->back().type == TokenType::VAR)) // !
			throw Errors::Syntax("Incorrect character sequence appears", line); 

		std::string VAR_NAME{ code[i++] };
		while (i < code_length && (std::isalpha(code[i]) || std::isdigit(code[i]) || code[i] == '_'))
			VAR_NAME += code[i++];

		--i;

		if (VAR_NAME == "while")
			tokens->emplace_back(Token{ TokenType::KEYWORD_WHILE, line, Priority::NONE, VAR_NAME });

		else if (VAR_NAME == "if")
			tokens->emplace_back(Token{ TokenType::KEYWORD_IF,    line, Priority::NONE, VAR_NAME });

		else if (VAR_NAME == "else")
			tokens->emplace_back(Token{ TokenType::KEYWORD_ELSE,  line, Priority::NONE, VAR_NAME });

		else if (VAR_NAME == "print")
			tokens->emplace_back(Token{ TokenType::KEYWORD_PRINT, line, Priority::NONE, VAR_NAME });
			
		else
			tokens->emplace_back(Token{ TokenType::VAR,			  line, Priority::NONE, VAR_NAME });
	}

	void Lexer::intTokenize(size_t& i)
	{
		std::string NUMBER{ code[i++] };
		while (i < code_length && std::isdigit(code[i]))
			NUMBER += code[i++];

		if (i < code_length && (std::isalpha(code[i]) || code[i] == '_'))
			throw Errors::Syntax("Incorrect character sequence appears", line);

		--i;

		tokens->emplace_back(Token{ TokenType::NUMBER, line, Priority::NONE, NUMBER });
	}

	Lexer::Lexer(const std::string& code)
		: code(code), code_length(code.length()), tokens(std::make_shared<std::vector<Token>>()), line(0)
	{ }

	void Lexer::tokenize()
	{
		for (size_t i = 0; i < code_length; ++i)
		{
			if (code[i] == '\n')
				++line;

			else if (code[i] == ' ' || code[i] == '\t')
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
						tokens->emplace_back(Token{ TokenType::OP_PLUS_SET, line, Priority::MIN});
						++i;
					}
					else if (next_i_ex && code[i + 1] == '+')
					{
						tokens->emplace_back(Token{ TokenType::UNOP_INC, line, Priority::OVER });
						++i;
					}
					else
						tokens->emplace_back(Token{ TokenType::OP_PLUS, line, Priority::AVER });
					break;

				case '-':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ TokenType::OP_MINUS_SET, line, Priority::MIN });
						++i;
					}
					else if (next_i_ex && code[i + 1] == '-')
					{
						tokens->emplace_back(Token{ TokenType::UNOP_DEC, line, Priority::OVER });
						++i;
					}
					else
						tokens->emplace_back(Token{ TokenType::OP_MINUS, line, Priority::AVER });
					break;

				case '*':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ TokenType::OP_MULT_SET, line, Priority::MIN });
						++i;
					}
					else
						tokens->emplace_back(Token{ TokenType::OP_MULT, line, Priority::MAX });
					break;

				case '/':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{ TokenType::OP_DIV_SET, line, Priority::MIN });
						++i;
					}
					else if (next_i_ex && code[i + 1] == '/')
					{
						++i;
						while (i < code_length)
						{
							if (code[i] == '\n')
							{
								++line;
								break;
							}
							++i;
						}
					}
					else if (next_i_ex && code[i + 1] == '*')
					{
						i += 2;
						while (i + 1 < code_length && (code[i] != '*' || code[i + 1] != '/'))
						{
							if (code[i] == '\n')
								++line;
							++i;
						}

						if (i + 1 == code_length)
							throw Errors::Syntax("Comments like /* should be closed", line);

						++i;
					}
					else
						tokens->emplace_back(Token{	TokenType::OP_DIV, line, Priority::MAX });
					break;

				case '%':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{	TokenType::OP_REM_SET, line, Priority::MIN });
						++i;
					}
					else
						tokens->emplace_back(Token{	TokenType::OP_REM, line, Priority::MAX });
					break;

				case '<':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{	TokenType::OP_LTEQ, line, Priority::MED });
						++i;
					}
					else
						tokens->emplace_back(Token{	TokenType::OP_LT, line, Priority::MED });

					break;

				case '>':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{	TokenType::OP_GTEQ, line, Priority::MED });
						++i;
					}
					else
						tokens->emplace_back(Token{	TokenType::OP_GT, line, Priority::MED });

					break;

				case '=':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{	TokenType::OP_EQUAL, line, Priority::LOW });
						++i;
					}
					else
					{
						tokens->emplace_back(Token{	TokenType::OP_SET, line, Priority::MIN });
					}
					break;

				case '!':
					if (next_i_ex && code[i + 1] == '=')
					{
						tokens->emplace_back(Token{	TokenType::OP_NEQUAL, line, Priority::LOW });
						++i;
					}
					else
						tokens->emplace_back(Token{ TokenType::UNOP_OPPNUM, line, Priority::OVER });
					break;

				case '?':
					tokens->emplace_back(Token{ TokenType::GETNUM, line});
					break;

				case '(':
					tokens->emplace_back(Token{ TokenType::OPEN_PAREN, line });
					break;

				case ')':
					tokens->emplace_back(Token{ TokenType::CLOSE_PAREN, line });
					break;

				case '{':
					tokens->emplace_back(Token{ TokenType::LSCOPE, line });
					break;

				case '}':
					tokens->emplace_back(Token{ TokenType::RSCOPE, line });
					break;


				case ';':
					tokens->emplace_back(Token{ TokenType::ENDCOLOM, line });
					break;

				default:
					throw Errors::Syntax("Invalid type", line);
				}
			}
		}
	}

	std::shared_ptr<std::vector<Token>> Lexer::gettokens()
	{
		return tokens;
	}
}