#include "parser.h"

#include "scope.h"

void Parser::parse() {
	parse_module();
	expect(Token_Kind::eoi);
}

bool is_numeric(Type::Ptr t) {
	return t == integer_type || t == real_type;
}

Value::Ptr Parser::parse_unary_plus(Value::Ptr left) {
	auto t { left->type() };
	if (! is_numeric(t)) { throw Error { "wrong type for unary +" }; }
	return left;
}

Value::Ptr Parser::parse_unary_minus(Value::Ptr left) {
	auto t { left->type() };
	if (! is_numeric(t)) { throw Error { "wrong type for unary -" }; }
	if (auto l { std::dynamic_pointer_cast<Integer_Literal>(left) }) {
		return Integer_Literal::create(-l->value());
	}
	if (auto l { std::dynamic_pointer_cast<Real_Literal>(left) }) {
		return Real_Literal::create(-l->value());
	}
	
	auto r { Reference::create(gen_.next_id(), t) };
	gen_.append(
		r->name() + " = neg " + get_ir_type(t) + " " + left->name()
	);
	return r;
}

Value::Ptr propagate_to_real(Value::Ptr v) {
	if (v->type() == real_type) { return v; }
	if (v->type() != integer_type) {
		throw Error { "cannot promote type to REAL" };
	}
	if (auto i { std::dynamic_pointer_cast<Integer_Literal>(v) }) {
		return Real_Literal::create(i->value());
	}
	throw Error { "cannot cast integer to REAL" }; // TODO
}

Value::Ptr Parser::parse_binary_plus(Value::Ptr left, Value::Ptr right) {
	auto lt { left->type() };
	auto rt { left->type() };
	if (! (is_numeric(lt) && is_numeric(rt))) {
		throw Error { "wrong type for binary +" };
	}

	if (lt == integer_type && rt == integer_type) {
		auto li { std::dynamic_pointer_cast<Integer_Literal>(left) };
		auto ri { std::dynamic_pointer_cast<Integer_Literal>(right) };

		if (li && ri) { return Integer_Literal::create(
			li->value() + ri->value()
		); }
		if (li && li->value() == 0) { return right; }
		if (ri && ri->value() == 0) { return left; }

		auto r { Reference::create(gen_.next_id(), integer_type) };
		gen_.append(
			r->name() + " = add i32 " + left->name() + ", " +
			right->name()
		);
		return r;
	}
	
	left = { propagate_to_real(left) };
	right = { propagate_to_real(right) };
	auto lr { std::dynamic_pointer_cast<Real_Literal>(left) };
	auto rr { std::dynamic_pointer_cast<Real_Literal>(right) };
	if (lr && rr) {
		return Real_Literal::create(lr->value() + rr->value());
	}
	if (lr && lr->value() == 0.0) { return right; }
	if (rr && rr->value() == 0.0) { return left; }

	auto r { Reference::create(gen_.next_id(), real_type) };
	gen_.append(
		r->name() + " = fadd double " + left->name() + ", " +
		right->name()
	);
	return r;
}

Value::Ptr Parser::parse_binary_minus(Value::Ptr left, Value::Ptr right) {
	auto lt { left->type() };
	auto rt { left->type() };
	if (! (is_numeric(lt) && is_numeric(rt))) {
		throw Error { "wrong type for binary -" };
	}

	if (lt == integer_type && rt == integer_type) {
		auto li { std::dynamic_pointer_cast<Integer_Literal>(left) };
		auto ri { std::dynamic_pointer_cast<Integer_Literal>(right) };

		if (li && ri) { return Integer_Literal::create(
			li->value() - ri->value()
		); }
		if (ri && ri->value() == 0) { return left; }

		auto r { Reference::create(gen_.next_id(), integer_type) };
		gen_.append(
			r->name() + " = sub i32 " + left->name() + ", " +
			right->name()
		);
		return r;
	}
	
	left = { propagate_to_real(left) };
	right = { propagate_to_real(right) };
	auto lr { std::dynamic_pointer_cast<Real_Literal>(left) };
	auto rr { std::dynamic_pointer_cast<Real_Literal>(right) };
	if (lr && rr) {
		return Real_Literal::create(lr->value() - rr->value());
	}
	if (rr && rr->value() == 0.0) { return left; }

	auto r { Reference::create(gen_.next_id(), real_type) };
	gen_.append(
		r->name() + " = fsub double " + left->name() + ", " +
		right->name()
	);
	return r;
}

