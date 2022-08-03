#pragma once

#include <utility>
#include <string>
#include <variant>
#include <vector>

namespace html
{
enum TokenType
{
	Doctype,
	StartTag,
	EndTag,
	Character,
	Comment,
	Eof,
};

struct DoctypeData
{
	std::string name;
	std::string public_identifier;
	std::string system_identifier;
	bool force_quirks = false;
};

struct TagData
{
	std::string name;
	std::vector<std::pair<std::string, std::string>> attributes;
	bool self_closing = false;
};

class Token
{
public:
	Token(TokenType);
	Token() = default;

	inline bool is_doctype() { return type == Doctype; }
	inline bool is_start_tag() { return type == StartTag; }
	inline bool is_end_tag() { return type == EndTag; }
	inline bool is_character() { return type == Character; }
	inline bool is_comment() { return type == Comment; }
	inline bool is_eof() { return type == Eof; }
	inline bool is_tag() { return is_start_tag() || is_end_tag(); }

	static Token make_doctype()
	{
		Token t(Doctype);
		t.data = DoctypeData {};
		return t;
	}

	static Token make_start_tag()
	{
		Token t(StartTag);
		t.data = TagData{};
		return t;
	}

	static Token make_end_tag()
	{
		Token t(EndTag);
		t.data = TagData{};
		return t;
	}

	static Token make_character(char c)
	{
		Token t(Character);
		t.data = c;
		return t;
	}

	static Token make_comment()
	{
		Token t(Comment);
		t.data = "";
		return t;
	}

	static Token make_eof()
	{
		Token t(Eof);
		t.data = std::monostate{};
		return t;
	}

	void new_attribute();
	void append_tag_name(char c);
	void append_attribute_name(char c);
	void append_attribute_value(char c);
	void doctype_set_name(char c);
	void set_force_quirks();

	std::string to_string();

private:
	TokenType type;
	std::variant<
		DoctypeData,   // Doctype
		TagData,       // StartTag, EndTag
		char,          // Character
		std::string,   // Comment
		std::monostate // EOF
	> data;
};
}
