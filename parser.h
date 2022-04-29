#pragma once

#include "ast.h"
#include "err.h"
#include "lexer.h"

class Parser {
		Lexer &lexer_;
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

		Expression::Ptr parse_expression();
		Expression::Ptr parse_plus_minus(Expression::Ptr left);
		Expression::Ptr parse_simple_expression();
		Expression::Ptr parse_term();
		Expression::Ptr parse_factor();
		void parse_designator();
		void parse_statement();
		void parse_statement_sequence();
		void parse_if();

		std::vector<std::string> parse_ident_list();
		Declaration::Ptr parse_qual_ident();
		void parse_variable_declaration();
		Declaration::Ptr parse_formal_type();
		void parse_fp_section();
		void parse_formal_parameters();
		std::string parse_procedure_heading();
		void parse_procedure_body();
		void parse_procedure_declaration();
		void parse_declaration_sequence();
		Module_Declaration::Ptr parse_module();

	public:
		Parser(Lexer &lexer): lexer_ { lexer } { advance(); }

		void parse();
};

