#pragma once

#include "ast.h"
#include "lexer.h"

#include "llvm/Support/raw_ostream.h"

class Parser {
		Lexer &lexer_;
		Token tok_;
		bool has_error_ { false };

		void error() {
			llvm::errs() << "Unexpected: '" << tok_.raw() << "'\n";
			has_error_ = true;
		}

		void advance() { lexer_.next(tok_); }

		bool expect(Token_Kind k) {
			if (tok_.kind() != k) {
				error();
				return true;
			}
			return false;
		}

		bool consume(Token_Kind k) {
			if (expect(k)) { return true; }
			advance();
			return false;
		}

		template<typename... Ts>
			bool skip_until(Ts... ks) {
				while (! tok_.is(Token_Kind::eoi) && ! tok_.is_one_of(ks...)) {
					advance();
				}
				return true;
			}

		AST *parse_calc();
		Expr *parse_expr();
		Expr *parse_term();
		Expr *parse_factor();
		bool parse_statement_sequence();
		bool parse_if();

	public:
		Parser(Lexer &lexer): lexer_ { lexer } {
			advance();
	       	}

		bool has_error() const { return has_error_; }

		AST *parse();
};

