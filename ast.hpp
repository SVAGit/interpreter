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
}

struct Declaration : public ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~Expression() = default;
}

struct Statement : public ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~Expression() = default;
}

using node = std::shared_ptr<ASTNode>;
using expr = std::shared_ptr<Expression>;
using statement = std::shared_ptr<Statement>;

/////////////////////////////////////////////////////////////////DECLARATION/////////////////////////////////////////////////////////////////////////

struct VarDefinition : public Declaration {
	std::string type;
	std::string name;
	expr value;

	VarDefinition(const std::string& type, const std::string& name, expr value)
		: type(type), name(name), value(value) {}
	void accept(Visitor&);
}

struct FuncDefinition : public Declaration {
	std::string returnType;
	std::string funcName;
	std::vector<std::shared_ptr<VarDefinition>> argsList;
	std::vector<statement> commandsList; 

	FuncDefinition(const std::string& returnType, const std::string& funcName, vector<std::shared_ptr<VarDefinition>> argsList, std::vector<statement> commandList)
		: returnType(returnType), funcName(funcName), argsList(argsList), commandsList(commandsList) {}
	void accept(Visitor&);
}

/////////////////////////////////////////////////////////////////STATEMENT/////////////////////////////////////////////////////////////////////

struct ExprStatement : public Statement {
	expr expression;

	ExprStatement(const expr& expression) : expression(expression) {}
	void accept(Visitor&);
}

struct CondStatement : public Statement {
	expr condition;
	std::vector<statement> instruction;

	CondStatement(const expr& condition, const std::vector<statement>& instruction) : condition(condition), instruction(instruction) {}
	void accept(Visitor&);
}

struct ForLoopStatement : public Statement {
	std::vector<statement> preInstructions;
	expr conditoin;
	std::vector<statement> postInstructions;
	std::vector<statement> instructions;

	ForLoopStatement(const std::vector<statement>& preInstructions, const expr& condition, const std::vector<statement>& postInstructions, const std::vector<statement>& instructions)
		: preInstructions(preInstructions), condition(condition), postInstructions(postInstructions), instructions(instructions) {}
	void accept(Visitor&);
}

struct WhileLoopStatement : public Statement {
	expr condition;
	std::vector<statment> instructions;

	WhileLoopStatement(const expr& condition, const std::vector<statement>& instructions) : condition(condition), instruction(instruction) {}
	void accept(Visitior&);
}

struct JumpStatement : public Statement {
	std::string jumpName;
	statement instruction;

	JumpStatemnt(const std::stirng& jumpName, const statement& instruction) : jumpName(jumpName), insturction(instruction) {}
	void accept(Visitor&);
}

/* struct DeclStatement : public Statement {
	std::vector<std::shared_ptr<VarDefinition>> vars;

	DeclStatement(const std::vector<std::shared_ptr<VarDefinition>>& vars) : vars(vars) {}
	void accept(Visitor&);
} */

/////////////////////////////////////////////////////////EXPRESSION//////////////////////////////////////////////////////////

struct BinaryNode : public Expression {
	std::string op;
	node left_branch, right_branch;

	BinaryNode(const std::string& op, const node& left_branch, const node& right_branch)
		: op(op), left_branch(left_branch), right_branch(right_branch) {}
	void accept(Visitor&);
};

struct UnaryNode : public Expression {
	std::string op;
	node branch;

	UnaryNode(const std::string& op, const node& branch)
		: op(op), branch(branch) {}
	void accept(Visitor&);
};

struct FunctionNode : public ASTNode {
	std::string name;
	std::vector<node> branches;

	FunctionNode(const std::string& name, const std::vector<node>& branches)
		: name(name), branches(branches) {}

	void accept(Visitor&);
};

struct NumberNode : public Expression {
	double value;

	NumberNode(double value)
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
	node expr;

	ParenthesizedNode(const node& expr) : expr(expr) {}
	void accept(Visitor&);
};