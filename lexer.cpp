#include "lexer.h"

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
	if (! *ptr_) { tok.kind_ = Token::eoi; return; }
	if (Char_Info::is_letter(*ptr_)) {
		const char *end { ptr_ + 1 };
		while (Char_Info::is_letter(*end)) { ++end; }
		llvm::StringRef name(ptr_, end - ptr_);
		Token::Kind k = name == "with" ? Token::KW_with : Token::ident;
		set_token(tok, end, k);
	} else if (Char_Info::is_digit(*ptr_)) {
		const char *end { ptr_ + 1 };
		while (Char_Info::is_digit(*end)) { ++end; }
		set_token(tok, end, Token::number);
	} else switch (*ptr_) {
		#define CASE(ch, tk) case ch: set_token(tok, ptr_ + 1, tk); break
		CASE('+', Token::plus);
		CASE('-', Token::minus);
		CASE('*', Token::star);
		CASE('/', Token::slash);
		CASE('(', Token::l_paren);
		CASE(')', Token::r_paren);
		CASE(':', Token::colon);
		CASE(',', Token::comma);
		#undef CASE
		default: set_token(tok, ptr_ + 1, Token::unknown);


	}
}

void Lexer::set_token(Token &tok, const char *end, Token::Kind kind) {
	tok.kind_ = kind;
	tok.text_ = llvm::StringRef(ptr_, end - ptr_);
	ptr_ = end;
}
