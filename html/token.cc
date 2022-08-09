#include "token.h"

#include <cassert>

namespace html
{
	Token::Token(TokenType type) :
		m_type(type)
	{ }

	void Token::new_attribute()
	{
		assert(is_tag());

		auto tag_data = std::get<TagData>(data);
		tag_data.attributes.push_back(std::make_pair("", ""));
		data = tag_data;
	}

	void Token::append_tag_name(char c)
	{
		assert(is_tag());
		auto *tag_data = std::get_if<TagData>(&data);
		tag_data->name += c;
	}

	void Token::append_attribute_name(char c)
	{
		assert(is_tag());
		auto tag_data = std::get<TagData>(data);
		auto [name, value] = tag_data.attributes.back();
		tag_data.attributes.pop_back();
		name += c;
		tag_data.attributes.push_back(std::make_pair(name, value));
		data = tag_data;
	}

	void Token::append_attribute_value(char c)
	{
		assert(is_tag());
		auto tag_data = std::get<TagData>(data);
		auto [name, value] = tag_data.attributes.back();
		tag_data.attributes.pop_back();
		value += c;
		tag_data.attributes.push_back(std::make_pair(name, value));
		data = tag_data;
	}

	void Token::doctype_set_name(char c)
	{
		assert(is_doctype());
		std::get<DoctypeData>(data).name = c;
	}

	void Token::append_doctype_name(char c)
	{
		assert(is_doctype());
		std::get<DoctypeData>(data).name += c;
	}

	void Token::set_force_quirks()
	{
		assert(is_doctype());
		std::get<DoctypeData>(data).force_quirks = true;
	}

	char Token::get_char()
	{
		assert(is_character());
		return std::get<char>(data);
	}

	std::string Token::tag_name()
	{
		assert(is_tag());
		return std::get<TagData>(data).name;
	}

	std::string Token::to_string()
	{
		return std::visit([this](auto &&arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, DoctypeData>)
			{
				std::string res = "Doctype: ";
				res += "name: " + arg.name + " ";
				res += "public id: " + arg.public_identifier + " ";
				res += "force quirks: " + std::to_string(arg.force_quirks);
				return res;
			}

			else if constexpr (std::is_same_v<T, TagData>)
			{
				std::string res = "<";
				if (is_end_tag())
					res += '/';

				res += arg.name;
				if (arg.self_closing)
					res += '/';

				res += '>';
				return res;
			}

			else if constexpr (std::is_same_v<T, char>)
			{
				return std::string(1, arg);
			}

			else if constexpr (std::is_same_v<T, std::string>)
			{
				return "Comment: " + arg; 
			}

			else if constexpr (std::is_same_v<T, std::monostate>)
			{
				return "EOF";
			}

			return "Unknown variant!";
		}, data);
	}
}
