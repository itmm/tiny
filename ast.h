#pragma once

#include <string>
#include <memory>
#include <vector>

class Declaration {
		const std::string name_;

	protected:
		Declaration(std::string name): name_ { name } { }

	public:
		using Ptr = std::shared_ptr<Declaration>;
		virtual ~Declaration() { }

		const std::string &name() const { return name_; }
};

class Type_Declaration: public Declaration {
		Type_Declaration(std::string name): Declaration(name) { }
	public:
		using Ptr = std::shared_ptr<Type_Declaration>;
		static auto create(std::string name) {
			return Ptr { new Type_Declaration { name } };
		}
};

extern Type_Declaration::Ptr boolean_type;
extern Type_Declaration::Ptr integer_type;

class Expression {
	public:
		using Ptr = std::shared_ptr<Expression>;
		virtual ~Expression() { }
		virtual Type_Declaration::Ptr type() = 0;
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
		Type_Declaration::Ptr type() override {
			return boolean_type;
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
		Type_Declaration::Ptr type() override {
			return integer_type;
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
		static Expression::Ptr create(
			Operator op, Expression::Ptr left, Expression::Ptr right
		);
		auto left() const { return left_; }
		auto right() const { return right_; }
		Operator op() const { return op_; }
		Type_Declaration::Ptr type() override;
};

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

class Variable: public Expression {
		std::string name_;
	protected:
		Variable(std::string name): name_ { name } { }
	public:
		using Ptr = std::shared_ptr<Variable>;
		static Variable::Ptr create(
			std::string name, Type_Declaration::Ptr type
		);
		const std::string &name() const { return name_; }
};

class Bool_Variable: public Variable {
		bool value_;
		Bool_Variable(std::string name): Variable { name } { }
	public:
		using Ptr = std::shared_ptr<Bool_Variable>;
		static auto create(std::string name) {
			return Ptr { new Bool_Variable { name } };
		}
		const bool &value() const { return value_; }
		bool &value() { return value_; }
		Type_Declaration::Ptr type() override { return boolean_type; }
};

class Integer_Variable: public Variable {
		int value_;
		Integer_Variable(std::string name): Variable { name } { }
	public:
		using Ptr = std::shared_ptr<Integer_Variable>;
		static auto create(std::string name) {
			return Ptr { new Integer_Variable { name } };
		}
		const int &value() const { return value_; }
		int &value() { return value_; }
		Type_Declaration::Ptr type() override { return integer_type; }
};

class Variable_Declaration: public Declaration {
		Type_Declaration::Ptr type_;
		Variable::Ptr variable_;

		Variable_Declaration(
		       	std::string name, Type_Declaration::Ptr type,
			Variable::Ptr variable
		):
			Declaration(name), type_ { type }, variable_ { variable }
		{ }
	public:
		using Ptr = std::shared_ptr<Variable_Declaration>;
		static auto create(
		       	std::string name, Type_Declaration::Ptr type,
			Variable::Ptr variable
		) {
			return Ptr { new Variable_Declaration {
			       	name, type, variable
		       	} };
		}
		auto type() { return type_; }
		auto variable() { return variable_; }
};

