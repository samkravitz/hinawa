// implements the HTML tokenization algorithm from
// https://html.spec.whatwg.org/multipage/parsing.html#tokenization

#include "tokenizer.h"

#include <iostream>

namespace html
{
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
	while (1)
	{
		consume_next_input_character();

		switch (state)
		{
			// 13.2.5.1 Data state
			case State::Data:
				switch (current_input_character)
				{
					case '&':
						return_state = State::Data;
						state = State::CharacterReference;
						break;
					case '<': state = State::TagOpen; break;
					case '\0': return Token::make_eof(); break;
					// case EOF
					default: return Token::make_character(current_input_character);
				}
				break;

			// 13.2.5.2 RCDATA state
			case State::RCDATA:
				switch (current_input_character)
				{
					case '&':
						return_state = State::RCDATA;
						state = State::CharacterReference;
						break;
					case '<': state = State::RCDATALessThanSign; break;
					case '\0': break;
					// case EOF
					default: return Token::make_character(current_input_character);
				}
				break;

			// 13.2.5.3 RAWTEXT state
			case State::RAWTEXT:
				switch (current_input_character)
				{
					case '<': state = State::RAWTEXTLessThanSign; break;
					case '\0': break;
					// case EOF
					default: Token::make_character(current_input_character);
				}
				break;

			// 13.2.5.4 Script data state
			case State::ScriptData:
				switch (current_input_character)
				{
					case '<': state = State::ScriptDataDoubleEscapedLessThanSign; break;
					case '\0': break;
					// case EOF
					default: return Token::make_character(current_input_character);
				}
				break;

			// 13.2.5.5 PLAINTEXT state
			case State::PLAINTEXT: break;

			// 13.2.5.6 Tag open state
			case State::TagOpen:
				if (current_input_character == '!')
					state = State::MarkupDeclarationOpen;

				else if (current_input_character == '/')
					state = State::EndTagOpen;

				else if (isalpha(current_input_character))
				{
					current_token = Token::make_start_tag();
					reconsume_in(State::TagName);
				}

				else if (current_input_character == '?')
				{
					current_token = Token::make_comment();
					reconsume_in(State::BogusComment);
				}

				// TODO: EOF

				else
				{
					return Token::make_character('<');
					reconsume_in(State::Data);
				}
				break;

			// 13.2.5.7 End tag open state
			case State::EndTagOpen:
				if (isalpha(current_input_character))
				{
					current_token = Token::make_end_tag();
					reconsume_in(State::TagName);
				}

				else if (current_input_character == '>')
				{
					state = State::Data;
				}

				// TODO: EOF

				else
				{
					current_token = Token::make_comment();
					reconsume_in(State::BogusComment);
				}
				break;

			// 13.2.5.8 Tag name state
			case State::TagName:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ': state = State::BeforeAttributeName; break;

					case '/': state = State::SelfClosingStartTag; break;

					case '>':
						state = State::Data;
						return current_token;
						break;

					case '\0':
						// case EOF:
						break;

					default:
					{
						auto c = current_input_character;
						if (isupper(c))
							c += 32;

						current_token.append_tag_name(c);
					}
				}
				break;

			// 13.2.5.9 RCDATA less-than sign state
			case State::RCDATALessThanSign: break;

			// 13.2.5.10 RCDATA end tag open state
			case State::RCDATAEndTagOpen: break;

			// 13.2.5.11 RCDATA end tag name state
			case State::RCDATAEndTagName: break;

			// 13.2.5.12 RAWTEXT less-than sign state
			case State::RAWTEXTLessThanSign: break;

			// 13.2.5.13 RAWTEXT end tag open state
			case State::RAWTEXTEndTagOpen: break;

			// 13.2.5.14 RAWTEXT end tag name state
			case State::RAWTEXTEndTagName: break;

			// 13.2.5.15 Script data less-than sign state
			case State::ScriptDataLessThanSign: break;

			// 13.2.5.16 Script data end tag open state
			case State::ScriptDataEndTagOpen: break;

			// 13.2.5.17 Script data end tag name state
			case State::ScriptDataEndTagName: break;

			// 13.2.5.18 Script data escape start state
			case State::ScriptDataEscapeStart: break;

			// 13.2.5.19 Script data escape start dash state
			case State::ScriptDataEscapeStartDash: break;

			// 13.2.5.20 Script data escaped state
			case State::ScriptDataEscaped: break;

			// 13.2.5.21 Script data escaped dash state
			case State::ScriptDataEscapedDash: break;

			// 13.2.5.22 Script data escaped dash dash state
			case State::ScriptDataEscapedDashDash: break;

			// 13.2.5.23 Script data escaped less-than sign state
			case State::ScriptDataEscapedLessThanSign: break;

			// 13.2.5.24 Script data escaped end tag open state
			case State::ScriptDataEscapedEndTagOpen: break;

			// 13.2.5.25 Script data escaped end tag name state
			case State::ScriptDataEscapedEndTagName: break;

			// 13.2.5.26 Script data double escape start state
			case State::ScriptDataDoubleEscapeStart: break;

			// 13.2.5.27 Script data double escaped state
			case State::ScriptDataDoubleEscaped: break;

			// 13.2.5.28 Script data double escaped dash state
			case State::ScriptDataDoubleEscapedDash: break;

			// 13.2.5.29 Script data double escaped dash dash state
			case State::ScriptDataDoubleEscapedDashDash: break;

			// 13.2.5.30 Script data double escaped less-than sign state
			case State::ScriptDataDoubleEscapedLessThanSign: break;

			// 13.2.5.31 Script data double escape end state
			case State::ScriptDataDoubleEscapeEnd: break;

			// 13.2.5.32 Before attribute name state
			case State::BeforeAttributeName:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ': break;    // ignore the character

					case '/':
					case '>':
						// case EOF:
						reconsume_in(State::AfterAttributeName);
						break;

					case '=':
						current_token.new_attribute();
						current_token.append_attribute_name(current_input_character);
						state = State::AttributeName;
						break;

					default: current_token.new_attribute(); reconsume_in(State::AttributeName);
				}
				break;

