#include "ast.h"

#include "err.h"
#include "ll_type.h"

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

template<typename RESULT, typename ARGS, typename FN> Value::Ptr apply_casted(
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

template<typename FN> inline auto apply_bool_real_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_bool_casted<Real_Literal, FN>(left, right, fn);
}

template<typename FN> inline auto literal_num_relation(Literal::Ptr left, Literal::Ptr right, FN fn) {
	auto il { std::dynamic_pointer_cast<Integer_Literal>(left) };
	auto ir { std::dynamic_pointer_cast<Integer_Literal>(right) };

	if (il && ir) { return apply_bool_int_casted(left, right, fn); }
	
	auto rl { std::dynamic_pointer_cast<Real_Literal>(left) };
	auto rr { std::dynamic_pointer_cast<Real_Literal>(right) };

	if (! rl && il) { rl = Real_Literal::create(il->value()); }
	if (! rr && ir) { rr = Real_Literal::create(ir->value()); }

	if (rl && rr) { return apply_bool_real_casted(left, right, fn); }

	throw Error { "wrong numeric relation argument types" };
}

template<typename FN> inline auto literal_full_relation(Literal::Ptr left, Literal::Ptr right, FN fn) {
	if (auto res { apply_bool_int_casted(left, right, fn) }) { return res; }
	return literal_num_relation(left, right, fn);
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

template<typename ARGS, typename FN> inline auto apply_real_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_casted<Real_Literal, ARGS, FN>(left, right, fn);
}

template<typename FN> inline auto apply_real_real_casted(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	return apply_real_casted<Real_Literal, FN>(left, right, fn);
}

template<typename FN> inline auto literal_bin_numeric(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	auto il { std::dynamic_pointer_cast<Integer_Literal>(left) };
	auto ir { std::dynamic_pointer_cast<Integer_Literal>(right) };

	if (il && ir) { return apply_int_int_casted(left, right, fn); }
	
	auto rl { std::dynamic_pointer_cast<Real_Literal>(left) };
	auto rr { std::dynamic_pointer_cast<Real_Literal>(right) };

	if (! rl && il) { rl = Real_Literal::create(il->value()); }
	if (! rr && ir) { rr = Real_Literal::create(ir->value()); }

	if (rl && rr) { return apply_real_real_casted(left, right, fn); }

	throw Error { "binary numeric: wrong argument types" };
}

template<typename FN> inline auto literal_bin_int(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	if (auto res { apply_int_int_casted(left, right, fn) }) { return res; }

	throw Error { "binary numeric: wrong argument types" };
}

template<typename FN> inline auto literal_bin_bool(
	Literal::Ptr left, Literal::Ptr right, FN fn
) {
	if (auto res { apply_bool_bool_casted(left, right, fn) }) { return res; }

	throw Error { "binary boolean: wrong argument types" };
}

struct Equal {
	bool operator()(int a, int b) { return a == b; }
	bool operator()(bool a, bool b) { return a == b; }
	bool operator()(double a, double b) { return a == b; }
};

struct Not_Equal {
	bool operator()(int a, int b) { return a != b; }
	bool operator()(bool a, bool b) { return a != b; }
	bool operator()(double a, double b) { return a != b; }
};

struct Less {
	bool operator()(int a, int b) { return a < b; }
	bool operator()(double a, double b) { return a < b; }
};

struct Less_Or_Equal {
	bool operator()(int a, int b) { return a <= b; }
	bool operator()(double a, double b) { return a <= b; }
};

struct Greater {
	bool operator()(int a, int b) { return a > b; }
	bool operator()(double a, double b) { return a > b; }
};

struct Greater_Or_Equal {
	bool operator()(int a, int b) { return a >= b; }
	bool operator()(double a, double b) { return a >= b; }
};

struct Add {
	int operator()(int a, int b) { return a + b; }
	double operator()(double a, double b) { return a + b; }
};

struct Sub {
	int operator()(int a, int b) { return a - b; }
	double operator()(double a, double b) { return a - b; }
};

struct Mul {
	int operator()(int a, int b) { return a * b; }
	double operator()(double a, double b) { return a * b; }
};

