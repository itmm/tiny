#pragma once

#include <map>
#include <string>

class Decl;

class Scope {
		Scope *parent_;
		std::map<std::string, Decl *> symbols_;
	public:
		Scope(Scope *parent = nullptr): parent_ { parent } { }
		bool insert(Decl *declaration);
		Decl *lookup(std::string name);
		Scope *parent() { return parent_; }
};

extern Scope *current_scope;
