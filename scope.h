#pragma once

#include <map>
#include <string>

class Declaration;

class Scope {
		Scope *parent_;
		std::map<std::string, Declaration *> symbols_;
	public:
		Scope(Scope *parent): parent_ { parent } { }
		bool insert(Declaration *declaration);
		Declaration *lookup(std::string name);
};

extern Declaration *parent_declaration;
extern Scope *current_scope;

class Pushed_Scope {
		Declaration *old_parent_declaration_;
		Scope *old_current_scope_;
	public:
		Pushed_Scope(Declaration *parent) {
			old_parent_declaration_ = parent_declaration;
			old_current_scope_ = current_scope;
			parent_declaration = parent;
			current_scope = new Scope { old_current_scope_ };
		}
		~Pushed_Scope() {
			delete current_scope;
			current_scope = old_current_scope_;
			parent_declaration = old_parent_declaration_;
		}
};
