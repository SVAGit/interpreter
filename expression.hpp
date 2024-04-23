#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <unordered_set>

#include "ast.hpp"

using func_dict = std::unordered_map<std::string, node>;
using var_set = std::unordered_set<std::string>;

class Expression {
public:
    Expression(const std::string&);
    Expression(const node&);

    void print() const;
    double evaluate(const std::unordered_map<std::string, double>&,
    const std::unordered_map<std::string, std::function<double(const std::vector<double>&)>>&) const;
    Expression differentiate(const std::string&) const;
    std::pair<var_set, func_dict> analyze();
private:
    node root;
};
