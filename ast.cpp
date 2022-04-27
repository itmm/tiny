#include "ast.h"

#include "err.h"

Variable::Ptr Variable::create(std::string name, Type_Declaration::Ptr type) {
	if (! type) { throw Error { "no type in creation of '" + name + "'" }; }
	if (type->name() == "INTEGER") {
		return Integer_Variable::create(name);
	} else if (type->name() == "BOOLEAN") {
		return Bool_Variable::create(name);
	}
	throw Error { "can't create variable from " + type->name() };
}

static Expression::Ptr literal_bin_op(
	Binary_Op::Operator op, Literal::Ptr left, Literal::Ptr right
) {
	throw Error { "not implemented yet" };
}

Expression::Ptr Binary_Op::create(
	Operator op, Expression::Ptr left, Expression::Ptr right
) {
	auto ll { std::dynamic_pointer_cast<Literal>(left) };
	auto lr { std::dynamic_pointer_cast<Literal>(right) };
	if (ll && lr) {
		return literal_bin_op(op, ll, lr);
	} else {
		return Ptr { new Binary_Op { op, left, right } };
	}
}
