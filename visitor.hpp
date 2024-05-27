#include "ast.hpp"
#include <unordered_map>
#include <functional>
#include <unordered_set>

//using func_dict = std::unordered_map<std::string, node>;
using var_set = std::unordered_set<std::string>;

class Visitor {
public:
    virtual void visit(BinaryNode&) = 0;
    virtual void visit(UnaryNode&) = 0;
    virtual void visit(FunctionNode&) = 0;
    virtual void visit(IdentifierNode&) = 0;
    virtual void visit(NumberNode&) = 0;
    virtual void visit(ParenthesizedNode&) = 0;
    virtual void visit(FuncDefinition&) = 0;
    virtual void visit(VarDefinition&) = 0;
    virtual void visit(ExprStatement&) = 0;
    virtual void visit(CondStatement&) = 0;
    virtual void visit(ForLoopStatement&) = 0;
    virtual void visit(WhileLoopStatement&) = 0;
    virtual void visit(JumpStatement&) = 0;
};

class Printer : public Visitor {
public:
    void visit(BinaryNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    void visit(IdentifierNode&);
    void visit(NumberNode&);
    void visit(ParenthesizedNode&);
    void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&);

    void print(const node&);
};

class Evaluator : public Visitor {
public:
    Evaluator(
        const std::unordered_map<std::string, double>&,
        const std::unordered_map<std::string, std::function<double(const std::vector<double>&)>>&
    );

    void visit(BinaryNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    void visit(IdentifierNode&);
    void visit(NumberNode&);
    void visit(ParenthesizedNode&);
    void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&);

    double evaluate(const node&);
private:
    static const std::unordered_map<std::string, std::function<double(double)>> unary_operations;
    static const std::unordered_map<std::string, std::function<double(double, double)>> binary_operations;
    static const std::unordered_map<std::string, std::function<double(const std::vector<double>&)>> builtin_funcs;

    std::unordered_map<std::string, double> vars;
    std::unordered_map<std::string, std::function<double(const std::vector<double>&)>> custom_funcs;
    double current_result;
};

class Differentiator : public Visitor {
public:
    Differentiator(const std::string&);

    void visit(BinaryNode&);
    void visit(ParenthesizedNode&);
    void visit(NumberNode&);
    void visit(IdentifierNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    /* void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&); */

    node differentiate(const node&);
private:
    std::string var;
    node current_result;
};

/* class Analyzer : public Visitor {
public: 

    void visit(BinaryNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    void visit(IdentifierNode&);
    void visit(NumberNode&);
    void visit(ParenthesizedNode&);
    void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&);

    std::pair<var_set, func_dict> analyze(const node&);
private:
    var_set vars;
    func_dict custom_funcs;
}; */