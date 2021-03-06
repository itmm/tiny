#include "scope.h"

#include "err.h"
#include "type.h"

Type::Ptr boolean_type = Type::create("BOOLEAN");
Type::Ptr integer_type = Type::create("INTEGER");
Type::Ptr real_type = Type::create("REAL");

class Initial_Scope: public Scope {
	public:
		Initial_Scope(): Scope { nullptr } {
			insert(integer_type);
			insert(boolean_type);
			insert(real_type);
		}
};

Scope::Ptr current_scope = std::make_shared<Initial_Scope>();

bool Scope::insert(Declaration::Ptr declaration) {
	if (! declaration) { throw Error { "insert nullptr" }; return false; }
	return symbols_.insert({ declaration->name(), declaration }).second;
}

Declaration::Ptr Scope::lookup(std::string name) {
	for (auto cur { current_scope }; cur; cur = cur->parent_) {
		auto got { cur->symbols_.find(name) };
		if (got != cur->symbols_.end()) { return got->second; }
	}
	return nullptr;
}
