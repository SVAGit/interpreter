#include <iostream>
#include "visitor.hpp"
#include <cmath>

void Printer::print(const node& root) {
    root->accept(*this);
    std::cout << std::endl;
}

void Printer::visit(BinaryNode& root) {
    root.left_branch->accept(*this);
    std::cout << root.op;
    root.right_branch->accept(*this);
}

void Printer::visit(UnaryNode& root) {
    std::cout << root.op;
    root.branch->accept(*this);
}

void Printer::visit(FunctionNode& root) {
    std::cout << root.name << "(";
    for (std::size_t i = 0; i < root.branches.size(); ++i) {
        root.branches[i]->accept(*this);
        if (i != root.branches.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ")";
}

void Printer::visit(IdentifierNode& root) {
   std::cout << root.name;
}

void Printer::visit(NumberNode& root) {
    std::cout << root.value;
}

void Printer::visit(ParenthesizedNode& root) {
    std::cout << "(";
    root.expr->accept(*this);
    std::cout << ")";
}

//////////////////////////////////////////////////

Evaluator::Evaluator(
    const std::unordered_map<std::string, double>& vars,
    const std::unordered_map<std::string, std::function<double(const std::vector<double>&)>>& custom_funcs
) : vars(vars), custom_funcs(custom_funcs) {}

void Evaluator::visit(BinaryNode& root) {
    root.left_branch->accept(*this);
    auto lhs = current_result;
    root.right_branch->accept(*this);
    auto rhs = current_result;

    current_result = binary_operations.at(root.op)(lhs, rhs);
}

void Evaluator::visit(UnaryNode& root) {
    root.branch->accept(*this);
    current_result = unary_operations.at(root.op)(current_result);
}

void Evaluator::visit(FunctionNode& root) {
    std::vector<double> args;
    for (auto& branch : root.branches) {
        branch->accept(*this);
        args.push_back(current_result);
    }
    if (builtin_funcs.contains(root.name)) {
        current_result = builtin_funcs.at(root.name)(args);
    } else if (custom_funcs.contains(root.name)) {
        current_result = custom_funcs.at(root.name)(args);
    } else {
        throw std::runtime_error("Unknown function " + root.name);
    }
}



void Evaluator::visit(IdentifierNode& root) {
   current_result = vars.at(root.name);
}

void Evaluator::visit(NumberNode& root) {
    current_result = root.value;
}

void Evaluator::visit(ParenthesizedNode& root) {
    root.expr->accept(*this);
}

double Evaluator::evaluate(const node& root) {
    root->accept(*this);
    return current_result;
}

const std::unordered_map<std::string, std::function<double(double, double)>> Evaluator::binary_operations = {
    {"+", [](double lhs, double rhs) -> double {return lhs + rhs;}},
    {"-", [](double lhs, double rhs) -> double {return lhs - rhs;}},
    {"*", [](double lhs, double rhs) -> double {return lhs * rhs;}},
    {"/", [](double lhs, double rhs) -> double {return lhs / rhs;}},
    {"^", [](double lhs, double rhs) -> double {return std::pow(lhs, rhs);}},
};

const std::unordered_map<std::string, std::function<double(double)>> Evaluator::unary_operations = {
    {"-", [](double base) -> double {return -base;}},
    {"+", [](double base) -> double {return base;}}
};

const std::unordered_map<std::string, std::function<double(const std::vector<double>&)>> Evaluator::builtin_funcs = {
    {"sin", [](const std::vector<double>& x)->double {return std::sin(x.front());}},
    {"cos", [](const std::vector<double>& x)->double {return std::cos(x.front());}},
    {"exp", [](const std::vector<double>& x)->double {return std::exp(x.front());}},
    {"ln", [](const std::vector<double>& x)->double {return std::log(x.front());}},
};

///////////////////////////////////////////////////////

Differentiator::Differentiator(const std::string& var) : var(var) {}

node Differentiator::differentiate(const node& root) {
    root->accept(*this);
    return current_result;
}

void Differentiator::visit(BinaryNode& root) {
    root.left_branch->accept(*this);
    auto left_diff = current_result;
    root.right_branch->accept(*this);
    auto right_diff = current_result;


    if (root.op == "+") {
        current_result = std::make_shared<BinaryNode>("+", left_diff, right_diff);
    } else if (root.op == "-") {
        current_result = std::make_shared<BinaryNode>("-", left_diff, right_diff);
    } else if (root.op == "*") {
        auto lhs = std::make_shared<BinaryNode>("*", left_diff, root.right_branch);
        auto rhs = std::make_shared<BinaryNode>("*", root.left_branch, right_diff);
        current_result = std::make_shared<BinaryNode>("+", lhs, rhs);
    } else if (root.op == "/") {
        auto lhs = std::make_shared<BinaryNode>("*", left_diff, root.right_branch);
        auto rhs = std::make_shared<BinaryNode>("*", root.left_branch, right_diff);   
        auto numerator = std::make_shared<ParenthesizedNode>(std::make_shared<BinaryNode>("-", lhs, rhs));
        auto denumerator = std::make_shared<BinaryNode>("^", root.right_branch, std::make_shared<NumberNode>(2));
        current_result = std::make_shared<BinaryNode>("/", numerator, denumerator);
    }
}

void Differentiator::visit(FunctionNode& root) {
    if (root.branches.size() == 1) {
        root.branches.front()->accept(*this);
        auto arg_diff = current_result;

        if (root.name == "exp") {
            current_result = std::make_shared<BinaryNode>("*", arg_diff, std::make_shared<FunctionNode>(root));
        } else if (root.name == "ln") {
            current_result = std::make_shared<BinaryNode>("/", arg_diff, root.branches.front());
        } else if (root.name == "sin") {
            current_result = std::make_shared<BinaryNode>("*", arg_diff, std::make_shared<FunctionNode>("cos", root.branches));
        } else if (root.name == "cos") {
            current_result = std::make_shared<UnaryNode>("-",
                std::make_shared<BinaryNode>("*", arg_diff, std::make_shared<FunctionNode>("sin", root.branches))
            );
        } else {
            throw std::runtime_error("Unsupported function");
        }
    }
}

void Differentiator::visit(UnaryNode& root) {
    root.branch->accept(*this);
    current_result = std::make_shared<UnaryNode>(root.op, current_result);
}

void Differentiator::visit(ParenthesizedNode& root) {
    root.accept(*this);
}

void Differentiator::visit(IdentifierNode& root) {
    std::cout << root.name << " " << root.name << std::endl;
    if (root.name == var) {
        current_result = std::make_shared<NumberNode>(1);
    } else {
        current_result = std::make_shared<NumberNode>(0); 
    }
}

void Differentiator::visit(NumberNode&) {
    current_result = std::make_shared<NumberNode>(0);
}


//////////////////////////////////////////////////

std::pair<var_set, func_dict> Analyzer::analyze(const node& root) {
    root -> accept(*this);
    return std::make_pair(vars, custom_funcs);
}

void Analyzer::visit(BinaryNode& root) { 
    root.left_branch->accept(*this);
    root.right_branch->accept(*this);
}

void Analyzer::visit(UnaryNode& root) {
    root.branch->accept(*this);
}
void Analyzer::visit(FunctionNode& root){
    if (custom_funcs.contains(root.name)) {
            if (root.branches.size() != (std::dynamic_pointer_cast<FunctionNode>(custom_funcs.at(root.name)))->branches.size()) {
                throw std::runtime_error("Redefined Function");
            }
    } else {
        custom_funcs[root.name] = std::make_shared<FunctionNode>(root);
        for (auto& branch : root.branches) {
            branch->accept(*this);
        }
    }
}
void Analyzer::visit(IdentifierNode& root){
    vars.insert(root.name);
}
void Analyzer::visit(NumberNode&){
}
void Analyzer::visit(ParenthesizedNode& root){
    root.expr->accept(*this);
}