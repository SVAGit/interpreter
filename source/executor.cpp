#pragma once
#include <iostream>
#include "visitor.hpp"

using variable = std::variant<int, double, char, bool>;

Type Executor::get_type(std::string type) {
	if(type == "int") 
		return Type::INT;
	if(type == "double")
		return Type::DOUBLE;
	if(type == "char") 
		return Type::CHAR;
	if(type == "void")
		return Type::VOID;
	else return Type::BOOL;
}

variable Executor::default_value(Type type) {
	if(type == Type::INT)
		return (int) 0;
	if(type == Type::DOUBLE)
		return (double) 0;
	if(type == Type::CHAR)
		return (char) 0;
	else return false;
}

std::vector<std::pair<std::string, std::shared_ptr<Variable>>> Executor::get_arguments(std::vector<std::shared_ptr<VarDefinition>> root){
	std::vector<std::pair<std::string, std::shared_ptr<Variable>>> args;
	for(int i = 0; i < root.size(); i++){
		auto name = root[i]->name;
		args.push_back(std::make_pair(name, std::make_shared<Variable>(get_type(root[i]->type))));
	}
	return args;
}

void Executor::execute(const std::vector<statement>& root){
    scope_control.enterScope();
    for(int i = 0; i < root.size(); i++){
        root[i]->accept(*this);
    }
    scope_control.exitScope();
}

void Executor::visit(BinaryNode& root){
    root.left_branch->accept(*this);
    auto lhs = currRes;
    auto tmp = var;
    root.right_branch->accept(*this);
    auto rhs = currRes;
    if(assignment_operators.contains(root.op)){
        *tmp = assignment_operations.at(root.op)(lhs,rhs);
    }else{
        currRes = binary_operations.at(root.op)(lhs, rhs);
    }
}

void Executor::visit(UnaryNode& root){
    root.branch->accept(*this);
	currRes = unary_operations.at(root.op)(currRes);
}

void Executor::visit(PostfixNode& root){
    root.branch->accept(*this);
	auto res = currRes;
	auto tmp = var;
	if(root.op == "++" || root.op == "--"){
		*tmp = unary_operations.at(root.op)(res);
		currRes = *tmp;
	}else{
		currRes = unary_operations.at(root.op)(res);
	}
}

void Executor::visit(PrefixNode& root){
    root.branch->accept(*this);
	auto res = currRes;
	auto tmp = var;
	if(root.op == "++" || root.op == "--"){
		*tmp = unary_operations.at(root.op)(res);
		currRes = *tmp;
	}else{
		currRes = unary_operations.at(root.op)(res);
	}
}

void Executor::visit(FunctionNode& root){
	scope_control.enterScope();
    if(builtin_funcs.contains(root.name)){
        if(root.name == "print"){
            for(int i = 0; i < root.branches.size(); i++){
                root.branches[i]->accept(*this);
                builtin_funcs.at(root.name)(currRes);
            }
        }else{
            for(int i = 0; i < root.branches.size(); i++){
                root.branches[i]->accept(*this);
                builtin_funcs.at(root.name)(*var);
            }
        }
        return;
    }
	if(auto func = std::dynamic_pointer_cast<Function>(scope_control.scopes.top()->get_symbol(root.name)); !func){
		throw std::runtime_error("func");
	}else{
		for(std::size_t i = 0; i != root.branches.size(); i++){
			root.branches[i]->accept(*this);
			auto name = func->arguments[i].first;
			auto type = func->arguments[i].second->type;
			auto value = std::make_shared<variable>(currRes);
			auto var = std::make_shared<Variable>(type, value);
			scope_control.scopes.top()->executorAdd(name, var);
		}
		func->body->accept(*this);
	}
	return_flag = false;
	scope_control.exitScope();
}

void Executor::visit(IdentifierNode& root){
    var = scope_control.scopes.top()->get_value(root.name);
    currRes = *var;
}

void Executor::visit(IntNode& root){
    currRes = root.value;
}

void Executor::visit(DoubleNode& root){
    currRes = root.value;
}

void Executor::visit(CharNode& root){
    currRes = root.value;
}   

