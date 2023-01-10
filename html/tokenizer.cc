// implements the HTML tokenization algorithm from
// https://html.spec.whatwg.org/multipage/parsing.html#tokenization

#include "tokenizer.h"

#include <cassert>
#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-label"

namespace html
{

#define BEGIN_STATE(state) \
	state:                 \
	case State::state:     \
	{                      \
		{
#define END_STATE  \
	assert(false); \
	break;         \
	}              \
	}

#define IGNORE_CHARACTER() ;

#define SWITCH_TO(new_state)      \
	do                            \
	{                             \
		state = State::new_state; \
		goto new_state;           \
	} while (false)

#define RECONSUME_IN(new_state)                         \
	do                                                  \
	{                                                   \
		state = State::new_state;                       \
		next_input_character = current_input_character; \
		current_input_character = input[--pos];         \
		goto new_state;                                 \
	} while (false)

#define RECONSUME_IN_RETURN_STATE()                     \
	do                                                  \
	{                                                   \
		state = return_state;                           \
		next_input_character = current_input_character; \
		current_input_character = input[--pos];         \
		goto loop_start;                                \
	} while (false)

#define ON(codepoint) if (current_input_character == codepoint)
#define ON_NULL       if (current_input_character == 0xcafebabe)
#define ON_EOF        if (eof())

#define ON_WHITESPACE                                                                                            \
	if (current_input_character == '\t' || current_input_character == '\n' || current_input_character == '\f' || \
	    current_input_character == ' ')

#define ON_ASCII_ALPHA           if (isalpha(current_input_character))
#define ON_ASCII_DIGIT           if (isdigit(current_input_character))
#define ON_ASCII_UPPER_ALPHA     if (isupper(current_input_character))
#define ON_ASCII_LOWER_ALPHA     if (isalpha(current_input_character) && isupper(current_input_character))
#define ON_ASCII_UPPER_HEX_DIGIT if (current_input_character >= 'A' && current_input_character <= 'F')
#define ON_ASCII_LOWER_HEX_DIGIT if (current_input_character >= 'a' && current_input_character <= 'f')
#define ON_ASCII_ALPHANUMERIC    if (isalpha(current_input_character) || isdigit(current_input_character))

#define ANYTHING_ELSE            if (1)

#define EMIT_CURRENT_INPUT_CHARACTER()                         \
	do                                                         \
	{                                                          \
		return Token::make_character(current_input_character); \
	} while (false)

#define EMIT_EOF()                \
	do                            \
	{                             \
		return Token::make_eof(); \
	} while (false)

#define EMIT_CHARACTER(codepoint)                \
	do                                           \
	{                                            \
		return Token::make_character(codepoint); \
	} while (false)

#define EMIT_CURRENT_TOKEN()  \
	do                        \
	{                         \
		return current_token; \
	} while (false)

#define SWITCH_TO_AND_EMIT_CHARACTER(new_state, codepoint) \
	do                                                     \
	{                                                      \
		state = State::new_state;                          \
		return Token::make_character(codepoint);           \
	} while (false)

#define SWITCH_TO_AND_EMIT_TOKEN(new_state, token) \
	do                                             \
	{                                              \
		state = State::new_state;                  \
		return token;                              \
	} while (false)

#define RECONSUME_AND_EMIT_CHARACTER(new_state, codepoint) \
	do                                                     \
	{                                                      \
		state = State::new_state;                          \
		next_input_character = current_input_character;    \
		current_input_character = input[--pos];            \
		return Token::make_character(codepoint);           \
	} while (false)

Tokenizer::Tokenizer(std::string const input) :
    input(input)
{
	// consume first input character; prime the engines
	next_input_character = input[0];
}

std::vector<Token> Tokenizer::scan_all()
{
	std::vector<Token> tokens;
	Token token;

	while (!(token = next()).is_eof())
		tokens.push_back(token);

	return tokens;
}

Token Tokenizer::next()
{
loop_start:
	while (1)
	{
		while (!emitted_tokens.empty())
		{
			auto token = emitted_tokens.front();
			emitted_tokens.pop_front();
			return token;
		}

		if (flush_temporary_buffer)
		{
			auto char_token = Token::make_character(temporary_buffer[0]);
			temporary_buffer.erase(0, 1);
			if (temporary_buffer.empty())
				flush_temporary_buffer = false;

			return char_token;
		}

		switch (state)
		{
			// 13.2.5.1 Data state
			BEGIN_STATE(Data)
			{
				consume_next_input_character();
				ON('&')
				{
					return_state = State::Data;
					SWITCH_TO(CharacterReference);
				}

				ON('<')
				{
					SWITCH_TO(TagOpen);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					EMIT_CURRENT_INPUT_CHARACTER();
				}

				ON_EOF
				{
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					EMIT_CURRENT_INPUT_CHARACTER();
				}
			}
			END_STATE

			// 13.2.5.2 RCDATA state
			BEGIN_STATE(RCDATA)
			{
				consume_next_input_character();
				ON('&')
				{
					return_state = State::RCDATA;
					SWITCH_TO(CharacterReference);
				}

				ON('<')
				{
					SWITCH_TO(RCDATALessThanSign);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					EMIT_CHARACTER(U'\ufffd');
				}

				ON_EOF
				{
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					EMIT_CURRENT_INPUT_CHARACTER();
				}
			}
			END_STATE

			// 13.2.5.3 RAWTEXT state
			BEGIN_STATE(RAWTEXT)
			{
				consume_next_input_character();
				ON('<')
				{
					SWITCH_TO(RAWTEXTLessThanSign);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					EMIT_CHARACTER(U'\ufffd');
				}

				ON_EOF
				{
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					EMIT_CURRENT_INPUT_CHARACTER();
				}
			}
			END_STATE

			// 13.2.5.4 Script data state
			BEGIN_STATE(ScriptData)
			{
				consume_next_input_character();
				ON('<')
				{
					SWITCH_TO(ScriptDataLessThanSign);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					EMIT_CHARACTER(U'\ufffd');
				}

				ON_EOF
				{
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					EMIT_CURRENT_INPUT_CHARACTER();
				}
			}
			END_STATE

			// 13.2.5.5 PLAINTEXT state
			BEGIN_STATE(PLAINTEXT)
			{
				consume_next_input_character();
				ON_NULL
				{
					parse_error("unexpected-null-character");
					EMIT_CHARACTER(U'\ufffd');
				}

				ON_EOF
				{
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					EMIT_CURRENT_INPUT_CHARACTER();
				}
			}
			END_STATE

			// 13.2.5.6 Tag open state
			BEGIN_STATE(TagOpen)
			{
				consume_next_input_character();
				ON('!')
				{
					SWITCH_TO(MarkupDeclarationOpen);
				}

				ON('/')
				{
					SWITCH_TO(EndTagOpen);
				}

				ON_ASCII_ALPHA
				{
					current_token = Token::make_start_tag();
					RECONSUME_IN(TagName);
				}

				ON('?')
				{
					parse_error("unexpected-question-mark-instead-of-tag-name");
					current_token = Token::make_comment();
					RECONSUME_IN(BogusComment);
				}

				ON_EOF
				{
					parse_error("eof-before-tag-name");
					// TODO - handle multiple emits
					EMIT_CHARACTER('<');
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					parse_error("invalid-first-character-of-tag-name");
					RECONSUME_AND_EMIT_CHARACTER(Data, '<');
				}
			}
			END_STATE

			// 13.2.5.7 End tag open state
			BEGIN_STATE(EndTagOpen)
			{
				consume_next_input_character();
				ON_ASCII_ALPHA
				{
					current_token = Token::make_end_tag();
					RECONSUME_IN(TagName);
				}

				ON('>')
				{
					parse_error("missing-end-tag-name");
					SWITCH_TO(Data);
				}

				ON_EOF
				{
					parse_error("eof-before-tag-name");
					EMIT_CHARACTER('<');
					EMIT_CHARACTER('/');
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					parse_error("invalid-first-character-of-tag-name");
					current_token = Token::make_comment();
					RECONSUME_IN(BogusComment);
				}
			}
			END_STATE

			// 13.2.5.8 Tag name state
			BEGIN_STATE(TagName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					SWITCH_TO(BeforeAttributeName);
				}

				ON('/')
				{
					SWITCH_TO(SelfClosingStartTag);
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_ASCII_UPPER_ALPHA
				{
					current_token.append_tag_name(current_input_character + 0x20);
					continue;
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_tag_name(U'\ufffd');
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_tag_name(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.9 RCDATA less-than sign state
			BEGIN_STATE(RCDATALessThanSign)
			{
				consume_next_input_character();
				ON('/')
				{
					temporary_buffer = "";
					SWITCH_TO(RCDATAEndTagOpen);
				}

				ANYTHING_ELSE
				{
					RECONSUME_AND_EMIT_CHARACTER(RCDATA, '<');
				}
			}
			END_STATE

			// 13.2.5.10 RCDATA end tag open state
			BEGIN_STATE(RCDATAEndTagOpen)
			{
				consume_next_input_character();
				ON_ASCII_ALPHA
				{
					current_token = Token::make_end_tag();
					RECONSUME_IN(RCDATAEndTagName);
				}

				ANYTHING_ELSE
				{
					// TODO - handle multiple emits
					assert(false);
					EMIT_CHARACTER('<');
					EMIT_CHARACTER('/');
					RECONSUME_IN(RCDATA);
				}
			}
			END_STATE

		// 13.2.5.11 RCDATA end tag name state
		RCDATAEndTagName:
		case State::RCDATAEndTagName:
			break;

		// 13.2.5.12 RAWTEXT less-than sign state
		RAWTEXTLessThanSign:
		case State::RAWTEXTLessThanSign:
			break;

		// 13.2.5.13 RAWTEXT end tag open state
		RAWTEXTEndTagOpen:
		case State::RAWTEXTEndTagOpen:
			break;

		// 13.2.5.14 RAWTEXT end tag name state
		RAWTEXTEndTagName:
		case State::RAWTEXTEndTagName:
			break;

			// 13.2.5.15 Script data less-than sign state
			BEGIN_STATE(ScriptDataLessThanSign)
			{
				consume_next_input_character();
				ON('/')
				{
					temporary_buffer = "";
					SWITCH_TO(ScriptDataEndTagOpen);
				}

				ON('!')
				{
					// TODO - handle multiple emits
					assert(false);
					EMIT_CHARACTER('<');
					EMIT_CHARACTER('!');
					SWITCH_TO(ScriptData);
				}
			}
			END_STATE

			// 13.2.5.16 Script data end tag open state
			BEGIN_STATE(ScriptDataEndTagOpen)
			{
				consume_next_input_character();
				ON_ASCII_ALPHA
				{
					current_token = Token::make_end_tag();
					RECONSUME_IN(ScriptData);
				}

				ANYTHING_ELSE
				{
					// TODO - handle multiple emits
					assert(false);
					EMIT_CHARACTER('<');
					EMIT_CHARACTER('/');
					RECONSUME_IN(ScriptData);
				}
			}
			END_STATE

			// 13.2.5.17 Script data end tag name state
			BEGIN_STATE(ScriptDataEndTagName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					// TODO
				}

				ON('/')
				{
					// TODO
				}

				ON('>')
				{
					// TODO
				}

				ON_ASCII_UPPER_ALPHA
				{
					current_token.append_attribute_name(current_input_character + 0x20);
					temporary_buffer += current_input_character;
					continue;
				}

				ON_ASCII_LOWER_ALPHA
				{
					current_token.append_attribute_name(current_input_character + 0x20);
					temporary_buffer += current_input_character;
					continue;
				}

				ANYTHING_ELSE
				{
					EMIT_CHARACTER('<');
					EMIT_CHARACTER('/');
					// TODO
				}
			}
			END_STATE

			// 13.2.5.18 Script data escape start state
			BEGIN_STATE(ScriptDataEscapeStart)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscapeStartDash, '-');
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(ScriptData);
				}
			}
			END_STATE

			// 13.2.5.19 Script data escape start dash state
			BEGIN_STATE(ScriptDataEscapeStartDash)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscapedDashDash, '-');
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(ScriptData);
				}
			}
			END_STATE

			// 13.2.5.20 Script data escaped state
			BEGIN_STATE(ScriptDataEscaped)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscapedDash, '-');
				}

				ON('<')
				{
					SWITCH_TO(ScriptDataEscapedLessThanSign);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					EMIT_CHARACTER(U'\ufffd');
				}

				ON_EOF
				{
					parse_error("eof-in-script-html-comment-like-text");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					EMIT_CURRENT_INPUT_CHARACTER();
				}
			}
			END_STATE

			// 13.2.5.21 Script data escaped dash state
			BEGIN_STATE(ScriptDataEscapedDash)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscapedDashDash, '-');
				}

				ON('<')
				{
					SWITCH_TO(ScriptDataEscapedLessThanSign);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscaped, U'\ufffd');
				}

				ON_EOF
				{
					parse_error("eof-in-script-html-comment-like-text");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscaped, current_input_character);
				}
			}
			END_STATE

			// 13.2.5.22 Script data escaped dash dash state
			BEGIN_STATE(ScriptDataEscapedDashDash)
			{
				consume_next_input_character();
				ON('-')
				{
					EMIT_CHARACTER('-');
				}

				ON('<')
				{
					SWITCH_TO(ScriptDataEscapedLessThanSign);
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptData, '>');
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscaped, U'\ufffd');
				}

				ON_EOF
				{
					parse_error("eof-in-script-html-comment-like-text");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					SWITCH_TO_AND_EMIT_CHARACTER(ScriptDataEscaped, current_input_character);
				}
			}
			END_STATE

			// 13.2.5.23 Script data escaped less-than sign state
			BEGIN_STATE(ScriptDataEscapedLessThanSign)
			{
				consume_next_input_character();
				ON('/')
				{
					temporary_buffer = "";
					SWITCH_TO(ScriptDataEscapedEndTagOpen);
				}

				ON_ASCII_ALPHA
				{
					temporary_buffer = "";
					RECONSUME_AND_EMIT_CHARACTER(ScriptDataEscaped, '<');
				}

				ANYTHING_ELSE
				{
					RECONSUME_AND_EMIT_CHARACTER(ScriptDataEscaped, '<');
				}
			}
			END_STATE

		// 13.2.5.24 Script data escaped end tag open state
		ScriptDataEscapedEndTagOpen:
		case State::ScriptDataEscapedEndTagOpen:
			break;

		// 13.2.5.25 Script data escaped end tag name state
		ScriptDataEscapedEndTagName:
		case State::ScriptDataEscapedEndTagName:
			break;

		// 13.2.5.26 Script data double escape start state
		ScriptDataDoubleEscapeStart:
		case State::ScriptDataDoubleEscapeStart:
			break;

		// 13.2.5.27 Script data double escaped state
		ScriptDataDoubleEscaped:
		case State::ScriptDataDoubleEscaped:
			break;

		// 13.2.5.28 Script data double escaped dash state
		ScriptDataDoubleEscapedDash:
		case State::ScriptDataDoubleEscapedDash:
			break;

		// 13.2.5.29 Script data double escaped dash dash state
		ScriptDataDoubleEscapedDashDash:
		case State::ScriptDataDoubleEscapedDashDash:
			break;

		// 13.2.5.30 Script data double escaped less-than sign state
		ScriptDataDoubleEscapedLessThanSign:
		case State::ScriptDataDoubleEscapedLessThanSign:
			break;

		// 13.2.5.31 Script data double escape end state
		ScriptDataDoubleEscapeEnd:
		case State::ScriptDataDoubleEscapeEnd:
			break;

			// 13.2.5.32 Before attribute name state
			BEGIN_STATE(BeforeAttributeName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					IGNORE_CHARACTER();
				}

				ON('/')
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON('>')
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON_EOF
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON('=')
				{
					parse_error("unexpected-equals-sign-before-attribute-name");
					current_token.new_attribute();
					current_token.append_attribute_name(current_input_character);
					SWITCH_TO(AttributeName);
				}

				ANYTHING_ELSE
				{
					current_token.new_attribute();
					RECONSUME_IN(AttributeName);
				}
			}
			END_STATE

			// 13.2.5.33 Attribute name state
			BEGIN_STATE(AttributeName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON('/')
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON('>')
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON_EOF
				{
					RECONSUME_IN(AfterAttributeName);
				}

				ON('=')
				{
					SWITCH_TO(BeforeAttributeValue);
				}

				ON_ASCII_UPPER_ALPHA
				{
					current_token.append_attribute_name(current_input_character + 0x20);
					continue;
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_attribute_name(U'\ufffd');
					continue;
				}

				ON('"')
				{
					parse_error("unexpected-character-in-attribute-name");
					current_token.append_attribute_name(current_input_character);
					continue;
				}

				ON('\'')
				{
					parse_error("unexpected-character-in-attribute-name");
					current_token.append_attribute_name(current_input_character);
					continue;
				}

				ON('<')
				{
					parse_error("unexpected-character-in-attribute-name");
					current_token.append_attribute_name(current_input_character);
					continue;
				}

				ANYTHING_ELSE
				{
					current_token.append_attribute_name(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.34 After attribute name state
			BEGIN_STATE(AfterAttributeName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					IGNORE_CHARACTER();
				}

				ON('/')
				{
					SWITCH_TO(SelfClosingStartTag);
				}

				ON('=')
				{
					SWITCH_TO(BeforeAttributeValue);
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.new_attribute();
					RECONSUME_IN(AttributeName);
				}
			}
			END_STATE

			// 13.2.5.35 Before attribute value state
			BEGIN_STATE(BeforeAttributeValue)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					IGNORE_CHARACTER();
				}

				ON('"')
				{
					SWITCH_TO(AttributeValueDoubleQuoted);
				}

				ON('\'')
				{
					SWITCH_TO(AttributeValueSingleQuoted);
				}

				ON('>')
				{
					parse_error("missing-attribute-value");
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(AttributeValueUnquoted);
				}
			}
			END_STATE

			// 13.2.5.36 Attribute value (double-quoted) state
			BEGIN_STATE(AttributeValueDoubleQuoted)
			{
				consume_next_input_character();
				ON('"')
				{
					SWITCH_TO(AfterAttributeValueQuoted);
				}

				ON('&')
				{
					return_state = State::AttributeValueDoubleQuoted;
					SWITCH_TO(CharacterReference);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_attribute_value(U'\ufffd');
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_attribute_value(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.37 Attribute value (single-quoted) state
			BEGIN_STATE(AttributeValueSingleQuoted)
			{
				consume_next_input_character();
				ON('"')
				{
					SWITCH_TO(AfterAttributeValueQuoted);
				}

				ON('&')
				{
					return_state = State::AttributeValueSingleQuoted;
					SWITCH_TO(CharacterReference);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_attribute_value(U'\ufffd');
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_attribute_value(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.38 Attribute value (unquoted) state
			BEGIN_STATE(AttributeValueUnquoted)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					SWITCH_TO(BeforeAttributeName);
				}

				ON('&')
				{
					return_state = State::AttributeValueUnquoted;
					SWITCH_TO(CharacterReference);
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_attribute_value(U'\ufffd');
					continue;
				}

				ON('"')
				{
					parse_error("unexpected-character-in-unquoted-attribute-value");
					current_token.append_attribute_value(current_input_character);
					continue;
				}

				ON('\'')
				{
					parse_error("unexpected-character-in-unquoted-attribute-value");
					current_token.append_attribute_value(current_input_character);
					continue;
				}

				ON('<')
				{
					parse_error("unexpected-character-in-unquoted-attribute-value");
					current_token.append_attribute_value(current_input_character);
					continue;
				}

				ON('=')
				{
					parse_error("unexpected-character-in-unquoted-attribute-value");
					current_token.append_attribute_value(current_input_character);
					continue;
				}

				ON('`')
				{
					parse_error("unexpected-character-in-unquoted-attribute-value");
					current_token.append_attribute_value(current_input_character);
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_attribute_value(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.39 After attribute value (quoted) state
			BEGIN_STATE(AfterAttributeValueQuoted)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					SWITCH_TO(BeforeAttributeName);
				}

				ON('/')
				{
					SWITCH_TO(SelfClosingStartTag);
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					parse_error("missing-whitespace-between-attributes parse error");
					RECONSUME_IN(BeforeAttributeName);
				}
			}
			END_STATE

			// 13.2.5.40 Self-closing start tag state
			BEGIN_STATE(SelfClosingStartTag)
			{
				consume_next_input_character();
				ON('>')
				{
					current_token.set_self_closing();
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-tag");
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					parse_error("unexpected-solidus-in-tag");
					RECONSUME_IN(BeforeAttributeName);
				}
			}
			END_STATE

			// 13.2.5.41 Bogus comment state
			BEGIN_STATE(BogusComment)
			{
				consume_next_input_character();
				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					// TODO - handle multiple emits
					assert(false);
					EMIT_CURRENT_TOKEN();
					EMIT_EOF();
				}

				ON_NULL
				{
					parse_error("unexpected-parse-error");
					current_token.append_comment(U'\ufffd');
					continue;
				}

				ANYTHING_ELSE
				{
					current_token.append_comment(current_input_character);
				}
			}
			END_STATE

			// 13.2.5.42 Markup declaration open state
			BEGIN_STATE(MarkupDeclarationOpen)
			{
				// TODO - this is not what the spec says.
				consume_next_input_character();
				if (consume_if_match("--"))
				{
					current_token = Token::make_comment();
					SWITCH_TO(CommentStart);
				}

				else if (consume_if_match("doctype"))
				{
					SWITCH_TO(DOCTYPE);
				}

				else if (consume_if_match("[CDATA["))
				{
					// TODO
				}

				else
				{
					parse_error("incorrectly-opened-comment");
					current_token = Token::make_comment();
					SWITCH_TO(BogusComment);
				}
			}
			END_STATE

			// 13.2.5.43 Comment start state
			BEGIN_STATE(CommentStart)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO(CommentStartDash);
				}

				ON('>')
				{
					parse_error("abrupt-closing-of-empty-comment");
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.44 Comment start dash state
			BEGIN_STATE(CommentStartDash)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO(CommentEnd);
				}

				ON('>')
				{
					parse_error("abrupt-closing-of-empty-comment");
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-comment");
					// TODO - handle multiple emits
					assert(false);
					EMIT_CURRENT_TOKEN();
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_comment('-');
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.45 Comment state
			BEGIN_STATE(Comment)
			{
				consume_next_input_character();
				ON('<')
				{
					current_token.append_comment('-');
					SWITCH_TO(CommentLessThanSign);
				}

				ON('-')
				{
					SWITCH_TO(CommentEndDash);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_comment(U'\ufffd');
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-comment");
					// TODO - handle multiple emits
					assert(false);
					EMIT_CURRENT_TOKEN();
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_comment(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.46 Comment less-than sign state
			BEGIN_STATE(CommentLessThanSign)
			{
				consume_next_input_character();
				ON('!')
				{
					current_token.append_comment(current_input_character);
					SWITCH_TO(CommentLessThanSignBang);
				}

				ON('<')
				{
					current_token.append_comment(current_input_character);
					continue;
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.47 Comment less-than sign bang state
			BEGIN_STATE(CommentLessThanSignBang)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO(CommentLessThanSignBangDash);
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.48 Comment less-than sign bang dash state
			BEGIN_STATE(CommentLessThanSignBangDash)
			{
				consume_next_input_character();
				ON('-')
				{
					SWITCH_TO(CommentLessThanSignBangDashDash);
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.49 Comment less-than sign bang dash dash state
			BEGIN_STATE(CommentLessThanSignBangDashDash)
			{
				consume_next_input_character();
				ON('>')
				{
					RECONSUME_IN(CommentEnd);
				}

				ON_EOF
				{
					RECONSUME_IN(CommentEnd);
				}

				ANYTHING_ELSE
				{
					parse_error("nested-comment");
					RECONSUME_IN(CommentEnd);
				}
			}
			END_STATE

			// 13.2.5.50 Comment end dash state
			BEGIN_STATE(CommentEndDash)
			{
				consume_next_input_character();
				ON('-')
				{
					RECONSUME_IN(CommentEnd);
				}

				ON_EOF
				{
					parse_error("eof-in-comment");
					// TODO - handle multiple emits
					assert(false);
					EMIT_CURRENT_TOKEN();
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_comment('-');
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.51 Comment end state
			BEGIN_STATE(CommentEnd)
			{
				consume_next_input_character();
				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON('!')
				{
					SWITCH_TO(CommentEndBang);
				}

				ON('-')
				{
					current_token.append_comment('-');
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-comment");
					// TODO - handle multiple emits
					assert(false);
					EMIT_CURRENT_TOKEN();
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_comment('-');
					current_token.append_comment('-');
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.52 Comment end bang state
			BEGIN_STATE(CommentEndBang)
			{
				consume_next_input_character();
				ON('-')
				{
					current_token.append_comment('-');
					current_token.append_comment('-');
					current_token.append_comment('!');
					SWITCH_TO(CommentEndDash);
				}

				ON('>')
				{
					parse_error("incorrectly-closed-comment");
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-comment");
					// TODO - handle multiple emits
					assert(false);
					EMIT_CURRENT_TOKEN();
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_comment('-');
					current_token.append_comment('!');
					RECONSUME_IN(Comment);
				}
			}
			END_STATE

			// 13.2.5.53 DOCTYPE state
			BEGIN_STATE(DOCTYPE)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					SWITCH_TO(BeforeDOCTYPEName);
				}

				ON('>')
				{
					RECONSUME_IN(BeforeDOCTYPEName);
				}

				ON_EOF
				{
					parse_error("eof-in-doctype");
					// TODO
				}

				ANYTHING_ELSE
				{
					parse_error("missing-whitespace-before-doctype-name");
					RECONSUME_IN(BeforeDOCTYPEName);
				}
			}
			END_STATE

			// 13.2.5.54 Before DOCTYPE name state
			BEGIN_STATE(BeforeDOCTYPEName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					IGNORE_CHARACTER();
				}

				ON_ASCII_UPPER_ALPHA
				{
					current_token = Token::make_doctype();
					current_token.doctype_set_name(current_input_character + 0x20);
					SWITCH_TO(DOCTYPEName);
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token = Token::make_doctype();
					current_token.doctype_set_name(U'\ufffd');
					SWITCH_TO(DOCTYPEName);
				}

				ON('>')
				{
					parse_error("missing-doctype-name");
					current_token = Token::make_doctype();
					current_token.set_force_quirks();
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-doctype");
					current_token.doctype_set_name(current_input_character);
					SWITCH_TO(DOCTYPEName);
				}

				ANYTHING_ELSE
				{
					current_token = Token::make_doctype();
					current_token.doctype_set_name(current_input_character);
					SWITCH_TO(DOCTYPEName);
				}
			}
			END_STATE

			// 13.2.5.55 DOCTYPE name state
			BEGIN_STATE(DOCTYPEName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					SWITCH_TO(AfterDOCTYPEName);
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_ASCII_UPPER_ALPHA
				{
					current_token.append_doctype_name(current_input_character + 0x20);
					continue;
				}

				ON_NULL
				{
					parse_error("unexpected-null-character");
					current_token.append_doctype_name(U'\ufffd');
					continue;
				}

				ON_EOF
				{
					parse_error("eof-in-doctype");
					current_token.set_force_quirks();
					// TODO - use EMIT macro
					emit_token(current_token);
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					current_token.append_doctype_name(current_input_character);
					continue;
				}
			}
			END_STATE

			// 13.2.5.56 After DOCTYPE name state
			BEGIN_STATE(AfterDOCTYPEName)
			{
				consume_next_input_character();
				ON_WHITESPACE
				{
					IGNORE_CHARACTER();
				}

				ON('>')
				{
					SWITCH_TO_AND_EMIT_TOKEN(Data, current_token);
				}

				ON_EOF
				{
					parse_error("eof-in-doctype");
					current_token.set_force_quirks();
					// TODO - use EMIT macro
					emit_token(current_token);
					EMIT_EOF();
				}

				ANYTHING_ELSE
				{
					if (consume_if_match("public"))
					{
						SWITCH_TO(AfterDOCTYPEPublicKeyword);
					}

					else if (consume_if_match("system"))
					{
						SWITCH_TO(AfterDOCTYPESystemKeyword);
					}

					else
					{
						parse_error("invalid-character-sequence-after-doctype-name");
						current_token.set_force_quirks();
						RECONSUME_IN(BogusDOCTYPE);
					}
				}
			}
			END_STATE

		// 13.2.5.57 After DOCTYPE public keyword state
		AfterDOCTYPEPublicKeyword:
		case State::AfterDOCTYPEPublicKeyword:
			break;

		// 13.2.5.58 Before DOCTYPE public identifier state
		BeforeDOCTYPEPublicIdentifier:
		case State::BeforeDOCTYPEPublicIdentifier:
			break;

		// 13.2.5.59 DOCTYPE public identifier (double-quoted) state
		DOCTYPEPublicIdentifierDoubleQuoted:
		case State::DOCTYPEPublicIdentifierDoubleQuoted:
			break;

		// 13.2.5.60 DOCTYPE public identifier (single-quoted) state
		DOCTYPEPublicIdentifierSingleQuoted:
		case State::DOCTYPEPublicIdentifierSingleQuoted:
			break;

		// 13.2.5.61 After DOCTYPE public identifier state
		AfterDOCTYPEPublicIdentifier:
		case State::AfterDOCTYPEPublicIdentifier:
			break;

		// 13.2.5.62 Between DOCTYPE public and system identifiers state
		BetweenDOCTYPEPublicAndSystemIdentifiers:
		case State::BetweenDOCTYPEPublicAndSystemIdentifiers:
			break;

		// 13.2.5.63 After DOCTYPE system keyword state
		AfterDOCTYPESystemKeyword:
		case State::AfterDOCTYPESystemKeyword:
			break;

		// 13.2.5.64 Before DOCTYPE system identifier state
		BeforeDOCTYPESystemIdentifier:
		case State::BeforeDOCTYPESystemIdentifier:
			break;

		// 13.2.5.65 DOCTYPE system identifier (double-quoted) state
		DOCTYPESystemIdentifierDoubleQuoted:
		case State::DOCTYPESystemIdentifierDoubleQuoted:
			break;

		// 13.2.5.66 DOCTYPE system identifier (single-quoted) state
		DOCTYPESystemIdentifierSingleQuoted:
		case State::DOCTYPESystemIdentifierSingleQuoted:
			break;

		// 13.2.5.67 After DOCTYPE system identifier state
		AfterDOCTYPESystemIdentifier:
		case State::AfterDOCTYPESystemIdentifier:
			break;

		// 13.2.5.68 Bogus DOCTYPE state
		BogusDOCTYPE:
		case State::BogusDOCTYPE:
			break;

		// 13.2.5.69 CDATA section state
		CDATASection:
		case State::CDATASection:
			break;

		// 13.2.5.70 CDATA section bracket state
		CDATASectionBracket:
		case State::CDATASectionBracket:
			break;

		// 13.2.5.71 CDATA section end state
		CDATASectionEnd:
		case State::CDATASectionEnd:
			break;

			// 13.2.5.72 Character reference state
			BEGIN_STATE(CharacterReference)
			{
				temporary_buffer = "&";
				consume_next_input_character();
				ON_ASCII_ALPHANUMERIC
				{
					RECONSUME_IN(NamedCharacterReference);
				}

				ON('#')
				{
					temporary_buffer += current_input_character;
					SWITCH_TO(NumericCharacterReference);
				}

				ANYTHING_ELSE
				{
					flush_temporary_buffer = true;
					RECONSUME_IN_RETURN_STATE();
				}
			}
			END_STATE

			// 13.2.5.73 Named character reference state
			// TODO - didn't follow the spec too closely for this one
			BEGIN_STATE(NamedCharacterReference)
			{
				if (consume_if_match("nbsp;"))
				{
					temporary_buffer += "nbsp;";
					flush_temporary_buffer = true;
					state = return_state;
				}

				else
				{
					SWITCH_TO(AmbiguousAmpersand);
				}
			}
			END_STATE

			// 13.2.5.74 Ambiguous ampersand state
			BEGIN_STATE(AmbiguousAmpersand)
			{
				consume_next_input_character();
				ON_ASCII_ALPHANUMERIC
				{
					if (consumed_as_part_of_an_attribute())
					{
						current_token.append_attribute_value(current_input_character);
						continue;
					}
					else
					{
						EMIT_CURRENT_INPUT_CHARACTER();
					}
				}

				ON(';')
				{
					parse_error("unknown-named-character-reference");
					RECONSUME_IN_RETURN_STATE();
				}

				ANYTHING_ELSE
				{
					RECONSUME_IN_RETURN_STATE();
				}
			}
			END_STATE

			// 13.2.5.75 Numeric character reference state
			BEGIN_STATE(NumericCharacterReference)
			{
				character_reference_code = 0;
				consume_next_input_character();

				if (current_input_character == 'x' || current_input_character == 'X')
				{
					temporary_buffer += current_input_character;
					SWITCH_TO(HexadecimalCharacterReferenceStart);
				}

				else
				{
					RECONSUME_IN(DecimalCharacterReferenceStart);
				}
			}
			END_STATE

			// 13.2.5.76 Hexadecimal character reference start state
			BEGIN_STATE(HexadecimalCharacterReferenceStart)
			{
				consume_next_input_character();
				if (std::isxdigit(current_input_character))
				{
					RECONSUME_IN(HexadecimalCharacterReference);
				}

				else
				{
					parse_error("absence-of-digits-in-numeric-character-reference");
					flush_temporary_buffer = true;
					RECONSUME_IN_RETURN_STATE();
				}
			}
			END_STATE

			// 13.2.5.77 Decimal character reference start state
			BEGIN_STATE(DecimalCharacterReferenceStart)
			{
				consume_next_input_character();
				ON_ASCII_DIGIT
				{
					RECONSUME_IN(DecimalCharacterReference);
				}

				else
				{
					parse_error("absence-of-digits-in-numeric-character-reference");
					flush_temporary_buffer = true;
					RECONSUME_IN_RETURN_STATE();
				}
			}
			END_STATE

			// 13.2.5.78 Hexadecimal character reference state
			BEGIN_STATE(HexadecimalCharacterReference)
			{
				consume_next_input_character();
				ON_ASCII_DIGIT
				{
					character_reference_code *= 16;
					character_reference_code += current_input_character - 0x30;
					continue;
				}

				ON_ASCII_UPPER_HEX_DIGIT
				{
					character_reference_code *= 16;
					character_reference_code += current_input_character - 0x37;
					continue;
				}

				ON_ASCII_LOWER_HEX_DIGIT
				{
					character_reference_code *= 16;
					character_reference_code += current_input_character - 0x57;
					continue;
				}

				ON(';')
				{
					SWITCH_TO(NumericCharacterReferenceEnd);
				}

				ANYTHING_ELSE
				{
					parse_error("missing-semicolon-after-character-reference");
					RECONSUME_IN(NumericCharacterReferenceEnd);
				}
			}
			END_STATE

			// 13.2.5.79 Decimal character reference state
			BEGIN_STATE(DecimalCharacterReference)
			{
				consume_next_input_character();
				ON_ASCII_DIGIT
				{
					character_reference_code *= 10;
					character_reference_code += current_input_character - 0x30;
					continue;
				}

				ON(';')
				{
					SWITCH_TO(NumericCharacterReferenceEnd);
				}

				ANYTHING_ELSE
				{
					parse_error("missing-semicolon-after-character-reference");
					RECONSUME_IN(NumericCharacterReferenceEnd);
				}
			}
			END_STATE

			// 13.2.5.80 Numeric character reference end state
			BEGIN_STATE(NumericCharacterReferenceEnd)
			{
				// TODO
				assert(false);
			}
			END_STATE
		}
	}
}

bool Tokenizer::eof()
{
	return pos >= input.length();
}

void Tokenizer::consume_next_input_character()
{
	current_input_character = next_input_character;

	if (eof())
	{
		next_input_character = 0xcafebabe;
		return;
	}

	next_input_character = input[++pos];
}

void Tokenizer::reconsume_in(State s)
{
	state = s;
	next_input_character = current_input_character;
	current_input_character = input[--pos];
}

bool Tokenizer::consume_if_match(std::string const &str, bool case_sensitive)
{
	auto size = str.size();
	if (pos + size >= input.size())
		return false;

	for (std::size_t i = 0; i < size; i++)
	{
		auto c1 = str[i];
		auto c2 = input[pos + i - 1];
		if (!case_sensitive)
		{
			c1 = std::tolower(c1);
			c2 = std::tolower(c2);
		}

		if (c1 != c2)
			return false;
	}

	for (std::size_t i = 0; i < size - 1; i++)
		consume_next_input_character();

	return true;
}

void Tokenizer::emit_token(Token token)
{
	emitted_tokens.push_back(token);
}

void Tokenizer::emit_tokens(const std::initializer_list<Token> &tokens)
{
	for (auto const &token : tokens)
		emitted_tokens.push_back(token);
}

void Tokenizer::parse_error(const char *msg)
{
	std::cout << "Parse error: " << msg << "\n";
}
// https://html.spec.whatwg.org/multipage/parsing.html#charref-in-attribute
bool Tokenizer::consumed_as_part_of_an_attribute() const
{
	return return_state == State::AttributeValueDoubleQuoted || return_state == State::AttributeValueSingleQuoted ||
	       return_state == State::AttributeValueUnquoted;
}
}

#pragma GCC diagnostic pop
