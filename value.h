#pragma once

#include "type.h"

#include <string>
#include <memory>

class Value {
	public:
		using Ptr = std::shared_ptr<Value>;
		virtual ~Value() { }
		virtual Type::Ptr type() = 0;
		virtual std::string name() = 0;
};

class Literal: public Value { };

template<typename TRAIT> class Concrete_Literal: public Literal {
		typename TRAIT::base_type value_;
		Concrete_Literal(typename TRAIT::base_type value):
			value_ { value }
		{ }
	public:
		using Ptr = std::shared_ptr<Concrete_Literal<TRAIT>>;
		static auto create(typename TRAIT::base_type value) {
			return Ptr { new Concrete_Literal<TRAIT> { value } };
		}
		Type::Ptr type() override {
			return TRAIT::oberon_type;
		}
		auto value() const { return value_; }
		std::string name() override { return std::to_string(value_); }
};

struct Bool_Trait {
	using base_type = bool;
	static Type::Ptr oberon_type;
};

using Bool_Literal = Concrete_Literal<Bool_Trait>;

struct Integer_Trait {
	using base_type = int;
	static Type::Ptr oberon_type;
};

using Integer_Literal = Concrete_Literal<Integer_Trait>;

struct Real_Trait {
	using base_type = double;
	static Type::Ptr oberon_type;
};

using Real_Literal = Concrete_Literal<Real_Trait>;

class Reference: public Value {
		int index_;
		Type::Ptr type_;

		Reference(int index, Type::Ptr type):
			index_ { index }, type_ { type }
		{ }
	public:
		using Ptr = std::shared_ptr<Reference>;
		static auto create(int index, Type::Ptr type) {
			return Ptr { new Reference { index, type } };
		}
		auto index() const { return index_; }
		Type::Ptr type() override { return type_; }

		std::string name() override {
			return "%" + std::to_string(index_);
		}
};
