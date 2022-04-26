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

		std::shared_ptr<Expression> parse_expression();
		std::shared_ptr<Expression> parse_simple_expression();
		std::shared_ptr<Expression> parse_term();
		std::shared_ptr<Expression> parse_factor();
		void parse_designator();
		void parse_statement();
		void parse_statement_sequence();
		void parse_if();

		std::vector<std::string> parse_ident_list();
		std::shared_ptr<Declaration> parse_qual_ident();
		void parse_variable_declaration();
		std::shared_ptr<Declaration> parse_formal_type();
		void parse_fp_section();
		void parse_formal_parameters();
		std::string parse_procedure_heading();
		void parse_procedure_body();
		void parse_procedure_declaration();
		void parse_declaration_sequence();
		std::shared_ptr<Module_Declaration> parse_module();

	public:
		Parser(Lexer &lexer): lexer_ { lexer } { advance(); }

		void parse();
};

