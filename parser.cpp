#include <string>
#include <vector>
#include <stdexcept>

#include "parser.hpp"

#define MIN_PRECEDENCE 0

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), offset(0) {}

node Parser::parse() {
	return parse_binary_expression(MIN_PRECEDENCE);
}

node Parser::parse_binary_expression(int min_precedence) {
	auto lhs = parse_base_expression();

	for (auto op = tokens[offset].value; operators.contains(op) && operators.at(op) >= min_precedence; op = tokens[offset].value) {
		offset++;
		auto rhs = parse_binary_expression(operators.at(op));
		lhs = std::make_shared<BinaryNode>(op, lhs, rhs);
	}
	return lhs;
}

node Parser::parse_base_expression() {
	if (match(TokenType::NUMBER)) {
		return std::make_shared<NumberNode>(std::stod(extract(TokenType::NUMBER)));
	} else if (match(TokenType::IDENTIFIER)) {
		if (auto identifier = extract(TokenType::IDENTIFIER); match(TokenType::LPAREN)) {
			return std::make_shared<FunctionNode>(identifier, parse_function_interior());
		} else {
			return std::make_shared<IdentifierNode>(identifier);
		}
	} else if (tokens[offset] == "+" || tokens[offset] == "-") {
		offset++;
		return std::make_shared<UnaryNode>(tokens[offset].value, parse_base_expression());
	} else if (match(TokenType::LPAREN)) {
		return parse_parenthesized_expression();
	} else {
		throw std::runtime_error("Syntax error - unexpected token");
	}
}

node Parser::parse_parenthesized_expression() {
	extract(TokenType::LPAREN);
	auto node = parse();
	extract(TokenType::RPAREN);
	return std::make_shared<ParenthesizedNode>(node);
}

std::vector<node> Parser::parse_function_interior() {
	extract(TokenType::LPAREN);
	std::vector<node> args;
	if (!match(TokenType::RPAREN)) {
		while (true) {
			args.push_back(parse());
			if (match(TokenType::COMMA)) {
				extract(TokenType::COMMA);
			} else {
				extract(TokenType::RPAREN);
				break;
			}
		}
	}
	return args;
}

bool Parser::match(TokenType expected_type) const {
	return tokens[offset] == expected_type;
}

std::string Parser::extract(TokenType expected_type) {
	if (!match(expected_type)) {
		throw std::runtime_error("Unexpected token " + tokens[offset].value);
	}
	return tokens[offset++].value;
}

const std::unordered_map<std::string, int> Parser::operators = {
	{"+", 0}, {"-", 0},
	{"*", 1}, {"/", 1},
	{"^", 2}
};