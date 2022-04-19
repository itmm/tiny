#include "parser.h"

AST *Parser::parse() {
	auto res { parse_calc() };
	expect(Token::eoi);
	return res;
}

AST *Parser::parse_calc() {
	Expr *e;
	llvm::SmallVector<llvm::StringRef, 8> vars;
	if (tok_.is(Token::KW_with)) {
		advance();
		if (expect(Token::ident)) { goto error_; }
		vars.push_back(tok_.text());
		advance();
		while (tok_.is(Token::comma)) {
			advance();
			if (expect(Token::ident)) { goto error_; }
			vars.push_back(tok_.text());
			advance();
		}
		if (consume(Token::colon)) { goto error_; }
	}
	e = parse_expr();
	if (vars.empty()) { return e; }
	return new With_Decl { vars, e };
error_:
	while (! tok_.is(Token::eoi)) { advance(); }
	return nullptr;
}

Expr *Parser::parse_expr() {
	Expr *left { parse_term() };
	while (tok_.isOneOf(Token::plus, Token::minus)) {
		Binary_Op::Operator op {
			tok_.is(Token::plus) ? Binary_Op::plus : Binary_Op::minus
		};
		advance();
		Expr *right { parse_term() };
		left = new Binary_Op { op, left, right };
	}
	return left;
}

Expr *Parser::parse_term() {
	Expr *left { parse_factor() };
	while (tok_.isOneOf(Token::star, Token::slash)) {
		Binary_Op::Operator op {
			tok_.is(Token::star) ? Binary_Op::mul : Binary_Op::div
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
		case Token::number:
			res = new Factor { Factor::number, tok_.text() };
			advance(); break;
		case Token::ident:
			res = new Factor { Factor::ident, tok_.text() };
			advance(); break;
		case Token::l_paren:
			advance();
			res = parse_expr();
			if (! consume(Token::r_paren)) { break; }
			// fallthrough
		default:
			if (! res) { error(); }
			while (! tok_.isOneOf(Token::r_paren, Token::star, Token::plus, Token::minus, Token::slash, Token::eoi)) {
				advance();
			}
	}
	return res;
}
