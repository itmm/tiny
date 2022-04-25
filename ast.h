#pragma once

#include <string>
#include <memory>
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
		static std::shared_ptr<Factor> create(Kind kind, std::string value) {
			return std::make_shared<Factor>(kind, value);
		}
		Kind kind() const { return kind_; }
		std::string value() const { return value_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
			
};

class Binary_Op: public Expr {
	public:
		enum Operator { plus, minus, mul, div, not_equal, mod };
	private:
		std::shared_ptr<Expr> left_;
		std::shared_ptr<Expr> right_;
		Operator op_;
	public:
		Binary_Op(Operator op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right):
			left_ { left }, right_ { right }, op_ { op }
		{ }
		static std::shared_ptr<Binary_Op> create(
			Operator op, std::shared_ptr<Expr> left,
			std::shared_ptr<Expr> right
		) {
			return std::make_shared<Binary_Op>(op, left, right);
		}
		std::shared_ptr<Expr> left() const { return left_; }
		std::shared_ptr<Expr> right() const { return right_; }
		Operator op() const { return op_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
};

class Declaration {
		std::shared_ptr<Declaration> enclosing_declaration_;
		const std::string name_;

	protected:
		Declaration(std::shared_ptr<Declaration> enclosing_declaration, std::string name):
			enclosing_declaration_ { enclosing_declaration }, name_ { name }
		{ }
	public:
		virtual ~Declaration() { }

		const std::string &name() const { return name_; }
		std::shared_ptr<Declaration> enclosing_decl() const { return enclosing_declaration_; }
};

using Decl_List = std::vector<std::shared_ptr<Declaration>>;
using Ident_List = std::vector<std::string>;

class Module_Declaration: public Declaration {
	public:
		Module_Declaration(std::shared_ptr<Declaration> enclosing_declaration, std::string name):
			Declaration(enclosing_declaration, name)
		{ }
		static std::shared_ptr<Module_Declaration> create(
			std::shared_ptr<Declaration> enclosing_declaration, std::string name
		) {
			return std::make_shared<Module_Declaration>(enclosing_declaration, name);
		}
};

class Procedure_Declaration: public Declaration {
	public:
		Procedure_Declaration(std::shared_ptr<Declaration> enclosing_declaration, std::string name):
			Declaration(enclosing_declaration, name)
		{ }
		static std::shared_ptr<Procedure_Declaration> create(
			std::shared_ptr<Declaration> enclosing_declaration, std::string name
		) {
			return std::make_shared<Procedure_Declaration>(enclosing_declaration, name);
		}
};

class Type_Declaration: public Declaration {
	public:
		Type_Declaration(std::shared_ptr<Declaration> enclosing_declaration, std::string name):
			Declaration(enclosing_declaration, name)
		{ }
		static std::shared_ptr<Type_Declaration> create(
			std::shared_ptr<Declaration> enclosing_declaration, std::string name
		) {
			return std::make_shared<Type_Declaration>(enclosing_declaration, name);
		}
};

class Variable_Declaration: public Declaration {
		std::shared_ptr<Type_Declaration> type_;
	public:
		Variable_Declaration(
			std::shared_ptr<Declaration> enclosing_declaration, std::string name,
			std::shared_ptr<Type_Declaration> type
		):
			Declaration(enclosing_declaration, name), type_ { type }
		{ }
		static std::shared_ptr<Variable_Declaration> create(
			std::shared_ptr<Declaration> enclosing_declaration, std::string name,
			std::shared_ptr<Type_Declaration> type
		) {
			return std::make_shared<Variable_Declaration>(enclosing_declaration, name, type);
		}
		std::shared_ptr<Type_Declaration> type() { return type_; }
};

