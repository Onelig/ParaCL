#include <parser.hpp>
#include <algorithm>
#include <map>

std::unordered_map<std::string, int> VarInt;
std::unordered_set<TokenType> MaxPriorCont{ TokenType::OP_PLUS_SET, TokenType::OP_MINUS_SET, TokenType::OP_REM_SET, TokenType::OP_MULT_SET, TokenType::OP_DIV_SET, TokenType::OP_SET };


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

std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> Node::getLRHS() 
{
	return std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*>(nullptr, nullptr);
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
	if (MaxPriorCont.contains(tokentype) && (this->LHS->getType() != TokenType::VAR && this->LHS->getLRHS().first == nullptr))
	{
		throw std::invalid_argument("invalid LHS");
	}
}

std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> BinOper::getLRHS() 
{ 
	return std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*>(&LHS, &RHS); 
}

TokenType BinOper::getType() const
{
	return tokentype;
}

std::optional<int> BinOper::execute() const
{
	int RHS_int = RHS->execute().value();
	int	result = RHS_int;

	if (tokentype == TokenType::OP_SET)
		*LHS->execute_pointer() = RHS_int;

	else if (tokentype == TokenType::OP_PLUS_SET || tokentype == TokenType::OP_MINUS_SET ||
			 tokentype == TokenType::OP_MULT_SET || tokentype == TokenType::OP_DIV_SET   || 
			 tokentype == TokenType::OP_REM_SET)
	{
		int* ptr = LHS->execute_pointer();
		int LHS_int = LHS->execute().value();
		switch (tokentype)
		{
		case TokenType::OP_PLUS_SET: *ptr += RHS_int; break;
		case TokenType::OP_MINUS_SET: *ptr -= RHS_int; break;
		case TokenType::OP_REM_SET: *ptr %= RHS_int; break;
		case TokenType::OP_MULT_SET: *ptr *= RHS_int; break;
		case TokenType::OP_DIV_SET: *ptr /= RHS_int; break;
		}
	}
	else
	{
		int LHS_int = LHS->execute().value();

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

		case TokenType::OP_REM:
			result = LHS_int % RHS_int;
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
		}
	}
	

	return std::make_optional<int>(result);
}



