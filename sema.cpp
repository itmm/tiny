#include "sema.h"

#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {

	class Decl_Check: public AST_Visitor {
			llvm::StringSet<> scope_;
			bool has_error_ { false };
			enum Error_Type { twice, undef };
			void error(Error_Type type, llvm::StringRef v) {
				llvm::errs() << "Variable " << v << " "
					<< (type == twice ? "already" : "not")
					<< " declared\n";
				has_error_ = true;
			}
		public:
			bool has_error() { return has_error_; }

			void visit(Factor &node) override {
				if (node.kind() == Factor::ident) {
					if (scope_.find(node.value()) == scope_.end()) {
						error(undef, node.value());
					}
				}
			}

			void visit(Binary_Op &node) override {
				if (node.left()) {
					node.left()->accept(*this);
				} else has_error_ = true;
				if (node.right()) {
					node.right()->accept(*this);
				} else has_error_ = true;
			}

			void visit(With_Decl &node) override {
				for (auto i { node.begin() }, e { node.end() }; i != e; ++i) {
					if (! scope_.insert(*i).second) {
						error(twice, *i);
					}
				}
				if (node.expr()) {
					node.expr()->accept(*this);
				} else has_error_ = true;
			}
	};
}

bool Sema::semantic(AST *tree) {
	if (! tree) { return false; }
	Decl_Check ck;
	tree->accept(ck);
	return ck.has_error();
}

void Sema::act_on_variable_declaration(Decl_List &decls, Ident_List &ids, Decl *d) {
}

