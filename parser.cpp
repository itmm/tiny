#include "parser.h"
#include "scope.h"

void Parser::parse() {
	parse_module();
	expect(Token_Kind::eoi);
}

std::shared_ptr<Expression> Parser::parse_simple_expression() {
	auto left { parse_term() };
	while (tok_.is_one_of(Token_Kind::plus, Token_Kind::minus)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::plus) ? Binary_Op::plus : Binary_Op::minus
		};
		advance();
		auto right { parse_term() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

std::shared_ptr<Expression> Parser::parse_expression() {
	auto left { parse_simple_expression() };
	while (tok_.is(Token_Kind::not_equal)) {
		Binary_Op::Operator op { Binary_Op::not_equal };
		advance();
		auto right { parse_simple_expression() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

std::shared_ptr<Expression> Parser::parse_term() {
	auto left { parse_factor() };
	while (tok_.is_one_of(Token_Kind::star, Token_Kind::slash, Token_Kind::kw_MOD)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::star) ? Binary_Op::mul :
			tok_.is(Token_Kind::kw_MOD) ? Binary_Op::mod : Binary_Op::div
		};
		advance();
		auto right { parse_factor() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

std::shared_ptr<Expression> Parser::parse_factor() {
	std::shared_ptr<Expression> res;
	switch(tok_.kind()) {
		case Token_Kind::integer_literal:
			res = Integer_Literal::create(std::stoi(tok_.literal_data()));
			advance(); break;
		case Token_Kind::identifier:
			res = std::static_pointer_cast<Expression>(parse_qual_ident());
			break;
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
	// TODO: Procedure Call
	// TODO: if statement
	// TODO: case statement
	if (tok_.is(Token_Kind::kw_WHILE)) {
		advance();
		parse_expression();
		consume(Token_Kind::kw_DO);
		parse_statement_sequence();
		consume(Token_Kind::kw_END);
		return;
	}

	// TODO: repeat statement
	// TODO: for statement

	if (! tok_.is(Token_Kind::identifier)) { return; }

	parse_designator();
	if (tok_.is(Token_Kind::assign)) {
		advance();
		parse_expression();
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

void Parser::parse_ident_list(Ident_List &ids) {
	expect(Token_Kind::identifier);
	ids.push_back(tok_.identifier());
	advance();
	while (tok_.is(Token_Kind::comma)) {
		advance();
		expect(Token_Kind::identifier);
		ids.push_back(tok_.identifier());
		advance();
	}
}

std::shared_ptr<Declaration> Parser::parse_qual_ident() {
	expect(Token_Kind::identifier);
	auto got { current_scope->lookup(tok_.identifier()) };
	if (! got) {
		throw Error { "unknown identifier '" + tok_.identifier() + "'" };
	}
	advance();
	return got;
}

void Parser::parse_variable_declaration(Decl_List &decls) {
	Ident_List ids;
	parse_ident_list(ids);
	consume(Token_Kind::colon);
	auto d { parse_qual_ident() };
	auto t { dynamic_cast<Type_Declaration *>(d.get()) };
	if (! t) { throw Error { d->name() + " is no type" }; }
	for (auto &n : ids) {
		auto dcl = Variable_Declaration::create(n, std::dynamic_pointer_cast<Type_Declaration>(d));
		current_scope->insert(dcl);
		decls.push_back(dcl);
	}
	actions_.act_on_variable_declaration(decls, ids, d.get());
}

std::shared_ptr<Declaration> Parser::parse_formal_type() {
	if (tok_.is(Token_Kind::kw_ARRAY)) {
		advance();
		consume(Token_Kind::kw_OF);
	}
	return parse_qual_ident();
}

void Parser::parse_fp_section() {
	if (tok_.is(Token_Kind::kw_VAR)) { advance(); }
	Ident_List ids;
	expect(Token_Kind::identifier);
	ids.push_back(tok_.identifier());
	advance();
	while (tok_.is(Token_Kind::comma)) {
		advance();
		expect(Token_Kind::identifier);
		ids.push_back(tok_.identifier());
		advance();
	}
	consume(Token_Kind::colon);
	auto got { parse_formal_type() };
	auto t { dynamic_cast<Type_Declaration *>(got.get()) };
	if (! t) { throw Error { got->name() + " is no type" }; }
	for (auto &n : ids) {
		auto dcl = Variable_Declaration::create(n, std::dynamic_pointer_cast<Type_Declaration>(got));
		current_scope->insert(dcl);
	}
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
		throw Error { "PROCEDURE '" + name + "' ends with name '" + tok_.identifier() + "'" };
	}
	advance();
}

void Parser::parse_declaration_sequence() {
	if (tok_.is(Token_Kind::kw_CONST)) {
		throw Error { "CONST not implemented" };
	}
	if (tok_.is(Token_Kind::kw_TYPE)) {
		throw Error { "TYPE not imp]emented" };
	}
	if (tok_.is(Token_Kind::kw_VAR)) {
		advance();
		while (! tok_.is_one_of(Token_Kind::eoi, Token_Kind::kw_END, Token_Kind::kw_BEGIN, Token_Kind::kw_PROCEDURE)) {
			Decl_List l;
			parse_variable_declaration(l);
			consume(Token_Kind::semicolon);
		}
	}
	while (tok_.is(Token_Kind::kw_PROCEDURE)) {
		parse_procedure_declaration();
		consume(Token_Kind::semicolon);
	}
}

std::shared_ptr<Module_Declaration> Parser::parse_module() {
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
		throw Error { "MODULE statements not implemented" };
	}

	consume(Token_Kind::kw_END);
	expect(Token_Kind::identifier);
	if (tok_.identifier() != mod->name()) {
		throw Error { "MODULE '" + mod->name() + "' ends in name '" +
			tok_.identifier() + "'" };
	}
	advance();
	consume(Token_Kind::period);
	return mod;
};

