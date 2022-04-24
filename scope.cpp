#include "scope.h"

#include "ast.h"

class Initial_Scope: public Scope {
	public:
		Initial_Scope() {
			auto int_ty { new Type_Declaration(nullptr, "INTEGER") };
			insert(int_ty);
			auto bool_ty { new Type_Declaration(nullptr, "BOOL") };
			insert(bool_ty);
		}
};

Scope *current_scope = new Initial_Scope { };

bool Scope::insert(Decl *declaration) {
	return symbols_.insert({ declaration->name(), declaration }).second;
}

Decl *Scope::lookup(std::string name) {
	for (Scope *cur { this }; cur; cur = cur->parent_) {
		auto got { symbols_.find(name) };
		if (got != symbols_.end()) { return got->second; }
	}
	return nullptr;
}