static Value::Ptr literal_bin_op(
	Binary_Op::Operator op, Literal::Ptr left, Literal::Ptr right
) {
	if (op == Binary_Op::equal) {
		return literal_full_relation(left, right, Equal { });
	} else if (op == Binary_Op::not_equal) {
		return literal_full_relation(left, right, Not_Equal { });
	} else if (op == Binary_Op::less) {
		return literal_num_relation(left, right, Less { });
	} else if (op == Binary_Op::less_equal) {
		return literal_num_relation(left, right, Less_Or_Equal { });
	} else if (op == Binary_Op::greater) {
		return literal_num_relation(left, right, Greater { });
	} else if (op == Binary_Op::greater_equal) {
		return literal_num_relation(left, right, Greater_Or_Equal { });
	} else if (op == Binary_Op::plus) {
		return literal_bin_numeric(left, right, Add { });
	} else if (op == Binary_Op::minus) {
		return literal_bin_numeric(left, right, Sub { });
	} else if (op == Binary_Op::mul) {
		return literal_bin_numeric(left, right, Mul { });
	} else if (op == Binary_Op::div) {
		return literal_bin_int(
			left, right, [](int a, int b) { return a / b; }
		);
	} else if (op == Binary_Op::mod) {
		return literal_bin_int(
			left, right, [](int a, int b) { return a % b; }
		);
	} else if (op == Binary_Op::op_and) {
		return literal_bin_bool(
			left, right, [](bool a, bool b) { return a && b; }
		);
	} else if (op == Binary_Op::op_or) {
		return literal_bin_bool(
			left, right, [](bool a, bool b) { return a || b; }
		);
	}
	throw Error { "not implemented yet" };
}

Value::Ptr Binary_Op::create(
	Operator op, Value::Ptr left, Value::Ptr right, Gen &gen
) {
	auto ll { std::dynamic_pointer_cast<Literal>(left) };
	auto lr { std::dynamic_pointer_cast<Literal>(right) };
	if (ll && lr) { return literal_bin_op(op, ll, lr); }

	switch (op) {
		case Binary_Op::equal: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = icmp eq " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, boolean_type);
		}
		case Binary_Op::not_equal: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = icmp ne " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, boolean_type);
		}
		case Binary_Op::less: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = icmp slt " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, boolean_type);
		}
		case Binary_Op::less_equal: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = icmp sle " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, boolean_type);
		}
		case Binary_Op::greater: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = icmp sgt " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, boolean_type);
		}
		case Binary_Op::greater_equal: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = icmp sge " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, boolean_type);
		}
		case Binary_Op::mod: {
			auto r { gen.next_id() };
			gen.append("%" + std::to_string(r) + " = srem " +
				get_ir_type(left->type()) + " " +
				left->name() + ", " + right->name());
			return Reference::create(r, integer_type);
		}
		default: break;
	}
	return Ptr { new Binary_Op { op, left, right } };
}

Type_Declaration::Ptr Binary_Op::type() {
	switch (op()) {
		case Operator::none:
			return nullptr;
		case Operator::equal:
		case Operator::not_equal:
		case Operator::less:
		case Operator::less_equal:
		case Operator::greater:
		case Operator::greater_equal:
		case Operator::op_and:
		case Operator::op_or:
			return boolean_type;
		default: break;
	}

	auto lt { left_->type() };
	auto rt { right_->type() };
	if (lt == integer_type && rt == integer_type) {
		return integer_type;
	}
	if (lt == real_type && (rt == integer_type || rt == real_type)) {
		return real_type;
	}
	if (lt == integer_type && rt == real_type) {
		return real_type;
	}
	return nullptr;
}

Value::Ptr Unary_Op::create(Operator op, Value::Ptr arg) {
	if (op == Operator::op_not) {
		auto ba { std::dynamic_pointer_cast<Bool_Literal>(arg) };
		if (ba) { return Bool_Literal::create(! ba->value()); }
	}
	return Ptr { new Unary_Op { op, arg } };
}

Type_Declaration::Ptr Unary_Op::type() {
	if (op() == Operator::op_not) {
		return boolean_type;
	}
	return nullptr;
}

std::string Scoping_Declaration::mangle(std::string name) const {
	auto mangled = this->name() + "_" + name;
	if (parent_) {
		return parent_->mangle(mangled);
	}
	return mangled;
}
