#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "parser.hpp"

#define MIN_PRECEDENCE 0

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), offset(0) {}

std::vector<statement> Parser::parse() {
	std::vector<statement> declList;
	while(tokens[offset] != TokenType::END){
		declList.push_back(parse_decl_statement());
	}
	return declList;
}

std::string Parser::parse_var_type(){
	return extract(TokenType::VARTYPE);
}

std::string Parser::parse_identifier(){
	return extract(TokenType::IDENTIFIER);
}

std::vector<std::shared_ptr<VarDefinition>> Parser::parse_param_list(){
	extract(TokenType::LPAREN);
	std::vector<std::shared_ptr<VarDefinition>> args;
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
	bool flag = false;
	if(match(TokenType::CONST)){
		flag = true;
		extract(TokenType::CONST);
	}
	auto type = extract(TokenType::VARTYPE);
	auto name = extract(TokenType::IDENTIFIER);
	return make_shared<VarDefinition>(type, name, nullptr, flag);
}

statement Parser::parse_block_statement(){
	extract(TokenType::LBRACE);
	if (!match(TokenType::RBRACE)) {
		std::vector<statement> commands;
		while (true) {
			commands.push_back(parse_statement());
			if (match(TokenType::RBRACE)) {
				extract(TokenType::RBRACE);
				break;
			}
		}
		return make_shared<BlockStatement>(commands);
	}else {
		extract(TokenType::RBRACE);
		return nullptr;
	}
}

statement Parser::parse_statement(){
	if(tokens[offset].value == "if"){
		offset++;
		auto expr = parse_cond_statement();
		auto if_statement = parse_block_statement();
		if(tokens[offset].value == "else"){
			offset++;
			if(tokens[offset].value == "if"){
				return make_shared<CondStatement>(expr, if_statement, parse_statement());
			}else if(tokens[offset].value == "{"){
				return make_shared<CondStatement>(expr, if_statement, parse_block_statement());
			}else{
				std::runtime_error("Unknown instruction");
			}
		}else{
			return make_shared<CondStatement>(expr, if_statement);
		}
	}else if(tokens[offset].value == "while"){
		offset++;
		auto expr = parse_cond_statement();
		return make_shared<WhileLoopStatement>(expr, parse_block_statement());
	/*} else if(tokens[offset].value == "for"){
		offset++;
		return parse_for_statement(); */
	}else if(tokens[offset].type == TokenType::VARTYPE || tokens[offset].type == TokenType::CONST){
		return parse_decl_statement();
	}else if(tokens[offset].value == "break" || tokens[offset].value == "continue" || tokens[offset].value == "return"){
		return parse_jump_statement();
	}

	return parse_expr_statement();
}

