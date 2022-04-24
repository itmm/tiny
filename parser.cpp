#include "parser.h"
#include "scope.h"

void Parser::parse() {
	parse_module();
	expect(Token_Kind::eoi);
}

Expr *Parser::parse_simple_expression() {
	Expr *left { parse_term() };
	while (tok_.is_one_of(Token_Kind::plus, Token_Kind::minus)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::plus) ? Binary_Op::plus : Binary_Op::minus
		};
		advance();
		Expr *right { parse_term() };
		left = new Binary_Op { op, left, right };
	}
	return left;
}

Expr *Parser::parse_expression() {
	Expr *left { parse_simple_expression() };
	while (tok_.is(Token_Kind::not_equal)) {
		Binary_Op::Operator op { Binary_Op::not_equal };
		advance();
		Expr * right { parse_simple_expression() };
		left = new Binary_Op { op, left, right };
	}
	return left;
}

Expr *Parser::parse_term() {
	Expr *left { parse_factor() };
	while (tok_.is_one_of(Token_Kind::star, Token_Kind::slash, Token_Kind::kw_MOD)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::star) ? Binary_Op::mul :
			tok_.is(Token_Kind::kw_MOD) ? Binary_Op::mod : Binary_Op::div
		};
		advance();
		Expr *right { parse_factor() };
		left = new Binary_Op { op, left, right };
	}
	return left;
}

Expr *Parser::parse_factor() {
	Expr *res { nullptr };
	switch(tok_.kind()) {
		case Token_Kind::integer_literal:
			res = new Factor { Factor::number, tok_.literal_data() };
			advance(); break;
		case Token_Kind::identifier:
			res = new Factor { Factor::ident, tok_.identifier() };
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
	//parse_qual_ident();
	consume(Token_Kind::identifier);
	// TODO: selectors
}

void Parser::parse_statement() {
	// Procedure Call
	// if statement
	// case statement
	// while statement
	if (tok_.is(Token_Kind::kw_WHILE)) {
		advance();
		parse_expression();
		consume(Token_Kind::kw_DO);
		parse_statement_sequence();
		consume(Token_Kind::kw_END);
		return;
	}

	// repeat statement
	// for statement

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

Declaration *Parser::parse_qual_ident() {
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
	Declaration *d { parse_qual_ident() };
	actions_.act_on_variable_declaration(decls, ids, d);
}

void Parser::parse_formal_type() {
	if (tok_.is(Token_Kind::kw_ARRAY)) {
		advance();
		consume(Token_Kind::kw_OF);
	}
	parse_qual_ident();
}

void Parser::parse_fp_section() {
	if (tok_.is(Token_Kind::kw_VAR)) { advance(); }
	expect(Token_Kind::identifier);
	advance();
	while (tok_.is(Token_Kind::comma)) {
		advance();
		expect(Token_Kind::identifier);
		advance();
	}
	consume(Token_Kind::colon);
	parse_formal_type();
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
	if (tok_.is(Token_Kind::l_paren)) {
		parse_formal_parameters();
	}
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
	auto p { new Procedure_Declaration { parent_declaration, name } };
	Pushed_Scope pushed { p };
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

Module_Declaration *Parser::parse_module() {
	consume(Token_Kind::kw_MODULE);
	expect(Token_Kind::identifier);
	auto mod = new Module_Declaration { nullptr, tok_.identifier() };
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

