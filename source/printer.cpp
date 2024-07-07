#include <iostream>
#include "visitor.hpp"

void Printer::print(const std::vector<statement>& root){
    for(auto i = 0; i < root.size(); i++){
        root[i]->accept(*this);
        std::cout << std::endl; 
    }
}

void Printer::visit(VarDefinition& root){
    std::cout << "VarDefinition" << std::endl;
    if(root.const_specifier){
        std::cout << "Const: true" << std::endl;
    }
    std::cout << "Type: " << root.type << "\nName: " << root.name<< std::endl;
    std::cout << "Value: ";
    if(root.value != nullptr){
        root.value->accept(*this);
        std::cout << std::endl;
    }else{
        std::cout << "Empty" << std::endl;
    }
}

void Printer::visit(FuncDefinition& root){
    std::cout << "FuncDefiniton" << std::endl;
    std::cout << "Return type: " << root.returnType << std::endl;
    std::cout << "Function name: " << root.funcName << std::endl;
    std::cout << "Args list: ";

    if(root.argsList.size() != 0){
        for(auto i = 0; i < root.argsList.size(); i++){
            root.argsList[i]->accept(*this);
        }
    }else{
        std::cout << "Empty" << std::endl;
    }

    std::cout << "Commands:" << std::endl;
    if(root.commandsList != nullptr){
        root.commandsList->accept(*this);
    }else{
        std::cout << "Empty" << std::endl;
    }
};

void Printer::visit(ExprStatement& root){
    std::cout << "Expression" << std::endl;
    root.expression->accept(*this);
    std::cout << std::endl;
};

void Printer::visit(CondStatement& root){
    std::cout << "CondStatement" << std::endl;
    std::cout << "Condition: ";
    root.condition->accept(*this);
    std::cout << "\nIf instruction: ";
    if(root.if_instruction != nullptr){
        root.if_instruction->accept(*this);
    }else{
        std::cout << "Empty" << std::endl;
    }
    std::cout << "\nElse instruction: ";
    if(root.else_instruction != nullptr){
        root.else_instruction->accept(*this);
    }else{
        std::cout << "Empty" << std::endl;
    }
};

void Printer::visit(ForLoopStatement&){

};

void Printer::visit(WhileLoopStatement& root){
    std::cout << "WhileLoopStatement" << std::endl;
    std::cout << "Condition: ";
    if(root.condition != nullptr){
        root.condition->accept(*this);
    }else{
        std::cout << "Empty";
    }
    std::cout << "\nCommands: ";
    if(root.instructions != nullptr){
        root.instructions->accept(*this);
    }else{
        std::cout << "Empty" << std::endl;
    }
};

void Printer::visit(JumpStatement& root){
    std::cout << "JumpStatement" << std::endl;
    std::cout << "Jump: "<< root.jumpName << std::endl;
    std::cout << "Expression: ";
    if(root.instructions != nullptr){
        root.instructions->accept(*this);
    }else{
        std::cout << "Empty" << std::endl;
    }
};

void Printer::visit(BlockStatement& root){
    for(int i=0; i < root.instructions.size(); i++){
        root.instructions[i]->accept(*this);
    }
};

void Printer::visit(VarDeclStatement& root){
    root.var->accept(*this);
};

void Printer::visit(FuncDeclStatement& root){
    root.func->accept(*this);
};

void Printer::visit(PostfixNode& root){
    std::cout << "PostfixNode\n";
    root.branch->accept(*this);
    std::cout << root.op << std::endl;
};

void Printer::visit(PrefixNode& root){
    std::cout << "PrefixNode\n" << root.op;
    root.branch->accept(*this);
};

void Printer::visit(BinaryNode& root) {
    root.left_branch->accept(*this);
    std::cout << root.op;
    root.right_branch->accept(*this);
}

void Printer::visit(UnaryNode& root) {
    std::cout << root.op;
    root.branch->accept(*this);
}

void Printer::visit(FunctionNode& root) {
    std::cout << root.name << "(";
    for (std::size_t i = 0; i < root.branches.size(); ++i) {
        root.branches[i]->accept(*this);
        if (i != root.branches.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ")";
}

void Printer::visit(IdentifierNode& root) {
   std::cout << root.name;
}

void Printer::visit(IntNode& root) {
    std::cout << root.value;
}

void Printer::visit(DoubleNode& root) {
    std::cout << root.value;
}

void Printer::visit(CharNode& root) {
    std::cout << root.value;
}

void Printer::visit(BoolNode& root) {
    std::cout << root.value;
}

void Printer::visit(ParenthesizedNode& root) {
    std::cout << "(";
    root.expression->accept(*this);
    std::cout << ")";
}