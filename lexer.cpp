#include <string>
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <cctype>

#include "lexer.hpp"

Lexer::Lexer(const std::string& input) : input(input), offset(0) {}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> tokens;
	while(input[offset]) {
		if (std::isblank(input[offset])) {
			offset++;
		} else if (std::isdigit(input[offset]) || input[offset] == '.') {
			tokens.push_back(extract_number());
		} else if (std::isalpha(input[offset]) || input[offset] == '_') {
			tokens.push_back(extract_identifier());
		} else if (metachars.contains(input[offset])) {
			tokens.push_back(extract_operator());
		} else if (input[offset] == '(') {
			offset++;
			tokens.push_back(Token{TokenType::LPAREN, "("});
		} else if (input[offset] == ')') {
			offset++;
			tokens.push_back(Token{TokenType::RPAREN, ")"});
		} else if (input[offset] == '{') {
			offset++;
			tokens.push_back(Token{TokenType::LBRACE, "("});
		} else if (input[offset] == '}') {
			offset++;
			tokens.push_back(Token{TokenType::RBRACE, ")"});
		} else if (input[offset] == ',') {
			offset++;
			tokens.push_back(Token{TokenType::COMMA, ","});
		} else if (input[offset] == ';') {
			offset++;
			tokens.push_back(Token{TokenType::SEMICOLON, ","});
		} else {
			throw std::runtime_error("Unknown symbol");
		}
	}
	tokens.push_back(Token{TokenType::END, ""});
	return tokens;
}

Token Lexer::extract_identifier() {
	std::size_t i = 0;
	std::string op;
	for (; std::isalnum(input[offset + i]) || input[offset + i] == '_'; ++i){
		op += input[offset + i];
	}
	if(keyWords.contains(op)){
		Token token{TokenType::KEYWORDS, std::string(input, offset, i)};
	}else if(varType.contains(op)){
		Token token{TokenType::VARTYPE, std::string(input, offset, i)};
	}else{
		Token token{TokenType::IDENTIFIER, std::string(input, offset, i)};
	}
	offset += i;
	return token;
}

Token Lexer::extract_number() {
	std::size_t i = 0;
	for(; std::isdigit(input[offset + i]); i++);
	auto int_len = i;
	if (input[offset + i] == '.') {
		i++;
		auto j = i;
		for (; std::isdigit(input[offset + i]); ++i);
		if (i - j == 0 && int_len == 0) {
			throw std::runtime_error("Missing int and float part of number");
		}
	}
	Token token{TokenType::NUMBER, std::string(input, offset, i)};
	offset += i;
	return token;
}

Token Lexer::extract_operator() {
	size_t i = 0;
	std::string op;
	for (; metachars.contains(input[offset + i]); i++) {
		op += input[offset + i];
		if (!operators.contains(op)) {
			if (i == 0) {//cобытие, которое никогда не случится
				throw std::runtime_error("Invalid operator");
			}
			op.pop_back();
			break;
		}
	}
	Token token{TokenType::OPERATOR, op};
	offset += i;
	return token;
}

const std::string Lexer::metachars = "+-*/^=!";
const std::unordered_set<std::string> Lexer::operators = {"+", "-", "*", "/", "^", "=", "==", "!=", "+=", "-=", "!"};
const std::unordered_set<std::string> Lexer::keyWords = {"if", "while", "for"};
const std::unordered_set<std::string> Lexer::varTypes = { "int", "char", "float", "double"};