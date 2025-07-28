#include <iostream>
#include <fstream>
#include <cstdint>

enum TokenType {
    TOK_EOF = -1, // end of file
    TOK_FUNC = -2, // func
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
    TOK_GTEQ = -20, // >=
    TOK_LT = -17, // <
    TOK_LTEQ = -19, // <=
    TOK_EQEQ = -18, // ==
};

class Token {
    private:
        TokenType m_token_type;
        std::string m_identifier_name;
        std::int32_t m_number;
    
    public:
        Token(TokenType token_type, std::string identifier_name, std::int32_t number);
        std::string getName();
        std::int32_t getNumber();
        TokenType getType();
};

class Lexer {
    private:
        Token m_current_token;
        Token m_next_token;
        std::ifstream m_file_input_stream;
        int m_current_char;
        int m_next_char;
    
    public:
        Lexer(std::string file_path);
        Token getToken();
        Token peekNextToken();
        void eatToken();

    private:
        Token getNextToken();
        void LexError(std::string err_message);
        char getChar();
        void ungetChar();
};