#include "code-gen.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

namespace {
	class To_IR_Visitor: public AST_Visitor {
			llvm::Module *mod_;
			llvm::IRBuilder<> builder_;
			llvm::Type *void_ty_;
			llvm::Type *int32_ty_;
			llvm::Type *int8ptr_ty_;
			llvm::Type *int8ptrptr_ty_;
			llvm::Constant *int32_zero_;
			llvm::Value *value_;
			llvm::StringMap<llvm::Value *> name_map_;
		public:
			To_IR_Visitor(llvm::Module *m):
				mod_ { m }, builder_ { m->getContext() }
			{
				void_ty_ = llvm::Type::getVoidTy(m->getContext());
				int32_ty_ = llvm::Type::getInt32Ty(m->getContext());
				int8ptr_ty_ = llvm::Type::getInt8PtrTy(m->getContext());
				int8ptrptr_ty_ = int8ptr_ty_->getPointerTo();
				int32_zero_ = llvm::ConstantInt::get(int32_ty_, 0, true);
			}

			void run(AST *tree) {
				llvm::FunctionType *main_fty = llvm::FunctionType::get(int32_ty_, { int32_ty_, int8ptrptr_ty_ }, false);
				llvm::Function *main_f = llvm::Function::Create(main_fty, llvm::GlobalValue::ExternalLinkage, "main", mod_);
				llvm::BasicBlock *bb = llvm::BasicBlock::Create(mod_->getContext(), "entry", main_f);
				builder_.SetInsertPoint(bb);
				tree->accept(*this);
				llvm::FunctionType *calc_write_fty = llvm::FunctionType::get(void_ty_, { int32_ty_ }, false);
				llvm::Function *calc_write_f = llvm::Function::Create(calc_write_fty, llvm::GlobalValue::ExternalLinkage, "calc_write", mod_);
				builder_.CreateCall(calc_write_fty, calc_write_f, { value_ });
				builder_.CreateRet(int32_zero_);
			}

			void visit(With_Decl &node) override {
				llvm::FunctionType *read_fty = llvm::FunctionType::get(int32_ty_, { int8ptr_ty_ }, false);
				llvm::Function *read_f = llvm::Function::Create(read_fty, llvm::GlobalValue::ExternalLinkage, "calc_read", mod_);
				for (auto i { node.begin() }, e { node.end() }; i != e; ++i) {
					llvm::StringRef var = *i;
					llvm::Constant *text = llvm::ConstantDataArray::getString(mod_->getContext(), var);
					llvm::GlobalVariable *str = new llvm::GlobalVariable(
						*mod_, text->getType(), /*isConstant=*/true,
						llvm::GlobalValue::PrivateLinkage, text,
						llvm::Twine(var).concat(".str")
					);
					llvm::Value *ptr = builder_.CreateInBoundsGEP(
						str, { int32_zero_, int32_zero_ }, "ptr"
					);
					llvm::CallInst *call = builder_.CreateCall(read_fty, read_f, { ptr });
					name_map_[var] = call;
				}
				node.expr()->accept(*this);
			}

			void visit(Factor &node) override {
				if (node.kind() == Factor::ident) {
					value_ = name_map_[node.value()];
				} else {
					int int_val;
					node.value().getAsInteger(10, int_val);
					value_ = llvm::ConstantInt::get(int32_ty_, int_val, true);
				}
			}

			void visit(Binary_Op &node) override {
				node.left()->accept(*this);
				llvm::Value *left = value_;
				node.right()->accept(*this);
				llvm::Value *right = value_;
				switch (node.op()) {
					case Binary_Op::plus:
						value_ = builder_.CreateNSWAdd(left, right);
						break;
					case Binary_Op::minus:
						value_ = builder_.CreateNSWSub(left, right);
						break;
					case Binary_Op::mul:
						value_ = builder_.CreateNSWMul(left, right);
						break;
					case Binary_Op::div:
						value_ = builder_.CreateSDiv(left, right);
						break;
				}
			}
	};
}

void Code_Gen::compile(AST *tree) {
	llvm::LLVMContext ctx;
	llvm::Module *mod = new llvm::Module("calc.expr", ctx);
	To_IR_Visitor visitor(mod);
	visitor.run(tree);
	mod->print(llvm::outs(), nullptr);
}

