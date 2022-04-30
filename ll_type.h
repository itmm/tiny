#pragma once

#include "ast.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"

llvm::Type *get_ll_type(Type_Declaration::Ptr ty, llvm::LLVMContext &ctx);
