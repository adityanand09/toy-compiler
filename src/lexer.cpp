#include "lexer.hpp"


Token::Token(TokenType token_type, std::string identifier_name, std::int32_t number) : m_token_type(token_type), m_identifier_name(identifier_name), m_number(number) {}

std::string Token::getName() {
    return m_identifier_name;
}

std::int32_t Token::getNumber() {
    return m_number;
}

TokenType Token::getType() {
    return m_token_type;
}

Lexer::Lexer(std::string file_path) : m_current_token(TOK_EOF, "", 0), m_next_token(TOK_EOF, "", 0), m_file_input_stream(file_path, std::ifstream::in), m_current_char(' '), m_next_char(' ') {}

Token Lexer::getToken() {
    if (m_current_token.getType() == TOK_EOF) {
        eatToken();
    }
    return m_current_token;
}

Token Lexer::peekNextToken() {
}

void Lexer::eatToken() {
    if (m_next_token.getType() == TOK_EOF) m_current_token = getNextToken();
    else m_current_token = m_next_token;
    m_next_token = getNextToken();
}

Token Lexer::getNextToken() {

    char current_char = ' ';

    // skip spaces
    while (isspace(current_char)) {
        current_char = getChar();
    }

    // identifier
    if (isalpha(current_char)) {
        std::string _identifier = "" + current_char;
        while (isalnum(current_char = getChar())) {
            _identifier += current_char;
        }
        if (_identifier == "func") return Token(TOK_FUNC, _identifier, 0);
        else return Token(TOK_ID, _identifier, 0);
    }

    // number
    if (isdigit(current_char)) {
        int32_t _number = current_char - '0';
        while (isdigit(current_char = getChar())) {
            _number = _number * 10 + (current_char - '0');
        }
        return Token(TOK_NUMBER, "", _number);
    }

    // some other token
    switch (current_char) {
        case '(' : return Token(TOK_LPAREN, "", 0);
        case ')' : return Token(TOK_RPAREN, "", 0);
        case '{' : return Token(TOK_LBRACE, "", 0);
        case '}' : return Token(TOK_RBRACE, "", 0);
        case ',' : return Token(TOK_COMMA, "", 0);
        case '+' : return Token(TOK_PLUS, "", 0);
        case '-' : return Token(TOK_MINUS, "", 0);
        case '*' : return Token(TOK_MULT, "", 0);
        case '/' : return Token(TOK_DIV, "", 0);
        case '=' : 
            if (getChar() == '=') {
                return Token(TOK_EQEQ, "", 0);
            } else {
                ungetChar();
                return Token(TOK_EQ, "", 0);
            }
        case '>' : 
            if (getChar() == '=') {
                return Token(TOK_GTEQ, "", 0);
            } else {
                return Token(TOK_GT, "", 0);
            }
        case '<' : 
            if (getChar() == '=') {
                return Token(TOK_LTEQ, "", 0);
            } else {
                return Token(TOK_LT, "", 0);
            }
        default :
            LexError("invalid input");
    }
}

void Lexer::LexError(std::string err_message) {
    std::cout << err_message << std::endl;
}

char Lexer::getChar() {
    if (m_next_char != -1) {
        m_current_char = m_next_char;
        m_next_char = -1;
    } else {
        m_current_char = getchar();
    }
    return m_current_char;
}

void Lexer::ungetChar() {
    m_next_char = m_current_char;
}

// getChar -> a b c d
// ungetChar -> 