#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <stack>

using SymbolTable = std::unordered_map<std::string, Declaration*>;

class Scope {
public:
    Scope(const std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}

    void add(const std::string& name, Declaration* symbol) {
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

private:
    SymbolTable table;
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