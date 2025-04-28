#include <parser.hpp>
#include <LanguageErrors.hpp>
#include <algorithm>
#include <map>
#include <iostream>

namespace ParaCL
{
	// contains all exist variables
	std::unordered_map<std::string, int> VarInt; 
	std::unordered_set<ParaCL::TokenType> MaxPriorCont{ ParaCL::TokenType::OP_PLUS_SET, ParaCL::TokenType::OP_MINUS_SET, ParaCL::TokenType::OP_REM_SET, ParaCL::TokenType::OP_MULT_SET, ParaCL::TokenType::OP_DIV_SET, ParaCL::TokenType::OP_SET };


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

		// copy scope elements before new scope
		std::unordered_map<std::string, int> copyVarInt = VarInt;

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

		// update actually scope with changed value from last scope
		for (auto& [str, value] : copyVarInt)
		{
			std::unordered_map<std::string, int>::const_iterator iter = VarInt.find(str);
			if (iter != VarInt.end())
				value = iter->second;
		}

		// the set of elements that were before last scope
		VarInt = copyVarInt;

		return std::nullopt;
	}

	TokenType Scope::getType() const
	{
		return TokenType::NONE;
	}


	//
	// ------------------- BinOper -------------------
	//
	BinOper::BinOper(TokenType oper, std::unique_ptr<Node>&& LHS, std::unique_ptr<Node>&& RHS, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: tokentype(oper), LHS(std::move(LHS)), RHS(std::move(RHS)), LRHS(std::move(left), std::move(right))
	{
		if (this->LHS == nullptr || this->RHS == nullptr)
			throw Errors::Syntax("Not full expression", NULL);
	}

	std::unique_ptr<Node>& BinOper::getRHS()
	{
		return RHS;
	}

	TokenType BinOper::getType() const
	{
		return tokentype;
	}

	std::optional<int> BinOper::execute() const
	{
		int RHS_int = RHS->execute().value();
		int LHS_int = LHS->execute().value();
		int	result = 0;


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

		default:
			throw Errors::Syntax("Incorrect binary operator", NULL);
		}

		return std::make_optional<int>(result);
	}



	//
	// ------------------- BinOperAssign -------------------
	//
	BinOperAssign::BinOperAssign(TokenType oper, std::unique_ptr<LValue>&& LHS, std::unique_ptr<Node>&& RHS, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: tokentype(oper), LHS(std::move(LHS)), RHS(std::move(RHS)), LRHS(std::move(left), std::move(right))
	{
		if (this->LHS == nullptr || this->RHS == nullptr)
			throw Errors::Syntax("Not full expression", NULL);
	}

	std::unique_ptr<Node>& BinOperAssign::getRHS()
	{
		return RHS;
	}

	TokenType BinOperAssign::getType() const
	{
		return tokentype;
	}

	std::optional<int> BinOperAssign::execute() const
	{
		int RHS_int = RHS->execute().value();
		int* ptr = LHS->execute_pointer();

		switch (tokentype)
		{
		case TokenType::OP_SET:		  *ptr = RHS_int; break;
		case TokenType::OP_PLUS_SET:  *ptr += RHS_int; break;
		case TokenType::OP_MINUS_SET: *ptr -= RHS_int; break;
		case TokenType::OP_REM_SET:   *ptr %= RHS_int; break;
		case TokenType::OP_MULT_SET:  *ptr *= RHS_int; break;
		case TokenType::OP_DIV_SET:   *ptr /= RHS_int; break;
		default: throw Errors::Syntax("Incorrect compound assignment operator", NULL);
		}

		return std::make_optional<int>(*ptr);
	}



	//
	// ------------------- UnOperRv -------------------
	//
	UnOperRv::UnOperRv(TokenType unoper, std::unique_ptr<Node>&& element, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: tokentype(unoper), element(std::move(element)), Node(std::move(left), std::move(right))
	{ 
		if (this->element == nullptr)
			throw Errors::Syntax("Not full expression", NULL);
	}

	TokenType UnOperRv::getType() const
	{
		return tokentype;
	}

	std::optional<int> UnOperRv::execute() const
	{
		int result = NULL;

		switch (tokentype)
		{
		case TokenType::UNOP_OPPNUM: result = !(element->execute().value()); break;
		case TokenType::OP_MINUS:  result = -element->execute().value();   break;
		default: throw Errors::Syntax("Incorrect un-operator2", NULL);
		}

		return std::make_optional<int>(result);
	}



	//
	// ------------------- UnOperLv -------------------
	//
	UnOperLv::UnOperLv(TokenType unoper, std::unique_ptr<LValue>&& element, bool prefix, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: tokentype(unoper), element(std::move(element)), prefix(prefix), LValue(std::move(left), std::move(right))
	{
		if (this->element == nullptr)
			throw Errors::Syntax("Not full expression", NULL);

		else if (tokentype != TokenType::UNOP_DEC && tokentype != TokenType::UNOP_INC)
			throw Errors::Syntax("Incorrect operator with value", NULL);
	}

	TokenType UnOperLv::getType() const
	{
		return tokentype;
	}

	std::optional<int> UnOperLv::execute() const
	{
		int result = NULL;

		switch (tokentype)
		{
		case TokenType::UNOP_INC:
			if (prefix)
				result = ++(*element->execute_pointer());

			else
			{
				result = *element->execute_pointer();
				++(*element->execute_pointer());
			}
			break;

		case TokenType::UNOP_DEC:
			if (prefix)
				result = --(*element->execute_pointer());

			else
			{
				result = *element->execute_pointer();
				--(*element->execute_pointer());
			}
			break;

		default: throw Errors::Syntax("Incorrect un-operator3", NULL);
		}

		return std::make_optional<int>(result);
	}

	int* UnOperLv::execute_pointer() 
	{
		int* result = nullptr;

		if (prefix)
		{
			switch (tokentype)
			{
			case TokenType::UNOP_INC:
				*result = ++(*element->execute_pointer());
				break;

			case TokenType::UNOP_DEC:
				*result = --(*element->execute_pointer());
				break;

			default: throw Errors::Syntax("Incorrect un-operator1", NULL);
			}
		}
		else
			throw Errors::Syntax("Cannot be used with r-value", NULL);


		return result;
	}


	//
	// ------------------- IFKeyW -------------------
	//
	IFKeyW::IFKeyW(std::unique_ptr<Node>&& condition, std::unique_ptr<Scope>&& scope, std::unique_ptr<Scope>&& else_, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: scope(std::move(scope)), else_(std::move(else_)), condition(std::move(condition)), Node(std::move(left), std::move(right))
	{ 
		if (this->condition == nullptr)
			throw Errors::Syntax("Condition was skipped", NULL); 
	}

	TokenType IFKeyW::getType() const
	{
		return TokenType::KEYWORD_IF;
	}

	std::optional<int> IFKeyW::execute() const
	{
		if (condition->execute().value())
			scope->execute();

		else if (else_)
			else_->execute();

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
		while (condition->execute().value())
			scope->execute();

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
		: value(nullptr), name(name), LValue(std::move(left), std::move(right))
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
			throw Errors::Runtime("Input failure", NULL);

		return tvalue;
	}




	//
	// ------------------- Parser -------------------
	//

	std::optional<const Token> Parser::peek()
	{
		if (token_iter == tokens->end())
			return std::nullopt;

		return *token_iter;
	}

	std::optional<const Token> Parser::get()
	{
		return *(token_iter++);
	}

	std::unique_ptr<Node> Parser::ifOverOper()
	{
		if (peek().value().priority != Priority::OVER && peek().value().type != TokenType::OP_MINUS)
			throw Errors::Syntax("Missed value", NULL);


		if (peek().value().type == TokenType::UNOP_DEC || peek().value().type == TokenType::UNOP_INC)
		{
			TokenType tt = get().value().type;
			return std::make_unique<UnOperLv>(tt, std::unique_ptr<LValue>(dynamic_cast<LValue*>(factor().release())), true);
		}

		TokenType tt = get().value().type;

		return std::make_unique<UnOperRv>(tt, factor());
	}

	std::unique_ptr<Node> Parser::factor()
	{
		if (peek().value().type == TokenType::NUMBER)
			return std::make_unique<Num>(get().value().value);

		else if (peek().value().type == TokenType::VAR)
		{
			std::unique_ptr<LValue> nd = std::make_unique<VAR>(get().value().value);

			if (peek() && (peek().value().type == TokenType::UNOP_DEC || peek().value().type == TokenType::UNOP_INC))
			{
				nd = std::make_unique<UnOperLv>(peek().value().type, std::move(nd), false);
				get();
			}
			return std::move(nd);
		}
		else if (peek().value().type == TokenType::OPEN_PAREN)
		{
			get();
			std::unique_ptr<Node> expr = lowprior_expr();
			if (peek().value().type != TokenType::CLOSE_PAREN)
				throw Errors::Syntax("Paren was not closed", NULL);

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

		return return_v ? std::move(return_v) : std::move(result);
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

		return std::move(return_v ? return_v : result);
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

		return std::move(return_v ? return_v : result);
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

		return std::move(return_v ? return_v : result);
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
				LRHS* lrhs_ = static_cast<LRHS*>(result_v_p->get());

				std::unique_ptr<LValue> RHS_(dynamic_cast<LValue*>(lrhs_->getRHS().release()));

				if (RHS_ == nullptr)
					throw Errors::Syntax("Cannot modify r-value", NULL);

				(lrhs_->getRHS()) = std::make_unique<BinOperAssign>(gettype, std::move(RHS_), std::move(lowprior_expr()));
				result_v_p = &lrhs_->getRHS();
			}

			else
			{
				if (typeid(*result) != typeid(VAR)) 
					throw Errors::Syntax("Cannot assign r-value", NULL);

				std::unique_ptr<LValue> vl(static_cast<LValue*>(result.release()));
				(*result_v_p) = std::make_unique<BinOperAssign>(gettype, std::move(vl), std::move(lowprior_expr()));
			}
		}

		return return_v ? std::move(return_v) : std::move(result);
	}

	std::unique_ptr<Node> Parser::cond(size_t& i)
	{
		// condition from '(' to ')'
		if (!peek().has_value() || peek()->type != TokenType::OPEN_PAREN)
			throw Errors::Syntax("Cannot find condition", NULL);

		std::vector<Token>::const_iterator token_iter_b = token_iter;
		++token_iter; // token_iter == after '('
		
		std::unique_ptr<Node> lw_expr = lowprior_expr();
		if (!peek().has_value() || token_iter->type != TokenType::CLOSE_PAREN)
			throw Errors::Syntax("Condition does not close", NULL);

		i += token_iter - token_iter_b;

		return lw_expr;
	}

	std::unique_ptr<Scope> Parser::scope(size_t& i)
	{
		if (!peek().has_value() || token_iter->type != TokenType::LSCOPE)
			throw Errors::Syntax("Scope should be opened { ... }", token_iter->line);

		size_t sScope = NULL;

		auto end_scope = std::find_if(token_iter, tokens->cend(), [&sScope](const Token& token)
			{
				if (token.type == TokenType::LSCOPE)
					++sScope;

				else if (token.type == TokenType::RSCOPE)
					--sScope;

				return !sScope;
			});

		if (!peek().has_value() || end_scope == tokens->cend())
			throw Errors::Syntax("Scope should be closed { ... }", (tokens->end() - 1)->line);


		size_t newI = i + (end_scope - token_iter);
		auto b = parse_(i + 1, newI);
		i = newI;
		return std::make_unique<Scope>(std::move(b->left), std::move(b->right));
	}

	std::unique_ptr<Node> Parser::IfElseS(size_t& i)
	{
		++token_iter;
		++i;
		std::unique_ptr<Node> cond_ = cond(i);
		++token_iter;
		++i;
		std::unique_ptr<Scope> scope_ = scope(i);
		++token_iter;
		++i;

		std::unique_ptr<Scope> else_ = nullptr;

		if (i + 1 < tokens->size() && (*tokens)[i + 1].type == TokenType::KEYWORD_ELSE)
		{
			++token_iter;
			++i;
			else_ = scope(i);
		}

		
		return std::make_unique<IFKeyW>(std::move(cond_), std::move(scope_), std::move(else_));
	}

	std::unique_ptr<Node> Parser::WhileS(size_t& i)
	{
		++i;
		++token_iter;
		std::unique_ptr<Node> cond_ = cond(i);
		++token_iter;
		++i;
		std::unique_ptr<Scope> scope_ = scope(i);

		return std::make_unique<WhileKeyW>(std::move(scope_), std::move(cond_));
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
				child = IfElseS(i);
				break;
			}
			case TokenType::KEYWORD_WHILE:
			{
				child = WhileS(i);
				break;
			}

			case TokenType::KEYWORD_PRINT:
			{
				++token_iter;
				++i;
				std::vector<Token>::const_iterator token_iter_b = token_iter;

				child = std::make_unique<PrintKeyW>(lowprior_expr());

				if (!peek().has_value() || token_iter->type != TokenType::ENDCOLOM)
					throw Errors::Syntax("Scope does not close", NULL);

				i += token_iter - token_iter_b;

				break;
			}
			case TokenType::LSCOPE:
				child = scope(i);
				break;

			case TokenType::ENDCOLOM:
				continue;
				break;

			default:
			{
				std::vector<Token>::const_iterator token_iter_b = token_iter;

				child = minprior_expr();
				if (!peek().has_value() || token_iter->type != TokenType::ENDCOLOM)
					throw Errors::Syntax("Scope does not close", NULL);

				i += token_iter - token_iter_b;

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
}