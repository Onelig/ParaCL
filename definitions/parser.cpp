#include <parser.hpp>
#include <algorithm>
#include <map>

std::unordered_map<std::string, int> VarInt;

void ClearBuffer()
{
	std::cin.clear();
}

//
// ------------------- Node -------------------
//
Node::Node(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: left(std::move(left)), right(std::move(right))
{ }

int* Node::execute_pointer()
{
	return nullptr;
}


//
// ------------------- Scope -------------------
//
Scope::Scope(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: Node(std::move(left), std::move(right))
{ }

std::optional<int> Scope::execute() const
{
	bool RHS_m = true;
	const std::unique_ptr<Node>* LHS_t = &left,
							   * RHS_t = &right;

	while (*LHS_t && RHS_m)
	{
		(*LHS_t)->execute();
		if (*RHS_t)
		{
			(*RHS_t)->execute();
			LHS_t = &(*RHS_t)->left;
			RHS_t = &(*RHS_t)->right;
		}
		else
			RHS_m = false;
	}

	return std::make_optional<int>(true);
}

TokenType Scope::getType() const
{
	return TokenType::NONE;
}


//
// ------------------- BinOper -------------------
//
BinOper::BinOper(TokenType oper, std::unique_ptr<Node>&& LHS, std::unique_ptr<Node>&& RHS, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: tokentype(oper), LHS(std::move(LHS)), RHS(std::move(RHS)), Node(std::move(left), std::move(right))
{ 
	if (tokentype == TokenType::OP_SET && this->LHS->getType() != TokenType::VAR)
	{
		throw std::invalid_argument("invalid LHS");
	}
}

TokenType BinOper::getType() const
{
	return tokentype;
}

std::optional<int> BinOper::execute() const
{
	int LHS_int = LHS->execute().value(),
		RHS_int = RHS->execute().value();
	int	result  = RHS_int;

	switch (tokentype)
	{
	case TokenType::OP_PLUS:
		result = LHS_int + RHS_int;
		break;

	case TokenType::OP_MINUS:
		result = LHS_int - RHS_int;
		break;

	case TokenType::OP_MULT:
		result = LHS_int * RHS_int;
		break;

	case TokenType::OP_DIV:
		result = LHS_int / RHS_int;
		break;

	case TokenType::OP_LT:
		result = LHS_int < RHS_int;
		break;

	case TokenType::OP_LTEQ:
		result = LHS_int <= RHS_int;
		break;

	case TokenType::OP_GT:
		result = LHS_int > RHS_int;
		break;

	case TokenType::OP_GTEQ:
		result = LHS_int >= RHS_int;
		break;

	case TokenType::OP_EQUAL:
		result = LHS_int == RHS_int;
		break;

	case TokenType::OP_NEQUAL:
		result = LHS_int != RHS_int;
		break;
	
	case TokenType::OP_SET:
		*LHS->execute_pointer() = RHS_int;
		result = RHS_int;
		break;
	}

	return std::make_optional<int>(result);
}



//
// ------------------- UnOper -------------------
//
UnOper::UnOper(TokenType unoper, std::unique_ptr<Node>&& element, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: tokentype(unoper), element(std::move(element)), Node(std::move(left), std::move(right))
{ }

TokenType UnOper::getType() const
{
	return tokentype;
}

std::optional<int> UnOper::execute() const
{
	return std::make_optional<int>(!element->execute());
}



//
// ------------------- IFKeyW -------------------
//
IFKeyW::IFKeyW(std::unique_ptr<Node>&& scope, std::unique_ptr<Node>&& condition, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: LHS(std::move(scope->left)), RHS(std::move(scope->right)), condition(std::move(condition)), Node(std::move(left), std::move(right))
{ }

TokenType IFKeyW::getType() const
{
	return TokenType::KEYWORD_IF;
}

std::optional<int> IFKeyW::execute() const
{
	if (condition->execute().value())
	{
		std::unordered_map<std::string, int> copyVarInt = VarInt;

		bool RHS_m = true;
		std::unique_ptr<Node>* LHS_t = &LHS,
							 * RHS_t = &RHS;

		while (*LHS_t && RHS_m)
		{
			(*LHS_t)->execute();
			if (*RHS_t)
			{
				(*RHS_t)->execute();
				LHS_t = &(*RHS_t)->left;
				RHS_t = &(*RHS_t)->right;
			}
			else
				RHS_m = false;
		}

		for (auto& [str, value] : copyVarInt)
		{
			std::unordered_map<std::string, int>::const_iterator iter = VarInt.find(str);
			if (iter != VarInt.end())
			{
				value = iter->second;
			}
		}

		VarInt = copyVarInt;
	}

	return std::nullopt;
}



//
// ------------------- WhileKeyW -------------------
//
WhileKeyW::WhileKeyW(std::unique_ptr<Node>&& scope, std::unique_ptr<Node>&& condition, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: LHS(std::move(scope->left)), RHS(std::move(scope->right)), condition(std::move(condition)), Node(std::move(left), std::move(right))
{ }

TokenType WhileKeyW::getType() const
{
	return TokenType::KEYWORD_WHILE;
}

std::optional<int> WhileKeyW::execute() const
{
	std::unique_ptr<Node>* LHS_t = &LHS,
						 * RHS_t = &RHS;

	std::unordered_map<std::string, int> copyVarInt = VarInt;

	while (condition->execute().value())
	{
		bool RHS_m = true;

		while (*LHS_t && RHS_m)
		{
			(*LHS_t)->execute();
			if (*RHS_t)
			{
				(*RHS_t)->execute();
				LHS_t = &(*RHS_t)->left;
				RHS_t = &(*RHS_t)->right;
			}
			else
				RHS_m = false;
		}

		LHS_t = &LHS;
		RHS_t = &RHS;
	}

	for (auto& [str, value] : copyVarInt)
	{
		std::unordered_map<std::string, int>::const_iterator iter = VarInt.find(str);
		if (iter != VarInt.end())
		{
			value = iter->second;
		}
	}

	VarInt = copyVarInt;

	return std::nullopt;
}



//
// ------------------- PrintKeyW -------------------
//
PrintKeyW::PrintKeyW(std::unique_ptr<Node>&& print_node, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: str_cout(std::move(print_node)), Node(std::move(left), std::move(right))
{ }

TokenType PrintKeyW::getType() const
{
	return TokenType::KEYWORD_PRINT;
}

std::optional<int> PrintKeyW::execute() const
{
	std::cout << str_cout->execute().value() << std::endl;

	return std::make_optional<int>(1);
}



//
// ------------------- VAR -------------------
//
VAR::VAR(const std::string& name, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: value(VarInt[name]), Node(std::move(left), std::move(right))
{ }

TokenType VAR::getType() const
{
	return TokenType::VAR;
}

std::optional<int> VAR::execute() const
{
	return std::make_optional<int>(value);
}

int* VAR::execute_pointer()
{
	return &value;
}



//
// ------------------- Num -------------------
//
Num::Num(int value, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: value(value), Node(std::move(left), std::move(right))
{ }

TokenType Num::getType() const
{
	return TokenType::NUMBER;
}

std::optional<int> Num::execute() const
{
	return std::make_optional<int>(value);
}



//
// ------------------- GetNum -------------------
//
GetNum::GetNum(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: Node(std::move(left), std::move(right))
{ }

TokenType GetNum::getType() const
{
	return TokenType::GETNUM;
}

std::optional<int> GetNum::execute() const
{
	int tvalue;
	ClearBuffer();
	std::cin >> tvalue;
	if (!std::cin.good())
		throw std::invalid_argument("invalid input");

	return tvalue;
}




//
// ------------------- Parser -------------------
//

std::optional<const Token> Parser::peek()
{
	if (token_iter == it_end)
	{
		return std::nullopt;
	}
	return *token_iter;
}

std::optional<const Token> Parser::get()
{
	return *(token_iter++);
}

std::unique_ptr<Node> Parser::factor()
{
	if (peek().value().type == TokenType::NUMBER)
		return std::make_unique<Num>(std::stoi(get().value().value));

	else if (peek().value().type == TokenType::VAR)
		return std::make_unique<VAR>(get().value().value);

	else if (peek().value().type == TokenType::GETNUM)
	{
		get();
		return std::make_unique<GetNum>();
	}

	/*else if (peek().value().type == TokenType::OPEN_PAREN)
	{
		get();

		get();
	}*/

	return nullptr;
}


std::unique_ptr<Node> Parser::maxprior_expr()
{
	std::unique_ptr<Node> result = factor();
	std::unique_ptr<Node> return_v = nullptr;

	while (peek().has_value() && (peek().value().priority == Priority::MAX))
	{
		TokenType gettype = get().value().type;

		if (return_v)
			return_v = std::make_unique<BinOper>(gettype, std::move(return_v), std::move(factor()));
		
		else
			return_v = std::make_unique<BinOper>(gettype, std::move(result), std::move(factor()));
	}

	return (return_v ? std::move(return_v) : std::move(result));
}

std::unique_ptr<Node> Parser::averprior_expr()
{
	std::unique_ptr<Node> result = maxprior_expr();
	std::unique_ptr<Node> return_v = nullptr;

	while (peek().has_value() && (peek().value().priority == Priority::AVER))
	{
		TokenType gettype = get().value().type;

		if (return_v)
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(return_v), std::move(maxprior_expr()));
		}

		else
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(result), std::move(maxprior_expr()));
		}
	}
	
	return (return_v ? std::move(return_v) : std::move(result));
}

std::unique_ptr<Node> Parser::medprior_expr()
{
	std::unique_ptr<Node> result = averprior_expr();
	std::unique_ptr<Node> return_v = nullptr;

	while (peek().has_value() && (peek().value().priority == Priority::MED))
	{
		TokenType gettype = get().value().type;

		if (return_v)
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(return_v), std::move(averprior_expr()));
		}

		else
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(result), std::move(averprior_expr()));
		}
	}

	return (return_v ? std::move(return_v) : std::move(result));
}

std::unique_ptr<Node> Parser::lowprior_expr()
{
	std::unique_ptr<Node> result = medprior_expr();
	std::unique_ptr<Node> return_v = nullptr;

	while (peek().has_value() && (peek().value().priority == Priority::LOW))
	{
		TokenType gettype = get().value().type;

		if (return_v)
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(return_v), std::move(medprior_expr()));
		}

		else
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(result), std::move(medprior_expr()));
		}
	}

	return (return_v ? std::move(return_v) : std::move(result));
}

