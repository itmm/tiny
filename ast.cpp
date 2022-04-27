#include "ast.h"

#include "err.h"

Type_Declaration::Ptr boolean_type = Type_Declaration::create("BOOLEAN");
Type_Declaration::Ptr integer_type = Type_Declaration::create("INTEGER");

Variable::Ptr Variable::create(std::string name, Type_Declaration::Ptr type) {
	if (! type) { throw Error { "no type in creation of '" + name + "'" }; }
	if (type == integer_type) {
		return Integer_Variable::create(name);
	} else if (type == boolean_type) {
		return Bool_Variable::create(name);
	}
	throw Error { "can't create variable from " + type->name() };
}

static Expression::Ptr literal_not_equal(
	Literal::Ptr left, Literal::Ptr right
) {
	if (auto il { std::dynamic_pointer_cast<Integer_Literal>(left) }) {
		auto ir { std::dynamic_pointer_cast<Integer_Literal>(right) };
		if (! ir) { throw Error { "#: not both literals integers" }; }
		return Bool_Literal::create(il->value() != ir->value());
	}

	if (auto bl { std::dynamic_pointer_cast<Bool_Literal>(left) }) {
		auto br { std::dynamic_pointer_cast<Bool_Literal>(right) };
		if (! br) { throw Error { "#: not both literals boolean" }; }
		return Bool_Literal::create(bl->value() != br->value());
	}

	throw Error {"#: wrong literal argument types" };
}

static Expression::Ptr literal_bin_op(
	Binary_Op::Operator op, Literal::Ptr left, Literal::Ptr right
) {
	if (op == Binary_Op::not_equal) {
		return literal_not_equal(left, right);
	}
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

Type_Declaration::Ptr Binary_Op::type() {
	if (op() == Operator::not_equal) {
		return boolean_type;
	}
	return integer_type;

}
