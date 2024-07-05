#include "ast.hpp"
#include "scope.hpp"
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <variant>

using func_dict = std::unordered_map<std::string, node>;
using var_set = std::unordered_set<std::string>;

class Visitor {
public:
    virtual void visit(BinaryNode&) = 0;
    virtual void visit(UnaryNode&) = 0;
    virtual void visit(FunctionNode&) = 0;
    virtual void visit(IdentifierNode&) = 0;
    virtual void visit(IntNode&) = 0;
    virtual void visit(DoubleNode&) = 0;
    virtual void visit(CharNode&) = 0;
    virtual void visit(ParenthesizedNode&) = 0;
    virtual void visit(FuncDefinition&) = 0;
    virtual void visit(VarDefinition&) = 0;
    virtual void visit(ExprStatement&) = 0;
    virtual void visit(CondStatement&) = 0;
    virtual void visit(ForLoopStatement&) = 0;
    virtual void visit(WhileLoopStatement&) = 0;
    virtual void visit(JumpStatement&) = 0;
    virtual void visit(BlockStatement&) = 0;
    virtual void visit(VarDeclStatement&) = 0;
    virtual void visit(FuncDeclStatement&) = 0;
    virtual void visit(PostfixNode&) = 0;
    virtual void visit(PrefixNode&) = 0;
    virtual void visit(BoolNode&) = 0;
};

class Printer : public Visitor {
public:
    void visit(BinaryNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    void visit(IdentifierNode&);
    void visit(IntNode&);
    void visit(DoubleNode&);
    void visit(CharNode&);
    void visit(ParenthesizedNode&);
    void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&);
    void visit(BlockStatement&);
    void visit(VarDeclStatement&);
    void visit(FuncDeclStatement&);
    void visit(PostfixNode&);
    void visit(PrefixNode&);
    void visit(BoolNode&);

    void print(const std::vector<statement>&);
};

class Analyzer : public Visitor {
public: 

    void visit(BinaryNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    void visit(IdentifierNode&);
    void visit(IntNode&);
    void visit(DoubleNode&);
    void visit(CharNode&);
    void visit(ParenthesizedNode&);
    void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&);
    void visit(PostfixNode&);
    void visit(PrefixNode&);
    void visit(VarDeclStatement&);
    void visit(FuncDeclStatement&);
    void visit(BlockStatement&);
    void visit(BoolNode&);
    
    void analyze(const std::vector<statement>&);
private:
    ScopeManager scope_control;
    Type currType = Type::VOID;
    std::stack<int> loopFlag;
    int mainFlag = 0;
    int equalsFlag = 0;
    int lhsFlag = 0;
};

class Executor : public Visitor{
public:

    Executor() = default;
    using operand = std::variant<int, double, char, bool>;

    void visit(BinaryNode&);
    void visit(UnaryNode&);
    void visit(FunctionNode&);
    void visit(IdentifierNode&);
    void visit(IntNode&);
    void visit(DoubleNode&);
    void visit(CharNode&);
    void visit(ParenthesizedNode&);
    void visit(FuncDefinition&);
    void visit(VarDefinition&);
    void visit(ExprStatement&);
    void visit(CondStatement&);
    void visit(ForLoopStatement&);
    void visit(WhileLoopStatement&);
    void visit(JumpStatement&);
    void visit(PostfixNode&);
    void visit(PrefixNode&);
    void visit(VarDeclStatement&);
    void visit(FuncDeclStatement&);
    void visit(BlockStatement&);
    void visit(BoolNode&);

    void execute(const std::vector<statement>&);
    operand default_value(const Type&);
	Type get_type(const std::string&);
	std::vector<std::pair<std::string, std::shared_ptr<Variable>>> get_arguments(std::vector<std::shared_ptr<VarDefinition>>);

private:
    static const std::unordered_map<std::string, std::function<void(operand&)>> builtin_funcs;
	static const std::unordered_map<std::string, std::function<operand(operand)>> unary_operations;
	static const std::unordered_map<std::string, std::function<operand(operand, operand)>> assignment_operations;
    static const std::unordered_map<std::string, std::function<operand(operand, operand)>> binary_operations;
	static const std::unordered_set<std::string> assignment_operators;
	static const std::unordered_set<std::string> funcs;

    ScopeManager scope_control;
	operand current_result;
	std::shared_ptr<operand> var;
	bool return_flag = false;
	bool continue_flag = false;
	bool break_flag = false;
};