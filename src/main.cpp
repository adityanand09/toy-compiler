#include "lexer.hpp"

int main() {
    Lexer lexer(std::cin);
    Token tok = lexer.getToken();
    while (tok.getType() != TOK_EOF) {
        // std::cout << tok.getType() << ' ' << tok.getName() << ' ' << tok.getNumber() << '\n';
        tok.print();
        tok = lexer.getToken();
    }
}