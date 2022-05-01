#pragma once

#include "ast.h"
#include "err.h"
#include "gen.h"
#include "lexer.h"

class Parser {
		Lexer &lexer_;
		Token tok_;
		Gen gen_;

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

		Value::Ptr parse_expression();
		Value::Ptr parse_unary_plus(Value::Ptr left);
		Value::Ptr parse_unary_minus(Value::Ptr left);
		Value::Ptr parse_binary_plus(Value::Ptr left, Value::Ptr right);
		Value::Ptr parse_binary_minus(Value::Ptr left, Value::Ptr right);
		Value::Ptr parse_simple_expression();
		Value::Ptr parse_term();
		Value::Ptr parse_factor();
		Declaration::Ptr parse_designator();
		void parse_statement();
		void parse_statement_sequence();
		void parse_if();

		std::vector<std::string> parse_ident_list();
		Declaration::Ptr parse_qual_ident();
		std::vector<Variable_Declaration::Ptr> parse_variable_declaration();
		std::vector<Variable_Declaration::Ptr> parse_parameter_declaration(bool is_var);
		Declaration::Ptr parse_formal_type();
		std::vector<Variable_Declaration::Ptr> parse_fp_section(Procedure_Declaration::Ptr decl);
		void parse_formal_parameters(Procedure_Declaration::Ptr decl);
		std::string parse_procedure_heading();
		void parse_procedure_body(Procedure_Declaration::Ptr decl);
		Procedure_Declaration::Ptr parse_procedure_declaration(Scoping_Declaration::Ptr parent);
		void parse_declaration_sequence(Scoping_Declaration::Ptr parent);
		Module_Declaration::Ptr parse_module();

	public:
		Parser(Lexer &lexer): lexer_ { lexer } { advance(); }

		void parse();
};

