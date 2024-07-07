#include "ast.hpp"
#include "scope.hpp"
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <variant>

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
    Type get_type(const std::string&);
    static const std::unordered_set<std::string> assignment_operations;
private:
    ScopeManager scope_control;
    Type currType = Type::VOID;
    std::stack<int> loopFlag;
    bool anotherFunc = false;
    int mainFlag = 0;
    int equalsFlag = 0;
    int lhsFlag = 0;
};

class Executor : public Visitor{
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

    using variable = std::variant<int, double, char, bool>;

    void execute(const std::vector<statement>&);
    variable default_value(Type);
	Type get_type(std::string);
	std::vector<std::pair<std::string, std::shared_ptr<Variable>>> get_arguments(std::vector<std::shared_ptr<VarDefinition>>);

private:
    ScopeManager scope_control;
	variable currRes;
	std::shared_ptr<variable> var;
	bool return_flag = false;
	bool continue_flag = false;
	bool break_flag = false;

    static const std::unordered_set<std::string> assignment_operators;
	static const std::unordered_map<std::string, std::function<variable(variable, variable)>> assignment_operations;
    static const std::unordered_map<std::string, std::function<variable(variable)>> unary_operations;
    static const std::unordered_map<std::string, std::function<variable(variable, variable)>> binary_operations;
    static const std::unordered_map<std::string, std::function<void(variable&)>> builtin_funcs;
};