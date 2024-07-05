#pragma once

#include <string>
#include <vector>
#include <memory> 

class Visitor;

struct ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~ASTNode() = default;
};

struct Expression : public ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~Expression() = default;
};

struct Declaration : public ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~Declaration() = default;
};

struct Statement : public ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~Statement() = default;
};

using node = std::shared_ptr<ASTNode>;
using expr = std::shared_ptr<Expression>;
using statement = std::shared_ptr<Statement>;

/////////////////////////////////////////////////////////////////DECLARATION/////////////////////////////////////////////////////////////////////////
struct VarDefinition : public Declaration {
	std::string type;
	std::string name;
	expr value;
	int initialisedFlag = 0;

	VarDefinition(const std::string& type, const std::string& name, expr value)
		: type(type), name(name), value(value) {}

	VarDefinition(const VarDefinition& root){
		type = root.type;
		name = root.name;
		value = root.value;
		initialisedFlag = root.initialisedFlag;
	}
	void accept(Visitor&);
};

struct FuncDefinition : public Declaration {
	std::string returnType;
	std::string funcName;
	std::vector<std::shared_ptr<VarDefinition>> argsList;
	statement commandsList;
	int initialisedFlag = 0;

	FuncDefinition(const std::string& returnType, const std::string& funcName, std::vector<std::shared_ptr<VarDefinition>> argsList, const statement& commandsList)
		: returnType(returnType), funcName(funcName), argsList(argsList), commandsList(commandsList) {}

	FuncDefinition(const FuncDefinition& root){
		returnType = root.returnType;
		funcName = root.funcName;
		argsList = root.argsList;
		commandsList = root.commandsList;
		initialisedFlag = root.initialisedFlag;
	}
	void accept(Visitor&);
};

/////////////////////////////////////////////////////////////////STATEMENT/////////////////////////////////////////////////////////////////////
struct BlockStatement : public Statement {
	std::vector<statement> instructions;

	BlockStatement(const std::vector<statement>& instructions) : instructions(instructions) {}
	void accept(Visitor&);
};

struct ExprStatement : public Statement {
	expr expression;

	ExprStatement(const expr& expression) : expression(expression) {}
	void accept(Visitor&);
};

struct CondStatement : public Statement {
	expr condition;
	statement if_instruction;
	statement else_instruction;

	CondStatement(const expr& condition, const statement& if_instruction, const statement& else_instruction) : condition(condition), if_instruction(if_instruction), else_instruction(else_instruction) {}
	CondStatement(const expr& condition, const statement& if_instruction) : condition(condition), if_instruction(if_instruction) {}
	void accept(Visitor&);
};

struct ForLoopStatement : public Statement {
	std::vector<std::shared_ptr<VarDefinition>> preInstructions;
	expr condition, postInstructions;
	statement instructions;

	ForLoopStatement(const std::vector<std::shared_ptr<VarDefinition>>& preInstructions, const expr& condition, const expr& postInstructions, const statement& instructions)
		: preInstructions(preInstructions), condition(condition), postInstructions(postInstructions), instructions(instructions) {}
	void accept(Visitor&);
};

struct WhileLoopStatement : public Statement {
	expr condition;
	statement instructions;

	WhileLoopStatement(const expr& condition, const statement& instructions) : condition(condition), instructions(instructions) {}
	void accept(Visitor&);
};

struct JumpStatement : public Statement {
	std::string jumpName;
	expr instructions;

	JumpStatement(const std::string& jumpName, const expr& instructions) : jumpName(jumpName), instructions(instructions) {}
	void accept(Visitor&);
};

struct VarDeclStatement : public Statement {
	std::shared_ptr<VarDefinition> var;

	VarDeclStatement(const std::shared_ptr<VarDefinition>& var) : var(var) {}
	void accept(Visitor&);
};

struct FuncDeclStatement : public Statement {
	std::shared_ptr<FuncDefinition> func;

	FuncDeclStatement(const std::shared_ptr<FuncDefinition>& func) : func(func) {}
	void accept(Visitor&);
};
/////////////////////////////////////////////////////////EXPRESSION//////////////////////////////////////////////////////////

struct BinaryNode : public Expression {
	std::string op;
	expr left_branch, right_branch;

	BinaryNode(const std::string& op, const expr& left_branch, const expr& right_branch)
		: op(op), left_branch(left_branch), right_branch(right_branch) {}
	void accept(Visitor&);
};

struct PostfixNode : public Expression {
	std::string op;
	expr branch;

	PostfixNode(const std::string& op, const expr& branch)
		: op(op), branch(branch) {}
	void accept(Visitor&);
};

struct PrefixNode : public Expression {
	std::string op;
	expr branch;

	PrefixNode(const std::string& op, const expr& branch)
		: op(op), branch(branch) {}
	void accept(Visitor&);
};

struct UnaryNode : public Expression {
	std::string op;
	expr branch;

	UnaryNode(const std::string& op, const expr& branch)
		: op(op), branch(branch) {}
	void accept(Visitor&);
};

struct FunctionNode : public Expression {
	std::string name;
	std::vector<expr> branches;

	FunctionNode(const std::string& name, const std::vector<expr>& branches)
		: name(name), branches(branches) {}

	void accept(Visitor&);
};

struct IntNode : public Expression {
	int value;

	IntNode(int value)
		: value(value) {}
	void accept(Visitor&);
};

struct DoubleNode : public Expression {
	double value;

	DoubleNode(double value)
		: value(value) {}
	void accept(Visitor&);
};

struct CharNode : public Expression {
	char value;

	CharNode(std::string value)
		: value(value[0]) {}
	void accept(Visitor&);
};

struct BoolNode : public Expression {
	bool value;
	
	BoolNode(bool value)
		: value(value) {}
	void accept(Visitor&);
};

struct IdentifierNode : public Expression{
	std::string name;

	IdentifierNode(const std::string& name)
		: name(name) {}
	void accept(Visitor&);
};

struct ParenthesizedNode : public Expression {
	expr expression;

	ParenthesizedNode(const expr& expression) : expression(expression) {}
	void accept(Visitor&);
};