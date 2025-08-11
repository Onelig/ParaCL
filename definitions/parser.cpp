#include <parser.hpp>
#include <LanguageErrors.hpp>
#include <algorithm>
#include <map>
#include <iostream>

namespace ParaCL
{
	// contains all exist variables
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


	
	//
	// ------------------- LValue -------------------
	//
	LValue::LValue(unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line(line), Node(std::move(left), std::move(right))
	{ }

	unsigned short LValue::getLine() const
	{
		return line;
	}



	//
	// ------------------- GlobalScope -------------------
	//
	GlobalScope::GlobalScope(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: Node(std::move(left), std::move(right))
	{ }

	std::optional<int> GlobalScope::execute() const
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

		return std::nullopt;
	}

	TokenType GlobalScope::getType() const
	{
		return TokenType::NONE;
	}


	//
	// ------------------- Scope -------------------
	//
	Scope::Scope(std::unique_ptr<Node>&& left_, std::unique_ptr<Node>&& right_, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: left_(std::move(left_)), right_(std::move(right_)), Node(std::move(left), std::move(right))
	{ }

	std::optional<int> Scope::execute() const
	{
		bool RHS_m = true;
		const std::unique_ptr<Node>* LHS_t = &left_,
								   * RHS_t = &right_;

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
	BinOper::BinOper(TokenType oper, std::unique_ptr<Node>&& LHS, std::unique_ptr<Node>&& RHS, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line(line), tokentype(oper), LHS(std::move(LHS)), RHS(std::move(RHS)), LRHS(std::move(left), std::move(right))
	{
		if (this->LHS == nullptr || this->RHS == nullptr)
			throw Errors::Syntax("Not full expression", this->line);
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
			throw Errors::Syntax("Incorrect binary operator", this->line);
		}

		return std::make_optional<int>(result);
	}



	//
	// ------------------- BinOperAssign -------------------
	//
	BinOperAssign::BinOperAssign(TokenType oper, std::unique_ptr<LValue>&& LHS, std::unique_ptr<Node>&& RHS, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line(line), tokentype(oper), LHS(std::move(LHS)), RHS(std::move(RHS)), LRHS(std::move(left), std::move(right))
	{
		if (this->LHS == nullptr || this->RHS == nullptr)
			throw Errors::Syntax("Not full expression", this->line);
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
		default: throw Errors::Syntax("Incorrect compound assignment operator", line);
		}

		return std::make_optional<int>(*ptr);
	}



	//
	// ------------------- UnOperRv -------------------
	//
	UnOperRv::UnOperRv(TokenType unoper, std::unique_ptr<Node>&& element, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line(line), tokentype(unoper), element(std::move(element)), Node(std::move(left), std::move(right))
	{ 
		if (this->element == nullptr)
			throw Errors::Syntax("Not full expression", this->line);
	}

	TokenType UnOperRv::getType() const
	{
		return tokentype;
	}

	std::optional<int> UnOperRv::execute() const
	{
		int result = 0;

		switch (tokentype)
		{
		case TokenType::UNOP_OPPNUM: result = !(element->execute().value()); break;
		case TokenType::OP_MINUS:  result = -element->execute().value();   break;
		default: throw Errors::Syntax("Incorrect un-operator", line);
		}

		return std::make_optional<int>(result);
	}



	//
	// ------------------- UnOperLv -------------------
	//
	UnOperLv::UnOperLv(TokenType unoper, std::unique_ptr<LValue>&& element, unsigned short line, bool prefix, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line(line), tokentype(unoper), element(std::move(element)), prefix(prefix), LValue(line ,std::move(left), std::move(right))
	{
		if (this->element == nullptr)
			throw Errors::Syntax("Not full expression", line);

		else if (tokentype != TokenType::UNOP_DEC && tokentype != TokenType::UNOP_INC)
			throw Errors::Syntax("Incorrect operator with value", line);
	}

	TokenType UnOperLv::getType() const
	{
		return tokentype;
	}

	std::optional<int> UnOperLv::execute() const
	{
		int result = 0;

		switch (tokentype)
		{
		case TokenType::UNOP_INC:
			if (prefix)
				result = ++(*element->execute_pointer());

			else
				result = (*element->execute_pointer())++;

			break;

		case TokenType::UNOP_DEC:
			if (prefix)
				result = --(*element->execute_pointer());

			else
				result = (*element->execute_pointer())--;

			break;

		default: throw Errors::Syntax("Incorrect un-operator", line);
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
				result = &(++(*element->execute_pointer()));
				break;

			case TokenType::UNOP_DEC:
				result = &(--(*element->execute_pointer()));
				break;

			default: throw Errors::Syntax("Incorrect un-operator", line);
			}
		}
		else
			throw Errors::Syntax("Cannot be used with r-value", line);


		return result;
	}


	//
	// ------------------- IFKeyW -------------------
	//
	IFKeyW::IFKeyW(std::unique_ptr<Node>&& condition, std::unique_ptr<Scope>&& scope, std::unique_ptr<Scope>&& else_, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: scope(std::move(scope)), line(line), else_(std::move(else_)), condition(std::move(condition)), Node(std::move(left), std::move(right))
	{ 
		if (this->condition == nullptr)
			throw Errors::Syntax("Condition was skipped", this->line); 
	}

	TokenType IFKeyW::getType() const
	{
		return TokenType::KEYWORD_IF;
	}

	std::optional<int> IFKeyW::execute() const
	{
		std::optional<int> p = condition->execute();
		if (p.value())
			scope->execute();

		else if (else_)
			else_->execute();
			
		return std::nullopt;
	}


	//
	// ------------------- WhileKeyW -------------------
	//
	WhileKeyW::WhileKeyW(std::unique_ptr<Scope>&& scope, std::unique_ptr<Node>&& condition, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: scope(std::move(scope)), line(line), condition(std::move(condition)), Node(std::move(left), std::move(right))
	{
		if (this->condition == nullptr)
			throw Errors::Syntax("Condition was skipped", this->line);
	}

	TokenType WhileKeyW::getType() const
	{
		return TokenType::KEYWORD_WHILE;
	}

	std::optional<int> WhileKeyW::execute() const
	{
		std::optional<int> p = condition->execute();

		while (p.value())
		{
			scope->execute();
			p = condition->execute();
		}

		return std::nullopt;
	}



	//
	// ------------------- PrintKeyW -------------------
	//
	PrintKeyW::PrintKeyW(std::unique_ptr<Node>&& print_node, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line_(line), str_cout(std::move(print_node)), Node(std::move(left), std::move(right))
	{ }

	TokenType PrintKeyW::getType() const
	{
		return TokenType::KEYWORD_PRINT;
	}

	std::optional<int> PrintKeyW::execute() const
	{
		std::optional<int> v = str_cout->execute();

		std::cout << v.value() << std::endl;

		return std::nullopt;
	}



	//
	// ------------------- VAR -------------------
	//
	VAR::VAR(const std::string& name, unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: value(nullptr), name(name), LValue(line, std::move(left), std::move(right))
	{ }

	TokenType VAR::getType() const
	{
		return TokenType::VAR;
	}

	std::optional<int> VAR::execute() const
	{
		std::unordered_map<std::string, int>::iterator iter = VarInt.find(name);
		if (iter == VarInt.end())
			throw Errors::Syntax("Value was not declared in this scope", line);

		return iter->second;
	}

	int* VAR::execute_pointer()
	{
		bool isExist = true;
		if (VarInt.find(name) == VarInt.end())
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
	GetNum::GetNum(unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
		: line(line), Node(std::move(left), std::move(right))
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
			throw Errors::Runtime("Input failure", line);

		return tvalue;
	}




	//
	// ------------------- Parser -------------------
	//

	inline std::optional<const Token> Parser::peek(bool updateLine)
	{
		if (token_iter == tokens->end())
			return std::nullopt;

		if (updateLine)
			line_ = token_iter->line;
		
		return *token_iter;
	}

	inline std::optional<const Token> Parser::get()
	{
		line_ = token_iter->line;
		return *(token_iter++);
	}

	std::unique_ptr<Node> Parser::factor()
	{
		if (!peek().has_value())
			throw Errors::Syntax("Value was skipped", line_);

		Token token = peek(true).value();

		if (token.type == TokenType::NUMBER)
			return std::make_unique<Num>(get().value().value);

		else if (token.type == TokenType::VAR)
		{
			std::unique_ptr<LValue> nd = std::make_unique<VAR>(token.value, line_);
			get();

			if (peek() && (peek(true).value().type == TokenType::UNOP_DEC || peek(true).value().type == TokenType::UNOP_INC))
			{
				nd = std::make_unique<UnOperLv>(peek().value().type, std::move(nd), line_, false);
				get();
			}
			return std::move(nd);
		}
		else if (token.type == TokenType::OPEN_PAREN)
		{
			get();
			std::unique_ptr<Node> expr = lowprior_expr();
			if (!peek().has_value() || peek().value().type != TokenType::CLOSE_PAREN)
				throw Errors::Syntax("Paren was not closed", line_);

			get();

			if ((typeid(*expr) == typeid(VAR) || typeid(*expr) == typeid(UnOperLv)) && peek() && (peek().value().type == TokenType::UNOP_INC || peek().value().type == TokenType::UNOP_DEC))
			{
				expr = std::make_unique<UnOperLv>(peek().value().type, std::unique_ptr<LValue>(static_cast<LValue*>(expr.release())), line_, false);
				get();
			}
			return expr;
		}
		else if (token.type == TokenType::GETNUM)
		{
			get();
			return std::make_unique<GetNum>(line_);
		}

		return ifOverOper();
	}

	std::unique_ptr<Node> Parser::ifOverOper()
	{
		Token token = peek(true).value();

		if (token.priority != Priority::OVER && token.type != TokenType::OP_MINUS)
			throw Errors::Syntax("Missed value", line_);

		if (token.type == TokenType::UNOP_DEC || token.type == TokenType::UNOP_INC)
		{
			TokenType tt_type = get().value().type;
			return std::make_unique<UnOperLv>(tt_type, std::unique_ptr<LValue>(dynamic_cast<LValue*>(factor().release())), line_, true);
		}

		TokenType tt_type = get().value().type;

		return std::make_unique<UnOperRv>(tt_type, factor(), line_);
	}

	std::unique_ptr<Node> Parser::maxprior_expr()
	{
		std::unique_ptr<Node> result = factor();
		std::unique_ptr<Node> return_v = nullptr;

		while (peek() && (peek().value().priority == Priority::MAX))
		{
			Token tt = get().value();

			return_v = std::make_unique<BinOper>(tt.type, std::move(return_v ? return_v : result), std::move(factor()), line_);
		}

		return return_v ? std::move(return_v) : std::move(result);
	}

	std::unique_ptr<Node> Parser::averprior_expr()
	{
		std::unique_ptr<Node> result = maxprior_expr();
		std::unique_ptr<Node> return_v = nullptr;

		while (peek() && (peek().value().priority == Priority::AVER))
		{
			Token tt = get().value();

			return_v = std::make_unique<BinOper>(tt.type, std::move(return_v ? return_v : result), std::move(maxprior_expr()), line_);
		}

		return std::move(return_v ? return_v : result);
	}

	std::unique_ptr<Node> Parser::medprior_expr()
	{
		std::unique_ptr<Node> result = averprior_expr();
		std::unique_ptr<Node> return_v = nullptr;

		while (peek() && (peek().value().priority == Priority::MED))
		{
			Token tt = get().value();

			return_v = std::make_unique<BinOper>(tt.type, std::move(return_v ? return_v : result), std::move(averprior_expr()), line_);
		}

		return std::move(return_v ? return_v : result);
	}

	std::unique_ptr<Node> Parser::lowprior_expr()
	{
		std::unique_ptr<Node> result = medprior_expr();
		std::unique_ptr<Node> return_v = nullptr;

		while (peek() && (peek().value().priority == Priority::LOW))
		{
			Token tt = get().value();

			return_v = std::make_unique<BinOper>(tt.type, std::move(return_v ? return_v : result), std::move(medprior_expr()), line_);
		}

		return std::move(return_v ? return_v : result);
	}

	std::unique_ptr<Node> Parser::minprior_expr()
	{
		std::unique_ptr<Node> result = lowprior_expr();
		std::unique_ptr<Node> return_v = nullptr;
		std::unique_ptr<Node>* result_v_p = &return_v;

		while (peek() && (peek().value().priority == Priority::MIN))
		{
			Token tt = get().value();

			if (*result_v_p)
			{	
				LRHS* lrhs_ = static_cast<LRHS*>(result_v_p->get());

				std::unique_ptr<LValue> RHS_(dynamic_cast<LValue*>(lrhs_->getRHS().release()));

				if (RHS_ == nullptr)
					throw Errors::Syntax("Cannot modify r-value", line_);

				(lrhs_->getRHS()) = std::make_unique<BinOperAssign>(tt.type, std::move(RHS_), std::move(lowprior_expr()), line_);
				result_v_p = &lrhs_->getRHS();
			}

			else
			{
				if (typeid(*result) != typeid(VAR) && typeid(*result) != typeid(UnOperLv)) // !!!!!!!!!!!!!
					throw Errors::Syntax("Cannot assign r-value", line_);

				std::unique_ptr<LValue> vl(static_cast<LValue*>(result.release()));
				(*result_v_p) = std::make_unique<BinOperAssign>(tt.type, std::move(vl), std::move(lowprior_expr()), line_);
			}
		}

		return return_v ? std::move(return_v) : std::move(result);
	}


	std::unique_ptr<Node> Parser::cond(size_t& i)
	{
		// condition from '(' to ')'
		if (!peek().has_value() || peek()->type != TokenType::OPEN_PAREN)
			throw Errors::Syntax("Cannot find condition", line_);

		std::vector<Token>::const_iterator token_iter_b = token_iter;
		get(); // skip '('
		
		std::unique_ptr<Node> lw_expr = lowprior_expr();
		if (!peek().has_value() || peek()->type != TokenType::CLOSE_PAREN)
			throw Errors::Syntax("Condition does not correct", line_);

		i += token_iter - token_iter_b;

		return lw_expr;
	}

	std::unique_ptr<Scope> Parser::scope(size_t& i)
	{
		if (!peek().has_value() || peek()->type != TokenType::LSCOPE)
			throw Errors::Syntax("Scope should be opened { ... }", line_);

		size_t sScope = 0;

		auto end_scope = std::find_if(token_iter, tokens->cend(), [&sScope](const Token& token)
			{
				if (token.type == TokenType::LSCOPE)
					++sScope;

				else if (token.type == TokenType::RSCOPE)
					--sScope;

				return !sScope;
			});

		if (end_scope == tokens->cend())
			throw Errors::Syntax("Scope should be closed { ... }", line_);

		size_t newI = i + (end_scope - token_iter);
		auto b = parse_(i + 1, newI);

		get();

		if (!peek().has_value() || peek()->type != TokenType::RSCOPE)
			throw Errors::Syntax("Scope should be closed { ... }", line_);
			
		i = newI;
		return std::make_unique<Scope>(std::move(b->left), std::move(b->right));
	}

	std::unique_ptr<Node> Parser::IfElseS(size_t& i)
	{
		get();
		++i;
		std::unique_ptr<Node> cond_ = cond(i);
		get();
		++i;
		std::unique_ptr<Scope> scope_ = scope(i);
		get();
		++i;

		std::unique_ptr<Scope> else_ = nullptr;

		if (peek().has_value() && peek()->type == TokenType::KEYWORD_ELSE)
		{
			get();
			++i;
			else_ = scope(i);
		}

		
		return std::make_unique<IFKeyW>(std::move(cond_), std::move(scope_), std::move(else_), line_);
	}

	std::unique_ptr<Node> Parser::WhileS(size_t& i)
	{
		++i;
		get();
		std::unique_ptr<Node> cond_ = cond(i);
		get();
		++i;
		std::unique_ptr<Scope> scope_ = scope(i);

		return std::make_unique<WhileKeyW>(std::move(scope_), std::move(cond_), line_);
	}


	std::unique_ptr<Node> Parser::parse_(size_t begin, size_t end)
	{
		std::unique_ptr<Node> root_ = std::make_unique<GlobalScope>();
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
				get();
				++i;

				std::vector<Token>::const_iterator token_iter_b = token_iter;

				child = std::make_unique<PrintKeyW>(lowprior_expr(), line_);

				if (!peek().has_value() || peek()->type != TokenType::ENDCOLOM)
					throw Errors::Syntax("Endcolom was skipped", line_);


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
				if (!peek().has_value() || peek()->type != TokenType::ENDCOLOM)
					throw Errors::Syntax("Symb ; was skipped", line_);

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
		root = parse_(0, tokens->size());
	}

	std::unique_ptr<Node> Parser::getAST()
	{
		return std::move(root);
	}
}