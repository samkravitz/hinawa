#pragma once

#include <string>

#include "token.h"

namespace html
{
#define STATES \
	STATE(Data) \
	STATE(RCDATA) \
	STATE(RAWTEXT) \
	STATE(ScriptData) \
	STATE(PLAINTEXT) \
	STATE(TagOpen) \
	STATE(EndTagOpen) \
	STATE(TagName) \
	STATE(RCDATALessThanSign) \
	STATE(RCDATAEndTagOpen) \
	STATE(RCDATAEndTagName) \
	STATE(RAWTEXTLessThanSign) \
	STATE(RAWTEXTEndTagOpen) \
	STATE(RAWTEXTEndTagName) \
	STATE(ScriptDataLessThanSign) \
	STATE(ScriptDataEndTagOpen) \
	STATE(ScriptDataEndTagName) \
	STATE(ScriptDataEscapeStart) \
	STATE(ScriptDataEscapeStartDash) \
	STATE(ScriptDataEscaped) \
	STATE(ScriptDataEscapedDash) \
	STATE(ScriptDataEscapedDashDash) \
	STATE(ScriptDataEscapedLessThanSign) \
	STATE(ScriptDataEscapedEndTagOpen) \
	STATE(ScriptDataEscapedEndTagName) \
	STATE(ScriptDataDoubleEscapeStart) \
	STATE(ScriptDataDoubleEscaped) \
	STATE(ScriptDataDoubleEscapedDash) \
	STATE(ScriptDataDoubleEscapedDashDash) \
	STATE(ScriptDataDoubleEscapedLessThanSign) \
	STATE(ScriptDataDoubleEscapeEnd) \
	STATE(BeforeAttributeName) \
	STATE(AttributeName) \
	STATE(AfterAttributeName) \
	STATE(BeforeAttributeValue) \
	STATE(AttributeValueDoubleQuoted) \
	STATE(AttributeValueSingleQuoted) \
	STATE(AttributeValueUnquoted) \
	STATE(AfterAttributeValueQuoted) \
	STATE(SelfClosingStartTag) \
	STATE(BogusComment) \
	STATE(MarkupDeclarationOpen) \
	STATE(CommentStart) \
	STATE(CommentStartDash) \
	STATE(Comment) \
	STATE(CommentLessThanSign) \
	STATE(CommentLessThanSignBang) \
	STATE(CommentLessThanSignBangDash) \
	STATE(CommentLessThanSignBangDashDash) \
	STATE(CommentEndDash) \
	STATE(CommentEnd) \
	STATE(CommentEndBang) \
	STATE(DOCTYPE) \
	STATE(BeforeDOCTYPEName) \
	STATE(DOCTYPEName) \
	STATE(AfterDOCTYPEName) \
	STATE(AfterDOCTYPEPublicKeyword) \
	STATE(BeforeDOCTYPEPublicIdentifier) \
	STATE(DOCTYPEPublicIdentifierDoubleQuoted) \
	STATE(DOCTYPEPublicIdentifierSingleQuoted) \
	STATE(AfterDOCTYPEPublicIdentifier) \
	STATE(BetweenDOCTYPEPublicAndSystemIdentifiers) \
	STATE(AfterDOCTYPESystemKeyword) \
	STATE(BeforeDOCTYPESystemIdentifier) \
	STATE(DOCTYPESystemIdentifierDoubleQuoted) \
	STATE(DOCTYPESystemIdentifierSingleQuoted) \
	STATE(AfterDOCTYPESystemIdentifier) \
	STATE(BogusDOCTYPE) \
	STATE(CDATASection) \
	STATE(CDATASectionBracket) \
	STATE(CDATASectionEnd) \
	STATE(CharacterReference) \
	STATE(NamedCharacterReference) \
	STATE(AmbiguousAmpersand) \
	STATE(NumericCharacterReference) \
	STATE(HexadecimalCharacterReferenceStart) \
	STATE(DecimalCharacterReferenceStart) \
	STATE(HexadecimalCharacterReference) \
	STATE(DecimalCharacterReference) \
	STATE(NumericCharacterReferenceEnd)

class Tokenizer
{
public:
	Tokenizer(std::string const input) : input(input) { }
	enum class State
	{
#define STATE(state) state,
		STATES
#undef STATE
	};

	void run();

private:
	bool eof();
	void emit(Token);
	void consume_next_input_character();
	void reconsume_in(State);

	State state = State::Data;
	State return_state = State::Data;
	std::string input;
	std::size_t pos = 0;
	char current_input_character;
	char next_input_character;
	Token current_token;
};
}    // namespace html
