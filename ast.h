#pragma once

#include <string>
#include <memory>
#include <vector>

class Expression {
	public:
		using Ptr = std::shared_ptr<Expression>;
		virtual ~Expression() { }
};

class Literal: public Expression { };

class Bool_Literal: public Literal {
		bool value_;
		Bool_Literal(bool value): value_ { value } { }
	public:
		using Ptr = std::shared_ptr<Bool_Literal>;
		static auto create(bool value) {
			return Ptr { new Bool_Literal { value } };
		}
};

class Integer_Literal: public Literal {
		int value_;
		Integer_Literal(int value): value_ { value } { }
	public:
		using Ptr = std::shared_ptr<Integer_Literal>;
		static auto create(int value) {
			return Ptr { new Integer_Literal { value } };
		}
};

class Binary_Op: public Expression {
	public:
		enum Operator { plus, minus, mul, div, not_equal, mod };
	private:
		Expression::Ptr left_;
		Expression::Ptr right_;
		Operator op_;
		Binary_Op(
			Operator op, Expression::Ptr left, Expression::Ptr right
		):
			left_ { left }, right_ { right }, op_ { op }
		{ }
	public:
		using Ptr = std::shared_ptr<Binary_Op>;
		static auto create(
			Operator op, Expression::Ptr left, Expression::Ptr right
		) {
			return Ptr { new Binary_Op { op, left, right } };
		}
		auto left() const { return left_; }
		auto right() const { return right_; }
		Operator op() const { return op_; }
};

class Declaration: public Expression {
		const std::string name_;

	protected:
		Declaration(std::string name): name_ { name } { }
	public:
		using Ptr = std::shared_ptr<Declaration>;
		virtual ~Declaration() { }

		const std::string &name() const { return name_; }
};

using Decl_List = std::vector<Declaration::Ptr>;
using Ident_List = std::vector<std::string>;

class Module_Declaration: public Declaration {
		Module_Declaration(std::string name): Declaration(name) { }
	public:
		using Ptr = std::shared_ptr<Module_Declaration>;
		static auto create(std::string name) {
			return Ptr { new Module_Declaration { name } };
		}
};

class Procedure_Declaration: public Declaration {
		Procedure_Declaration(std::string name): Declaration(name) { }
	public:
		using Ptr = std::shared_ptr<Procedure_Declaration>;
		static auto create(std::string name) {

			return Ptr { new Procedure_Declaration { name } };
		}
};

class Type_Declaration: public Declaration {
		Type_Declaration(std::string name): Declaration(name) { }
	public:
		using Ptr = std::shared_ptr<Type_Declaration>;
		static auto create(std::string name) {
			return Ptr { new Type_Declaration { name } };
		}
};

class Variable_Declaration: public Declaration {
		Type_Declaration::Ptr type_;
		Variable_Declaration(
		       	std::string name, Type_Declaration::Ptr type
		):
			Declaration(name), type_ { type }
		{ }
	public:
		using Ptr = std::shared_ptr<Variable_Declaration>;
		static auto create(
		       	std::string name, Type_Declaration::Ptr type
		) {
			return Ptr { new Variable_Declaration { name, type } };
		}
		auto type() { return type_; }
};

