#pragma once

#include "declaration.h"

#include <map>
#include <memory>
#include <string>

class Scope {
	public:
		using Ptr = std::shared_ptr<Scope>;
	private:
		Scope::Ptr parent_;
		std::map<std::string, Declaration::Ptr> symbols_;
	public:
		Scope(Scope::Ptr parent): parent_ { parent } { }
		static auto create(Scope::Ptr parent) {
			return Ptr { new Scope { parent } };
		}
		bool insert(Declaration::Ptr declaration);
		Declaration::Ptr lookup(std::string name);
};

extern Scope::Ptr current_scope;

class Pushed_Scope {
		Scope::Ptr old_current_scope_;
	public:
		Pushed_Scope(Declaration::Ptr parent) {
			old_current_scope_ = current_scope;
			current_scope = Scope::create(old_current_scope_);
		}
		~Pushed_Scope() {
			current_scope = old_current_scope_;
		}
};
