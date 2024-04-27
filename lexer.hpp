#pragma once

#include <vector>
#include <unordered_set>

#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string&);//конструктор копирования
    std::vector<Token> tokenize();//создание вектора токенов
private:
    Token extract_number();
    Token extract_identifier();
    Token extract_operator();

    static const std::string metachars;
    static const std::unordered_set<std::string> operators;
    static const std::unordered_set<std::string> keyWords;
    static const std::unordered_ste<std::string> varTypes;

    std::string input;
    std::size_t offset;
};