			// 13.2.5.33 Attribute name state
			case State::AttributeName:
				if (current_input_character == '\t' || current_input_character == '\n' ||
				    current_input_character == '\f' || current_input_character == ' ' ||
				    current_input_character == '/' || current_input_character == '>'
				    // TODO: EOF
				)
				{
					reconsume_in(State::AfterAttributeName);
				}

				else if (current_input_character == '=')
				{
					state = State::BeforeAttributeValue;
				}

				else if (isupper(current_input_character))
				{
					current_token.append_attribute_name(current_input_character + 32);
				}

				else if (current_input_character == '\0')
				{ }

				else
				{
					current_token.append_attribute_name(current_input_character);
				}

				break;

			// 13.2.5.34 After attribute name state
			case State::AfterAttributeName:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ':
						// ignore the character
						break;

					case '/': state = State::SelfClosingStartTag; break;

					case '=': state = State::BeforeAttributeValue; break;

					case '>':
						state = State::Data;
						return current_token;
						break;

						// case EOF:

					default: current_token.new_attribute(); reconsume_in(State::AttributeName);
				}
				break;

			// 13.2.5.35 Before attribute value state
			case State::BeforeAttributeValue:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ':
						// ignore the character
						break;

					case '\"': state = State::AttributeValueDoubleQuoted; break;

					case '\'': state = State::AttributeValueSingleQuoted; break;

					case '>':
						state = State::Data;
						return current_token;
						break;

					default: current_token.new_attribute(); reconsume_in(State::AttributeValueUnquoted);
				}
				break;

			// 13.2.5.36 Attribute value (double-quoted) state
			case State::AttributeValueDoubleQuoted:
				switch (current_input_character)
				{
					case '\"': state = State::AfterAttributeValueQuoted; break;

					case '&':
						return_state = State::AttributeValueDoubleQuoted;
						state = State::CharacterReference;
						break;

					default: current_token.append_attribute_value(current_input_character);
				}
				break;

			// 13.2.5.37 Attribute value (single-quoted) state
			case State::AttributeValueSingleQuoted:
				switch (current_input_character)
				{
					case '\'': state = State::AfterAttributeValueQuoted; break;

					case '&':
						return_state = State::AttributeValueSingleQuoted;
						state = State::CharacterReference;
						break;

					default: current_token.append_attribute_value(current_input_character);
				}
				break;

			// 13.2.5.38 Attribute value (unquoted) state
			case State::AttributeValueUnquoted:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ': state = State::BeforeAttributeName; break;

					case '&':
						return_state = State::AttributeValueUnquoted;
						state = State::CharacterReference;
						break;

					case '>':
						state = State::Data;
						return current_token;
						break;

					case '\0':
						break;

						// case EOF:

					default: current_token.append_attribute_value(current_input_character);
				}
				break;

			// 13.2.5.39 After attribute value (quoted) state
			case State::AfterAttributeValueQuoted:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ': state = State::BeforeAttributeName; break;

					case '/': state = State::SelfClosingStartTag; break;

					case '>':
						state = State::Data;
						return current_token;
						break;

					default: reconsume_in(State::BeforeAttributeName);
				}
				break;

			// 13.2.5.40 Self-closing start tag state
			case State::SelfClosingStartTag:
				switch (current_input_character)
				{
					case '>':
						current_token.set_self_closing();
						state = State::Data;
						return current_token;
						break;

					default: reconsume_in(State::BeforeAttributeName);
				}
				break;

			// 13.2.5.41 Bogus comment state
			case State::BogusComment: break;

			// 13.2.5.42 Markup declaration open state
			case State::MarkupDeclarationOpen:
				if (consume_if_match("--"))
				{
					current_token = Token::make_comment();
					state = State::CommentStart;
				}

				else if (consume_if_match("doctype"))
				{
					state = State::DOCTYPE;
				}

				else if (consume_if_match("[CDATA["))
				{ }

				else
				{
					current_token = Token::make_comment();
					state = State::BogusComment;
				}
				break;

			// 13.2.5.43 Comment start state
			case State::CommentStart:
				switch (next_input_character)
				{
					case '-': state = State::CommentStartDash; break;

					case '>': state = State::Data; return current_token;

					default: reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.44 Comment start dash state
			case State::CommentStartDash:
				switch (next_input_character)
				{
					case '-': state = State::CommentEnd; break;

					case '>': state = State::Data; return current_token;

					default: current_token.append_comment(current_input_character); reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.45 Comment state
			case State::Comment:
				switch (next_input_character)
				{
					case '<':
						current_token.append_comment(next_input_character);
						state = State::CommentLessThanSign;
						break;

					case '-': state = State::CommentEndDash; break;

					case '/0': current_token.append_comment('\ufffd'); break;

					default: current_token.append_comment(current_input_character);
				}
				break;

			// 13.2.5.46 Comment less-than sign state
			case State::CommentLessThanSign:
				switch (next_input_character)
				{
					case '!':
						current_token.append_comment(current_input_character);
						state = State::CommentLessThanSignBang;
						break;

					case '<': current_token.append_comment(current_input_character); break;

					default: reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.47 Comment less-than sign bang state
			case State::CommentLessThanSignBang:
				switch (next_input_character)
				{
					case '-': state = State::CommentLessThanSignBangDash; break;

					default: reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.48 Comment less-than sign bang dash state
			case State::CommentLessThanSignBangDash:
				switch (next_input_character)
				{
					case '-': state = State::CommentLessThanSignBangDashDash; break;

					default: reconsume_in(State::CommentEndDash);
				}
				break;

			// 13.2.5.49 Comment less-than sign bang dash dash state
			case State::CommentLessThanSignBangDashDash:
				switch (next_input_character)
				{
					case '>': state = State::CommentEnd; break;

					default: reconsume_in(State::CommentEnd);
				}
				break;

			// 13.2.5.50 Comment end dash state
			case State::CommentEndDash:
				switch (next_input_character)
				{
					case '-': state = State::CommentEnd; break;

					default: current_token.append_comment('-'); reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.51 Comment end state
			case State::CommentEnd:
				switch (next_input_character)
				{
					case '>':
						state = State::Data;
						return current_token;
						break;

					case '!': state = State::CommentEndBang; break;

					case '-': current_token.append_comment('-'); break;

					default:
						current_token.append_comment('-');
						current_token.append_comment('-');
						reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.52 Comment end bang state
			case State::CommentEndBang:
				switch (next_input_character)
				{
					case '-':
						current_token.append_comment('-');
						current_token.append_comment('-');
						current_token.append_comment('!');
						state = State::CommentEndDash;
						break;

					case '>':
						state = State::Data;
						return current_token;
						break;

					default:
						current_token.append_comment('-');
						current_token.append_comment('-');
						current_token.append_comment('!');
						reconsume_in(State::Comment);
				}
				break;

			// 13.2.5.53 DOCTYPE state
			case State::DOCTYPE:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ': state = State::BeforeDOCTYPEName; break;

					case '>':
						reconsume_in(State::BeforeDOCTYPEName);
						break;

						// case EOF:

					default: reconsume_in(State::BeforeDOCTYPEName);
				}
				break;

			// 13.2.5.54 Before DOCTYPE name state
			case State::BeforeDOCTYPEName:
				if (current_input_character == '\t' || current_input_character == '\n' ||
				    current_input_character == '\f' || current_input_character == ' ')
				{
					// ignore the character
					;
				}

				else if (isupper(current_input_character))
				{
					current_token = Token::make_doctype();
					current_token.doctype_set_name(current_input_character + 32);
				}

				else if (current_input_character == '\0')
				{
					current_token = Token::make_doctype();
					current_token.doctype_set_name('\ufffd');
					state = State::DOCTYPEName;
				}

				else if (current_input_character == '>')
				{
					current_token = Token::make_doctype();
					current_token.set_force_quirks();
				}

				// TODO: EOF

				else
				{
					current_token = Token::make_doctype();
					current_token.doctype_set_name(current_input_character);
					state = State::DOCTYPEName;
				}
				break;

			// 13.2.5.55 DOCTYPE name state
			case State::DOCTYPEName:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ': state = State::AfterDOCTYPEName; break;

					case '>':
						state = State::Data;
						return current_token;
						break;

						// case EOF:

					default:
						if (std::isupper(current_input_character))
							current_input_character -= 32;

						current_token.append_doctype_name(current_input_character);
				}
				break;

			// 13.2.5.56 After DOCTYPE name state
			case State::AfterDOCTYPEName:
				switch (current_input_character)
				{
					case '\t':
					case '\n':
					case '\f':
					case ' ':
						// ignore the character
						break;

					case '>':
						state = State::Data;
						return current_token;
						break;

						// case EOF:

					default:
						if (consume_if_match("public"))
						{
							state = State::AfterDOCTYPEPublicKeyword;
						}

						else if (consume_if_match("system"))
						{
							state = State::AfterDOCTYPESystemKeyword;
						}

						else
						{
							current_token.set_force_quirks();
							reconsume_in(State::BogusDOCTYPE);
						}
				}
				break;
			
			// 13.2.5.57 After DOCTYPE public keyword state
			case State::AfterDOCTYPEPublicKeyword: break;

			// 13.2.5.58 Before DOCTYPE public identifier state
			case State::BeforeDOCTYPEPublicIdentifier: break;

			// 13.2.5.59 DOCTYPE public identifier (double-quoted) state
			case State::DOCTYPEPublicIdentifierDoubleQuoted: break;

			// 13.2.5.60 DOCTYPE public identifier (single-quoted) state
			case State::DOCTYPEPublicIdentifierSingleQuoted: break;

			// 13.2.5.61 After DOCTYPE public identifier state
			case State::AfterDOCTYPEPublicIdentifier: break;

			// 13.2.5.62 Between DOCTYPE public and system identifiers state
			case State::BetweenDOCTYPEPublicAndSystemIdentifiers: break;

			// 13.2.5.63 After DOCTYPE system keyword state
			case State::AfterDOCTYPESystemKeyword: break;

			// 13.2.5.64 Before DOCTYPE system identifier state
			case State::BeforeDOCTYPESystemIdentifier: break;

			// 13.2.5.65 DOCTYPE system identifier (double-quoted) state
			case State::DOCTYPESystemIdentifierDoubleQuoted: break;

			// 13.2.5.66 DOCTYPE system identifier (single-quoted) state
			case State::DOCTYPESystemIdentifierSingleQuoted: break;

			// 13.2.5.67 After DOCTYPE system identifier state
			case State::AfterDOCTYPESystemIdentifier: break;

			// 13.2.5.68 Bogus DOCTYPE state
			case State::BogusDOCTYPE: break;

			// 13.2.5.69 CDATA section state
			case State::CDATASection: break;

			// 13.2.5.70 CDATA section bracket state
			case State::CDATASectionBracket: break;

			// 13.2.5.71 CDATA section end state
			case State::CDATASectionEnd: break;

			// 13.2.5.72 Character reference state
			case State::CharacterReference: break;

			// 13.2.5.73 Named character reference state
			case State::NamedCharacterReference: break;

			// 13.2.5.74 Ambiguous ampersand state
			case State::AmbiguousAmpersand: break;

			// 13.2.5.75 Numeric character reference state
			case State::NumericCharacterReference: break;

			// 13.2.5.76 Hexadecimal character reference start state
			case State::HexadecimalCharacterReferenceStart: break;

			// 13.2.5.77 Decimal character reference start state
			case State::DecimalCharacterReferenceStart: break;

			// 13.2.5.78 Hexadecimal character reference state
			case State::HexadecimalCharacterReference: break;

			// 13.2.5.79 Decimal character reference state
			case State::DecimalCharacterReference: break;

			// 13.2.5.80 Numeric character reference end state
			case State::NumericCharacterReferenceEnd: break;
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
		next_input_character = '\0';
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
}
