#pragma once

#include <lexer.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>

namespace ParaCL
{
    class Node
    {
    public:
        std::unique_ptr<Node> left = nullptr;
        std::unique_ptr<Node> right = nullptr;

    public:
        Node(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right);
        Node(const Node& other) = delete;
        Node(Node&& other) = delete;
        Node& operator=(const Node& other) = delete;
        Node& operator=(Node&& other) = delete;

        // do smth and return result(nullopt - if keyword)
        virtual std::optional<int> execute() const = 0;
        virtual TokenType getType() const = 0;
        virtual ~Node() = default;
    };

    // + execute_pointer()
    class LValue : public Node
    {
    protected:
         const unsigned short line;
    public:
        LValue(unsigned short line, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right);
        // pointer to changeable result(basically - nullptr)
        virtual int* execute_pointer() = 0;
        unsigned short getLine() const;
    };

    // + getLRHS()
    class LRHS : public Node
    {
    public:
        using Node::Node;
        // receive LHS and RHS(basically - nullptr | nullpt)
        //virtual std::pair<std::unique_ptr<Node>&, std::unique_ptr<Node>&> getLRHS() = 0;
        virtual std::unique_ptr<Node>& getRHS() = 0;
    };

    // base
    class GlobalScope final : public Node // getType() execute()
    {
    public:
        GlobalScope(std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        std::optional<int> execute() const override;
        TokenType getType() const override;
    };

    // base
    class Scope final : public Node // getType() execute()
    {
    public:
        std::unique_ptr<Node> left_ = nullptr;
        std::unique_ptr<Node> right_ = nullptr;

    public:
        Scope(std::unique_ptr<Node>&& left_ = nullptr, std::unique_ptr<Node>&& right_ = nullptr, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        std::optional<int> execute() const override;
        TokenType getType() const override;
    };

    // base + getLRHS() | throw except
    class BinOper final : public LRHS // getLRHS() getType() execute()
    {
    private:
        TokenType tokentype;
        std::unique_ptr<Node> LHS,
                              RHS;
        unsigned short line;

    public:
        BinOper(TokenType oper, std::unique_ptr<Node>&& LHS, std::unique_ptr<Node>&& RHS, unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        // std::pair<std::unique_ptr<Node>&, std::unique_ptr<Node>&> getLRHS() override;
        std::unique_ptr<Node>& getRHS() override;
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base + getLRHS() | throw except
    class BinOperAssign final : public LRHS // getLRHS() getType() execute()
    {
    private:
        TokenType tokentype;
        std::unique_ptr<LValue> LHS;
        std::unique_ptr<Node>   RHS;
        const unsigned short line;

    public:
        BinOperAssign(TokenType oper, std::unique_ptr<LValue>&& LHS, std::unique_ptr<Node>&& RHS, unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        std::unique_ptr<Node>& getRHS() override;
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base | throw except
    class UnOperRv final : public Node // getType() execute()
    {
    private:
        TokenType tokentype;
        std::unique_ptr<Node> element;
        const unsigned short line;

    public:
        UnOperRv(TokenType unoper, std::unique_ptr<Node>&& element, unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base + execute_pointer() | throw except
    class UnOperLv final : public LValue  // getType() execute() execute_pointer()
    {
    private:
        TokenType tokentype;
        std::unique_ptr<LValue> element;
        // true -> should be just with prefix in(de)crement + l-value, false -> others
        bool prefix;
        const unsigned short line;
    public:
        UnOperLv(TokenType unoper, std::unique_ptr<LValue>&& element, unsigned short line, bool prefix = false, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
        int* execute_pointer() override;
    };

    // base | throw except
    class IFKeyW final : public Node // getType() execute()
    {
    private:
        std::unique_ptr<Scope> scope, else_;
        std::unique_ptr<Node> condition;

    public:
        IFKeyW(std::unique_ptr<Node>&& condition, std::unique_ptr<Scope>&& scope, std::unique_ptr<Scope>&& else_, unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base | throw except
    class WhileKeyW final : public Node // getType() execute()
    {
    private:
        std::unique_ptr<Node> scope;
        std::unique_ptr<Node> condition;

    public:
        WhileKeyW(std::unique_ptr<Scope>&& scope, std::unique_ptr<Node>&& condition, unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base
    class PrintKeyW final : public Node // getType() execute()
    {
    private:
        std::unique_ptr<Node> str_cout;

    public:
        PrintKeyW(std::unique_ptr<Node>&& print_node, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base + execute_pointer()
    class VAR final : public LValue // getType() execute() execute_pointer()
    {
    private:
        std::string name;
        int* value;

    public:
        VAR(const std::string& name, unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
        int* execute_pointer() override;
    };

    // base
    class Num final : public Node // getType() execute()
    {
    private:
        int value;

    public:
        Num(int value, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        Num(const std::string& value, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };

    // base | throw except
    class GetNum final : public Node // getType()  execute()
    {
        const unsigned short line;
    public:
        GetNum(unsigned short line, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };



    class Parser final
    {
    private:
        std::unique_ptr<Node> root;
        std::shared_ptr<std::vector<Token>> tokens;
        std::vector<Token>::const_iterator token_iter;
        unsigned short line_ = 0;

        std::optional<const Token> peek(bool updateLine = false);
        std::optional<const Token> get();
        std::unique_ptr<Node> factor();
        std::unique_ptr<Node> ifOverOper();

        std::unique_ptr<Node> maxprior_expr();
        std::unique_ptr<Node> averprior_expr();
        std::unique_ptr<Node> medprior_expr();
        std::unique_ptr<Node> lowprior_expr();
        std::unique_ptr<Node> minprior_expr();

        std::unique_ptr<Node> parse_(size_t begin, size_t end);

        std::unique_ptr<Node> cond(size_t& i);
        std::unique_ptr<Scope> scope(size_t& i);

        std::unique_ptr<Node> IfElseS(size_t& i);
        std::unique_ptr<Node> WhileS(size_t& i);
    public:
        Parser(std::shared_ptr<std::vector<Token>> token_vec);
        void parse();
        std::unique_ptr<Node> getAST();
    };
}