std::unique_ptr<Node> Parser::minprior_expr()
{
	std::unique_ptr<Node> result = lowprior_expr();
	std::unique_ptr<Node> return_v = nullptr;

	while (peek().has_value() && (peek().value().priority == Priority::MIN))
	{
		TokenType gettype = get().value().type;
		
		if (return_v)
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(return_v), std::move(lowprior_expr()));
		}
		
		else
		{
			return_v = std::make_unique<BinOper>(gettype, std::move(result), std::move(lowprior_expr()));
		}
	}

	return (return_v ? std::move(return_v) : std::move(result));
}

std::unique_ptr<Node> Parser::parse_(size_t begin, size_t end)
{
	std::unique_ptr<Node> root_ = std::make_unique<Scope>();
	std::unique_ptr<Node>* copy_root = &root_;

	for (size_t i = begin; i < end; ++i, token_iter = tokens->begin() + i)
	{
		std::unique_ptr<Node> child;

		switch ((*tokens)[i].type)
		{
		case TokenType::KEYWORD_IF:
		{
			// condition from '(' to ')'
			it_end = std::find_if(token_iter, tokens->cend(), [](const Token& token) { return token.type == TokenType::CLOSE_PAREN; });
			i += it_end - token_iter;
			token_iter += 2; // token_iter == after '('
			std::unique_ptr<Node> cond = lowprior_expr();

			++token_iter; // token_iter == '{'
			if (token_iter->type != TokenType::LSCOPE)
			{
				throw std::invalid_argument("should be in scope { } ");
			}
			size_t sScope = NULL;
			auto end_scope = std::find_if(token_iter, tokens->cend(), [&sScope](const Token& token) 
				{
					if (token.type == TokenType::LSCOPE)
						++sScope;

					else if (token.type == TokenType::RSCOPE)
						--sScope;

					return !sScope;
				});

			if (end_scope == tokens->cend())
			{
				throw std::invalid_argument("scope was not closed");
			}
			size_t newI = i + (end_scope - token_iter) + 1;
			++token_iter;
			auto b = parse_(i + 2, newI);
			child = std::make_unique<IFKeyW>(std::move(b), std::move(cond));
			i = newI;
			break;
		}
		case TokenType::KEYWORD_WHILE:
		{
			// condition from '(' to ')'
			it_end = std::find_if(token_iter, tokens->cend(), [](const Token& token) { return token.type == TokenType::CLOSE_PAREN; });
			i += it_end - token_iter;
			token_iter += 2; // token_iter == after '('
			std::unique_ptr<Node> cond = lowprior_expr();

			++token_iter; // token_iter == '{'
			if (token_iter->type != TokenType::LSCOPE)
			{
				throw std::invalid_argument("should be in scope { } ");
			}
			size_t sScope = NULL;
			auto end_scope = std::find_if(token_iter, tokens->cend(), [&sScope](const Token& token)
				{
					if (token.type == TokenType::LSCOPE)
						++sScope;

					else if (token.type == TokenType::RSCOPE)
						--sScope;

					return !sScope;
				});

			if (end_scope == tokens->cend())
			{
				throw std::invalid_argument("scope was not closed");
			}
			size_t newI = i + (end_scope - token_iter) + 1;
			++token_iter;
			auto b = parse_(i + 2, newI);
			child = std::make_unique<WhileKeyW>(std::move(b), std::move(cond));
			i = newI;
			break;
		}

		case TokenType::KEYWORD_PRINT:
			++token_iter;
			it_end = std::find_if(token_iter, tokens->cend(), [](const Token& token) { return token.type == TokenType::ENDCOLOM; });
			i += it_end - token_iter + 1;
			child = std::make_unique<PrintKeyW>(lowprior_expr());
			break;

		default:
		{
			it_end = std::find_if(token_iter, tokens->cend(), [](const Token& token) { return token.type == TokenType::ENDCOLOM; });
			i += it_end - token_iter;
			child = minprior_expr();

			break;
		}
		}

		if ((*copy_root)->left)
		{
			(*copy_root)->right = std::move(child);
			copy_root = &(*copy_root)->right;
		}
		else
			(*copy_root)->left = std::move(child);
	}

	return std::move(root_);
}

Parser::Parser(std::shared_ptr<std::vector<Token>> token_vec)
	: tokens(token_vec), token_iter(tokens->begin())
{ }

void Parser::parse()
{
	root = parse_(NULL, tokens->size());
}

std::unique_ptr<Node> Parser::getAST()
{
	return std::move(root);
}