#include "expression.hpp"

#include "lexer.hpp"
#include "parser.hpp"

#include "visitor.hpp"

Expression::Expression(const std::string& input) {
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    root = parser.parse();
}

Expression::Expression(const node& root) : root(root) {}

void Expression::print() const {
    Printer printer;
    printer.print(root);
}

double Expression::evaluate(const std::unordered_map<std::string, double>& vars,
    const std::unordered_map<std::string, std::function<double(const std::vector<double>&)>>& funcs) const {
    Evaluator evaluator(vars, funcs);
    return evaluator.evaluate(root);
}

Expression Expression::differentiate(const std::string& var) const {
    Differentiator differentiator(var);
    return Expression(differentiator.differentiate(root));
}

std::pair<var_set, func_dict> Expression::analyze() {
    Analyzer analyzer;
    return analyzer.analyze(root);
}