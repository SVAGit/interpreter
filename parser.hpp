#pragma once

#include <vector>
#include <unordered_map>

#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    Parser(const std::vector<Token>&);
    node parse();
private:
    node parse_binary_expression(int);
    node parse_base_expression();
    node parse_parenthesized_expression();
    std::vector<node> parse_function_interior();

    bool match(TokenType) const;
    std::string extract(TokenType);

    static const std::unordered_map<std::string, int> operators;

    std::vector<Token> tokens;
    std::size_t offset;
};