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

//////////////////////////////////////////////////
void Analyzer::visit(BinaryNode& root) { 
    if(assignment_operations.contains(root.op)){
        if(equalsFlag > 0){
            throw std::runtime_error("Sintaxis error, second =");
        }
        equalsFlag++;
        if(!dynamic_cast<IdentifierNode*>(root.left_branch.get()) && !dynamic_cast<PrefixNode*>(root.left_branch.get())){
            throw std::runtime_error("Not lvalue on left side of =");
        }
    }
    lhsFlag++;
    root.left_branch->accept(*this);
    lhsFlag--;
    auto lhs = currType;
    root.right_branch->accept(*this);
    auto rhs = currType;
    if(lhs != rhs){
        throw std::runtime_error("Uncorrect types");
    }
    if(root.op == "="){
        equalsFlag--;
    }
}

void Analyzer::visit(UnaryNode& root) {
    root.branch->accept(*this);
    if(currType == Type::CHAR || currType == Type::BOOL || currType == Type::VOID){
        throw std::runtime_error("Uncorrect unary operation");
    }
}

void Analyzer::visit(PrefixNode& root) {
    if(auto id = dynamic_pointer_cast<IdentifierNode>(root.branch)){
        auto element = scope_control.scopes.top()->get_element(id->name);
        if(auto var = dynamic_pointer_cast<VarDefinition>(element)){
            if(var->const_specifier){
                throw std::runtime_error(var->name + " is const");
            }else{
                root.branch->accept(*this);
            }
        }else{
            throw std::runtime_error("Uncorrect identifierNode");
        }
    }else{
        throw std::runtime_error("Uncorrect prefix operand");
    }
    if(currType == Type::CHAR || currType == Type::BOOL || currType == Type::VOID){
        throw std::runtime_error("Uncorrect prefix operation");
    }
}

void Analyzer::visit(PostfixNode& root) {
    root.branch->accept(*this);
    if(auto id = dynamic_pointer_cast<IdentifierNode>(root.branch)){
        auto element = scope_control.scopes.top()->get_element(id->name);
        if(auto var = dynamic_pointer_cast<VarDefinition>(element)){
            if(var->const_specifier){
                throw std::runtime_error(var->name + " is const");
            }
        }else{
            throw std::runtime_error("Uncorrect identifierNode");
        }
    }else{
        throw std::runtime_error("Uncorrect postfix operand");
    }
    if(currType == Type::CHAR || currType == Type::BOOL || currType == Type::VOID){
        throw std::runtime_error("Uncorrect postfix operation");
    }
}

void Analyzer::visit(FunctionNode& root){
    currType = scope_control.scopes.top()->search_type(root.name);
}

void Analyzer::visit(IdentifierNode& root){
    currType = scope_control.scopes.top()->search_type(root.name);
    if(lhsFlag && equalsFlag){
        auto obj = scope_control.scopes.top()->get_element(root.name);
        if(auto get = dynamic_cast<VarDefinition*>(obj.get())){
            if(get->const_specifier){
                throw std::runtime_error("Const variable " + get->name);
            }
            (get->initialisedFlag)++;
        }
        if(auto get = dynamic_cast<FuncDefinition*>(obj.get())){
            (get->initialisedFlag)++;
        }
    }else{
        if(!scope_control.scopes.top()->cheak_init(root.name) && mainFlag){
            throw std::runtime_error(root.name + " not initialized");
        }
    }
}

void Analyzer::visit(IntNode&){
    currType = Type::INT;
}

void Analyzer::visit(DoubleNode&){
    currType = Type::DOUBLE;
}

void Analyzer::visit(CharNode&){
    currType = Type::CHAR;
}

void Analyzer::visit(BoolNode&){
    currType = Type::BOOL;
}

void Analyzer::visit(ParenthesizedNode& root){
    root.expression->accept(*this);
}

/////////////////////////////////////////////////////
void Analyzer::analyze(const std::vector<statement>& root){
    scope_control.enterScope();
    for(int i = 0; i < root.size(); i++){
        root[i]->accept(*this);
    }
    if(mainFlag == 0){
        throw std::runtime_error("Main function was not declared");
    }
    scope_control.exitScope();
}

