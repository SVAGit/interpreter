#include <iostream>
#include <unordered_map>

#include "expression.hpp"

int main() {
    std::string input;
    std::getline(std::cin, input);

    Expression expr(input);
    //expr.print();

    //auto value = expr.evaluate({{"x", 1}});
    //std::cout << value << std::endl;


    auto f = [](const std::vector<double>& x) -> double { 
        return std::exp(x.front()); 
    };

    auto SymbTable = expr.analyze();
    for (auto el : SymbTable.first) {
        std::cout << el << std::endl;
    }   
    
    std::cout << expr.evaluate({{"x", 3}}, {{"f", f}});

    return 0;
}