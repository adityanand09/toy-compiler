#include "lexer.hpp"


Token::Token(TokenType token_type, std::string identifier_name, std::int32_t number) : 
    m_token_type(token_type), 
    m_identifier_name(identifier_name), 
    m_number(number) {}

std::string Token::getName() {
    return m_identifier_name;
}

std::int32_t Token::getNumber() {
    return m_number;
}

TokenType Token::getType() {
    return m_token_type;
}

void Token::print() {
    std::string type_str;
    switch (m_token_type) {
        case TOK_DEF: type_str = "TOK_DEF"; break;
        case TOK_RETURN: type_str = "TOK_RETURN"; break;
        case TOK_ID: type_str = "TOK_ID"; break;
        case TOK_NUMBER: type_str = "TOK_NUMBER"; break;
        case TOK_SEMICOLON: type_str = "TOK_SEMICOLON"; break;
        case TOK_LPAREN: type_str = "TOK_LPAREN"; break;
        case TOK_RPAREN: type_str = "TOK_RPAREN"; break;
        case TOK_LBRACE: type_str = "TOK_LBRACE"; break;
        case TOK_RBRACE: type_str = "TOK_RBRACE"; break;
        case TOK_COMMA: type_str = "TOK_COMMA"; break;
        case TOK_PLUS: type_str = "TOK_PLUS"; break;
        case TOK_MINUS: type_str = "TOK_MINUS"; break;
        case TOK_MULT: type_str = "TOK_MULT"; break;
        case TOK_DIV: type_str = "TOK_DIV"; break;
        case TOK_EQ: type_str = "TOK_EQ"; break;
        case TOK_EQEQ: type_str = "TOK_EQEQ"; break;
        case TOK_GT: type_str = "TOK_GT"; break;
        case TOK_GTEQ: type_str = "TOK_GTEQ"; break;
        case TOK_LT: type_str = "TOK_LT"; break;
        case TOK_LTEQ: type_str = "TOK_LTEQ"; break;
        case TOK_EOF: type_str = "TOK_EOF"; break;
        case TOK_ERR: type_str = "TOK_ERR"; break;
        case TOK_UBUF: type_str = "TOK_UBUF"; break;
        default: type_str = "UNKNOWN"; break;
    }
    std::cout << type_str << ' ' << m_identifier_name << ' ' << m_number << std::endl;
}

Lexer::Lexer(std::istream& input_stream) : 
    m_current_token(std::make_unique<Token>(TOK_EOF, "", 0)), 
    m_next_token(std::make_unique<Token>(TOK_UBUF, "", 0)), 
    m_input_stream(input_stream), 
    m_current_char(' '), 
    m_next_char(-1) {}

std::unique_ptr<Token> Lexer::getToken() {
    if (m_next_token->getType() != TOK_UBUF) {
        m_current_token = std::move(m_next_token);
        m_next_token = std::make_unique<Token>(TOK_UBUF, "", 0);
    } else {
        m_current_token = std::move(getNextToken());
    }
    return std::move(m_current_token);
}

std::unique_ptr<Token> Lexer::peekNextToken() {
    m_next_token = getNextToken();
    return std::move(m_next_token);
}

std::unique_ptr<Token> Lexer::getNextToken() {

    m_current_char = getChar();

    // skip spaces
    while (isspace(m_current_char)) {
        m_current_char = getChar();
    }

    // identifier
    if (isalpha(m_current_char)) {
        std::string _identifier(1, m_current_char);
        while (isalnum(m_current_char = getChar())) {
            _identifier += m_current_char;
        }
        ungetChar();
        if (_identifier == "def") return std::make_unique<Token>(TOK_DEF, _identifier, 0);
        else if (_identifier == "return") return std::make_unique<Token>(TOK_RETURN, _identifier, 0);
        else return std::make_unique<Token>(TOK_ID, _identifier, 0);
    }

    // number
    if (isdigit(m_current_char)) {
        int32_t _number = m_current_char - '0';
        while (isdigit(m_current_char = getChar())) {
            _number = _number * 10 + (m_current_char - '0');
        }
        ungetChar();
        return std::make_unique<Token>(TOK_NUMBER, "", _number);
    }

    // some other token
    switch (m_current_char) {
        case ';' : return std::make_unique<Token>(TOK_SEMICOLON, ";", 0);
        case '(' : return std::make_unique<Token>(TOK_LPAREN, "(", 0);
        case ')' : return std::make_unique<Token>(TOK_RPAREN, ")", 0);
        case '{' : return std::make_unique<Token>(TOK_LBRACE, "{", 0);
        case '}' : return std::make_unique<Token>(TOK_RBRACE, "}", 0);
        case ',' : return std::make_unique<Token>(TOK_COMMA, ",", 0);
        case '+' : return std::make_unique<Token>(TOK_PLUS, "+", 0);
        case '-' : return std::make_unique<Token>(TOK_MINUS, "-", 0);
        case '*' : return std::make_unique<Token>(TOK_MULT, "*", 0);
        case '/' : return std::make_unique<Token>(TOK_DIV, "/", 0);
        case '=' : 
            if (getChar() == '=') {
                return std::make_unique<Token>(TOK_EQEQ, "==", 0);
            } else {
                ungetChar();
                return std::make_unique<Token>(TOK_EQ, "=", 0);
            }
        case '>' : 
            if (getChar() == '=') {
                return std::make_unique<Token>(TOK_GTEQ, ">=", 0);
            } else {
                ungetChar();
                return std::make_unique<Token>(TOK_GT, ">", 0);
            }
        case '<' : 
            if (getChar() == '=') {
                return std::make_unique<Token>(TOK_LTEQ, "<=", 0);
            } else {
                ungetChar();
                return std::make_unique<Token>(TOK_LT, "<", 0);
            }
        case EOF :
            return std::make_unique<Token>(TOK_EOF, "0", 0);
        default :
            LexError("invalid input");
            return std::make_unique<Token>(TOK_ERR, "", 0);
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
        m_current_char = m_input_stream.get();
    }
    // std::cout << (char)m_current_char;
    return m_current_char;
}

void Lexer::ungetChar() {
    m_next_char = m_current_char;
}