#pragma once

#include <iostream>
#include <string>

class Gen {
		int next_id_ { 0 };
		int next_while_id_ { 0 };
		int next_if_id_ { 0 };
	public:
		int next_id() { return next_id_++; }
		int next_while_id() { return next_while_id_++; }
		int next_if_id() { return next_if_id_++; }
		void reset() { next_id_ = next_while_id_ = next_if_id_ = 0; }
		void append_raw(std::string str) { std::cout << str << "\n"; }
		void append(std::string str) { append_raw("\t" + str); }
};
