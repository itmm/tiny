#pragma once

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

class Decl;

class Scope {
		Scope *parent_;
		llvm::StringMap<Decl *> symbols_;
	public:
		Scope(Scope *parent = nullptr): parent_ { parent } { }
		bool insert(Decl *declaration);
		Decl *lookup(llvm::StringRef name);
		Scope *parent() { return parent_; }
};

extern Scope *current_scope;