//
// ------------------- UnOper -------------------
//
UnOper::UnOper(TokenType unoper, std::unique_ptr<Node>&& element, bool rElem, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: tokentype(unoper), rElem(rElem), element(std::move(element)), Node(std::move(left), std::move(right))
{ }

TokenType UnOper::getType() const
{
	return tokentype;
}

std::optional<int> UnOper::execute() const
{
	int result = NULL;

	switch (tokentype)
	{
	case TokenType::UNOP_OPPNUM:
		result = !(element->execute().value());
		break;
	case TokenType::UNOP_INC:
		if (element->getType() == TokenType::NUMBER)
		{
			throw std::invalid_argument("with ++ should be l-value");
		}

		if (rElem)
		{
			*element->execute_pointer() += 1;
			result = element->execute().value();
		}
		else
		{
			result = element->execute().value();
			*element->execute_pointer() += 1;
		}
		break;
	case TokenType::UNOP_DEC:
		if (element->getType() == TokenType::NUMBER)
		{
			throw std::invalid_argument("with -- should be l-value");
		}

		if (rElem)
		{
			*element->execute_pointer() -= 1;
			result = element->execute().value();
		}
		else
		{
			result = element->execute().value();
			*element->execute_pointer() -= 1;
		}
		break;
		
	case TokenType::UNOP_MINUS:
		result = -element->execute().value();
		break;
	}

	return std::make_optional<int>(result);
}

std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> UnOper::getLRHS()
{
	if (rElem)
	{
		return std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*>(&element, nullptr);
	}
	return std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*>(nullptr, &element);
}

int* UnOper::execute_pointer()
{
	//this->execute();
	return element->execute_pointer();
}


//
// ------------------- IFKeyW -------------------
//
IFKeyW::IFKeyW(std::unique_ptr<Node>&& scope, std::unique_ptr<Node>&& condition, std::unique_ptr<Node>&& else_, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: scope(std::move(scope)), else_(std::move(else_)), condition(std::move(condition)), Node(std::move(left), std::move(right))
{ }

TokenType IFKeyW::getType() const
{
	return TokenType::KEYWORD_IF;
}

std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> IFKeyW::getLRHS() 
{
	return std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*>(&else_, nullptr);
}

std::optional<int> IFKeyW::execute() const
{
	if (condition->execute().value())
	{
		std::unordered_map<std::string, int> copyVarInt = VarInt;

		scope->execute();

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
	else if (else_)
	{
		else_->execute();
	}

	return std::nullopt;
}


//
// ------------------- WhileKeyW -------------------
//
WhileKeyW::WhileKeyW(std::unique_ptr<Node>&& scope, std::unique_ptr<Node>&& condition, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: scope(std::move(scope)), condition(std::move(condition)), Node(std::move(left), std::move(right))
{ }

TokenType WhileKeyW::getType() const
{
	return TokenType::KEYWORD_WHILE;
}

std::optional<int> WhileKeyW::execute() const
{
	std::unordered_map<std::string, int> copyVarInt = VarInt;

	scope->execute();

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

	return std::nullopt;
}



//
// ------------------- VAR -------------------
//
VAR::VAR(const std::string& name, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: value(nullptr), name(name), Node(std::move(left), std::move(right))
{ }

TokenType VAR::getType() const
{
	return TokenType::VAR;
}

std::optional<int> VAR::execute() const
{
	std::unordered_map<std::string, int>::iterator iter = VarInt.find(name);
	if (iter != VarInt.end())
		return iter->second;

	return std::nullopt;
}

int* VAR::execute_pointer()
{
	bool isExist = true;
	if (this->execute() == std::nullopt)
		isExist = false;

	value = &VarInt[name];
	if (!isExist)
		*value = INT_MIN;

	return value;
}



//
// ------------------- Num -------------------
//
Num::Num(int value, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: value(value), Node(std::move(left), std::move(right))
{ }

Num::Num(const std::string& value, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
	: value(std::stoi(value)), Node(std::move(left), std::move(right))
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

std::unique_ptr<Node> Parser::ifOverOper()
{
	std::unique_ptr<Node> nd = nullptr;
	std::unique_ptr<Node>* nd_c = &nd;
	if (peek().value().priority == Priority::OVER)
	{
		while (peek() && peek().value().priority == Priority::OVER)
		{
			TokenType type = get().value().type;

			if (*nd_c)
			{
				std::unique_ptr<Node> element = std::move(*(*nd_c)->getLRHS().first);
				*(*nd_c)->getLRHS().first = std::make_unique<UnOper>(type, std::move(element), true);
				nd_c = (*nd_c)->getLRHS().first;
			}
			else
			{
				(*nd_c) = std::make_unique<UnOper>(type, nullptr, true);
			}
		}
	}
	else if (peek().value().type == TokenType::OP_MINUS)
	{
		get();
		nd = std::make_unique<UnOper>(TokenType::UNOP_MINUS, nullptr);
	}

	if (nd)
	{
		if (peek().value().type == TokenType::VAR)
		{
			*(*nd_c)->getLRHS().first = std::make_unique<VAR>(peek().value().value);

			get();
		}
		else if (peek().value().type == TokenType::NUMBER)
		{
			*(*nd_c)->getLRHS().first = std::make_unique<Num>(peek().value().value);

			get();
		}
		else
			*(*nd_c)->getLRHS().first = factor();
	}

	return nd;
}

std::unique_ptr<Node> Parser::factor()
{
	if (peek().value().type == TokenType::NUMBER)
		return std::make_unique<Num>(get().value().value);

	else if (peek().value().type == TokenType::VAR)
	{
		std::unique_ptr<Node> nd = std::make_unique<VAR>(get().value().value);

		if (peek() && (peek().value().type == TokenType::UNOP_DEC || peek().value().type == TokenType::UNOP_INC))
		{
			nd = std::make_unique<UnOper>(peek().value().type, std::move(nd), false);
			get();
		}
		return std::move(nd);
	}
	else if (peek().value().type == TokenType::OPEN_PAREN)
	{
		get();
		std::unique_ptr<Node> expr = minprior_expr();
		get();
		return expr;
	}

	else if (peek().value().type == TokenType::GETNUM)
	{
		get();
		return std::make_unique<GetNum>();
	}

	return ifOverOper();
}

std::unique_ptr<Node> Parser::maxprior_expr()
{
	std::unique_ptr<Node> result = factor();
	std::unique_ptr<Node> return_v = nullptr;

	while (peek().has_value() && (peek().value().priority == Priority::MAX))
	{
		TokenType gettype = get().value().type;

		return_v = std::make_unique<BinOper>(gettype, std::move(return_v ? return_v : result), std::move(factor()));
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

		return_v = std::make_unique<BinOper>(gettype, std::move(return_v ? return_v : result), std::move(maxprior_expr()));
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
		
		return_v = std::make_unique<BinOper>(gettype, std::move(return_v ? return_v : result), std::move(averprior_expr()));
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

		return_v = std::make_unique<BinOper>(gettype, std::move(return_v ? return_v : result), std::move(medprior_expr()));
	}

	return (return_v ? std::move(return_v) : std::move(result));
}

std::unique_ptr<Node> Parser::minprior_expr()
{
	std::unique_ptr<Node> result = lowprior_expr();
	std::unique_ptr<Node> return_v = nullptr;
	std::unique_ptr<Node>* result_v_p = &return_v;

	while (peek().has_value() && (peek().value().priority == Priority::MIN))
	{
		TokenType gettype = get().value().type;
		
		if (*result_v_p)
		{
			std::unique_ptr<Node> RHS_ = std::move(*(*result_v_p)->getLRHS().second);

			(*(*result_v_p)->getLRHS().second) = std::make_unique<BinOper>(gettype, std::move(RHS_), std::move(lowprior_expr()));
			result_v_p = &(*(*result_v_p)->getLRHS().second);
		}
		
		else
			(*result_v_p) = std::make_unique<BinOper>(gettype, std::move(result), std::move(lowprior_expr()));
	}

	return (return_v ? std::move(return_v) : std::move(result));
}

template<typename T>
std::unique_ptr<Node> Parser::IfWhileS(size_t& i, bool SkipCond)
{
	std::unique_ptr<Node> cond;
	if (!SkipCond)
	{
		// condition from '(' to ')'
		it_end = std::find_if(token_iter, tokens->cend(), [](const Token& token) { return token.type == TokenType::CLOSE_PAREN; });
		i += it_end - token_iter;
		token_iter += 2; // token_iter == after '('
		cond = lowprior_expr();
	}
	else
		token_iter += 2;

	++token_iter; // token_iter == '{'
	if (token_iter->type != TokenType::LSCOPE)
		throw std::invalid_argument("should be in scope { } ");

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
		throw std::invalid_argument("scope was not closed");


	size_t newI = i + (end_scope - token_iter) + 1;
	auto b = parse_(i + 2, newI);
	i = newI;
	return std::make_unique<T>(std::move(b), (SkipCond ? nullptr : std::move(cond)));
}

std::unique_ptr<Node> Parser::parse_(size_t begin, size_t end)
{
	std::unique_ptr<Node> root_ = std::make_unique<Scope>();
	std::unique_ptr<Node>* copy_root = &root_;

	for (size_t i = begin; i < end; ++i)
	{
		token_iter = tokens->begin() + i;
		std::unique_ptr<Node> child;

		switch ((*tokens)[i].type)
		{
		case TokenType::KEYWORD_IF:
		{
			child = IfWhileS<IFKeyW>(i);
			if (i + 1 < end && (*tokens)[i + 1].type == TokenType::KEYWORD_ELSE)
			{
				++i;
				*child->getLRHS().first = IfWhileS<Scope>(i, true);
			}
			break;
		}
		case TokenType::KEYWORD_WHILE:
		{
			child = IfWhileS<WhileKeyW>(i);
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

		if (child == nullptr) 
			throw std::invalid_argument("unknown symbol"); 

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
