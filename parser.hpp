#pragma once

#include <vector>
#include <unordered_map>

#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    Parser(const std::vector<Token>&);//конструктор копирования
    node parse();
private:
    expr parse_binary_expression(int);
    expr parse_base_expression();
    expr parse_parenthesized_expression();
    std::vector<expr> parse_function_interior();

    bool match(TokenType) const;//проверяет на соответсвие текущий токен из последовательности с токеном указанным в скобках
    std::string extract(TokenType);//возвращает значение текущего токена, если тип указанный в скобках совпал

    static const std::unordered_map<std::string, int> operators;

    std::vector<Token> tokens;
    std::size_t offset;
};