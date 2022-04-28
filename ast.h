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

template<typename TRAIT> class Concrete_Literal: public Literal {
		typename TRAIT::base_type value_;
		Concrete_Literal(typename TRAIT::base_type value): value_ { value } { }
	public:
		using Ptr = std::shared_ptr<Concrete_Literal<TRAIT>>;
		static auto create(typename TRAIT::base_type value) {
			return Ptr { new Concrete_Literal<TRAIT> { value } };
		}
		Type_Declaration::Ptr type() override {
			return TRAIT::oberon_type;
		}
		auto value() const { return value_; }
};

struct Bool_Trait {
	using base_type = bool;
	static Type_Declaration::Ptr oberon_type;
};

using Bool_Literal = Concrete_Literal<Bool_Trait>;

struct Integer_Trait {
	using base_type = int;
	static Type_Declaration::Ptr oberon_type;
};

using Integer_Literal = Concrete_Literal<Integer_Trait>;

class Binary_Op: public Expression {
	public:
		enum Operator { plus, minus, mul, div, equal, not_equal, less, less_equal, greater, greater_equal, mod };
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

template<typename TRAIT> class Concrete_Variable: public Variable {
		typename TRAIT::base_type value_;
		Concrete_Variable(std::string name): Variable { name } { }
	public:
		using Ptr = std::shared_ptr<Concrete_Variable<TRAIT>>;
		static auto create(std::string name) {
			return Ptr { new Concrete_Variable<TRAIT> { name } };
		}
		std::string name() const { return name_; }
		const auto &value() const { return value_; }
		auto &value() { return value_; }
		Type_Declaration::Ptr type() override {
		       	return TRAIT::oberon_type;
	       	}
};

using Bool_Variable = Concrete_Variable<Bool_Trait>;
using Integer_Variable = Concrete_Variable<Integer_Trait>;

class Variable_Declaration: public Declaration {
		Variable::Ptr variable_;

		Variable_Declaration(Variable::Ptr variable):
			Declaration(variable->name()), variable_ { variable }
		{ }
	public:
		using Ptr = std::shared_ptr<Variable_Declaration>;
		static auto create(Variable::Ptr variable) {
			return Ptr { new Variable_Declaration { variable } };
		}
		auto type() { return variable_->type(); }
		auto variable() { return variable_; }
};

