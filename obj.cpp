#include "obj.h"

Type::Ptr Bool_Trait::oberon_type = boolean_type;
Type::Ptr Integer_Trait::oberon_type = integer_type;
Type::Ptr Real_Trait::oberon_type = real_type;

std::string Scoping_Declaration::mangle(std::string name) const {
	auto mangled = this->name() + "_" + name;
	if (parent_) {
		return parent_->mangle(mangled);
	}
	return mangled;
}
