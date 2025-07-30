#include "parser.hpp"

// NumberExprAST
NumberExprAST::NumberExprAST(std::int32_t val) : m_val(val) {}

// VariableExprAST
VariableExprAST::VariableExprAST(std::string& name) : m_name(name) {}

// BinaryExprAST
BinaryExprAST::BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : 
    m_op(op),
    m_lhs(std::move(lhs)),
    m_rhs(std::move(rhs))
    {}

// CallExprAST
CallExprAST::CallExprAST(std::string& func_name, std::vector<std::unique_ptr<ExprAST>> args) :
    m_func_name(func_name),
    m_args(std::move(args))
    {}

// PrototypeAST
PrototypeAST::PrototypeAST(std::string& callee, std::vector<std::string> args) :
    m_callee(callee),
    m_args(std::move(args))
    {}

// FunctionAST
FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExprAST> body) :
    m_prototype(std::move(prototype)),
    m_body(std::move(body))
    {}


// Parser
Parser::Parser(std::unique_ptr<Lexer> lexer) :
    m_lexer(std::move(lexer))
    {
        m_binop_precedence[TOK_PLUS] = 20;
        m_binop_precedence[TOK_MINUS] = 20;
        m_binop_precedence[TOK_MULT] = 40;
        m_binop_precedence[TOK_DIV] = 40;
    }



// ************************** parsing functions ***************************

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {

    // ID ( decl )
    // decl -> ID names | ē
    // names -> , ID names | ē

    // ID
    if (m_lexer->peekNextToken()->getType() != TOK_ID) {
        ErrorHandler::LogError("Expected function name in prototype");
        return nullptr;
    }
    std::string function_name = m_lexer->getToken()->getName();


    // '('
    if (m_lexer->getToken()->getType() != TOK_LPAREN) {
        ErrorHandler::LogError("Expected '(' in prototype");
        return nullptr;
    }

    // decl
    std::vector<std::string> arg_names;
    if (m_lexer->peekNextToken()->getType() == TOK_ID) {
        arg_names.push_back(m_lexer->getToken()->getName());
        while (m_lexer->peekNextToken()->getType() == TOK_COMMA) {
            m_lexer->getToken(); // eat ','
            if (m_lexer->peekNextToken()->getType() != TOK_ID) {
                ErrorHandler::LogError("Expected parameter name after ',' in function prototype");
            } else {
                arg_names.push_back(m_lexer->getToken()->getName());
            }
        }
    }

    // ')'
    if (m_lexer->getToken()->getType() != TOK_RPAREN) {
        ErrorHandler::LogError("Expected ')' in prototype");
        return nullptr;
    }

    return std::make_unique<PrototypeAST>(function_name, std::move(arg_names));
}

std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
    if (m_lexer->peekNextToken()->getType() != TOK_NUMBER) {
        ErrorHandler::LogError("expecting number");
        return nullptr;
    }
    return std::make_unique<NumberExprAST>(m_lexer->getToken()->getNumber());
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
    std::string identifier = m_lexer->getToken()->getName();
    
    // check for function call
    if (m_lexer->peekNextToken()->getType() != TOK_LPAREN) {
        return std::make_unique<VariableExprAST>(identifier);
    }
    m_lexer->getToken(); // eat '('

    // check for arguments
    std::vector<std::unique_ptr<ExprAST>> args;
    if (m_lexer->peekNextToken()->getType() != TOK_RPAREN) {
        std::unique_ptr<ExprAST> arg = parseExpression();
        if (!arg) {
            ErrorHandler::LogError("expecting expression after '('");
            return nullptr;
        }
        args.push_back(std::move(arg));
        while (m_lexer->peekNextToken()->getType() == TOK_COMMA) {
            m_lexer->getToken(); // eat ','
            arg = parseExpression();
            if (!arg) {
                ErrorHandler::LogError("expecting expression after ','");
                return nullptr;
            }
            args.push_back(std::move(arg));
        }
    }

    // check for ')'
    if (m_lexer->peekNextToken()->getType() != TOK_RPAREN) {
        ErrorHandler::LogError("expecting ')' at the end of function call");
    }
    m_lexer->getToken();

    return std::make_unique<CallExprAST>(identifier, std::move(args));
}

