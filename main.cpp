#include <iostream>
#include <unordered_map>

#include "lexer.hpp"
#include "parser.hpp"
#include "visitor.hpp"

int main() {
    std::string input;
    std::getline(std::cin, input);

    Lexer lexer(input);
    Parser parser(lexer.tokenize());
    parser.print_tokens();
    auto save = parser.parse();
    Printer printer;
    printer.print(save);
    Analyzer analyzer;
    analyzer.analyze(save);
    Executor executor;
    executor.execute(save);
    return 0;
}