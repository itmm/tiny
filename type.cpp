#include "type.h"

#include "err.h"

std::string get_ir_type(Type::Ptr ty) {
	if (! ty) {
		return "void";
	} else if (ty == integer_type) {
		return "i32";
	} else if (ty == real_type) {
		return "double";
	} else if (ty == boolean_type) {
		return "i1";
	}
	throw Error { "no low level type for '" + ty->name() + "'" };
}
