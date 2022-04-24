#pragma once

#include "ast.h"
#include "err.h"
#include "lexer.h"
#include "sema.h"

class Parser {
		Lexer &lexer_;
		Sema &actions_;
		Token tok_;

		void error() {
			throw Error { "Unexpected: '" + tok_.raw() + "'\n" };
		}

		void advance() { lexer_.next(tok_); }

		void expect(Token_Kind k) {
			if (tok_.kind() != k) { error(); }
		}

		void consume(Token_Kind k) {
			expect(k); advance();
		}

		Expr *parse_expression();
		Expr *parse_simple_expression();
		Expr *parse_term();
		Expr *parse_factor();
		void parse_designator();
		void parse_statement();
		void parse_statement_sequence();
		void parse_if();

		void parse_ident_list(Ident_List &ids);
		Decl *parse_qual_ident();
		void parse_variable_declaration(Decl_List &decls);
		void parse_formal_type();
		void parse_fp_section();
		void parse_formal_parameters();
		std::string parse_procedure_heading();
		void parse_procedure_body();
		void parse_procedure_declaration();
		void parse_declaration_sequence();
		void parse_module();

	public:
		Parser(Lexer &lexer, Sema &actions):
			lexer_ { lexer }, actions_ { actions }
		{
			advance();
	       	}

		void parse();
};

