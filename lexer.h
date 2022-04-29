#pragma once

#include <cassert>
#include <iostream>
#include <string>

class Lexer;

enum class Token_Kind {
	eoi, identifier, comma, colon, assign, semicolon,
	plus, minus, star, slash, l_paren, r_paren,
	integer_literal, string_literal, period,
	equal, less, less_equal, not_equal, greater, greater_equal, kw_ARRAY,
	kw_BEGIN, kw_CONST, kw_DIV, kw_DO, kw_ELSE, kw_ELSIF, kw_END, kw_FALSE, kw_IF, kw_IMPORT, kw_MOD,
	kw_MODULE, kw_OF, kw_PROCEDURE, kw_REPEAT, kw_RETURN, kw_THEN, kw_TRUE, kw_TYPE, kw_UNTIL, kw_VAR, kw_WHILE, kw_WITH,
};

class Token {
		friend class Lexer;
		Token_Kind kind_;
		std::string raw_;
	
	public:
		Token_Kind kind() const { return kind_; }

		bool is(Token_Kind k) const { return kind_ == k; }
		bool is_one_of(Token_Kind k1) const { return is(k1); }
		template<typename... Ts>
			bool is_one_of(Token_Kind k1, Ts... ks) const {
				return is(k1) || is_one_of(ks...);
			}

		const std::string &raw() const { return raw_; }
		const std::string &identifier() const {
			assert(is(Token_Kind::identifier) && "cannot get identifier from non-identifier");
			return raw();
		}
		const std::string &literal_data() const{
			assert(is_one_of(Token_Kind::integer_literal, Token_Kind::string_literal) && "cannot get literal data from non-literal");
			return raw();
		}
};

class Lexer {
		std::istream &in_;
		int ch_;
		static int line_;
	public:
		Lexer(std::istream &in): in_ { in }, ch_ { in_.get() } { ++line_; }
		void next(Token &tok);

		static int current_line() { return line_; };
		static void reset_current_line() { line_ = 0; };
	private:
		void set_token(Token &tok, std::string raw, Token_Kind kind);
		void set_token(Token &tok, char raw, Token_Kind kind);
		void double_token(Token &tok, Token_Kind with_equals, Token_Kind without_equals);
};
