#include "code-gen.h"
#include "err.h"
#include "parser.h"
#include "sema.h"

#include <fstream>
#include <iostream>

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

int main(int argc, const char **argv) {
	llvm::InitLLVM x(argc, argv);

	try {
		for (auto cur { argv + 1}, end { argv + argc }; cur != end; ++cur) {
			if (**cur == '-') { continue; }
			std::cout << "compiling '" << *cur << "'\n";
			std::ifstream in { *cur };
			if (! in) { throw Error { "cannot open '"s + *cur + "' for reading" }; }
			Lexer lexer { in };
			Sema sema;
			Parser parser { lexer, sema };
			parser.parse();
		}
	} catch (const Error &e) {
		std::cerr << "error: " << e.what() << '\n';
		return 10;
	}
	return 0;
}
