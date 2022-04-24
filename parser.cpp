#include "parser.h"
#include "scope.h"

void Parser::parse() {
	parse_module();
	expect(Token_Kind::eoi);
}

Expr *Parser::parse_expr() {
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

Expr *Parser::parse_term() {
	Expr *left { parse_factor() };
	while (tok_.is_one_of(Token_Kind::star, Token_Kind::slash)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::star) ? Binary_Op::mul : Binary_Op::div
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
			res = parse_expr();
			consume(Token_Kind::r_paren);
		default:
			throw Error { "no factor: '" + tok_.raw() + "'" };
	}
	return res;
}

void Parser::parse_statement_sequence() {
	throw Error { "Statement_Sequence not implemented yet" };
}

void Parser::parse_if() {
	consume(Token_Kind::kw_IF);
	parse_expr();
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

void Parser::parse_qual_ident(Decl *decl) {
	expect(Token_Kind::identifier);
	auto got { current_scope->lookup(tok_.identifier()) };
	if (! got) {
		throw Error { "unknown identifier '" + tok_.identifier() + "'" };
	}
	advance();
	decl = got;
}

void Parser::parse_variable_declaration(Decl_List &decls) {
	Decl *d { nullptr };
	Ident_List ids;
	parse_ident_list(ids);
	consume(Token_Kind::colon);
	parse_qual_ident(d);
	actions_.act_on_variable_declaration(decls, ids, d);
}

void Parser::parse_module() {
	consume(Token_Kind::kw_MODULE);
	expect(Token_Kind::identifier);
	auto name = tok_.identifier();
	// TODO: act on module creation
	advance();
	consume(Token_Kind::semicolon);
	
	if (tok_.is(Token_Kind::kw_IMPORT)) {
		throw Error { "IMPORT not implemented" };
	}

	for (;;) {
		if (tok_.is(Token_Kind::kw_PROCEDURE)) {
			throw Error { "PROCEDURE not implemented" };
		} else if (tok_.is(Token_Kind::kw_CONST)) {
			throw Error { "CONST not implemented" };
		} else if (tok_.is(Token_Kind::kw_VAR)) {
			throw Error { "VAR not implemented" };
		} else if (tok_.is(Token_Kind::kw_TYPE)) {
			throw Error { "TYPE not imp]emented" };
		} else break;
	}

	if (tok_.is(Token_Kind::kw_BEGIN)) {
		throw Error { "MODULE statements not implemented" };
	}

	consume(Token_Kind::kw_END);
	expect(Token_Kind::identifier);
	if (tok_.identifier() != name) {
		throw Error { "MODULE '" + name + "' ends in name '" +
			tok_.identifier() + "'" };
	}
	advance();
	consume(Token_Kind::period);
};

