#pragma once

#include "declaration.h"

class Type: public Declaration {
		Type(std::string name): Declaration { name } { }
	public:
		using Ptr = std::shared_ptr<Type>;
		static auto create(std::string name) {
			return Ptr { new Type { name } };
		}
};

extern Type::Ptr boolean_type;
extern Type::Ptr integer_type;
extern Type::Ptr real_type;

std::string get_ir_type(Type::Ptr ty);
