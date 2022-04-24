#pragma once

#include <cassert>
#include <iostream>
#include <string>

class Lexer;

enum class Token_Kind {
	eoi, identifier, comma, colon, semicolon,
	plus, minus, star, slash, l_paren, r_paren,
	integer_literal, string_literal, period,
	less, less_equal,
	kw_BEGIN, kw_CONST, kw_ELSE, kw_ELSIF, kw_END, kw_IF, kw_IMPORT,
	kw_MODULE, kw_PROCEDURE, kw_THEN, kw_TYPE, kw_VAR, kw_WITH,
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
	public:
		Lexer(std::istream &in): in_ { in }, ch_ { in_.get() } { }
		void next(Token &tok);
	private:
		void set_token(Token &tok, std::string raw, Token_Kind kind);
		void set_token(Token &tok, char raw, Token_Kind kind);
};