statement Parser::parse_decl_statement(){
	bool flag = false;
	if(match(TokenType::CONST)){
		flag = true;
		extract(TokenType::CONST);
	}
	std::string type = extract(TokenType::VARTYPE);
	std::string name = extract(TokenType::IDENTIFIER);
	if(tokens[offset].type == TokenType::LPAREN && !flag){
		auto params = parse_param_list();
		auto statements = parse_block_statement();
		auto funcDefinition = std::make_shared<FuncDefinition>(type, name, params, statements);
		return std::make_shared<FuncDeclStatement>(funcDefinition);
	}else{
		auto varDefinition = std::make_shared<VarDefinition>(type, name, parse_var_value(), flag);
		return std::make_shared<VarDeclStatement>(varDefinition);
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
	if(!match(TokenType::RPAREN)){
		expr retVal = parse_binary_expression(MIN_PRECEDENCE);
		extract(TokenType::RPAREN);
		return retVal;
	}else{
		extract(TokenType::RPAREN);
		return nullptr;
	}
}

std::shared_ptr<JumpStatement> Parser::parse_jump_statement(){
	std::string val = tokens[offset++].value;
	if(val != "return"){
		extract(TokenType::SEMICOLON);
		return make_shared<JumpStatement>(val, nullptr);
	}else{
		if(match(TokenType::SEMICOLON)){
			extract(TokenType::SEMICOLON);
			return make_shared<JumpStatement>(val, nullptr);
		}else{
			auto expr = parse_binary_expression(MIN_PRECEDENCE);
			extract(TokenType::SEMICOLON);
			return make_shared<JumpStatement>(val, expr);
		}
	}
}

std::shared_ptr<ExprStatement> Parser::parse_expr_statement(){
	auto expr = parse_binary_expression(MIN_PRECEDENCE);
	auto retVal = std::make_shared<ExprStatement>(expr);
	extract(TokenType::SEMICOLON);
	return retVal;
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
	if (match(TokenType::INT_LITERAL)) {
		return std::make_shared<IntNode>(std::stod(extract(TokenType::INT_LITERAL)));
	}
	if (match(TokenType::DOUBLE_LITERAL)) {
		return std::make_shared<DoubleNode>(std::stod(extract(TokenType::DOUBLE_LITERAL)));
	}
	if (match(TokenType::CHAR_LITERAL)) {
		return std::make_shared<CharNode>(extract(TokenType::CHAR_LITERAL));
	}
	if (match(TokenType::BOOL_LITERAL)){
		bool value = true;
		if(tokens[offset].value == "false"){
			value = false;
		}
		extract(TokenType::BOOL_LITERAL);
		return std::make_shared<BoolNode>(value);
	}
	else if (match(TokenType::IDENTIFIER)) {
		if(auto identifier = extract(TokenType::IDENTIFIER); match(TokenType::LPAREN)) {
			return std::make_shared<FunctionNode>(identifier, parse_function_interior());
		}else if(tokens[offset] == "++" || tokens[offset] == "--"){
			auto help = make_shared<IdentifierNode>(identifier);
			return std::make_shared<PostfixNode>(tokens[offset++].value, help);
		}else{
			return std::make_shared<IdentifierNode>(identifier);
		}
	}else if(match(TokenType::KEYWORD)){
		if(tokens[offset] != "false" && tokens[offset] != "true"){
			throw std::runtime_error("Parser: Uncorrect expression");
		}
	} else if (auto token = tokens[offset++]; token == "+" || token == "-") {
		return std::make_shared<UnaryNode>(token.value, parse_base_expression());
	} else if (auto token = tokens[offset]; token == "++" || token == "--") {
		extract(TokenType::OPERATOR);
		auto identifier = extract(TokenType::IDENTIFIER);
		auto help = std::make_shared<IdentifierNode>(identifier);
		return std::make_shared<PrefixNode>(token.value, help);
	} else if (match(TokenType::LPAREN)) {
		return parse_parenthesized_expression();
	} else {
		throw std::runtime_error("Syntax error - unexpected token " + tokens[offset].value);
	}
	return nullptr;
}

expr Parser::parse_parenthesized_expression() {
	extract(TokenType::LPAREN);
	auto node = parse_binary_expression(MIN_PRECEDENCE);
	extract(TokenType::RPAREN);
	return std::make_shared<ParenthesizedNode>(node);
}

std::vector<expr> Parser::parse_function_interior() {
	extract(TokenType::LPAREN);
	std::vector<expr> args;
	if (!match(TokenType::RPAREN)) {
		while (true) {
			args.push_back(parse_identifier_or_digit());
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

expr Parser::parse_identifier_or_digit(){
	if(tokens[offset].type == TokenType::IDENTIFIER || tokens[offset].type == TokenType::INT_LITERAL || tokens[offset].type == TokenType::DOUBLE_LITERAL ||tokens[offset].type == TokenType::CHAR_LITERAL){
		return parse_binary_expression(MIN_PRECEDENCE);
	}else{
		throw std::runtime_error("Not identifier or digit");
	}
}

bool Parser::match(TokenType expected_type) const {
	return tokens[offset] == expected_type;
}

std::string Parser::extract(TokenType expected_type) {
	if (!match(expected_type)) {
		std::cout << "Ожидаемый тип: " << static_cast<int>(expected_type) << " Итоговый: "<<static_cast<int>(tokens[offset].type) << std::endl;
		throw std::runtime_error("Unexpected token " + tokens[offset].value );
	}
	return tokens[offset++].value;
}

void Parser::print_tokens(){
	for(int i = 0; i < tokens.size(); i++){
		switch(tokens[i].type){
			case TokenType::KEYWORD :
				std::cout << "KEYWORD ";
				break;
			case TokenType::VARTYPE :
				std::cout << "VARTYPE ";
				break;
			case TokenType::IDENTIFIER :
				std::cout << "IDENTIFIER ";
				break;
			case TokenType::CONST :
				std::cout << "CONST";
				break;
			case TokenType::INT_LITERAL : 
				std::cout << "INT_LITERAL ";
				break;
			case TokenType::DOUBLE_LITERAL : 
				std::cout << "DOUBLE_LITERAL ";
				break;
			case TokenType::CHAR_LITERAL : 
				std::cout << "CHAR_LITERAL ";
				break;
			case TokenType::OPERATOR :
				std::cout << "OPERATOR ";
				break;
			case TokenType::LPAREN :
				std::cout << "LPAREN ";
				break;
			case TokenType::RPAREN :
				std::cout << "RPAREN ";
				break;
			case TokenType::LBRACE :
				std::cout << "LBRACE ";
				break;
			case TokenType::RBRACE :
				std::cout << "RBRACE ";
				break;
			case TokenType::COMMA :
				std::cout << "COMMA ";
				break;
			case TokenType::SEMICOLON :
				std::cout << "SEMICOLON ";
				break;
			case TokenType::END :
				std::cout << "END ";
				break;
		}
	}
	std::cout << std::endl;
}

const std::unordered_map<std::string, int> Parser::operators = {
	{"+", 0}, {"-", 0},
	{"*", 1}, {"/", 1},
	{"^", 2}, {"<", 2}, {">", 2}, {"==", 2}, {"!=", 2}, {">=", 2}, {"<=", 2},
	{"=", 3}, {"+=", 3}, {"-=", 3}, {"*=", 3}, {"/=", 3}
};