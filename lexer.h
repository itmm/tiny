#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

class Lexer;

class Token {
		friend class Lexer;
	public:
		enum Kind : short {
			eoi, unknown, ident, number, comma, colon,
			plus, minus, star, slash, l_paren, r_paren,
			KW_with
		};
	private:
		Kind kind_;
		llvm::StringRef text_;
	
	public:
		Kind kind() const { return kind_; }
		llvm::StringRef text() const { return text_; }

		bool is(Kind k) const { return kind_ == k; }
		bool isOneOf(Kind k1, Kind k2) const { return is(k1) || is(k2); }
		template<typename... Ts>
			bool isOneOf(Kind k1, Kind k2, Ts... ks) const {
				return is(k1) || isOneOf(k2, ks...);
			}
};

class Lexer {
		const char *start_;
		const char *ptr_;
	public:
		Lexer(const llvm::StringRef &buffer):
			start_ { buffer.begin() }, ptr_ { start_ }
		{ }
		void next(Token &tok);
	private:
		void set_token(Token &tok, const char *end, Token::Kind kind);
};
