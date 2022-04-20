#include "parser.h"

AST *Parser::parse() {
	auto res { parse_calc() };
	expect(Token_Kind::eoi);
	return res;
}

AST *Parser::parse_calc() {
	Expr *e;
	llvm::SmallVector<llvm::StringRef, 8> vars;
	if (tok_.is(Token_Kind::kw_WITH)) {
		advance();
		if (expect(Token_Kind::identifier)) { goto error_; }
		vars.push_back(tok_.identifier());
		advance();
		while (tok_.is(Token_Kind::comma)) {
			advance();
			if (expect(Token_Kind::identifier)) { goto error_; }
			vars.push_back(tok_.identifier());
			advance();
		}
		if (consume(Token_Kind::colon)) { goto error_; }
	}
	e = parse_expr();
	if (vars.empty()) { return e; }
	return new With_Decl { vars, e };
error_:
	while (! tok_.is(Token_Kind::eoi)) { advance(); }
	return nullptr;
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
			if (! consume(Token_Kind::r_paren)) { break; }
			// fallthrough
		default:
			if (! res) { error(); }
			while (! tok_.is_one_of(
				Token_Kind::r_paren, Token_Kind::star,
				Token_Kind::plus, Token_Kind::minus,
				Token_Kind::slash, Token_Kind::eoi
			)) {
				advance();
			}
	}
	return res;
}

bool Parser::parse_statement_sequence() {
	return true;
}

bool Parser::parse_if() {
	auto error_handler = [this] {
		return skip_until(
			Token_Kind::semicolon, Token_Kind::kw_ELSE,
			Token_Kind::kw_END
		);
	};
	if (consume(Token_Kind::kw_IF)) { return error_handler(); }
	if (parse_expr()) { return error_handler(); }
	if (consume(Token_Kind::kw_THEN)) { return error_handler(); }
	if (parse_statement_sequence()) { return error_handler(); }
	if (tok_.is(Token_Kind::kw_ELSE)) {
		advance();
		if (parse_statement_sequence()) { return error_handler(); }
	}
	if (consume(Token_Kind::kw_END)) { return error_handler(); }
	return false;
};