Value::Ptr Parser::parse_conditional_or(Value::Ptr left) {
	if (left->type() != boolean_type) {
		throw Error { "wrong type for OR" };
	}
	if (auto lb { std::dynamic_pointer_cast<Bool_Literal>(left) }) {
		if (lb->value()) {
			gen_.hide();
			parse_term();
			gen_.show();
			return left;
		} else {
			return parse_term();
		}
	} else {
		auto id { std::to_string(gen_.next_or_id()) };
		auto store { Reference::create(gen_.next_id(), boolean_type) };
		gen_.alloca(store);
		gen_.append(
			"store i1 " + left->name() +
			", i1* " + store->name() + ", align 4"
		);
		gen_.conditional(left, "or_end_" + id, "or_alt_" + id);
		gen_.def_label("or_alt_" + id);
		auto right { parse_term() };
		gen_.append(
			"store i1 " + right->name() + 
			", i1* " + store->name() + ", align 4"
		);
		gen_.branch("or_end_" + id);
		gen_.def_label("or_end_" + id);
		auto r { Reference::create(gen_.next_id(), boolean_type) };
		gen_.append(
			r->name() + " = load i1, "
			"i1* " + store->name() + ", align 4"
		);
		return r;
	}
}

Value::Ptr Parser::parse_simple_expression() {
	Value::Ptr left;
	switch (tok_.kind()) {
		case Token_Kind::plus:
			advance();
			left = parse_unary_plus(parse_term());
			break;
		case Token_Kind::minus:
			advance();
			left = parse_unary_minus(parse_term());
			break;
		default:
			left = parse_term();
	}

	if (! left) { left = parse_term(); }
	for (;;) {
		switch (tok_.kind()) {
			case Token_Kind::plus:
				advance();
				left = parse_binary_plus(left, parse_term());
				break;
			case Token_Kind::minus:
				advance();
				left = parse_binary_minus(left, parse_term());
				break;
			case Token_Kind::kw_OR:
				advance();
				left = parse_conditional_or(left);
			default: return left;
		}
	}
}

template<typename FN> Value::Ptr Parser::parse_numeric_predicate(
	std::string cmd, FN fn, Value::Ptr left, Value::Ptr right
) {
	auto lt { left->type() };
	auto rt { left->type() };
	if (! (is_numeric(lt) && is_numeric(rt))) {
		throw Error { "wrong type for predicate" };
	}

	if (lt == integer_type && rt == integer_type) {
		auto li { std::dynamic_pointer_cast<Integer_Literal>(left) };
		auto ri { std::dynamic_pointer_cast<Integer_Literal>(right) };

		if (li && ri) { return Bool_Literal::create(
			fn(li->value(), ri->value())
		); }

		auto r { Reference::create(gen_.next_id(), boolean_type) };
		gen_.append(
			r->name() + " = icmp " + cmd +
			" i32 " + left->name() + ", " + right->name()
		);
		return r;
	}
	
	left = { propagate_to_real(left) };
	right = { propagate_to_real(right) };
	auto lr { std::dynamic_pointer_cast<Real_Literal>(left) };
	auto rr { std::dynamic_pointer_cast<Real_Literal>(right) };
	if (lr && rr) {
		return Real_Literal::create(fn(lr->value(), rr->value()));
	}

	auto r { Reference::create(gen_.next_id(), boolean_type) };
	gen_.append(
		r->name() + " = fcmp " + cmd + " double " + left->name() +
		", " + right->name()
	);
	return r;
}

