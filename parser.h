#pragma once

#include "ast.h"
#include "err.h"
#include "lexer.h"

#include "llvm/IR/IRBuilder.h"

class Parser {
		Lexer &lexer_;
		llvm::Module &mod_;
		llvm::IRBuilder<> &builder_;
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
		std::vector<Variable_Declaration::Ptr> parse_variable_declaration(bool is_var);
		Declaration::Ptr parse_formal_type();
		std::vector<Variable_Declaration::Ptr> parse_fp_section(Procedure_Declaration::Ptr decl);
		void parse_formal_parameters(Procedure_Declaration::Ptr decl);
		std::string parse_procedure_heading();
		void parse_procedure_body(Procedure_Declaration::Ptr decl);
		Procedure_Declaration::Ptr parse_procedure_declaration(Scoping_Declaration::Ptr parent);
		void parse_declaration_sequence(Scoping_Declaration::Ptr parent);
		Module_Declaration::Ptr parse_module();

	public:
		Parser(Lexer &lexer, llvm::Module &mod, llvm::IRBuilder<> &builder): lexer_ { lexer }, mod_ { mod }, builder_ { builder } { advance(); }

		void parse();
};

