#include "scope.h"

#include "ast.h"

bool Scope::insert(Decl *declaration) {
	return symbols_.insert({ declaration->name(), declaration }).second;
}

Decl *Scope::lookup(llvm::StringRef name) {
	for (Scope *cur { this }; cur; cur = cur->parent_) {
		auto got { symbols_.find(name) };
		if (got != symbols_.end()) { return got->second; }
	}
	return nullptr;
}
