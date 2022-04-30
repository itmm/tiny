#include "ll_type.h"

#include "err.h"

#include "llvm/IR/DerivedTypes.h"

llvm::Type *get_ll_type(Type_Declaration::Ptr ty, llvm::LLVMContext &ctx) {
	if (! ty) {
		return llvm::Type::getVoidTy(ctx);
	} else if (ty == integer_type) {
		return llvm::Type::getInt32Ty(ctx);
	} else if (ty == real_type) {
		return llvm::Type::getDoubleTy(ctx);
	} else if (ty == boolean_type) {
		return llvm::Type::getInt1Ty(ctx);
	}
	throw Error { "no low level type for '" + ty->name() + "'" };
}
