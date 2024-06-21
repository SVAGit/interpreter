#pragma once

#include <vector>
#include <unordered_map>

#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    Parser(const std::vector<Token>&);
    std::vector<statement> parse();
    void print_tokens();
private:
    std::vector<std::shared_ptr<VarDefinition>> parse_param_list();
    statement parse_decl_statement();
    statement parse_statement();
    statement parse_block_statement();
    std::shared_ptr<JumpStatement> parse_jump_statement();
    std::shared_ptr<ForLoopStatement> parse_for_statement();
    std::shared_ptr<VarDefinition> parse_arg();
    std::shared_ptr<ExprStatement> parse_expr_statement();
    std::shared_ptr<FuncDefinition> parse_function_definition();
    expr parse_binary_expression(int);
    expr parse_base_expression();
    expr parse_parenthesized_expression();
    expr parse_var_value();
    expr parse_cond_statement();
    expr parse_identifier_or_digit();
    std::vector<expr> parse_function_interior();
    std::string parse_var_type();
    std::string parse_identifier();

    bool match(TokenType) const;//проверяет на соответсвие текущий токен из последовательности с токеном указанным в скобках
    std::string extract(TokenType);//возвращает значение текущего токена, если тип указанный в скобках совпал

    static const std::unordered_map<std::string, int> operators;

    std::vector<Token> tokens;
    std::size_t offset;
};