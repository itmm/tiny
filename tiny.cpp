#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

#include "lexer.h"

int main(int argc_, const char **argv_) {
	llvm::InitLLVM(argc_, argv_);
	llvm::outs() << "Hello, I am Tinylang\n";
}
