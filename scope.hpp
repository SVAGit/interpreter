#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stack>
#include <variant>

#include "ast.hpp"

enum class Type{
    VOID, INT, DOUBLE, CHAR, BOOL,
};

using operand = std::variant<int, double, char, bool>;

struct Symbol{
    virtual ~Symbol() noexcept = default;
};

struct Variable : public Symbol {
    Type type;
    std::shared_ptr<operand> value;
    
    Variable(Type type, const std::shared_ptr<operand>& value = nullptr)
    	: type(type), value(value) {}

};

struct Function : public Symbol {
    Type returnType;
    std::vector<std::pair<std::string, std::shared_ptr<Variable>>> arguments;
    std::shared_ptr<BlockStatement> body;
    
    Function(Type& returnType, std::vector<std::pair<std::string, std::shared_ptr<Variable>>>& arguments, std::shared_ptr<BlockStatement>& body)
    	: returnType(returnType), arguments(arguments), body(body) {}
};

using DeclarationTable = std::unordered_map<std::string, std::shared_ptr<Declaration>>;
using SymbolTable = std::unordered_map<std::string, std::shared_ptr<Symbol>>;

class Scope {
public:
    Scope(const std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}

    void executorAdd(const std::string& name, std::shared_ptr<Symbol> symbol) {
        if(executeTable.contains(name)) {
            throw std::runtime_error("Redeclaration of symbol " + name + ".");
        }
        executeTable[name] = symbol;
    }

    void delete_symbol(const std::string& name){
    	if(executeTable.contains(name)){
    		executeTable.erase(name);
    	}
    }

    std::shared_ptr<Symbol> get_symbol(const std::string& name){
    	if(!table.contains(name)){
            if(parent == nullptr){
                return nullptr;
            }
		    return parent->get_symbol(name);
    	}
    	return executeTable[name];
    }

    bool executeLookup(const std::string& name){
        if(!executeTable.contains(name)){
            if(parent == nullptr){
                return false;
            }
            return parent->lookup(name);
        }
        return true;
    }
	
    std::shared_ptr<operand> get_value(const std::string& name){
    	if(!executeTable.contains(name)){
            if(parent == nullptr){
                return nullptr;
            }
		    return parent->get_value(name);
    	}
        if(auto symbol = std::dynamic_pointer_cast<Variable>(executeTable[name])){
    	    return symbol->value;
        }
        return nullptr;
    }


    void add(const std::string& name, std::shared_ptr<Declaration> symbol) {
        if(table.contains(name)) {
            throw std::runtime_error("Redeclaration of symbol " + name + ".");
        }
        table[name] = symbol;
    }

    bool lookup(const std::string& name) {
        if (!table.contains(name)) {
            if (parent == nullptr) {
                return false;
            }
            return parent->lookup(name);
        }
        return true;
    }

    std::shared_ptr<Declaration> get_element(std::string name){
        if(lookup(name)){
            if(!table.contains(name)){
                return parent->get_element(name);
            }else{
                return table[name];
            }
        }
        return nullptr;
    }

    Type search_type(std::string name){
        if(name == "print" || name == "scan"){
            return Type::VOID;
        }
        if(lookup(name)){
            if(!table.contains(name)){
                return parent->search_type(name);
            }else{
                if(auto varDef = dynamic_cast<VarDefinition*>(table[name].get())){
                    if(varDef->type == "int"){
                        return Type::INT;
                    }else if(varDef->type == "double"){
                        return Type::DOUBLE;
                    }else if(varDef->type == "char"){
                        return Type::CHAR;
                    }else if(varDef->type == "bool"){
                        return Type::BOOL;
                    }else if(varDef->type == "void"){
                        return Type::VOID;
                    }
                }else if(auto funcDef = dynamic_cast<FuncDefinition*>(table[name].get())){    
                    if(funcDef->returnType == "int"){
                        return Type::INT;
                    }else if(funcDef->returnType == "double"){
                        return Type::DOUBLE;
                    }else if(funcDef->returnType == "char"){
                        return Type::CHAR;
                    }else if(varDef->type == "bool"){
                        return Type::BOOL;
                    }else if(varDef->type == "void"){
                        return Type::VOID;
                    }
                }
            }
        }else{
            throw std::runtime_error("Undefined symbol " + name);
        }
        return Type::VOID;
    }

    bool cheak_init(std::string name){
        if(lookup(name)){
            if(!table.contains(name)){
                return parent->cheak_init(name);
            }else{
                if(auto varDef = dynamic_cast<VarDefinition*>(table[name].get())){
                    if(varDef->initialisedFlag){
                        return true;
                    }else{
                        return false;
                    }
                }else{
                    if(varDef->initialisedFlag){
                        return true;
                    }else{
                        return false;
                    }
                }
            }
        }else{
            throw std::runtime_error("Undefined symbol " + name);
        }
        return false;
    }

private:
    DeclarationTable table;
    SymbolTable executeTable;
    std::shared_ptr<Scope> parent;
};

struct ScopeManager {
    std::stack<std::shared_ptr<Scope>> scopes;

    ScopeManager() {
        scopes.push(std::make_shared<Scope>());
    }

    void enterScope() {
        if(!scopes.empty()){
            scopes.push(std::make_shared<Scope>(scopes.top()));
        }else{
            scopes.push(std::make_shared<Scope>());
        }
    }

    void exitScope(){
        scopes.pop();
    }
};