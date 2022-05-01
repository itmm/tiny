#include "ll_type.h"

#include "err.h"

std::string get_ir_type(Type_Declaration::Ptr ty) {
	if (! ty) {
		return "void";
	} else if (ty == integer_type) {
		return "i32";
	} else if (ty == real_type) {
		throw Error { "REAL not implemented" };
	} else if (ty == boolean_type) {
		return "i1";
	}
	throw Error { "no low level type for '" + ty->name() + "'" };
}
