#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>
#include <memory>

enum TokenType {
    TOK_ERR = -100,
    TOK_UBUF = -101, // next token is not buffered
    TOK_EOF = -1, // end of file
    TOK_DEF = -2, // function definition
    TOK_RETURN = -3, // return
    TOK_ID = -4, // identifier
    TOK_NUMBER = -5, // number
    TOK_LPAREN = -6, // (
    TOK_RPAREN = -7, // )
    TOK_LBRACE = -8, // {
    TOK_RBRACE = -9, // }
    TOK_COMMA = -10, // ,
    TOK_PLUS = -11, // +
    TOK_MINUS = -12, // -
    TOK_MULT = -13, // *
    TOK_DIV = -14, // /
    TOK_EQ = -15, // =
    TOK_GT = -16, // >
    TOK_GTEQ = -17, // >=
    TOK_LT = -18, // <
    TOK_LTEQ = -19, // <=
    TOK_EQEQ = -20, // ==
    TOK_SEMICOLON = -21, // ;
};

class Token {
    private:
        TokenType m_token_type;
        std::string m_identifier_name;
        std::int32_t m_number;
    
    public:
        Token() = delete;
        Token(TokenType token_type, std::string identifier_name, std::int32_t number);
        std::string getName();
        std::int32_t getNumber();
        TokenType getType();
        void print();
};

class Lexer {
    private:
        std::unique_ptr<Token> m_current_token;
        std::unique_ptr<Token> m_next_token;
        std::istream& m_input_stream;
        int m_current_char;
        int m_next_char;
    
    public:
        Lexer() = delete;
        Lexer(std::istream& input_stream);
        std::unique_ptr<Token> getToken();
        std::unique_ptr<Token> peekNextToken();

    private:
        std::unique_ptr<Token> getNextToken();
        void LexError(std::string err_message);
        char getChar();
        void ungetChar();
};