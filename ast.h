#pragma once

#include "value.h"

#include <string>
#include <memory>
#include <vector>

class Scoping_Declaration: public Declaration {
	public:
		using Ptr = std::shared_ptr<Scoping_Declaration>;
	private:
		Scoping_Declaration::Ptr parent_;
	protected:
		Scoping_Declaration(
			std::string name, Scoping_Declaration::Ptr parent
		):
			Declaration { name }, parent_ { parent }
		{ }
	public:
		auto parent() const { return parent_; }
		std::string mangle(std::string name) const;
};

class Module: public Scoping_Declaration {
		Module(std::string name):
			Scoping_Declaration(name, nullptr)
		{ }
	public:
		using Ptr = std::shared_ptr<Module>;
		static auto create(std::string name) {
			return Ptr { new Module { name } };
		}
};

class Variable: public Declaration {
		Reference::Ptr ref_;
		bool is_var_;
		bool with_load_;

		Variable(
			std::string name, Reference::Ptr ref,
			bool is_var, bool with_load
		):
			Declaration { name }, ref_ { ref }, 
			is_var_ { is_var }, with_load_ { with_load }
		{ }
	public:
		using Ptr = std::shared_ptr<Variable>;
		static auto create(
			std::string name, Reference::Ptr ref,
			bool is_var, bool with_load
		) {
			return Ptr { new Variable {
				name, ref, is_var, with_load
			} };
		}
		auto type() { return ref_ ? ref_->type() : nullptr; }
		auto ref() { return ref_; }
		void set_ref(Reference::Ptr ref) { ref_ = ref; }
		auto is_var() { return is_var_; }
		auto with_load() { return with_load_; }
};

class Procedure: public Scoping_Declaration {
		Type::Ptr returns_;
		std::vector<Variable::Ptr> arguments_;

		Procedure(std::string name, Scoping_Declaration::Ptr parent):
			Scoping_Declaration { name, parent }
		{ }
	public:
		using Ptr = std::shared_ptr<Procedure>;
		static auto create(
			std::string name, Scoping_Declaration::Ptr parent
		) {
			return Ptr { new Procedure { name, parent } };
		}
		auto returns() { return returns_; }
		void set_returns(Type::Ptr returns) { returns_ = returns; }
		void add_argument(Variable::Ptr arg) {
			arguments_.push_back(arg);
		}
		auto args_begin() { return arguments_.begin(); }
		auto args_end() { return arguments_.end(); }
};

class Const: public Declaration {
		Literal::Ptr literal_;

		Const(std::string name, Literal::Ptr literal):
			Declaration { name }, literal_ { literal }
		{ }
	public:
		using Ptr = std::shared_ptr<Const>;
		static auto create(std::string name, Literal::Ptr literal) {
			return Ptr { new Const { name, literal } };
		}
		auto value() { return literal_; }
};
