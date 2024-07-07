#pragma once
#include "visitor.hpp"
#include <stdexcept>
#include <iostream>

using operand = std::variant<int, double, char, bool>;

Type Executor::get_type(const std::string& type) {
	if (type == "void") return Type::VOID;
	else if (type == "char") return Type::CHAR;
	else if (type == "int") return Type::INT;
	else if (type == "double") return Type::DOUBLE;
	else return Type::BOOL;
}

operand Executor::default_value(const Type& type) {
	if (type == Type::CHAR) return (char) 0;
	else if (type == Type::INT) return (int) 0;
	else if (type == Type::DOUBLE) return (double) 0;
	else return false;
}

std::vector<std::pair<std::string, std::shared_ptr<Variable>>> Executor::get_arguments(std::vector<std::shared_ptr<VarDefinition>> root){
	std::vector<std::pair<std::string, std::shared_ptr<Variable>>> args;
	for (int i = 0; i < root.size(); i++) {
		auto name = root[i]->name;
		args.push_back(std::make_pair(name, std::make_shared<Variable>(get_type(root[i]->type))));
	}
	return args;
}

void Executor::execute(const std::vector<statement>& root){
    scope_control.enterScope();
    for(int i = 0; i < root.size(); i++){
        root[i]->accept(*this);
        /* if(scope_control.scopes.top()->executeLookup("main")){
            break;
        } */
    }
    scope_control.exitScope();
}

void Executor::visit(BinaryNode& root){
    root.left_branch->accept(*this);
    auto lhs = current_result;
    auto tmp = var;
    root.right_branch->accept(*this);
    auto rhs = current_result;
    if(assignment_operators.contains(root.op)){
        *tmp = assignment_operations.at(root.op)(lhs,rhs);
    }else{
        current_result = binary_operations.at(root.op)(lhs, rhs);
    }
}

void Executor::visit(UnaryNode& root){
    root.branch->accept(*this);
    auto result = current_result;
    if(root.op == "-"){
        *var = unary_operations.at(root.op)(result);
        current_result = *var;
    }else{
        current_result = unary_operations.at(root.op)(result);
    }
}

void Executor::visit(PostfixNode& root){
    root.branch->accept(*this);
	auto result = current_result;
	auto tmp = var;
	if(root.op == "++" || root.op == "--"){
		*tmp = unary_operations.at(root.op)(result);
		current_result = *tmp;
	}else{
		current_result = unary_operations.at(root.op)(result);
	}
}

void Executor::visit(PrefixNode& root){
    root.branch->accept(*this);
	auto result = current_result;
	auto tmp = var;
	if(root.op == "++" || root.op == "--"){
		*tmp = unary_operations.at(root.op)(result);
		current_result = *tmp;
	}else{
		current_result = unary_operations.at(root.op)(result);
	}
}

void Executor::visit(FunctionNode& root){
    if(funcs.contains(root.name)){
        if(root.name == "print"){
            for(int i = 0; i < root.branches.size(); i++){
                root.branches[i]->accept(*this);
                builtin_funcs.at(root.name)(current_result);
            }
        }else{
            for(int i = 0; i < root.branches.size(); i++){
                root.branches[i]->accept(*this);
                builtin_funcs.at(root.name)(*var);
            }
        }
        return;
    }
    scope_control.enterScope();
	if(auto func = std::dynamic_pointer_cast<Function>(scope_control.scopes.top()->get_symbol(root.name)); !func){
		throw std::runtime_error("func");
	}else{
		for(std::size_t i = 0; i != root.branches.size(); i++){
			root.branches[i]->accept(*this);
			auto name = func->arguments[i].first;
			auto type = func->arguments[i].second->type;
			auto value = std::make_shared<operand>(current_result);
			auto var = std::make_shared<Variable>(type, value);
			scope_control.scopes.top()->executorAdd(name, var);
		}
		func->body->accept(*this);
		return_flag = false;
		scope_control.exitScope();
	}
}

void Executor::visit(IdentifierNode& root){
    var = scope_control.scopes.top()->get_value(root.name);
    current_result = *var;
}

void Executor::visit(IntNode& root){
    current_result = root.value;
}

void Executor::visit(DoubleNode& root){
    current_result = root.value;
}

void Executor::visit(CharNode& root){
    current_result = root.value;
}   

void Executor::visit(BoolNode& root){
    current_result = root.value;
}

void Executor::visit(ParenthesizedNode& root){
    root.expression->accept(*this);
}

void Executor::visit(FuncDefinition& root){
    auto type = get_type(root.returnType);
    auto args = get_arguments(root.argsList);
    if(auto block_statement = std::dynamic_pointer_cast<BlockStatement>(root.commandsList)){
        scope_control.scopes.top()->executorAdd(root.funcName, std::make_shared<Function>(type, args, block_statement));
    }
    if(root.funcName == "main"){
        scope_control.enterScope();
        root.commandsList->accept(*this);
        scope_control.exitScope();
    }
}

