#include "lexer.h"

#include "llvm/ADT/StringMap.h"

static llvm::StringMap<Token_Kind> keywords {
	{ "BEGIN", Token_Kind::kw_BEGIN },
	{ "CONST", Token_Kind::kw_CONST },
	{ "IF", Token_Kind::kw_IF },
	{ "ELSE", Token_Kind::kw_ELSE },
	{ "END", Token_Kind::kw_END },
	{ "THEN", Token_Kind::kw_THEN },
	{ "WITH", Token_Kind::kw_WITH }
};

namespace Char_Info {
	LLVM_READNONE inline bool is_whitespace(char c) {
		return c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
			c == '\r' || c == '\n';
	}
	LLVM_READNONE inline bool is_digit(char c) {
		return c >= '0' && c <= '9';
	}
	LLVM_READNONE inline bool is_letter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
}

void Lexer::next(Token &tok) {
	while (*ptr_ && Char_Info::is_whitespace(*ptr_)) { ++ptr_; }
	if (! *ptr_) { tok.kind_ = Token_Kind::eoi; return; }
	if (Char_Info::is_letter(*ptr_)) {
		const char *end { ptr_ + 1 };
		while (Char_Info::is_letter(*end)) { ++end; }
		llvm::StringRef name(ptr_, end - ptr_);
		auto got { keywords.find(name) };
		Token_Kind k = got != keywords.end() ? got->second : Token_Kind::identifier;
		set_token(tok, end, k);
	} else if (Char_Info::is_digit(*ptr_)) {
		const char *end { ptr_ + 1 };
		while (Char_Info::is_digit(*end)) { ++end; }
		set_token(tok, end, Token_Kind::integer_literal);
	} else switch (*ptr_) {
		#define CASE(ch, tk) case ch: set_token(tok, ptr_ + 1, tk); break
		CASE('+', Token_Kind::plus);
		CASE('-', Token_Kind::minus);
		CASE('*', Token_Kind::star);
		CASE('/', Token_Kind::slash);
		CASE('(', Token_Kind::l_paren);
		CASE(')', Token_Kind::r_paren);
		CASE(':', Token_Kind::colon);
		CASE(',', Token_Kind::comma);
		CASE(';', Token_Kind::semicolon);
		#undef CASE
		case '<':
			if (ptr_[1] == '=') {
				set_token(tok, ptr_ + 2, Token_Kind::less_equal);
			} else {
				set_token(tok, ptr_ + 1, Token_Kind::less);
			}
			break;
		default: set_token(tok, ptr_ + 1, Token_Kind::unknown);


	}
}

void Lexer::set_token(Token &tok, const char *end, Token_Kind kind) {
	tok.kind_ = kind;
	tok.ptr_ = ptr_;
	tok.length_ = end - ptr_;
	ptr_ = end;
}
