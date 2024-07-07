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
		if (std::isblank(input[offset]) || input[offset] == '\n') {
			offset++;
		} else if (std::isdigit(input[offset]) || input[offset] == '.') {
			tokens.push_back(extract_number());
		} else if (std::isalpha(input[offset]) || input[offset] == '_' || input[offset] == '\'') {
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
			tokens.push_back(Token{TokenType::LBRACE, "{"});
		} else if (input[offset] == '}') {
			offset++;
			tokens.push_back(Token{TokenType::RBRACE, "}"});
		} else if (input[offset] == ',') {
			offset++;
			tokens.push_back(Token{TokenType::COMMA, ","});
		} else if (input[offset] == ';') {
			offset++;
			tokens.push_back(Token{TokenType::SEMICOLON, ";"});
		} else {
			throw std::runtime_error("Unknown symbol" + input[offset]);
		}
	}
	tokens.push_back(Token{TokenType::END, ""});
	return tokens;
}

Token Lexer::extract_identifier() {
	std::size_t i = 0;
	std::string op;
	if(input[offset] == '\''){
		if(input[offset + 2] == '\''){
			op += input[offset + 1];
			offset += 3;
			Token token{TokenType::CHAR_LITERAL, op};
			return token;
		}else{
			throw std::runtime_error("invalid char literal");
		}
	}
	/* if(input[offset] == '\"'){
		i++;
		for(;input[offset + i] != '\"' || offset + i < input.size(); i++){
			op += input[offset + i];
		}
		if(offset + i == input.size()){
			throw std::runtime_error("Infinity string");
		}
		offset += i;
		Token token(TokenType::STRING_LITERAL, op);
		return token;
	} */
	for (; std::isalnum(input[offset + i]) || input[offset + i] == '_'; ++i){
		op += input[offset + i];
	}
	if(keyWords.contains(op)){
		if(op == "true" || op == "false"){
			Token token{TokenType::BOOL_LITERAL, std::string(input, offset, i)};
			offset += i;
			return token;
		}
		Token token{TokenType::KEYWORD, std::string(input, offset, i)};
		offset += i;
		return token;
	}else if(varTypes.contains(op)){
		Token token{TokenType::VARTYPE, std::string(input, offset, i)};
		offset += i;
		return token;
	}else if(op == "const"){
		Token token{TokenType::CONST, std::string(input, offset, i)};
		offset += i;
		return token;
	}else{
		Token token{TokenType::IDENTIFIER, std::string(input, offset, i)};
		offset += i;
		return token;
	}
}

Token Lexer::extract_number() {
	std::size_t i = 0;
	for(; std::isdigit(input[offset + i]); i++);
	auto int_len = i;
	if(input[offset + i] == '.') {
		i++;
		auto j = i;
		for (; std::isdigit(input[offset + i]); ++i);
		if (i - j == 0 && int_len == 0) {
			throw std::runtime_error("Missing int and float part of number");
		}
		Token token{TokenType::DOUBLE_LITERAL, std::string(input, offset, i)};
		offset += i;
		return token;
	}else{
		Token token{TokenType::INT_LITERAL, std::string(input, offset, i)};
		offset += i;
		return token;
	}
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

const std::string Lexer::metachars = "+-*/^=!<>|&";
const std::unordered_set<std::string> Lexer::operators = {"+", "-", "*", "/", "^", "=", "==", "!=", "+=", "-=", "!", "++", "--", "<", ">", "<=", ">=", "||", "&&", "|", "&"};
const std::unordered_set<std::string> Lexer::keyWords = {"if", "while", "for", "return", "break", "continue", "true", "false"};
const std::unordered_set<std::string> Lexer::varTypes = { "int", "char", "float", "double", "bool", "void"};