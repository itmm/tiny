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
		Type_Declaration(std::string name): Declaration { name } { }
	public:
		using Ptr = std::shared_ptr<Type_Declaration>;
		static auto create(std::string name) {
			return Ptr { new Type_Declaration { name } };
		}
};

extern Type_Declaration::Ptr boolean_type;
extern Type_Declaration::Ptr integer_type;
extern Type_Declaration::Ptr real_type;

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

struct Real_Trait {
	using base_type = double;
	static Type_Declaration::Ptr oberon_type;
};

using Real_Literal = Concrete_Literal<Real_Trait>;

class Binary_Op: public Expression {
	public:
		enum Operator {
		       	none, plus, minus, mul, div, equal, not_equal,
			less, less_equal, greater, greater_equal, mod,
			op_and, op_or
		};
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
		auto op() const { return op_; }
		Type_Declaration::Ptr type() override;
};

class Unary_Op: public Expression {
	public:
		enum Operator { none, op_not };
	private:
		Expression::Ptr arg_;
		Operator op_;
		Unary_Op(
			Operator op, Expression::Ptr arg
		): arg_ { arg }, op_ { op } { }
	public:
		using Ptr = std::shared_ptr<Unary_Op>;
		static Expression::Ptr create(
			Operator op, Expression::Ptr arg
		);
		auto arg() const { return arg_; }
		auto op() const { return op_; }
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
using Real_Variable = Concrete_Variable<Real_Trait>;

class Variable_Declaration: public Declaration {
		Variable::Ptr variable_;
		bool is_var_;

		Variable_Declaration(Variable::Ptr variable, bool is_var):
			Declaration { variable->name() },
			variable_ { variable }, is_var_ { is_var }
		{ }
	public:
		using Ptr = std::shared_ptr<Variable_Declaration>;
		static auto create(Variable::Ptr variable, bool is_var) {
			return Ptr { new Variable_Declaration { variable, is_var } };
		}
		auto type() { return variable_->type(); }
		auto variable() { return variable_; }
};

class Procedure_Declaration: public Declaration {
		Type_Declaration::Ptr returns_;
		std::vector<Variable_Declaration::Ptr> arguments_;

		Procedure_Declaration(std::string name): Declaration(name) { }
	public:
		using Ptr = std::shared_ptr<Procedure_Declaration>;
		static auto create(std::string name) {

			return Ptr { new Procedure_Declaration { name } };
		}
		auto returns() { return returns_; }
		void set_returns(Type_Declaration::Ptr returns) {
			returns_ = returns;
		}
		void add_argument(Variable_Declaration::Ptr arg) {
			arguments_.push_back(arg);
		}
		auto args_begin() { return arguments_.begin(); }
		auto args_end() { return arguments_.end(); }
};

class Const_Declaration: public Declaration {
		Literal::Ptr literal_;

		Const_Declaration(std::string name, Literal::Ptr literal):
			Declaration { name }, literal_ { literal }
		{ }
	public:
		using Ptr = std::shared_ptr<Const_Declaration>;
		static auto create(std::string name, Literal::Ptr literal) {
			return Ptr { new Const_Declaration { name, literal } };
		}
		auto value() { return literal_; }
};