template<typename FN> Value::Ptr Parser::parse_full_predicate(
	std::string cmd, FN fn, Value::Ptr left, Value::Ptr right
) {
	auto lt { left->type() };
	auto rt { left->type() };
	if (lt == boolean_type && rt == boolean_type) {
		auto lb { std::dynamic_pointer_cast<Bool_Literal>(left) };
		auto rb { std::dynamic_pointer_cast<Bool_Literal>(right) };

		if (lb && rb) { return Bool_Literal::create(
			fn(lb->value(), rb->value())
		); }

		auto r { Reference::create(gen_.next_id(), boolean_type) };
		gen_.append(
			r->name() + " = icmp " + cmd +
			" i1 " + left->name() + ", " + right->name()
		);
		return r;
	}
	return parse_numeric_predicate(cmd, fn, left, right);
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

Value::Ptr Parser::parse_expression() {
	auto left { parse_simple_expression() };
	for (;;) {
		switch (tok_.kind()) {
			case Token_Kind::equal:
				advance();
				left = parse_full_predicate(
					"eq", Equal { }, left,
					parse_simple_expression()
				);
				break;
			case Token_Kind::not_equal:
				advance();
				left = parse_full_predicate(
					"ne", Not_Equal { }, left, 
					parse_simple_expression()
				);
				break;
			case Token_Kind::less:
				advance();
				left = parse_numeric_predicate(
					"slt", Less { }, left, 
					parse_simple_expression()
				);
				break;
			case Token_Kind::less_equal:
				advance();
				left = parse_numeric_predicate(
					"sle", Less_Or_Equal { }, left, 
					parse_simple_expression()
				);
				break;
			case Token_Kind::greater:
				advance();
				left = parse_numeric_predicate(
					"sgt", Greater { }, left, 
					parse_simple_expression()
				);
				break;
			case Token_Kind::greater_equal:
				advance();
				left = parse_numeric_predicate(
					"sge", Greater_Or_Equal { }, left,
					parse_simple_expression()
				);
				break;
			default: return left;
		}
	}
	return left;
}

Value::Ptr Parser::parse_binary_mul(Value::Ptr left, Value::Ptr right) {
	auto lt { left->type() };
	auto rt { left->type() };
	if (! (is_numeric(lt) && is_numeric(rt))) {
		throw Error { "wrong type for binary *" };
	}

	if (lt == integer_type && rt == integer_type) {
		auto li { std::dynamic_pointer_cast<Integer_Literal>(left) };
		auto ri { std::dynamic_pointer_cast<Integer_Literal>(right) };

		if (li && ri) { return Integer_Literal::create(
			li->value() * ri->value()
		); }
		if (li && li->value() == 1) { return right; }
		if (ri && ri->value() == 1) { return left; }
		if ((li && li->value() == 0) || (ri && ri->value() == 0)) {
			return Integer_Literal::create(0);
		}

		auto r { Reference::create(gen_.next_id(), integer_type) };
		gen_.append(
			r->name() + " = mul i32 " + left->name() + ", " +
			right->name()
		);
		return r;
	}
	
	left = { propagate_to_real(left) };
	right = { propagate_to_real(right) };
	auto lr { std::dynamic_pointer_cast<Real_Literal>(left) };
	auto rr { std::dynamic_pointer_cast<Real_Literal>(right) };
	if (lr && rr) {
		return Real_Literal::create(lr->value() + rr->value());
	}
	if (lr && lr->value() == 1.0) { return right; }
	if (rr && rr->value() == 1.0) { return left; }
	if ((lr && lr->value() == 0.0) || (rr && rr->value() == 0.0)) {
		return Real_Literal::create(0.0);
	}

	auto r { Reference::create(gen_.next_id(), real_type) };
	gen_.append(
		r->name() + " = fmul double " + left->name() + ", " +
		right->name()
	);
	return r;
}

Value::Ptr Parser::parse_binary_int_div(Value::Ptr left, Value::Ptr right) {
	if (left->type() != integer_type || right->type() != integer_type) {
		throw Error { "wrong type for DIV" };
	}

	auto li { std::dynamic_pointer_cast<Integer_Literal>(left) };
	auto ri { std::dynamic_pointer_cast<Integer_Literal>(right) };

	if (ri && ri->value() == 1) { return left; }

	if (li && ri) {
		if (ri->value() == 0) {
			throw Error { "division by zero" };
		}
		return Integer_Literal::create(li->value() / ri->value());
	}

	auto r { Reference::create(gen_.next_id(), integer_type) };
	gen_.append(
		r->name() + " = div i32 " + left->name() + ", " +
		right->name()
	);
	return r;
}

Value::Ptr Parser::parse_binary_mod(Value::Ptr left, Value::Ptr right) {
	if (left->type() != integer_type || right->type() != integer_type) {
		throw Error { "wrong type for MOD" };
	}

	auto li { std::dynamic_pointer_cast<Integer_Literal>(left) };
	auto ri { std::dynamic_pointer_cast<Integer_Literal>(right) };

	if (li && ri) {
		return Integer_Literal::create(li->value() % ri->value());
	}

	auto r { Reference::create(gen_.next_id(), integer_type) };
	gen_.append(
		r->name() + " = srem i32 " + left->name() + ", " +
		right->name()
	);
	return r;
}

Value::Ptr Parser::parse_term() {
	auto left { parse_factor() };
	for (;;) {
		switch (tok_.kind()) {
			case Token_Kind::star:
				advance();
				left = parse_binary_mul(left, parse_factor());
				break;
			case Token_Kind::kw_DIV: {
				advance();
				left = parse_binary_int_div(
					left, parse_factor()
				);
				break;
			}
			case Token_Kind::kw_MOD: {
				advance();
				left = parse_binary_mod(left, parse_factor());
			       	break;
			}
			default: return left;
		}
	}
	return left;
}

Value::Ptr Parser::parse_unary_not(Value::Ptr left) {
	auto t { left->type() };
	if (t != boolean_type) {
		throw Error { "wrong type for unary ~" };
	}
	if (auto l { std::dynamic_pointer_cast<Bool_Literal>(left) }) {
		return Bool_Literal::create(! l->value());
	}
	auto r { Reference::create(gen_.next_id(), t) };
	gen_.append(
		r->name() + " = not " + get_ir_type(t) + " " + left->name()
	);
	return r;
}

Value::Ptr Parser::parse_factor() {
	std::shared_ptr<Value> res;
	switch(tok_.kind()) {
		case Token_Kind::integer_literal:
			res = Integer_Literal::create(
				std::stoi(tok_.literal_data())
			);
			advance();
			break;
		case Token_Kind::identifier: {
			auto got { parse_qual_ident() };
			if (auto var { std::dynamic_pointer_cast<Variable>(
				got
			) }) {
				if (var->with_load()) {
					auto r { gen_.next_id() };
					res = Reference::create(
						r, var->type()
					);
					gen_.append(
						res->name() + " = load " +
						get_ir_type(var->type()) +
						", " +
						get_ir_type(var->type()) +
						"* " + var->ref()->name() +
						", align 4"
					);
				} else {
					res = var->ref();
				}
			} else if (auto c { std::dynamic_pointer_cast<Const>(
				got
			) }) {
				res = c->value();
			} else { throw Error { got->name() + " not found" }; }
			break;
		}
		case Token_Kind::kw_FALSE:
			res = Bool_Literal::create(false);
			advance();
			break;
		case Token_Kind::kw_TRUE:
			res = Bool_Literal::create(true);
			advance();
			break;
		case Token_Kind::sym_not:
			advance();
			res = parse_unary_not(parse_expression());
			break;
		case Token_Kind::l_paren:
			advance();
			res = parse_expression();
			consume(Token_Kind::r_paren);
		default:
			throw Error { "no factor: '" + tok_.raw() + "'" };
	}
	return res;
}

Declaration::Ptr Parser::parse_designator() {
	return parse_qual_ident();
	// TODO: selectors
}

void Parser::parse_statement() {
	if (tok_.is(Token_Kind::kw_IF)) {
		auto id { std::to_string(gen_.next_if_id()) };
		int alt { 0 };
		gen_.branch("if_cond_" + id + "_", alt);
		gen_.def_label("if_cond_" + id + "_", alt);
		advance();
		auto expr { parse_expression() };
		gen_.conditional(
			expr, "if_body_" + id + "_", alt,
			"if_cond_" + id + "_", alt + 1
		);
		gen_.def_label("if_body_" + id + "_", alt);
		++alt;
		consume(Token_Kind::kw_THEN);
		parse_statement_sequence();
		gen_.branch("if_end_" + id);
		gen_.def_label("if_cond_" + id + "_", alt);
		while (tok_.is(Token_Kind::kw_ELSIF)) {
			advance();
			auto expr { parse_expression() };
			gen_.conditional(
				expr, "if_body_" + id + "_", alt,
				"if_cond_" + id + "_", alt + 1
			);
			gen_.def_label("if_body_" + id + "_", alt);
			++alt;
			consume(Token_Kind::kw_THEN);
			parse_statement_sequence();
			gen_.branch("if_end_" + id);
		}
		if (tok_.is(Token_Kind::kw_ELSE)) {
			advance();
			parse_statement_sequence();
		}
		gen_.branch("if_end_" + id);
		gen_.def_label("if_end_" + id);
		consume(Token_Kind::kw_END);
		return;
	}
	// TODO: case statement
	if (tok_.is(Token_Kind::kw_WHILE)) {
		auto id { std::to_string(gen_.next_while_id()) };
		int alt { 0 };
		gen_.branch("while_cond_" + id + "_", alt);
		gen_.def_label("while_cond_" + id + "_", alt);
		advance();
		auto expr { parse_expression() };
		gen_.conditional(
			expr, "while_body_" + id + "_", alt,
			"while_cond_" + id + "_", alt + 1
		);
		consume(Token_Kind::kw_DO);
		gen_.def_label("while_body_" + id + "_", alt);
		parse_statement_sequence();
		gen_.branch("while_cond_" + id + "_0");
		++alt;
		gen_.def_label("while_cond_" + id + "_", alt);

		while (tok_.is(Token_Kind::kw_ELSIF)) {
			advance();
			auto expr { parse_expression() };
			gen_.conditional(
				expr, "while_body_" + id + "_", alt,
				"while_cond_" + id + "_", alt + 1
			);
			consume(Token_Kind::kw_DO);
			gen_.def_label("while_body_" + id + "_", alt);
			parse_statement_sequence();
			gen_.branch("while_cond_" + id + "_0");
			++alt;
			gen_.def_label("while_cond_" + id + "_", alt);
		}
		consume(Token_Kind::kw_END);
		return;
	}

	if (tok_.is(Token_Kind::kw_REPEAT)) {
		advance();
		parse_statement_sequence();
		consume(Token_Kind::kw_UNTIL);
		parse_expression();
		return;
	}
	// TODO: for statement

	if (! tok_.is(Token_Kind::identifier)) { return; }

	auto id { parse_designator() };
	if (tok_.is(Token_Kind::assign)) {
		advance();
		auto v { std::dynamic_pointer_cast<Variable>(id) };
		if (! v) { throw Error {
			v->name() + " is no variable for assignment"
		}; }
		auto e { parse_expression() };
		gen_.append(
			"store " + get_ir_type(v->type()) + " " +
			e->name() + ", " + get_ir_type(v->type()) +
			"* " + v->ref()->name() + ", align 4"
		);
	} else if (tok_.is(Token_Kind::l_paren)) {
		advance();
		if (! tok_.is(Token_Kind::r_paren)) {
			parse_expression();
			while (tok_.is(Token_Kind::comma)) {
				advance();
				parse_expression();
			}
		}
		consume(Token_Kind::r_paren);
	}
}

void Parser::parse_statement_sequence() {
	parse_statement();
	while (tok_.is(Token_Kind::semicolon)) {
		advance();
		parse_statement();
	}
}

std::vector<std::string> Parser::parse_ident_list() {
	std::vector<std::string> ids;

	expect(Token_Kind::identifier);
	ids.push_back(tok_.identifier());
	advance();
	while (tok_.is(Token_Kind::comma)) {
		advance();
		expect(Token_Kind::identifier);
		ids.push_back(tok_.identifier());
		advance();
	}
	return ids;
}

Declaration::Ptr Parser::parse_qual_ident() {
	expect(Token_Kind::identifier);
	auto got { current_scope->lookup(tok_.identifier()) };
	if (! got) {
		throw Error {
			"unknown identifier '" + tok_.identifier() + "'"
		};
	}
	advance();
	return got;
}

std::vector<Variable::Ptr> Parser::parse_parameter_declaration(bool is_var) {
	auto ids { parse_ident_list() };
	consume(Token_Kind::colon);
	auto d { parse_qual_ident() };
	auto t { std::dynamic_pointer_cast<Type>(d) };
	if (! t) { throw Error { d->name() + " is no type" }; }
	std::vector<Variable::Ptr> result;
	for (auto &n : ids) {
		auto dcl = Variable::create(
			n, Reference::create(gen_.next_id(), t), is_var, false
		);
		current_scope->insert(dcl);
		result.push_back(dcl);
	}
	return result;
}

std::vector<Variable::Ptr> Parser::parse_variable_declaration() {
	auto ids { parse_ident_list() };
	consume(Token_Kind::colon);
	auto d { parse_qual_ident() };
	auto t { std::dynamic_pointer_cast<Type>(d) };
	if (! t) { throw Error { d->name() + " is no type" }; }
	std::vector<Variable::Ptr> result;
	for (auto &n : ids) {
		auto r { Reference::create(gen_.next_id(), t) };
		gen_.alloca(r);
		auto dcl = Variable::create(n, r, false, true);
		current_scope->insert(dcl);
		result.push_back(dcl);
	}
	return result;
}

Declaration::Ptr Parser::parse_formal_type() {
	if (tok_.is(Token_Kind::kw_ARRAY)) {
		advance();
		consume(Token_Kind::kw_OF);
	}
	return parse_qual_ident();
}

std::vector<Variable::Ptr> Parser::parse_fp_section(Procedure::Ptr decl) {
	bool is_var { tok_.is(Token_Kind::kw_VAR) };
	if (is_var) { advance(); }
	return parse_parameter_declaration(is_var);
}

void Parser::parse_formal_parameters(Procedure::Ptr decl) {
	consume(Token_Kind::l_paren);
	if (! tok_.is_one_of(Token_Kind::r_paren, Token_Kind::eoi)) {
		auto args { parse_fp_section(decl) };
		for (auto arg : args) {
			decl->add_argument(arg);
		}
		while (tok_.is(Token_Kind::comma)) {
			advance();
			auto args { parse_fp_section(decl) };
			for (auto arg : args) {
				decl->add_argument(arg);
			}
		}
	}
	consume(Token_Kind::r_paren);
	if (tok_.is(Token_Kind::colon)) {
		advance();
		auto got { parse_qual_ident() };
		auto ty { std::dynamic_pointer_cast<Type>(got) };
		if (! ty) { Error { got->name() + " is not a type" }; }
		decl->set_returns(ty);
	}

}

std::string Parser::parse_procedure_heading() {
	consume(Token_Kind::kw_PROCEDURE);
	expect(Token_Kind::identifier);
	auto name { tok_.identifier() };
	advance();
	return name;
}

void Parser::parse_procedure_body(Procedure::Ptr decl) {
	parse_declaration_sequence(decl);
	if (tok_.is(Token_Kind::kw_BEGIN)) {
		advance();
		parse_statement_sequence();
	}
	if (tok_.is(Token_Kind::kw_RETURN)) {
		advance();
		gen_.ret(parse_expression());

	} else {
		gen_.ret();
	}
	consume(Token_Kind::kw_END);
}

Procedure::Ptr Parser::parse_procedure_declaration(
	Scoping_Declaration::Ptr parent
) {
	auto name { parse_procedure_heading() };
	auto decl { Procedure::create(name, parent) };
	Pushed_Scope pushed { decl };
	if (tok_.is(Token_Kind::l_paren)) {
		parse_formal_parameters(decl);
	}
	consume(Token_Kind::semicolon);

	gen_.reset();
	std::string def { "define " + get_ir_type(decl->returns()) + " @" };
	def += parent->mangle(decl->name()) + "(";
	int j { 0 };
	for (
		auto i { decl->args_begin() }, e { decl->args_end() };
		i != e; ++i, ++j
	) {
		if (j) { def += ", "; }
		auto r { Reference::create(gen_.next_id(), (**i).type()) };
		def += get_ir_type(r->type()) + " " + r->name();
		(**i).set_ref(r);
	}
	def += ") {";
	gen_.append_raw(def);
	gen_.def_label("entry");
	parse_procedure_body(decl);
	gen_.append_raw("}");
	expect(Token_Kind::identifier);
	if (name != tok_.identifier()) {
		throw Error {
			"PROCEDURE '" + name + "' ends with name '" +
			tok_.identifier() + "'"
		};
	}
	advance();
	return decl;
}

void Parser::parse_declaration_sequence(Scoping_Declaration::Ptr parent) {
	if (tok_.is(Token_Kind::kw_CONST)) {
		advance();
		while (tok_.is(Token_Kind::identifier)) {
			auto name { tok_.identifier() };
			advance();
			consume(Token_Kind::equal);
			auto got { parse_expression() };
			auto lit { std::dynamic_pointer_cast<Literal>(got) };
			if (! lit) {
				throw Error { "expression is not const" };
			}
			if (! current_scope->insert(
				Const::create(name, lit)
			)) {
				throw Error { name + " already defined" };
			}
			consume(Token_Kind::semicolon);
		}
	}
	if (tok_.is(Token_Kind::kw_TYPE)) {
		throw Error { "TYPE not imp]emented" }; // TODO
	}
	if (tok_.is(Token_Kind::kw_VAR)) {
		advance();
		while (! tok_.is_one_of(
			Token_Kind::eoi, Token_Kind::kw_END,
			Token_Kind::kw_BEGIN, Token_Kind::kw_PROCEDURE
		)) {
			auto vars { parse_variable_declaration() };
			consume(Token_Kind::semicolon);
		}
	}
	while (tok_.is(Token_Kind::kw_PROCEDURE)) {
		parse_procedure_declaration(parent);
		consume(Token_Kind::semicolon);
	}
}

Module::Ptr Parser::parse_module() {
	consume(Token_Kind::kw_MODULE);
	expect(Token_Kind::identifier);
	auto mod = Module::create(tok_.identifier());
	current_scope->insert(mod);
	Pushed_Scope pushed { mod };

	advance();
	consume(Token_Kind::semicolon);
	
	if (tok_.is(Token_Kind::kw_IMPORT)) {
		throw Error { "IMPORT not implemented" }; // TODO
	}

	parse_declaration_sequence(mod);

	gen_.reset();
	gen_.append_raw("define void @" + mod->mangle("_init") + "() {");
	gen_.def_label("entry");
	if (tok_.is(Token_Kind::kw_BEGIN)) {
		advance();
		parse_statement_sequence();
	}
	gen_.ret();
	gen_.append_raw("}");

	consume(Token_Kind::kw_END);
	expect(Token_Kind::identifier);
	if (tok_.identifier() != mod->name()) {
		throw Error {
			"MODULE '" + mod->name() + "' ends in name '" +
			tok_.identifier() + "'" 
		};
	}
	advance();
	consume(Token_Kind::period);

	return mod;
};

