#include "lexer.h"

#include "err.h"

#include <map>

using namespace std::literals::string_literals;

class Unknown_Char_Err: public Error {
	public:
		Unknown_Char_Err(char ch):
			Error { "unknown input character '"s + ch + "'\n"s }
		{ }
};

static std::map<std::string, Token_Kind> keywords {
	{ "ARRAY", Token_Kind::kw_ARRAY },
	{ "BEGIN", Token_Kind::kw_BEGIN },
	{ "CONST", Token_Kind::kw_CONST },
	{ "DO", Token_Kind::kw_DO },
	{ "IF", Token_Kind::kw_IF },
	{ "IMPORT", Token_Kind::kw_IMPORT },
	{ "ELSE", Token_Kind::kw_ELSE },
	{ "ELSIF", Token_Kind::kw_ELSIF },
	{ "END", Token_Kind::kw_END },
	{ "MOD", Token_Kind::kw_MOD },
	{ "MODULE", Token_Kind::kw_MODULE },
	{ "OF", Token_Kind::kw_OF },
	{ "PROCEDURE", Token_Kind::kw_PROCEDURE },
	{ "RETURN", Token_Kind::kw_RETURN },
	{ "THEN", Token_Kind::kw_THEN },
	{ "TYPE", Token_Kind::kw_TYPE },
	{ "VAR", Token_Kind::kw_VAR },
	{ "WHILE", Token_Kind::kw_WHILE },
	{ "WITH", Token_Kind::kw_WITH }
};

namespace Char_Info {
	inline bool is_whitespace(int c) {
		return c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
			c == '\r' || c == '\n';
	}
	inline bool is_digit(int c) {
		return c >= '0' && c <= '9';
	}
	inline bool is_letter(int c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
}

void Lexer::next(Token &tok) {
	while (Char_Info::is_whitespace(ch_)) {
		if (ch_ == '\n') { ++line_; }
		ch_ = in_.get();
	}
	if (ch_ == EOF) { tok.kind_ = Token_Kind::eoi; return; }
	if (Char_Info::is_letter(ch_)) {
		std::string name;
		while (Char_Info::is_letter(ch_)) {
			name += ch_; ch_ = in_.get();
		}
		auto got { keywords.find(name) };
		Token_Kind k = got != keywords.end() ? got->second : Token_Kind::identifier;
		set_token(tok, name, k);
	} else if (Char_Info::is_digit(ch_)) {
		std::string name;
		while (Char_Info::is_digit(ch_)) {
			name += ch_; ch_ = in_.get();
		}
		set_token(tok, name, Token_Kind::integer_literal);
	} else switch (ch_) {
		#define CASE(ch, tk) case ch: set_token(tok, ch, tk); ch_ = in_.get(); break
		CASE('+', Token_Kind::plus);
		CASE('-', Token_Kind::minus);
		CASE('*', Token_Kind::star);
		CASE('/', Token_Kind::slash);
		CASE('(', Token_Kind::l_paren);
		CASE(')', Token_Kind::r_paren);
		CASE(',', Token_Kind::comma);
		CASE(';', Token_Kind::semicolon);
		CASE('.', Token_Kind::period);
		CASE('#', Token_Kind::not_equal);
		#undef CASE
		case ':':
			ch_ = in_.get();
			if (ch_ == '=') {
				set_token(tok, ":=", Token_Kind::assign);
				ch_ = in_.get();
			} else {
				set_token(tok, ":", Token_Kind::colon);
			}
			break;
		case '<':
			ch_ = in_.get();
			if (ch_ == '=') {
				set_token(tok, "<=", Token_Kind::less_equal);
				ch_ = in_.get();
			} else {
				set_token(tok, "<", Token_Kind::less);
			}
			break;
		default: throw Unknown_Char_Err { static_cast<char>(ch_) };


	}
}

int Lexer::line_ { 0 };

void Lexer::set_token(Token &tok, std::string raw, Token_Kind kind) {
	tok.kind_ = kind;
	tok.raw_ = raw;
}

void Lexer::set_token(Token &tok, char raw, Token_Kind kind) {
	set_token(tok, std::string { } + raw, kind);
}

