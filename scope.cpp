#include "scope.h"

#include "ast.h"

std::shared_ptr<Declaration> parent_declaration = nullptr;

class Initial_Scope: public Scope {
	public:
		Initial_Scope(): Scope { nullptr } {
			insert(std::make_shared<Type_Declaration>(nullptr, "INTEGER"));
			insert(std::make_shared<Type_Declaration>(nullptr, "BOOL"));
		}
};

std::shared_ptr<Scope> current_scope = std::make_shared<Initial_Scope>();

bool Scope::insert(std::shared_ptr<Declaration> declaration) {
	return symbols_.insert({ declaration->name(), declaration }).second;
}

std::shared_ptr<Declaration> Scope::lookup(std::string name) {
	for (auto cur { current_scope }; cur; cur = cur->parent_) {
		auto got { cur->symbols_.find(name) };
		if (got != cur->symbols_.end()) { return got->second; }
	}
	return nullptr;
}
