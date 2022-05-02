#pragma once

#include <iostream>
#include <string>

#include "value.h"

class Gen {
		int next_id_ { 0 };
		int next_while_id_ { 0 };
		int next_if_id_ { 0 };
		int next_or_id_ { 0 };
		int next_and_id_ { 0 };
		int hidden_ { 0 };
	public:
		int next_id() { return hidden_ ? -1 : next_id_++; }
		int next_while_id() { return hidden_ ? -1 : next_while_id_++; }
		int next_if_id() { return hidden_ ? -1 : next_if_id_++; }
		int next_or_id() { return hidden_ ? -1 : next_or_id_++; }
		int next_and_id() { return hidden_ ? -1 : next_and_id_++; }

		void hide() { ++hidden_; }
		void show() { --hidden_; }
		void reset() {
			next_id_ = next_while_id_ = next_if_id_ = 0;
			next_or_id_ = next_and_id_ = hidden_ = 0;
		}

		void append_raw(std::string str) { 
			if (! hidden_) { std::cout << str << "\n"; }
		}
		void append(std::string str) { append_raw("\t" + str); }

		void def_label(std::string label) { append_raw(label + ":"); }
		void def_label(std::string label, int idx) {
			def_label(label + std::to_string(idx));
		}
		void branch(std::string label) {
			append("br label %" + label);
		}
		void branch(std::string label, int idx) {
			branch(label + std::to_string(idx));
		}
		void conditional(
			Value::Ptr value, std::string true_label,
			std::string false_label
		) {
			append(
				"br " + get_ir_type(value->type()) + " " +
				value->name() + ", label %" + true_label +
				", label %" + false_label
			);
		}
		void conditional(
			Value::Ptr value, std::string true_label, int true_idx,
			std::string false_label, int false_idx
		) {
			conditional(
				value, true_label + std::to_string(true_idx),
				false_label + std::to_string(false_idx)
			);
		}
		void conditional(
			Value::Ptr value, std::string true_label, int true_idx,
			std::string false_label
		) {
			conditional(
				value, true_label + std::to_string(true_idx),
				false_label
			);
		}
		void conditional(
			Value::Ptr value, std::string true_label,
			std::string false_label, int false_idx
		) {
			conditional(
				value, true_label,
				false_label + std::to_string(false_idx)
			);
		}
		void ret() { append("ret void"); }
		void ret(Value::Ptr value) {
			append(
				"ret " + get_ir_type(value->type()) + " " +
				value->name()
			);
		}
		void alloca(Reference::Ptr ref) {
			append(
				ref->name() + " = alloca " +
				get_ir_type(ref->type()) + ", align 4"
			);
		}
};
