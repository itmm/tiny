#pragma once

#include <map>
#include <memory>
#include <string>

class Declaration;

class Scope {
		std::shared_ptr<Scope> parent_;
		std::map<std::string, std::shared_ptr<Declaration>> symbols_;
	public:
		Scope(std::shared_ptr<Scope> parent): parent_ { parent } { }
		static std::shared_ptr<Scope> create(std::shared_ptr<Scope> parent) {
			return std::make_shared<Scope>(parent);
		}
		bool insert(std::shared_ptr<Declaration> declaration);
		std::shared_ptr<Declaration> lookup(std::string name);
};

extern std::shared_ptr<Declaration> parent_declaration;
extern std::shared_ptr<Scope> current_scope;

class Pushed_Scope {
		std::shared_ptr<Declaration> old_parent_declaration_;
		std::shared_ptr<Scope> old_current_scope_;
	public:
		Pushed_Scope(std::shared_ptr<Declaration> parent) {
			old_parent_declaration_ = parent_declaration;
			old_current_scope_ = current_scope;
			parent_declaration = parent;
			current_scope = Scope::create(old_current_scope_);
		}
		~Pushed_Scope() {
			current_scope = old_current_scope_;
			parent_declaration = old_parent_declaration_;
		}
};