void Executor::visit(VarDefinition& root){
    auto type = get_type(root.type);
    if(root.value){
        root.value->accept(*this);
    }else{
        current_result = default_value(type);
    }
    scope_control.scopes.top()->executorAdd(root.name, std::make_shared<Variable>(type, std::make_shared<operand>(current_result)));
}

void Executor::visit(ExprStatement& root){
    root.expression->accept(*this);
}

void Executor::visit(CondStatement& root){
    root.condition->accept(*this);
    if(std::get<bool>(current_result)){
        scope_control.enterScope();
        root.if_instruction->accept(*this);
        scope_control.exitScope();
    }else{
        root.else_instruction->accept(*this);
    }
}

void Executor::visit(ForLoopStatement& root){}
//operand
void Executor::visit(WhileLoopStatement& root){
    root.condition->accept(*this);
    while(std::get<bool>(current_result)){
        scope_control.enterScope();
        root.instructions->accept(*this);
        scope_control.exitScope();
        if(return_flag){
            break;
        }else if(continue_flag){
            continue_flag = false;
            continue;
        }else if(break_flag){
            break_flag = false;
            break;
        }
        root.condition->accept(*this);
    }
}

void Executor::visit(JumpStatement& root){
    if(root.jumpName == "return"){
        if(root.instructions){
            root.instructions->accept(*this);
        }
        return_flag = true;
    }else if(root.jumpName == "continue"){
        continue_flag = true;
    }else{
        break_flag = true;
    }
}

void Executor::visit(VarDeclStatement& root){
    root.var->accept(*this);
}

void Executor::visit(FuncDeclStatement& root){
    root.func->accept(*this);
}

void Executor::visit(BlockStatement& root){
    for(int i = 0; i < root.instructions.size(); i++){
        root.instructions[i]->accept(*this);
        if(return_flag || continue_flag || break_flag){
            break;
        }
    }
}


const std::unordered_map<std::string, std::function<operand(operand)>> Executor::unary_operations = {
	{"++", [](operand op) -> operand {
		return std::visit([](operand arg)->operand {
			if (std::holds_alternative<int>(arg)) return operand(std::get<int>(arg) + 1);
			if (std::holds_alternative<double>(arg)) return  operand(std::get<double>(arg) + 1);
			if (std::holds_alternative<char>(arg)) return  operand(std::get<char>(arg) + 1);
			return arg;
	}, op);
	}}, 
	{"--", [](operand op) -> operand {
		return std::visit([](operand arg)->operand {
			if (std::holds_alternative<int>(arg)) return operand(std::get<int>(arg) - 1);
			if (std::holds_alternative<double>(arg)) return  operand(std::get<double>(arg) - 1);
			if (std::holds_alternative<char>(arg)) return  operand(std::get<char>(arg) - 1);
			return arg;
	}, op);
	}},
	{"-", [](operand op) -> operand {
		return std::visit([](auto&& arg)->operand {
			return -arg;
	}, op);
	}},
	{"+", [](operand op) -> operand {
		return std::visit([](auto&& arg)->operand {
			return arg;
	}, op);
	}}, 
	{"!", [](operand op) -> operand {
		return std::visit([](auto&& arg)->operand {
			return !arg;
	}, op);
	}}
};


const std::unordered_map<std::string, std::function<void(operand&)>> Executor::builtin_funcs = {
	{"print", [](operand& arg) {
		std::visit([](auto&& arg) { std::cout << arg <<std::endl; }, arg);
	}},
	{"scan", [](operand& arg) {
		std::visit([](auto&& arg) { std::cin >> arg; }, arg);
	}}
};


const std::unordered_map<std::string, std::function<operand(operand, operand)>> Executor::assignment_operations = {
	{"=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&&, auto&& arg2)->operand {
			return arg2;
		}, op1, op2);
	}},
	{"+=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 + arg2;
		}, op1, op2);
	}},
	{"-=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 - arg2;
		}, op1, op2);
	}},
	{"*=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 * arg2;
		}, op1, op2);
	}},
	{"/=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 / arg2;
		}, op1, op2);
	}},
};


const std::unordered_map<std::string, std::function<operand(operand, operand)>> Executor::binary_operations = {
	{"+", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 + arg2;
		}, op1, op2);
	}},
	{"-", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 - arg2;
		}, op1, op2);
	}},
	{"*", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 * arg2;
		}, op1, op2);
	}},
	{"/", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 / arg2;
		}, op1, op2);
	}},
	{"==", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 == arg2;
		}, op1, op2);
	}},
	{"!=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 != arg2;
		}, op1, op2);
	}},
	{">", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 > arg2;
		}, op1, op2);
	}},
	{">=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 >= arg2;
		}, op1, op2);
	}},
	{"<", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 < arg2;
		}, op1, op2);
	}},
	{"<=", [](operand op1, operand op2) -> operand {
		return std::visit([](auto&& arg1, auto&& arg2)->operand {
			return arg1 <= arg2;
		}, op1, op2);
	}}
};

const std::unordered_set<std::string> Executor::funcs = {"print", "scan"};

const std::unordered_set<std::string> Executor::assignment_operators = {"=", "+=", "-=", "*=", "/="};

