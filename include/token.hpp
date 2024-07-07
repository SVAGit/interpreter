#pragma once

#include <string>

enum class TokenType {
	KEYWORD, VARTYPE, IDENTIFIER, CONST, INT_LITERAL, DOUBLE_LITERAL, CHAR_LITERAL, BOOL_LITERAL, OPERATOR, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, SEMICOLON, END
};

struct Token {
	TokenType type;
	std::string value;

	bool operator==(TokenType other_type) const {
		return type == other_type;
	}

	bool operator==(const std::string& other_value) const {
		return value == other_value;
	}
};