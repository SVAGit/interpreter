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