#pragma once

#include "lexer.hpp"
#include <vector>
#include <unordered_map>

// Base class for all expression nodes.
class ExprAST {
    private:
    public:
        virtual ~ExprAST() = default;
};

// NumberExprAST - Expression class for numeric literals like "1".
class NumberExprAST : public ExprAST {
    private:
        std::int32_t m_val;
    public:
        NumberExprAST(std::int32_t val);
};

// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    private:
        std::string m_name;
    public:
        VariableExprAST(std::string& name);
};

// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    private:
        int m_op;
        std::unique_ptr<ExprAST> m_lhs, m_rhs;
    public:
        BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs);
};

// CallExprAST - Expression class for function calls
class CallExprAST : public ExprAST {
    private:
        std::string m_func_name;
        std::vector<std::unique_ptr<ExprAST>> m_args;
    public:
        CallExprAST(std::string& func_name, std::vector<std::unique_ptr<ExprAST>> args);
};

// PrototypeAST - This class represents the "prototype" for a function,
// which captures its name, and its argument names (thus implicitly the number
// of arguments the function takes).
class PrototypeAST : public ExprAST {
    private:
        std::string m_callee;
        std::vector<std::string> m_args;
    public:
        PrototypeAST(std::string& callee, std::vector<std::string> args);
};

class FunctionAST : public ExprAST {
    private:
        std::unique_ptr<PrototypeAST> m_prototype;
        std::unique_ptr<ExprAST> m_body;
    public:
        FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body);
};


class Parser {
    private:
        std::unique_ptr<Lexer> m_lexer;
        std::unordered_map<int, int> m_binop_precedence;
    public:
        Parser(std::unique_ptr<Lexer> lexer);
        void parse();
    private:
        std::unique_ptr<ExprAST> parseNumberExpr();
        std::unique_ptr<PrototypeAST> parsePrototype();
        std::unique_ptr<ExprAST> parseIdentifierExpr();
        std::unique_ptr<ExprAST> parseParenExpr();
        std::unique_ptr<ExprAST> parsePrimary();
        std::unique_ptr<ExprAST> parseExpression();
        std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS);
        std::unique_ptr<FunctionAST> parseDefinition();
        int getTokenPrecedence(TokenType token_type);
        void handleFunctionDefinition();
        void handleTopLevelExpression();




};

class ErrorHandler {
    public:
        ErrorHandler() = delete;
        static void LogError(std::string err_message);
};