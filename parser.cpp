#include "parser.h"

#include "ll_type.h"
#include "scope.h"

#include "llvm/IR/Constants.h"

void Parser::parse() {
	parse_module();
	expect(Token_Kind::eoi);
}

Expression::Ptr Parser::parse_plus_minus(Expression::Ptr left) {
	if (tok_.is_one_of(Token_Kind::plus, Token_Kind::minus)) {
		Binary_Op::Operator op {
			tok_.is(Token_Kind::plus) ? Binary_Op::plus : Binary_Op::minus
		};
		advance();
		auto right { parse_term() };
		return Binary_Op::create(op, Integer_Literal::create(0), right);
	} else {
		return nullptr;
	}
}

Expression::Ptr Parser::parse_simple_expression() {
	Expression::Ptr left { parse_plus_minus(Integer_Literal::create(0)) };
	if (! left) { left = parse_term(); }
	while (auto got { parse_plus_minus(left) }) {
		left = got;
	}
	return left;
}

Expression::Ptr Parser::parse_expression() {
	auto left { parse_simple_expression() };
	for (;;) {
		auto op { Binary_Op::none };
		switch (tok_.kind()) {
			case Token_Kind::equal: op = Binary_Op::equal; break;
			case Token_Kind::not_equal: op = Binary_Op::not_equal; break;
			case Token_Kind::less: op = Binary_Op::less; break;
			case Token_Kind::less_equal: op = Binary_Op::less_equal; break;
			case Token_Kind::greater: op = Binary_Op::greater; break;
			case Token_Kind::greater_equal: op = Binary_Op::greater_equal; break;
			default: break;
		}
		if (op == Binary_Op::none) { break; }
		advance();
		auto right { parse_simple_expression() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

Expression::Ptr Parser::parse_term() {
	auto left { parse_factor() };
	for (;;) {
		auto op { Binary_Op::none };
		switch (tok_.kind()) {
			case Token_Kind::star: op = Binary_Op::mul; break;
			case Token_Kind::kw_DIV: op = Binary_Op::div; break;
			case Token_Kind::kw_MOD: op = Binary_Op::mod; break;
			default: break;
		}
		if (op == Binary_Op::none) { break; }
		advance();
		auto right { parse_factor() };
		left = Binary_Op::create(op, left, right);
	}
	return left;
}

Expression::Ptr Parser::parse_factor() {
	std::shared_ptr<Expression> res;
	switch(tok_.kind()) {
		case Token_Kind::integer_literal:
			res = Integer_Literal::create(std::stoi(tok_.literal_data()));
			advance(); break;
		case Token_Kind::identifier: {
			auto got { parse_qual_ident() };
			if (auto var { std::dynamic_pointer_cast<Variable_Declaration>(got) }) {
				res = var->variable();
			} else if (auto cnst { std::dynamic_pointer_cast<Const_Declaration>(got) }) {
				res = cnst->value();
			} else { throw Error { got->name() + " not found" }; }
			break;
		}
		case Token_Kind::kw_FALSE:
			res = Bool_Literal::create(false);
			advance(); break;
		case Token_Kind::kw_TRUE:
			res = Bool_Literal::create(true);
			advance(); break;
		case Token_Kind::sym_not:
			advance();
			res = Unary_Op::create(Unary_Op::op_not, parse_expression());
			break;
		case Token_Kind::l_paren:
			advance();
			res = parse_expression();
			consume(Token_Kind::r_paren);
		default:
			throw Error { "no factor: '" + tok_.raw() + "'" };
	}
	return res;
}

Declaration::Ptr Parser::parse_designator() {
	return parse_qual_ident();
	// TODO: selectors
}

void Parser::parse_statement() {
	if (tok_.is(Token_Kind::kw_IF)) {
		advance();
		parse_expression();
		consume(Token_Kind::kw_THEN);
		parse_statement_sequence();
		while (tok_.is(Token_Kind::kw_ELSIF)) {
			advance();
			parse_expression();
			consume(Token_Kind::kw_THEN);
			parse_statement_sequence();
		}
		if (tok_.is(Token_Kind::kw_ELSE)) {
			advance();
			parse_statement_sequence();
		}
		consume(Token_Kind::kw_END);
		return;
	}
	// TODO: case statement
	if (tok_.is(Token_Kind::kw_WHILE)) {
		auto cond_bb { llvm::BasicBlock::Create(mod_.getContext(), "while.cond", fn_) };
		auto body_bb { llvm::BasicBlock::Create(mod_.getContext(), "while.body", fn_) };
		auto after_bb { llvm::BasicBlock::Create(mod_.getContext(), "while.after", fn_) };
		builder_.CreateBr(cond_bb);
		builder_.SetInsertPoint(cond_bb);
		advance();
		auto cond { expr_to_value(parse_expression()) };
		builder_.CreateCondBr(cond, body_bb, after_bb);
		consume(Token_Kind::kw_DO);
		builder_.SetInsertPoint(body_bb);
		parse_statement_sequence();
		builder_.CreateBr(cond_bb);
		builder_.SetInsertPoint(after_bb);
		/*
		while (tok_.is(Token_Kind::kw_ELSIF)) {
			advance();
			parse_expression();
			consume(Token_Kind::kw_DO);
			parse_statement_sequence();
		}
		*/
		consume(Token_Kind::kw_END);
		return;
	}

	if (tok_.is(Token_Kind::kw_REPEAT)) {
		advance();
		parse_statement_sequence();
		consume(Token_Kind::kw_UNTIL);
		parse_expression();
		return;
	}
	// TODO: for statement

	if (! tok_.is(Token_Kind::identifier)) { return; }

	auto id { parse_designator() };
	if (tok_.is(Token_Kind::assign)) {
		advance();
		auto v { std::dynamic_pointer_cast<Variable_Declaration>(id) };
		if (! v) { throw Error { v->name() + " is no variable for assignment" }; }
		auto e { parse_expression() };
		auto l { expr_to_value(e) };
		builder_.CreateStore(l, v->variable()->llvm_value());
	} else if (tok_.is(Token_Kind::l_paren)) {
		advance();
		if (! tok_.is(Token_Kind::r_paren)) {
			parse_expression();
			while (tok_.is(Token_Kind::comma)) {
				advance();
				parse_expression();
			}
		}
		consume(Token_Kind::r_paren);
	}
}

void Parser::parse_statement_sequence() {
	parse_statement();
	while (tok_.is(Token_Kind::semicolon)) {
		advance();
		parse_statement();
	}
}

void Parser::parse_if() {
	consume(Token_Kind::kw_IF);
	parse_expression();
	consume(Token_Kind::kw_THEN);
	parse_statement_sequence();
	if (tok_.is(Token_Kind::kw_ELSE)) {
		advance();
		parse_statement_sequence();
	}
	consume(Token_Kind::kw_END);
};

std::vector<std::string> Parser::parse_ident_list() {
	std::vector<std::string> ids;

	expect(Token_Kind::identifier);
	ids.push_back(tok_.identifier());
	advance();
	while (tok_.is(Token_Kind::comma)) {
		advance();
		expect(Token_Kind::identifier);
		ids.push_back(tok_.identifier());
		advance();
	}
	return ids;
}

Declaration::Ptr Parser::parse_qual_ident() {
	expect(Token_Kind::identifier);
	auto got { current_scope->lookup(tok_.identifier()) };
	if (! got) {
		throw Error {
			"unknown identifier '" + tok_.identifier() + "'"
		};
	}
	advance();
	return got;
}

std::vector<Variable_Declaration::Ptr> Parser::parse_parameter_declaration(bool is_var) {
	auto ids { parse_ident_list() };
	consume(Token_Kind::colon);
	auto d { parse_qual_ident() };
	auto t { std::dynamic_pointer_cast<Type_Declaration>(d) };
	if (! t) { throw Error { d->name() + " is no type" }; }
	std::vector<Variable_Declaration::Ptr> result;
	for (auto &n : ids) {
		auto dcl = Variable_Declaration::create(Variable::create(n, t, nullptr, false), is_var);
		current_scope->insert(dcl);
		result.push_back(dcl);
	}
	return result;
}

std::vector<Variable_Declaration::Ptr> Parser::parse_variable_declaration() {
	auto ids { parse_ident_list() };
	consume(Token_Kind::colon);
	auto d { parse_qual_ident() };
	auto t { std::dynamic_pointer_cast<Type_Declaration>(d) };
	if (! t) { throw Error { d->name() + " is no type" }; }
	std::vector<Variable_Declaration::Ptr> result;
	for (auto &n : ids) {
		auto v { builder_.CreateAlloca(get_ll_type(t, mod_.getContext())) };
		auto dcl = Variable_Declaration::create(Variable::create(n, t, v, true), false);
		current_scope->insert(dcl);
		result.push_back(dcl);
	}
	return result;
}

Declaration::Ptr Parser::parse_formal_type() {
	if (tok_.is(Token_Kind::kw_ARRAY)) {
		advance();
		consume(Token_Kind::kw_OF);
	}
	return parse_qual_ident();
}

std::vector<Variable_Declaration::Ptr> Parser::parse_fp_section(Procedure_Declaration::Ptr decl) {
	bool is_var { tok_.is(Token_Kind::kw_VAR) };
	if (is_var) { advance(); }
	return parse_parameter_declaration(is_var);
}

void Parser::parse_formal_parameters(Procedure_Declaration::Ptr decl) {
	consume(Token_Kind::l_paren);
	if (! tok_.is_one_of(Token_Kind::r_paren, Token_Kind::eoi)) {
		auto args { parse_fp_section(decl) };
		for (auto arg : args) {
			decl->add_argument(arg);
		}
		while (tok_.is(Token_Kind::comma)) {
			advance();
			auto args { parse_fp_section(decl) };
			for (auto arg : args) {
				decl->add_argument(arg);
			}
		}
	}
	consume(Token_Kind::r_paren);
	if (tok_.is(Token_Kind::colon)) {
		advance();
		auto got { parse_qual_ident() };
		auto ty { std::dynamic_pointer_cast<Type_Declaration>(got) };
		if (! ty) { Error { got->name() + " is not a type" }; }
		decl->set_returns(ty);
	}

}

std::string Parser::parse_procedure_heading() {
	consume(Token_Kind::kw_PROCEDURE);
	expect(Token_Kind::identifier);
	auto name { tok_.identifier() };
	advance();
	return name;
}

void Parser::parse_procedure_body(Procedure_Declaration::Ptr decl) {
	parse_declaration_sequence(decl);
	if (tok_.is(Token_Kind::kw_BEGIN)) {
		advance();
		parse_statement_sequence();
	}
	if (tok_.is(Token_Kind::kw_RETURN)) {
		advance();
		auto ret { expr_to_value(parse_expression()) };
		builder_.CreateRet(ret);
	} else {
		builder_.CreateRetVoid();
	}
	consume(Token_Kind::kw_END);
}

Procedure_Declaration::Ptr Parser::parse_procedure_declaration(Scoping_Declaration::Ptr parent) {
	auto name { parse_procedure_heading() };
	auto decl { Procedure_Declaration::create(name, parent) };
	Pushed_Scope pushed { decl };
	if (tok_.is(Token_Kind::l_paren)) {
		parse_formal_parameters(decl);
	}
	consume(Token_Kind::semicolon);

	auto ll_result { get_ll_type(decl->returns(), mod_.getContext()) };
	std::vector<llvm::Type *> ll_args;
	for (auto i { decl->args_begin() }, e { decl->args_end() }; i != e; ++i) {
		ll_args.push_back(get_ll_type((**i).variable()->type(), mod_.getContext()));
	}
	auto fty { llvm::FunctionType::get(ll_result, ll_args, false) };
	fn_ = llvm::Function::Create(fty, llvm::GlobalValue::ExternalLinkage, parent->mangle(decl->name()), mod_);
	int j { 0 };
	for (auto i { decl->args_begin() }, e { decl->args_end() }; i != e; ++i, ++j) {
		(**i).variable()->set_llvm_value(fn_->getArg(j));
	}

	auto entry { llvm::BasicBlock::Create(mod_.getContext(), "entry", fn_) };
	builder_.SetInsertPoint(entry);

	parse_procedure_body(decl);
	expect(Token_Kind::identifier);
	if (name != tok_.identifier()) {
		throw Error {
			"PROCEDURE '" + name + "' ends with name '" +
			tok_.identifier() + "'"
		};
	}
	advance();
	return decl;
}

void Parser::parse_declaration_sequence(Scoping_Declaration::Ptr parent) {
	if (tok_.is(Token_Kind::kw_CONST)) {
		advance();
		while (tok_.is(Token_Kind::identifier)) {
			auto name { tok_.identifier() };
			advance();
			consume(Token_Kind::equal);
			auto got { parse_expression() };
			auto lit { std::dynamic_pointer_cast<Literal>(got) };
			if (! lit) { throw Error { "expression is not const" }; }
			if (! current_scope->insert(Const_Declaration::create(name, lit))) {
				throw Error { name + " already defined" };
			}
			consume(Token_Kind::semicolon);
		}
	}
	if (tok_.is(Token_Kind::kw_TYPE)) {
		throw Error { "TYPE not imp]emented" }; // TODO
	}
	if (tok_.is(Token_Kind::kw_VAR)) {
		advance();
		while (! tok_.is_one_of(
			Token_Kind::eoi, Token_Kind::kw_END,
			Token_Kind::kw_BEGIN, Token_Kind::kw_PROCEDURE
		)) {
			auto vars { parse_variable_declaration() };
			consume(Token_Kind::semicolon);
		}
	}
	while (tok_.is(Token_Kind::kw_PROCEDURE)) {
		parse_procedure_declaration(parent);
		consume(Token_Kind::semicolon);
	}
}

Module_Declaration::Ptr Parser::parse_module() {
	consume(Token_Kind::kw_MODULE);
	expect(Token_Kind::identifier);
	auto mod = Module_Declaration::create(tok_.identifier());
	current_scope->insert(mod);
	Pushed_Scope pushed { mod };

	advance();
	consume(Token_Kind::semicolon);
	
	if (tok_.is(Token_Kind::kw_IMPORT)) {
		throw Error { "IMPORT not implemented" }; // TODO
	}

	parse_declaration_sequence(mod);

	auto ll_result { get_ll_type(nullptr, mod_.getContext()) };
	std::vector<llvm::Type *> ll_args;
	auto fty { llvm::FunctionType::get(ll_result, ll_args, false) };
	fn_ = llvm::Function::Create(fty, llvm::GlobalValue::ExternalLinkage, mod->mangle("_init"), mod_);
	auto entry { llvm::BasicBlock::Create(mod_.getContext(), "entry", fn_) };
	builder_.SetInsertPoint(entry);


	if (tok_.is(Token_Kind::kw_BEGIN)) {
		advance();
		parse_statement_sequence();
	}

	builder_.CreateRetVoid();

	consume(Token_Kind::kw_END);
	expect(Token_Kind::identifier);
	if (tok_.identifier() != mod->name()) {
		throw Error {

			"MODULE '" + mod->name() + "' ends in name '" +
			tok_.identifier() + "'" 
		};
	}
	advance();
	consume(Token_Kind::period);

	return mod;
};

llvm::Value *Parser::expr_to_value(Expression::Ptr expr) {
	if (auto v { std::dynamic_pointer_cast<Variable>(expr) }) {
		if (v->with_load()) {
			return builder_.CreateLoad(get_ll_type(v->type(), mod_.getContext()), v->llvm_value());
		} else {
			return v->llvm_value();
		}
	}
	if (auto v { std::dynamic_pointer_cast<Bool_Literal>(expr) }) {
		if (v->value()) {
			return llvm::ConstantInt::getTrue(get_ll_type(boolean_type, mod_.getContext()));
		} else {
			return llvm::ConstantInt::getFalse(get_ll_type(boolean_type, mod_.getContext()));
		}
	}
	if (auto v { std::dynamic_pointer_cast<Integer_Literal>(expr) }) {
		return llvm::ConstantInt::get(get_ll_type(integer_type, mod_.getContext()), v->value());
	}
	if (auto v { std::dynamic_pointer_cast<Real_Literal>(expr) }) {
		return llvm::ConstantFP::get(get_ll_type(real_type, mod_.getContext()), v->value());
	}
	if (auto v { std::dynamic_pointer_cast<Binary_Op>(expr) }) {
		auto left { expr_to_value(v->left()) };
		auto right { expr_to_value(v->right()) };
		switch (v->op()) {
		       	case Binary_Op::plus:
				return builder_.CreateAdd(left, right);
			case Binary_Op::minus:
				return builder_.CreateSub(left, right);
			case Binary_Op::mul:
				return builder_.CreateMul(left, right);
			case Binary_Op::div:
				return builder_.CreateSDiv(left, right);
			case Binary_Op::equal:
				return builder_.CreateICmpEQ(left, right);
			case Binary_Op::not_equal:
				return builder_.CreateICmpNE(left, right);
			case Binary_Op::less:
				return builder_.CreateICmpSLT(left, right);
			case Binary_Op::less_equal:
				return builder_.CreateICmpSLE(left, right);
			case Binary_Op::greater:
				return builder_.CreateICmpSGT(left, right);
			case Binary_Op::greater_equal:
				return builder_.CreateICmpSGE(left, right);
			case Binary_Op::mod:
				return builder_.CreateSRem(left, right);
			case Binary_Op::op_and:
				return builder_.CreateAnd(left, right);
			case Binary_Op::op_or:
				return builder_.CreateOr(left, right);
			default: throw Error { "unknown binary" };
		}
	}
	throw Error { "no value" };
}

