#pragma once

#include <iostream>
#include <string>

class Gen {
		int next_id_ { 0 };
	public:
		int next_id() { return next_id_++; }
		int cur_id() { return next_id_ - 1; }
		void reset() { next_id_ = 0; }
		void append_raw(std::string str) { std::cout << str << "\n"; }
		void append(std::string str) { append_raw("\t" + str); }
};
