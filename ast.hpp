#pragma once
#include <string>
#include <vector>
#include <memory>              

class Visitor;

struct ASTNode {
	virtual void accept(Visitor&) = 0;
	virtual ~ASTNode() = default;
};

using node = std::shared_ptr<ASTNode>;

struct BinaryNode : public ASTNode {
	std::string op;
	node left_branch, right_branch;

	BinaryNode(const std::string& op, const node& left_branch, const node& right_branch)
		: op(op), left_branch(left_branch), right_branch(right_branch) {}
	void accept(Visitor&);
};

struct UnaryNode : public ASTNode {
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

struct NumberNode : public ASTNode {
	double value;

	NumberNode(double value)
		: value(value) {}
	void accept(Visitor&);
};

struct IdentifierNode : public ASTNode {
	std::string name;

	IdentifierNode(const std::string& name)
		: name(name) {}
	void accept(Visitor&);
};

struct ParenthesizedNode : public ASTNode {
	node expr;

	ParenthesizedNode(const node& expr) : expr(expr) {}
	void accept(Visitor&);
};
