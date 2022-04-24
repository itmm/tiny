#pragma once

#include <string>
#include <vector>

class AST;
class Expr;
class Factor;
class Binary_Op;
class With_Decl;
class AST_Visitor {
	public:
		virtual void visit(AST &) { }
		virtual void visit(Expr &) { }
		virtual void visit(Factor &) = 0;
		virtual void visit(Binary_Op &) = 0;
		virtual void visit(With_Decl &) = 0;
};

class AST {
	public:
		virtual ~AST() { }
		virtual void accept(AST_Visitor &) = 0;
};

class Expr: public AST { };

class Factor: public Expr {
	public:
		enum Kind { ident, number };
	private:
		Kind kind_;
		std::string value_;
	public:
		Factor(Kind kind, std::string value): kind_ { kind }, value_ { value } { }
		Kind kind() const { return kind_; }
		std::string value() const { return value_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
			
};

class Binary_Op: public Expr {
	public:
		enum Operator { plus, minus, mul, div, not_equal, mod };
	private:
		Expr *left_;
		Expr *right_;
		Operator op_;
	public:
		Binary_Op(Operator op, Expr *left, Expr *right):
			left_ { left }, right_ { right }, op_ { op }
		{ }
		Expr *left() const { return left_; }
		Expr *right() const { return right_; }
		Operator op() const { return op_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
};

class With_Decl: public Expr {
		using Var_Vector = std::vector<std::string>;
		Var_Vector vars_;
		Expr *e_;
	public:
		With_Decl(Var_Vector vars, Expr *e):
			vars_ { vars }, e_ { e }
		{ }
		Var_Vector::const_iterator begin() { return vars_.begin(); }
		Var_Vector::const_iterator end() { return vars_.end(); }
		Expr *expr() { return e_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
};

class Declaration {
		const Declaration *enclosing_declaration_;
		const std::string name_;

	public:
		Declaration(Declaration *enclosing_declaration, std::string name):
			enclosing_declaration_ { enclosing_declaration }, name_ { name }
		{ }
		virtual ~Declaration() { }

		const std::string &name() const { return name_; }
		const Declaration *enclosing_decl() const { return enclosing_declaration_; }
};

using Decl_List = std::vector<Declaration *>;
using Ident_List = std::vector<std::string>;

class Module_Declaration: public Declaration {
	public:
		Module_Declaration(Declaration *enclosing_declaration, std::string name):
			Declaration(enclosing_declaration, name)
		{ }
};

class Procedure_Declaration: public Declaration {
	public:
		Procedure_Declaration(Declaration *enclosing_declaration, std::string name):
			Declaration(enclosing_declaration, name)
		{ }
};

class Type_Declaration: public Declaration {
	public:
		Type_Declaration(Declaration *enclosing_declaration, std::string name):
			Declaration(enclosing_declaration, name)
		{ }
};

class Variable_Declaration: public Declaration {
		Type_Declaration *type_;
	public:
		Variable_Declaration(
			Declaration *enclosing_declaration, std::string name,
			Type_Declaration *type
		):
			Declaration(enclosing_declaration, name), type_ { type }
		{ }

		Type_Declaration *type() { return type_; }
};

