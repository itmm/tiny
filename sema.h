#pragma once

#include "ast.h"
#include "lexer.h"

class Sema {
	public:
		bool semantic(AST *tree);
		void act_on_variable_declaration(Decl_List &decls, Ident_List &ids, Decl *d);
};
