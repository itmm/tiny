#include "ast.h"

#include "err.h"

Type_Declaration::Ptr boolean_type = Type_Declaration::create("BOOLEAN");
Type_Declaration::Ptr integer_type = Type_Declaration::create("INTEGER");

Type_Declaration::Ptr Bool_Trait::oberon_type = boolean_type;
Type_Declaration::Ptr Integer_Trait::oberon_type = integer_type;

Variable::Ptr Variable::create(std::string name, Type_Declaration::Ptr type) {
	if (! type) { throw Error { "no type in creation of '" + name + "'" }; }
	if (type == integer_type) {
		return Integer_Variable::create(name);
	} else if (type == boolean_type) {
		return Bool_Variable::create(name);
	}
	throw Error { "can't create variable from " + type->name() };
}

template<typename RESULT, typename ARGS, typename FN> Expression::Ptr apply_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	if (auto cl { std::dynamic_pointer_cast<ARGS>(left) }) {
		auto cr { std::dynamic_pointer_cast<ARGS>(right) };
		if (! cr) { throw Error { "type casting error" }; }
		return RESULT::create(fn(cl->value(), cr->value()));
	}
	return nullptr;
}

template<typename ARGS, typename FN> inline auto apply_bool_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_casted<Bool_Literal, ARGS, FN>(left, right, fn);
}

template<typename FN> inline auto apply_bool_bool_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_bool_casted<Bool_Literal, FN>(left, right, fn);
}

template<typename FN> inline auto apply_bool_int_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_bool_casted<Integer_Literal, FN>(left, right, fn);
}

template<typename FN> inline auto literal_full_relation(Literal::Ptr left, Literal::Ptr right, FN fn) {
	if (auto res { apply_bool_int_casted(left, right, fn) }) { return res; }
	if (auto res { apply_bool_bool_casted(left, right, fn) }) { return res; }

	throw Error { "wrong full relation argument types" };
}

template<typename FN> inline auto literal_num_relation(Literal::Ptr left, Literal::Ptr right, FN fn) {
	if (auto res { apply_bool_int_casted(left, right, fn) }) { return res; }

	throw Error { "wrong numeric relation argument types" };
}

template<typename ARGS, typename FN> inline auto apply_int_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_casted<Integer_Literal, ARGS, FN>(left, right, fn);
}

template<typename FN> inline auto apply_int_int_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_int_casted<Integer_Literal, FN>(left, right, fn);
}

template<typename FN> inline auto literal_bin_numeric(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	if (auto res { apply_int_int_casted(left, right, fn) }) { return res; }

	throw Error { "binary numeric: wrong argument types" };
}

struct Equal {
	bool operator()(int a, int b) { return a == b; }
	bool operator()(bool a, bool b) { return a == b; }
};

struct Not_Equal {
	bool operator()(int a, int b) { return a != b; }
	bool operator()(bool a, bool b) { return a != b; }
};

static Expression::Ptr literal_bin_op(
	Binary_Op::Operator op, Literal::Ptr left, Literal::Ptr right
) {
	if (op == Binary_Op::equal) {
		return literal_full_relation(left, right, Equal{ });
	} else if (op == Binary_Op::not_equal) {
		return literal_full_relation(left, right, Not_Equal{ });
	} else if (op == Binary_Op::less) {
		return literal_num_relation(
			left, right, [](int a, int b) { return a < b; }
		);
	} else if (op == Binary_Op::less_equal) {
		return literal_num_relation(
			left, right, [](int a, int b) { return a <= b; }
		);
	} else if (op == Binary_Op::greater) {
		return literal_num_relation(
			left, right, [](int a, int b) { return a > b; }
		);
	} else if (op == Binary_Op::greater_equal) {
		return literal_num_relation(
			left, right, [](int a, int b) { return a >= b; }
		);
	} else if (op == Binary_Op::plus) {
		return literal_bin_numeric(
			left, right, [](int a, int b) { return a + b; }
		);
	} else if (op == Binary_Op::minus) {
		return literal_bin_numeric(
			left, right, [](int a, int b) { return a - b; }
		);
	} else if (op == Binary_Op::mul) {
		return literal_bin_numeric(
			left, right, [](int a, int b) { return a * b; }
		);
	} else if (op == Binary_Op::div) {
		return literal_bin_numeric(
			left, right, [](int a, int b) { return a / b; }
		);
	} else if (op == Binary_Op::mod) {
		return literal_bin_numeric(
			left, right, [](int a, int b) { return a % b; }
		);
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
