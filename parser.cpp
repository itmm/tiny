#include "parser.h"
#include "scope.h"

void Parser::parse() {
	parse_module();
	expect(Token_Kind::eoi);
}

Expression::Ptr Parser::parse_plus_minus(Expression::Ptr left) {
	if (tok_.is_one_of(Token_Kind::plus, Token_Kind::minus)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::plus) ? Binary_Op::plus : Binary_Op::minus
		};
		advance();
		auto right { parse_term() };
		return Binary_Op::create(op, Integer_Literal::create(0), right);
	} else {
		return nullptr;
	}
}

Expression::Ptr Parser::parse_simple_expression() {
	Expression::Ptr left { parse_plus_minus(Integer_Literal::create(0)) };
	if (! left) { left = parse_term(); }
	while (auto got { parse_plus_minus(left) }) {
		left = got;
	}
	return left;
}

Expression::Ptr Parser::parse_expression() {
	auto left { parse_simple_expression() };
	for (;;) {
		auto op { Binary_Op::none };
		switch (tok_.kind()) {
			case Token_Kind::equal: op = Binary_Op::equal; break;
			case Token_Kind::not_equal: op = Binary_Op::not_equal; break;
			case Token_Kind::less: op = Binary_Op::less; break;
			case Token_Kind::less_equal: op = Binary_Op::less_equal; break;
			case Token_Kind::greater: op = Binary_Op::greater; break;
			case Token_Kind::greater_equal: op = Binary_Op::greater_equal; break;
			default: break;
		}
		if (op == Binary_Op::none) { break; }
		advance();
		auto right { parse_simple_expression() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

Expression::Ptr Parser::parse_term() {
	auto left { parse_factor() };
	for (;;) {
		auto op { Binary_Op::none };
		switch (tok_.kind()) {
			case Token_Kind::star: op = Binary_Op::mul; break;
			case Token_Kind::kw_DIV: op = Binary_Op::div; break;
			case Token_Kind::kw_MOD: op = Binary_Op::mod; break;
			default: break;
		}
		if (op == Binary_Op::none) { break; }
		advance();
		auto right { parse_factor() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

Expression::Ptr Parser::parse_factor() {
	std::shared_ptr<Expression> res;
	switch(tok_.kind()) {
		case Token_Kind::integer_literal:
			res = Integer_Literal::create(std::stoi(tok_.literal_data()));
			advance(); break;
		case Token_Kind::identifier: {
			auto got { parse_qual_ident() };
			if (auto var { std::dynamic_pointer_cast<Variable_Declaration>(got) }) {
				res = var->variable();
			} else if (auto cnst { std::dynamic_pointer_cast<Const_Declaration>(got) }) {
				res = cnst->value();
			} else { throw Error { got->name() + " not found" }; }
			break;
		}
		case Token_Kind::kw_FALSE:
			res = Bool_Literal::create(false);
			advance(); break;
		case Token_Kind::kw_TRUE:
			res = Bool_Literal::create(true);
			advance(); break;
		case Token_Kind::l_paren:
			advance();
			res = parse_expression();
			consume(Token_Kind::r_paren);
		default:
			throw Error { "no factor: '" + tok_.raw() + "'" };
	}
	return res;
}

void Parser::parse_designator() {
	parse_qual_ident();
	// TODO: selectors
}

void Parser::parse_statement() {
	if (tok_.is(Token_Kind::kw_IF)) {
		advance();
		parse_expression();
		consume(Token_Kind::kw_THEN);
		parse_statement_sequence();
		while (tok_.is(Token_Kind::kw_ELSIF)) {
			advance();
			parse_expression();
			consume(Token_Kind::kw_THEN);
			parse_statement_sequence();
		}
		if (tok_.is(Token_Kind::kw_ELSE)) {
			advance();
			parse_statement_sequence();
		}
		consume(Token_Kind::kw_END);
		return;
	}
	// TODO: case statement
	if (tok_.is(Token_Kind::kw_WHILE)) {
		advance();
		parse_expression();
		consume(Token_Kind::kw_DO);
		parse_statement_sequence();
		while (tok_.is(Token_Kind::kw_ELSIF)) {
			advance();
			parse_expression();
			consume(Token_Kind::kw_DO);
			parse_statement_sequence();
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

	parse_designator();
	if (tok_.is(Token_Kind::assign)) {
		advance();
		parse_expression();
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

void Parser::parse_if() {
	consume(Token_Kind::kw_IF);
	parse_expression();
	consume(Token_Kind::kw_THEN);
	parse_statement_sequence();
	if (tok_.is(Token_Kind::kw_ELSE)) {
		advance();
		parse_statement_sequence();
	}
	consume(Token_Kind::kw_END);
};

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

void Parser::parse_variable_declaration() {
	auto ids { parse_ident_list() };
	consume(Token_Kind::colon);
	auto d { parse_qual_ident() };
	auto t { std::dynamic_pointer_cast<Type_Declaration>(d) };
	if (! t) { throw Error { d->name() + " is no type" }; }
	for (auto &n : ids) {
		auto dcl = Variable_Declaration::create(Variable::create(n, t));
		current_scope->insert(dcl);
	}
}

Declaration::Ptr Parser::parse_formal_type() {
	if (tok_.is(Token_Kind::kw_ARRAY)) {
		advance();
		consume(Token_Kind::kw_OF);
	}
	return parse_qual_ident();
}

void Parser::parse_fp_section() {
	if (tok_.is(Token_Kind::kw_VAR)) { advance(); }
	parse_variable_declaration();
}

void Parser::parse_formal_parameters() {
	consume(Token_Kind::l_paren);
	if (! tok_.is_one_of(Token_Kind::r_paren, Token_Kind::eoi)) {
		parse_fp_section();
		while (tok_.is(Token_Kind::comma)) {
			advance();
			parse_fp_section();
		}
	}
	consume(Token_Kind::r_paren);
	if (tok_.is(Token_Kind::colon)) {
		advance();
		parse_qual_ident();
	}
}

std::string Parser::parse_procedure_heading() {
	consume(Token_Kind::kw_PROCEDURE);
	expect(Token_Kind::identifier);
	auto name { tok_.identifier() };
	advance();
	return name;
}

void Parser::parse_procedure_body() {
	parse_declaration_sequence();
	if (tok_.is(Token_Kind::kw_BEGIN)) {
		advance();
		parse_statement_sequence();
	}
	if (tok_.is(Token_Kind::kw_RETURN)) {
		advance();
		parse_expression();
	}
	consume(Token_Kind::kw_END);
}

void Parser::parse_procedure_declaration() {
	auto name { parse_procedure_heading() };
	auto p { Procedure_Declaration::create(name) };
	Pushed_Scope pushed { p };
	if (tok_.is(Token_Kind::l_paren)) {
		parse_formal_parameters();
	}
	consume(Token_Kind::semicolon);
	parse_procedure_body();
	expect(Token_Kind::identifier);
	if (name != tok_.identifier()) {
		throw Error {
			"PROCEDURE '" + name + "' ends with name '" +
			tok_.identifier() + "'"
		};
	}
	advance();
}

void Parser::parse_declaration_sequence() {
	if (tok_.is(Token_Kind::kw_CONST)) {
		advance();
		while (tok_.is(Token_Kind::identifier)) {
			auto name { tok_.identifier() };
			advance();
			consume(Token_Kind::equal);
			auto got { parse_expression() };
			auto lit { std::dynamic_pointer_cast<Literal>(got) };
			if (! lit) { throw Error { "expression is not const" }; }
			if (! current_scope->insert(Const_Declaration::create(name, lit))) {
				throw Error { name + " already defined" };
			}
			consume(Token_Kind::semicolon);
		}
		throw Error { "CONST not implemented" };
	}
	if (tok_.is(Token_Kind::kw_TYPE)) {
		throw Error { "TYPE not imp]emented" };
	}
	if (tok_.is(Token_Kind::kw_VAR)) {
		advance();
		while (! tok_.is_one_of(
			Token_Kind::eoi, Token_Kind::kw_END,
			Token_Kind::kw_BEGIN, Token_Kind::kw_PROCEDURE
		)) {
			parse_variable_declaration();
			consume(Token_Kind::semicolon);
		}
	}
	while (tok_.is(Token_Kind::kw_PROCEDURE)) {
		parse_procedure_declaration();
		consume(Token_Kind::semicolon);
	}
}

Module_Declaration::Ptr Parser::parse_module() {
	consume(Token_Kind::kw_MODULE);
	expect(Token_Kind::identifier);
	auto mod = Module_Declaration::create(tok_.identifier());
	current_scope->insert(mod);
	Pushed_Scope pushed { mod };

	advance();
	consume(Token_Kind::semicolon);
	
	if (tok_.is(Token_Kind::kw_IMPORT)) {
		throw Error { "IMPORT not implemented" };
	}

	parse_declaration_sequence();

	if (tok_.is(Token_Kind::kw_BEGIN)) {
		advance();
		parse_statement_sequence();
	}

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

