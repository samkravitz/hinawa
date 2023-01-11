#pragma once

#include <initializer_list>
#include <list>
#include <string>

#include "token.h"
#include "util/hinawa.h"

namespace html
{
#define STATES                                      \
	STATE(Data)                                     \
	STATE(RCDATA)                                   \
	STATE(RAWTEXT)                                  \
	STATE(ScriptData)                               \
	STATE(PLAINTEXT)                                \
	STATE(TagOpen)                                  \
	STATE(EndTagOpen)                               \
	STATE(TagName)                                  \
	STATE(RCDATALessThanSign)                       \
	STATE(RCDATAEndTagOpen)                         \
	STATE(RCDATAEndTagName)                         \
	STATE(RAWTEXTLessThanSign)                      \
	STATE(RAWTEXTEndTagOpen)                        \
	STATE(RAWTEXTEndTagName)                        \
	STATE(ScriptDataLessThanSign)                   \
	STATE(ScriptDataEndTagOpen)                     \
	STATE(ScriptDataEndTagName)                     \
	STATE(ScriptDataEscapeStart)                    \
	STATE(ScriptDataEscapeStartDash)                \
	STATE(ScriptDataEscaped)                        \
	STATE(ScriptDataEscapedDash)                    \
	STATE(ScriptDataEscapedDashDash)                \
	STATE(ScriptDataEscapedLessThanSign)            \
	STATE(ScriptDataEscapedEndTagOpen)              \
	STATE(ScriptDataEscapedEndTagName)              \
	STATE(ScriptDataDoubleEscapeStart)              \
	STATE(ScriptDataDoubleEscaped)                  \
	STATE(ScriptDataDoubleEscapedDash)              \
	STATE(ScriptDataDoubleEscapedDashDash)          \
	STATE(ScriptDataDoubleEscapedLessThanSign)      \
	STATE(ScriptDataDoubleEscapeEnd)                \
	STATE(BeforeAttributeName)                      \
	STATE(AttributeName)                            \
	STATE(AfterAttributeName)                       \
	STATE(BeforeAttributeValue)                     \
	STATE(AttributeValueDoubleQuoted)               \
	STATE(AttributeValueSingleQuoted)               \
	STATE(AttributeValueUnquoted)                   \
	STATE(AfterAttributeValueQuoted)                \
	STATE(SelfClosingStartTag)                      \
	STATE(BogusComment)                             \
	STATE(MarkupDeclarationOpen)                    \
	STATE(CommentStart)                             \
	STATE(CommentStartDash)                         \
	STATE(Comment)                                  \
	STATE(CommentLessThanSign)                      \
	STATE(CommentLessThanSignBang)                  \
	STATE(CommentLessThanSignBangDash)              \
	STATE(CommentLessThanSignBangDashDash)          \
	STATE(CommentEndDash)                           \
	STATE(CommentEnd)                               \
	STATE(CommentEndBang)                           \
	STATE(DOCTYPE)                                  \
	STATE(BeforeDOCTYPEName)                        \
	STATE(DOCTYPEName)                              \
	STATE(AfterDOCTYPEName)                         \
	STATE(AfterDOCTYPEPublicKeyword)                \
	STATE(BeforeDOCTYPEPublicIdentifier)            \
	STATE(DOCTYPEPublicIdentifierDoubleQuoted)      \
	STATE(DOCTYPEPublicIdentifierSingleQuoted)      \
	STATE(AfterDOCTYPEPublicIdentifier)             \
	STATE(BetweenDOCTYPEPublicAndSystemIdentifiers) \
	STATE(AfterDOCTYPESystemKeyword)                \
	STATE(BeforeDOCTYPESystemIdentifier)            \
	STATE(DOCTYPESystemIdentifierDoubleQuoted)      \
	STATE(DOCTYPESystemIdentifierSingleQuoted)      \
	STATE(AfterDOCTYPESystemIdentifier)             \
	STATE(BogusDOCTYPE)                             \
	STATE(CDATASection)                             \
	STATE(CDATASectionBracket)                      \
	STATE(CDATASectionEnd)                          \
	STATE(CharacterReference)                       \
	STATE(NamedCharacterReference)                  \
	STATE(AmbiguousAmpersand)                       \
	STATE(NumericCharacterReference)                \
	STATE(HexadecimalCharacterReferenceStart)       \
	STATE(DecimalCharacterReferenceStart)           \
	STATE(HexadecimalCharacterReference)            \
	STATE(DecimalCharacterReference)                \
	STATE(NumericCharacterReferenceEnd)

class Tokenizer
{
public:
	Tokenizer() = default;
	Tokenizer(std::string const input);
	enum class State
	{
#define STATE(state) state,
		STATES
#undef STATE
	};

	std::vector<Token> scan_all();
	Token next();
	void set_state(State s) { state = s; }

private:
	bool eof();
	void consume_next_input_character();
	bool consume_if_match(std::string const &str, bool case_sensitive = false);
	void parse_error(const char *);
	bool consumed_as_part_of_an_attribute() const;
	bool appropriate_end_tag_token() const;

	State state = State::Data;
	State return_state = State::Data;
	std::string input;
	std::size_t pos = 0;
	u32 current_input_character;
	u32 next_input_character;
	Token current_token;
	std::string temporary_buffer;
	bool flush_temporary_buffer = false;
	unsigned int character_reference_code = 0;

	// tokens that have been emitted but are still waiting to be returned
	std::list<Token> emitted_tokens;
};
}
