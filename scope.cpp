#include "scope.h"

#include "ast.h"

Declaration *parent_declaration = nullptr;

class Initial_Scope: public Scope {
	public:
		Initial_Scope(): Scope { nullptr } {
			insert(new Type_Declaration(nullptr, "INTEGER"));
			insert(new Type_Declaration(nullptr, "BOOL"));
		}
};

Scope *current_scope = new Initial_Scope { };

bool Scope::insert(Declaration *declaration) {
	return symbols_.insert({ declaration->name(), declaration }).second;
}

Declaration *Scope::lookup(std::string name) {
	for (Scope *cur { this }; cur; cur = cur->parent_) {
		auto got { cur->symbols_.find(name) };
		if (got != cur->symbols_.end()) { return got->second; }
	}
	return nullptr;
}
