#include "ast.h"

#include "err.h"

Type_Declaration::Ptr boolean_type = Type_Declaration::create("BOOLEAN");
Type_Declaration::Ptr integer_type = Type_Declaration::create("INTEGER");
Type_Declaration::Ptr real_type = Type_Declaration::create("REAL");

Type_Declaration::Ptr Bool_Trait::oberon_type = boolean_type;
Type_Declaration::Ptr Integer_Trait::oberon_type = integer_type;
Type_Declaration::Ptr Real_Trait::oberon_type = real_type;

std::string Reference::name() {
	return "%" + std::to_string(index_);
};

std::string Scoping_Declaration::mangle(std::string name) const {
	auto mangled = this->name() + "_" + name;
	if (parent_) {
		return parent_->mangle(mangled);
	}
	return mangled;
}
