#include "err.h"
#include "parser.h"

#include <fstream>
#include <iostream>

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

int main(int argc, const char **argv) {
	llvm::InitLLVM x(argc, argv);

	std::string current_file;

	try {
		for (auto cur { argv + 1}, end { argv + argc }; cur != end; ++cur) {
			if (**cur == '-') { continue; }
			Lexer::reset_current_line();
			current_file = *cur;
			std::cout << "compiling '" << current_file << "'\n";
			std::ifstream in { *cur };
			if (! in) { throw Error { "cannot open for reading" }; }
			Lexer lexer { in };
			Parser parser { lexer };
			parser.parse();
		}
	} catch (const Error &e) {
		std::cerr << current_file << ':';
		auto line { Lexer::current_line() };
		if (line > 0) { std::cerr << line << ':'; }
		std::cerr << ' ' << e.what() << '\n';
		return 10;
	}
	return 0;
}
