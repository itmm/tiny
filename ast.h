#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

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
		llvm::StringRef value_;
	public:
		Factor(Kind kind, llvm::StringRef value): kind_ { kind }, value_ { value } { }
		Kind kind() const { return kind_; }
		llvm::StringRef value() const { return value_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
			
};

class Binary_Op: public Expr {
	public:
		enum Operator { plus, minus, mul, div };
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
		using Var_Vector = llvm::SmallVector<llvm::StringRef, 8>;
		Var_Vector vars_;
		Expr *e_;
	public:
		With_Decl(llvm::SmallVector<llvm::StringRef, 8> vars, Expr *e):
			vars_ { vars }, e_ { e }
		{ }
		Var_Vector::const_iterator begin() { return vars_.begin(); }
		Var_Vector::const_iterator end() { return vars_.end(); }
		Expr *expr() { return e_; }
		void accept(AST_Visitor &v) override { v.visit(*this); }
};
