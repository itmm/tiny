#pragma once

#include "ast.h"
#include "lexer.h"

#include "llvm/Support/raw_ostream.h"

class Parser {
		Lexer &lexer_;
		Token tok_;
		bool has_error_ { false };

		void error() {
			llvm::errs() << "Unexpected: " << tok_.text() << '\n';
			has_error_ = true;
		}

		void advance() { lexer_.next(tok_); }

		bool expect(Token::Kind k) {
			if (tok_.kind() != k) {
				error();
				return true;
			}
			return false;
		}

		bool consume(Token::Kind k) {
			if (expect(k)) { return true; }
			advance();
			return false;
		}

		AST *parse_calc();
		Expr *parse_expr();
		Expr *parse_term();
		Expr *parse_factor();
	
	public:
		Parser(Lexer &lexer): lexer_ { lexer } {
			advance();
	       	}

		bool has_error() const { return has_error_; }

		AST *parse();
};

