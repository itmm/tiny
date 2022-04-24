#pragma once

#include "ast.h"
#include "err.h"
#include "lexer.h"
#include "sema.h"

#include "llvm/Support/raw_ostream.h"

using namespace std::literals::string_literals;

class Parser {
		Lexer &lexer_;
		Sema &actions_;
		Token tok_;

		void error() {
			throw Error { "Unexpected: '" + (std::string) tok_.raw() + "'\n" };
		}

		void advance() { lexer_.next(tok_); }

		void expect(Token_Kind k) {
			if (tok_.kind() != k) { error(); }
		}

		void consume(Token_Kind k) {
			expect(k); advance();
		}

		Expr *parse_expr();
		Expr *parse_term();
		Expr *parse_factor();
		void parse_statement_sequence();
		void parse_if();

		void parse_ident_list(Ident_List &ids);
		void parse_qual_ident(Decl *decl);
		void parse_variable_declaration(Decl_List &decls);
		void parse_module();

	public:
		Parser(Lexer &lexer, Sema &actions):
			lexer_ { lexer }, actions_ { actions }
		{
			advance();
	       	}

		void parse();
};

