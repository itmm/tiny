#include "code-gen.h"
#include "parser.h"
#include "sema.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

static llvm::cl::opt<std::string> input (
	llvm::cl::Positional, llvm::cl::desc("<input expression>"),
	llvm::cl::init("")
);

int main(int argc, const char **argv) {
	llvm::InitLLVM x(argc, argv);
	llvm::cl::ParseCommandLineOptions(
		argc, argv, "calc - the expression compiler\n"
	);
	Lexer lexer(input);
	Parser parser(lexer);
	AST *tree = parser.parse();
	if (! tree || parser.has_error()) {
		llvm::errs() << "syntax errors occured\n";
		return 1;
	}
	Sema semantic;
	if (semantic.semantic(tree)) {
		llvm::errs() << "semantic errors occured\n";
		return 1;
	}
	Code_Gen code_gen;
	code_gen.compile(tree);
	return 0;
}
