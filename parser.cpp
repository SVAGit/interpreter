#include <string>
#include <vector>
#include <stdexcept>

#include "parser.hpp"

#define MIN_PRECEDENCE 0

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), offset(0) {}

node Parser::parse() {
	return parse_function_definition();
}

node Parser::parse_function_definition(){
	return make_shared<FuncDefinition>(parse_var_type(), parse_identifier(), parse_param_list(), parse_block_statements());
}

std::string Parser::parse_var_type(){
	return extract(TokenType::VARTYPE);
}

std::string Parser::parse_identifier(){
	return extract(TokenType::IDENTIFIER);
}

std::vector<std::shared_ptr<VarDefinition>> Parser::parse_param_list(){
	extract(TokenType::LPAREN);
	std::vector<std::shared_ptr<VarDefinition>>> args;
	if (!match(TokenType::RPAREN)) {
		while (true) {
			args.push_back(parse_arg());
			if (match(TokenType::COMMA)) {
				extract(TokenType::COMMA);
			} else {
				extract(TokenType::RPAREN);
				break;
			}
		}
	}else {
		extract(TokenType::RPAREN);
	}
	return args;
}

std::shared_ptr<VarDefinition> Parser::parse_arg(){
	return make_shared<VarDefinition>(extract(TokenType::VARTYPE), extract(TokenType::IDENTIFIER), nullptr);
}

std::vector<statement> Parser::parse_block_statements(){
	extract(TokenType::LBRACE);
	std::vector<statement> commands;
	if (!match(TokenType::RBRACE)) {
		while (true) {
			commands.push_back(parse_statement());
			if (match(TokenType::RBRACE)) {
				extract(TokenType::RBRACE);
				break;
			}
		}
	}else {
		extract(TokenType::RBRACE);
	}
	return commands;
}

statement Parser::parse_statement(){
	if(tokens[offset].value == "if"){
		offset++;
		auto expr = parse_cond_statement();
		return make_shared<CondStatement>(expr, parse_block_statements());
	}else if(tokens[offset].value == "while"){
		offset++;
		auto expr = parse_cond_statement();
		return make_shared<WhileLoopStatementLoopStatement>(expr, parse_block_statement());
	}else if(tokens[offset].value == "for"){
		offset++;
		return parse_for_statement();
	}else if(tokens[offset].type == TokenType::VARTYPE){
		return parse_decl_statement();
	}else if(token[offset].value == "break" || token[offset].value == "continue", token[offset].value == "return"){
		return parse_jump_statement();
	}else{
		throw std::runtime_error("Syntax error, unknown statement");
	}
}

statement Parser::parse_decl_statement(){
	std::string type = extract(TokenType::VARTYPE);
	std::string name = extract(TokenType::IDENTIFIER);
	if(tokens[offset].type ==TokenType::LPAREN){
		return std::make_shared<FuncDefinition>(type, name, parse_param_list(), parse_block_statements());
	}else{
		offset++;
		return std::make_shared<VarDefinition>(type, name, parse_var_value());
	}
}

expr Parser::parse_var_value(){
	expr retValue;
	if(tokens[offset].value == "="){
		offset++;
		auto retVal = parse_binary_expression(MIN_PRECEDENCE);
		extract(TokenType::SEMICOLON);
		return retVal;
	}else{
		extract(TokenType::SEMICOLON);
		return nullptr;
	}
}

expr Parser::parse_cond_statement(){
	extract(TokenType::LPAREN);
	expr retVal = parse_binary_expression(MIN_PRECEDENCE);
	extract(TokenType::RPAREN);
	return retVal;
}

std::shared_ptr<JumpStatement> Parser::parse_jump_statement(){
	std::string val = tokens[offset++].value;
	if(val != "return"){
		extract(TokenType::SEMICOLON);
		return make_shared<JumpStatement>(val, nullptr);
	}else{
		statement expr = parse_statement();
		extract(TokenType::SEMICOLON);
		return make_shared<JumpStatement>(val, expr);
	}
}

std::shared_ptr<ForLoopStatement> Parser::parse_for_statement(){
	extract(TokenType::LPAREN);
	std::vector<statement> preInstructions;
	expr condition;
	std::vector<statement> postInstructions;
	if(!match(TokenType::SEMICOLON)){
		while(true){
			preInstructions.push_back(parse_comma_statement());
			if(match(TokenType::SEMICOLON)){
				extract(TokenType::SEMICOLON)
				break;
			}
		}
	}
	condition = parse_base_expression(MIN_PRECEDENCE);//должна останавливаться на ;
	if(!match(TokenType::RPAREN)){
		while(true){
			postInstructions.push_back(parse_comma_statement());
			if(match(TokenType::RPAREN)){
				extract(TokenType::RPAREN);
				break;
			}
		}
	}
	std::vector<statement> instructions = parse_block_statement();
	return make_shared<ForLoopStatement>(preInstructions, condition, postInstructions, insturctions);
}

expr Parser::parse_binary_expression(int min_precedence) {
	auto lhs = parse_base_expression();

	for (auto op = tokens[offset].value; operators.contains(op) && operators.at(op) >= min_precedence; op = tokens[offset].value) {
		offset++;
		auto rhs = parse_binary_expression(operators.at(op));
		lhs = std::make_shared<BinaryNode>(op, lhs, rhs);
	}
	return lhs;
}

expr Parser::parse_base_expression() {
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

expr Parser::parse_parenthesized_expression() {
	extract(TokenType::LPAREN);
	auto node = parse();
	extract(TokenType::RPAREN);
	return std::make_shared<ParenthesizedNode>(node);
}

std::vector<expr> Parser::parse_function_interior() {
	extract(TokenType::LPAREN);
	std::vector<node> args;
	if (!match(TokenType::RPAREN)) {
		while (true) {
			args.push_back(parse());// написать спец функцию, которая будет парсить только цифры и идентификаторы
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