#include "visitor.hpp"

void BinaryNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void UnaryNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void FunctionNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void NumberNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void IdentifierNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ParenthesizedNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void VarDefinition::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void FuncDefinition::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ExprStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void CondStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ForLoopStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void WhileLoopStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void JumpStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}