std::unique_ptr<ExprAST> Parser::parseParenExpr() {
    // expecting '('
    if (m_lexer->peekNextToken()->getType() != TOK_LPAREN) {
        return nullptr;
    }
    m_lexer->getToken();

    // expecting expression;
    std::unique_ptr<ExprAST> expr = parseExpression();
    if (!expr) return nullptr;

    // expecting ')'
    if (m_lexer->peekNextToken()->getType() != TOK_RPAREN) {
        return nullptr;
    }
    m_lexer->getToken();

    return std::move(expr);
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    switch (m_lexer->peekNextToken()->getType()) {
        case TOK_ID :
            return parseIdentifierExpr();
        case TOK_NUMBER :
            return parseNumberExpr();
        case TOK_LPAREN :
            return parseParenExpr();
        default :
            // ErrorHandler::LogError("expecting expression");
            return nullptr;
    }
}

std::unique_ptr<ExprAST> Parser::parseExpression() {

    std::unique_ptr<ExprAST> LHS = parsePrimary();
    if (!LHS) return nullptr;
    return parseBinOpRHS(0, std::move(LHS));

}

std:: unique_ptr<ExprAST> Parser::parseBinOpRHS(int expr_prec, std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int tok_prec = getTokenPrecedence(m_lexer->peekNextToken()->getType());

        if (tok_prec < expr_prec) return nullptr;

        int bin_op = m_lexer->getToken()->getType();

        std::unique_ptr<ExprAST> RHS = parsePrimary();

        if (!RHS) return nullptr;

        int next_prec = getTokenPrecedence(m_lexer->peekNextToken()->getType());
        if (tok_prec < next_prec) {
            RHS = parseBinOpRHS(tok_prec + 1, std::move(RHS));
            if (!RHS) return nullptr;
        }
        LHS = std::make_unique<BinaryExprAST>(bin_op, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<FunctionAST> Parser::parseDefinition() {
    // def prototype { body }
    if (m_lexer->peekNextToken()->getType() != TOK_DEF) {
        ErrorHandler::LogError("Expected def before a function definition");
        return nullptr;
    }
    m_lexer->getToken(); // eat one token : def keyword

    // prototype
    std::unique_ptr<PrototypeAST> prototype = parsePrototype();
    if (!prototype) {
        return nullptr;
    }

    // '{'
    if (m_lexer->peekNextToken()->getType() != TOK_LPAREN) {
        ErrorHandler::LogError("Expected '{' at start of function body");
        return nullptr;
    }
    m_lexer->getToken(); // eat one toke : '{'

    // body : single expression for now
    // TODO : multi expression body
    std::unique_ptr<ExprAST> body = parseExpression();
    if (!body) {
        return nullptr;
    }

    // '}'
    if (m_lexer->peekNextToken()->getType() != TOK_RPAREN) {
        ErrorHandler::LogError("Expected '}' at end of function body");
        return nullptr;
    }
    m_lexer->getToken(); // eat one token : '}'

    return std::make_unique<FunctionAST>(std::move(prototype), std::move(body));
}

int Parser::getTokenPrecedence(TokenType token_type) {
    int tok_prec = m_binop_precedence[token_type];
    if (tok_prec <= 0) return -1;
    return tok_prec;
}

void Parser::handleFunctionDefinition() {
    if (parseDefinition()) {
        std::cout << "Parsed a function definition.\n";
    }
}

void Parser::handleTopLevelExpression() {

}

void Parser::parse() {
    while (true) {
        switch(m_lexer->peekNextToken()->getType()) {
            case TOK_EOF :
                return;
            case TOK_SEMICOLON : 
                m_lexer->getToken();
                break;
            case TOK_DEF :
                handleFunctionDefinition();
                break;
            default :
                handleTopLevelExpression();
                break;
        }
    }
}

// ************************** end parsing functions ***********************

// ************************** log error ***********************************

void ErrorHandler::LogError(std::string err_message) {
    std::cout << err_message << std::endl;
}

// ************************** end log error *******************************