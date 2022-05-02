#pragma once

#include <iostream>
#include <string>

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
		void reset() { next_id_ = next_while_id_ = next_if_id_ = next_or_id_ = next_and_id_ = hidden_ = 0; }
		void append_raw(std::string str) { if (! hidden_) { std::cout << str << "\n"; } }
		void append(std::string str) { append_raw("\t" + str); }
};
