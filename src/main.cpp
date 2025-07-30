#include "lexer.hpp"

int main() {
    Lexer lexer(std::cin);
    std::unique_ptr<Token> tok = lexer.getToken();
    while (tok->getType() != TOK_EOF) {
        // std::cout << tok.getType() << ' ' << tok.getName() << ' ' << tok.getNumber() << '\n';
        tok->print();
        tok = lexer.getToken();
    }
}