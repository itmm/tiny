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

Variable::Ptr Variable::create(std::string name, Type_Declaration::Ptr type, bool with_load) {
	if (! type) { throw Error { "no type in creation of '" + name + "'" }; }
	if (type == integer_type) {
		return Integer_Variable::create(name, with_load);
	} else if (type == boolean_type) {
		return Bool_Variable::create(name, with_load);
	} else if (type == real_type) {
		return Real_Variable::create(name, with_load);
	}
	throw Error { "can't create variable from " + type->name() };
}

std::string Scoping_Declaration::mangle(std::string name) const {
	auto mangled = this->name() + "_" + name;
	if (parent_) {
		return parent_->mangle(mangled);
	}
	return mangled;
}