void Executor::visit(BoolNode& root){
    currRes = root.value;
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
        currRes = default_value(type);
    }
    scope_control.scopes.top()->executorAdd(root.name, std::make_shared<Variable>(type, std::make_shared<variable>(currRes)));
}

void Executor::visit(ExprStatement& root){
    root.expression->accept(*this);
}

void Executor::visit(CondStatement& root){
    root.condition->accept(*this);
    if(std::get<bool>(currRes)){
        scope_control.enterScope();
        root.if_instruction->accept(*this);
        scope_control.exitScope();
    }else if(root.else_instruction){
		root.else_instruction->accept(*this);
    }
}

void Executor::visit(ForLoopStatement& root){}

void Executor::visit(WhileLoopStatement& root){
    root.condition->accept(*this);
    while(std::get<bool>(currRes)){
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

const std::unordered_set<std::string> Executor::assignment_operators = {"=", "+=", "-=", "*=", "/="};

const std::unordered_map<std::string, std::function<variable(variable, variable)>> Executor::assignment_operations = {
	{"=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&&, auto&& arg2)->variable {
			return arg2;
		}, op1, op2);
	}},
	{"+=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 + arg2;
		}, op1, op2);
	}},
	{"-=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 - arg2;
		}, op1, op2);
	}},
	{"*=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 * arg2;
		}, op1, op2);
	}},
	{"/=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 / arg2;
		}, op1, op2);
	}},
};

const std::unordered_map<std::string, std::function<variable(variable)>> Executor::unary_operations = {
	{"++", [](variable op) -> variable {
		return std::visit([](variable arg)->variable {
			if (std::holds_alternative<int>(arg)) return variable(std::get<int>(arg) + 1);
			if (std::holds_alternative<double>(arg)) return  variable(std::get<double>(arg) + 1);
			if (std::holds_alternative<char>(arg)) return  variable(std::get<char>(arg) + 1);
			return arg;
	}, op);
	}}, 
	{"--", [](variable op) -> variable {
		return std::visit([](variable arg)->variable {
			if (std::holds_alternative<int>(arg)) return variable(std::get<int>(arg) - 1);
			if (std::holds_alternative<double>(arg)) return  variable(std::get<double>(arg) - 1);
			if (std::holds_alternative<char>(arg)) return  variable(std::get<char>(arg) - 1);
			return arg;
	}, op);
	}},
	{"-", [](variable op) -> variable {
		return std::visit([](auto&& arg)->variable {
			return -arg;
	}, op);
	}},
	{"+", [](variable op) -> variable {
		return std::visit([](auto&& arg)->variable {
			return arg;
	}, op);
	}}, 
	{"!", [](variable op) -> variable {
		return std::visit([](auto&& arg)->variable {
			return !arg;
	}, op);
	}}
};

const std::unordered_map<std::string, std::function<variable(variable, variable)>> Executor::binary_operations = {
	{"+", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 + arg2;
		}, op1, op2);
	}},
	{"-", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 - arg2;
		}, op1, op2);
	}},
	{"*", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 * arg2;
		}, op1, op2);
	}},
	{"/", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 / arg2;
		}, op1, op2);
	}},
	{"==", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 == arg2;
		}, op1, op2);
	}},
	{"!=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 != arg2;
		}, op1, op2);
	}},
	{">", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 || arg2;
		}, op1, op2);
	}},
	{">=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 >= arg2;
		}, op1, op2);
	}},
	{"<", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 < arg2;
		}, op1, op2);
	}},
	{"<=", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 <= arg2;
		}, op1, op2);
	}},
	{"||", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 != arg2;
		}, op1, op2);
	}},
	{"&&", [](variable op1, variable op2) -> variable {
		return std::visit([](auto&& arg1, auto&& arg2)->variable {
			return arg1 && arg2;
		}, op1, op2);
	}}
};

const std::unordered_map<std::string, std::function<void(variable&)>> Executor::builtin_funcs = {
	{"print", [](variable& arg) {
		std::visit([](auto&& arg) { std::cout << arg <<std::endl; }, arg);
	}},
	{"scan", [](variable& arg) {
		std::visit([](auto&& arg) { std::cin >> arg; }, arg);
	}}
};