void Analyzer::visit(VarDefinition& root){
    if(root.type == "void"){
        throw std::runtime_error("Variable can't be void type");
    }
    if(root.value != nullptr){
        root.initialisedFlag++;
        root.value->accept(*this);
        switch(currType){
            case Type::INT :
                if(root.type != "int"){
                    throw std::runtime_error("Variable not int");
                }
                break;
            case Type::DOUBLE :
                if(root.type != "double"){
                    throw std::runtime_error("Variable not double");
                }
                break;
            case Type::CHAR :
                if(root.type != "char"){
                    throw std::runtime_error("Variable not char");
                }
                break;
            case Type::BOOL :
                if(root.type != "bool"){
                    throw std::runtime_error("Variable not bool");
                }
                break;
        }
    }
    auto scope = scope_control.scopes.top();
    scope->add(root.name, std::make_shared<VarDefinition>(root));
}

void Analyzer::visit(FuncDefinition& root){
    auto scope = scope_control.scopes.top();
    scope->add(root.funcName, std::make_shared<FuncDefinition>(root));
    scope_control.enterScope();
    loopFlag.push(0);

    if(root.funcName == "main"){
        mainFlag++;
    }

    for(int i = 0; i < root.argsList.size(); i++){
        root.argsList[i]->accept(*this);
    }

    if(root.commandsList != nullptr){
        root.commandsList->accept(*this);
    }

    auto block = dynamic_cast<BlockStatement*>(root.commandsList.get());
    int flag = 0;
    for(int i = 0; block && i < block->instructions.size(); i++){
        if(auto jump = dynamic_cast<JumpStatement*>(block->instructions[i].get())){
            flag++;
            if(jump->jumpName == "return"){
                if(jump->instructions == nullptr){
                    if(root.returnType != "void"){
                        throw std::runtime_error("Non void function");
                    }
                }else{
                    jump->instructions->accept(*this);
                    switch(currType){
                        case Type::INT :
                            if(root.returnType != "int"){
                                throw std::runtime_error("Retruned not int");
                            }
                            break;
                        case Type::DOUBLE :
                            if(root.returnType != "double"){
                                throw std::runtime_error("Retruned not double");
                            }
                            break;
                        case Type::CHAR :
                            if(root.returnType != "char"){
                                throw std::runtime_error("Retruned not char");
                            }
                            break;
                        case Type::BOOL :
                            if(root.returnType != "bool"){
                                throw std::runtime_error("Retruned not bool");
                            }
                            break;
                    }
                }
            }else{
                throw std::runtime_error("break or continue in function " + root.funcName);
            }
        }
    }
    if(!flag && root.returnType != "void"){
        throw std::runtime_error("Non void function");
    }

    scope_control.exitScope();
    loopFlag.pop();
}

void Analyzer::visit(BlockStatement& root){
    for(int i = 0; i < root.instructions.size(); i++){
        root.instructions[i]->accept(*this);
    }
}

void Analyzer::visit(ExprStatement& root){
    root.expression->accept(*this);
}

void Analyzer::visit(CondStatement& root){
    scope_control.enterScope();
    if(root.condition != nullptr){
       root.condition->accept(*this); 
    }
    if(root.condition != nullptr){
        root.if_instruction->accept(*this);
    }
    if(root.else_instruction != nullptr){
        root.else_instruction->accept(*this);
    }
    scope_control.exitScope();
}

void Analyzer::visit(ForLoopStatement& root){}

void Analyzer::visit(WhileLoopStatement& root){
    scope_control.enterScope();
    if(root.condition != nullptr){
        root.condition->accept(*this);
    }
    if(root.instructions != nullptr){
        (loopFlag.top())++;
        root.instructions->accept(*this);
        (loopFlag.top())--;
    }
    scope_control.exitScope();
}

void Analyzer::visit(JumpStatement& root){
    if(loopFlag.top() == 0 && root.jumpName != "return"){
        throw std::runtime_error("Break or continue jump not inside loopStatement");
    }
}

void Analyzer::visit(VarDeclStatement& root){
    root.var->accept(*this);
}

void Analyzer::visit(FuncDeclStatement& root){
    root.func->accept(*this);
}

const std::unordered_set<std::string> Analyzer::assignment_operations = {"=", "+=", "-=", "*=", "/="};