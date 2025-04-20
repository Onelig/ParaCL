#pragma once

#include <string>
#include <lexer.hpp>
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

        virtual std::optional<int> execute() const = 0;
        virtual int* execute_pointer();
        virtual TokenType getType() const = 0;
        virtual std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> getLRHS();
        virtual ~Node() = default;
    };

    class Scope final : public Node
    {
    public:
        Scope(std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        std::optional<int> execute() const override;
        TokenType getType() const override;
    };

    class BinOper final : public Node
    {
    private:
        TokenType tokentype;
        std::unique_ptr<Node> LHS,
            RHS;

    public:
        BinOper(TokenType oper, std::unique_ptr<Node>&& LHS, std::unique_ptr<Node>&& RHS, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> getLRHS() override;
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };


    class UnOper final : public Node
    {
    private:
        TokenType tokentype;
        std::unique_ptr<Node> element;
        bool rElem;

    public:
        UnOper(TokenType unoper, std::unique_ptr<Node>&& element, bool rElem = true, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
        std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> getLRHS() override;
        virtual int* execute_pointer() override;
    };


    class IFKeyW final : public Node
    {
    private:
        mutable std::unique_ptr<Node> scope, else_;
        std::unique_ptr<Node> condition;

    public:
        IFKeyW(std::unique_ptr<Node>&& scope, std::unique_ptr<Node>&& condition, std::unique_ptr<Node>&& else_ = nullptr, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::pair<std::unique_ptr<Node>*, std::unique_ptr<Node>*> getLRHS() override;
        std::optional<int> execute() const override;
    };


    class WhileKeyW final : public Node
    {
    private:
        mutable std::unique_ptr<Node> scope;
        std::unique_ptr<Node> condition;

    public:
        WhileKeyW(std::unique_ptr<Node>&& scope, std::unique_ptr<Node>&& condition, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };


    class PrintKeyW final : public Node
    {
    private:
        std::unique_ptr<Node> str_cout;

    public:
        PrintKeyW(std::unique_ptr<Node>&& print_node, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };


    class VAR final : public Node
    {
    private:
        std::string name;
        mutable int* value;

    public:
        VAR(const std::string& name, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
        int* execute_pointer() override;
    };


    class Num final : public Node
    {
    private:
        int value;

    public:
        Num(int value, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        Num(const std::string& value, std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };


    class GetNum final : public Node
    {
    public:
        GetNum(std::unique_ptr<Node>&& left = nullptr, std::unique_ptr<Node>&& right = nullptr);
        TokenType getType() const override;
        std::optional<int> execute() const override;
    };



    class Parser
    {
    private:
        std::unique_ptr<Node> root;
        std::shared_ptr<std::vector<Token>> tokens;
        std::vector<Token>::const_iterator token_iter,
            it_end;

        std::optional<const Token> peek();
        std::optional<const Token> get();
        std::unique_ptr<Scope> scope(size_t& i);
        std::unique_ptr<Node> ifOverOper();
        std::unique_ptr<Node> factor();

        std::unique_ptr<Node> maxprior_expr();
        std::unique_ptr<Node> averprior_expr();
        std::unique_ptr<Node> medprior_expr();
        std::unique_ptr<Node> lowprior_expr();
        std::unique_ptr<Node> minprior_expr();

        std::unique_ptr<Node> parse_(size_t begin, size_t end);

        template<typename T>
        std::unique_ptr<Node> IfWhileS(size_t& i, bool SkipCond = false);
    public:
        Parser(std::shared_ptr<std::vector<Token>> token_vec);
        void parse();
        std::unique_ptr<Node> getAST();
    };
}