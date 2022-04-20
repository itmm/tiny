#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SMLoc.h"

class Lexer;

enum class Token_Kind {
	eoi, unknown, identifier, comma, colon, semicolon,
	plus, minus, star, slash, l_paren, r_paren,
	integer_literal, string_literal,
	less, less_equal,
	kw_BEGIN, kw_CONST, kw_IF, kw_ELSE, kw_END, kw_THEN, kw_WITH
};

class Token {
		friend class Lexer;
		const char *ptr_;
		size_t length_;
		Token_Kind kind_;
	
	public:
		Token_Kind kind() const { return kind_; }
		size_t length() const { return length_; }
		llvm::SMLoc location() const { return llvm::SMLoc::getFromPointer(ptr_); }

		bool is(Token_Kind k) const { return kind_ == k; }
		bool is_one_of(Token_Kind k1, Token_Kind k2) const { return is(k1) || is(k2); }
		template<typename... Ts>
			bool is_one_of(Token_Kind k1, Token_Kind k2, Ts... ks) const {
				return is(k1) || is_one_of(k2, ks...);
			}

		llvm::StringRef raw() const { return llvm::StringRef(ptr_, length_); }
		llvm::StringRef identifier() const {
			assert(is(Token_Kind::identifier) && "cannot get identifier from non-identifier");
			return raw();
		}
		llvm::StringRef literal_data() const{
			assert(is_one_of(Token_Kind::integer_literal, Token_Kind::string_literal) && "cannot get literal data from non-literal");
			return raw();
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
		void set_token(Token &tok, const char *end, Token_Kind kind);
};
