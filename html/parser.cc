// parses an HTML document into a DOM tree according to
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

#include "parser.h"

#include <initializer_list>

#include <fmt/format.h>

#include "document/element.h"
#include "document/element_factory.h"
#include "document/node.h"
#include "document/text.h"
#include "token.h"

namespace html
{

static inline bool is_one_of(const std::string &str, const std::initializer_list<std::string> &strs)
{
	for (const auto &s : strs)
	{
		if (str == s)
			return true;
	}

	return false;
}

Parser::Parser(Document &document) :
    m_document(&document)
{ }

void Parser::parse(std::string const &input, Document &document)
{
	// The parser is associated with a document when it is created
	// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction
	auto parser = Parser(document);
	parser.run(input);
}

void Parser::run(const std::string &input)
{
	tokenizer = Tokenizer(input);
	while (auto token = tokenizer.next())
	{
		// std::cout << token.to_string() << "\n";
		process_using_the_rules_for(insertion_mode, token);
	}
}

void Parser::process_using_the_rules_for(InsertionMode mode, Token &token)
{
	switch (mode)
	{
		case InsertionMode::Initial:
			handle_initial(token);
			break;
		case InsertionMode::BeforeHtml:
			handle_before_html(token);
			break;
		case InsertionMode::BeforeHead:
			handle_before_head(token);
			break;
		case InsertionMode::InHead:
			handle_in_head(token);
			break;
		case InsertionMode::InHeadNoScript:
			handle_in_head_noscript(token);
			break;
		case InsertionMode::AfterHead:
			handle_after_head(token);
			break;
		case InsertionMode::InBody:
			handle_in_body(token);
			break;
		case InsertionMode::AfterBody:
			handle_after_body(token);
			break;
		case InsertionMode::AfterAfterBody:
			handle_after_after_body(token);
			break;
		case InsertionMode::Text:
			handle_text(token);
			break;
		case InsertionMode::InTable:
			handle_in_table(token);
			break;
		case InsertionMode::InTableBody:
			handle_in_table_body(token);
			break;
		case InsertionMode::InRow:
			handle_in_row(token);
			break;
		case InsertionMode::InCell:
			handle_in_cell(token);
			break;
		case InsertionMode::InTableText:
			handle_in_table_text(token);
			break;
		case InsertionMode::InSelectInTable:
			handle_in_select_in_table(token);
			break;
		case InsertionMode::InSelect:
			handle_in_select(token);
			break;
		case InsertionMode::InCaption:
			handle_in_caption(token);
			break;
		case InsertionMode::InColumnGroup:
			handle_in_column_group(token);
			break;
		case InsertionMode::InTemplate:
			handle_in_template(token);
			break;
		case InsertionMode::InFrameset:
			handle_in_frameset(token);
			break;
		case InsertionMode::AfterFrameset:
			handle_after_frameset(token);
			break;
		case InsertionMode::AfterAfterFrameset:
			handle_after_after_frameset(token);
			break;
	}
}

// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
// 13.2.6.4.1 The "initial" insertion mode
void Parser::handle_initial(Token &token)
{
	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// ignore the token
		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		insert_comment(token);
		return;
	}

	// -> anything else:
	// if the document is not an iframe srcdoc document, then this is a parse error; if the parser cannot change the mode flag is false, set the Document to quirks mode

	// in any case, switch the insertion mode to "before html", then reprocess the token
	insertion_mode = InsertionMode::BeforeHtml;
	//process_using_the_rules_for(insertion_mode, token);
}

//https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
// 13.2.6.4.2 The "before html" insertion mode
void Parser::handle_before_html(Token &token)
{
	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// ignore the token
		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		insert_comment(token);
		return;
	}

	// -> a start tag whose tag name is "html"
	if (token.is_start_tag() && token.tag_name() == "html")
	{
		// create an element for the token in the HTML namespace, with the Document as the intended parent. Append it to the Document object. Put this element in the stack of open elements
		auto html_element = create_element(document(), "html");
		document().add_child(html_element);
		stack_of_open_elements.push_back(html_element);

		// switch the insertion mode to "before head"
		insertion_mode = InsertionMode::BeforeHead;
		return;
	}

	// -> an end tag whose tag name is one of: "head", "body", "html", "br"
	if (token.is_end_tag() && is_one_of(token.tag_name(), {"head", "body", "html", "br"}))
	{
		// act as described in the "anything else" entry below
		goto anything_else;
	}

	// -> any other end tag
	if (token.is_end_tag())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

anything_else:
	// -> anything else

	// create an html element whose node document is the Document object
	auto html_element = create_element(document(), "html");

	// append it to the Document object
	document().add_child(html_element);

	// put this element in the stack of open elements
	stack_of_open_elements.push_back(html_element);

	// switch the insertion mode to "before head", then reprocess the token
	insertion_mode = InsertionMode::BeforeHead;
	process_using_the_rules_for(insertion_mode, token);
}

// https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
// 13.2.6.4.3 The "before head" insertion mode
void Parser::handle_before_head(Token &token)
{
	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// ignore the token
		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		insert_comment(token);
		return;
	}

	// -> a start tag whose tag name is "html"
	if (token.is_start_tag() && token.tag_name() == "html")
	{
		// process the token using the rules for the "in body" insertion mode
		process_using_the_rules_for(InsertionMode::InBody, token);
		return;
	}

	// -> a start tag whose tag name is "head"
	if (token.is_start_tag() && token.tag_name() == "head")
	{
		// insert an HTML element for the token
		auto head_element = create_element(document(), "head");

		// set the head element pointer to the newly created head element
		insert_element(head_element);

		// switch the insertion mode to "in head"
		insertion_mode = InsertionMode::InHead;
		return;
	}

	// -> an end tag whose tag name is one of: "head", "body", "html", "br"
	if (token.is_end_tag() && is_one_of(token.tag_name(), {"head", "body", "html", "br"}))
	{
		// act as described in the "anything else" entry below.
		goto anything_else;
	}

	// -> any other end tag
	if (token.is_end_tag())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

anything_else:
	// -> anything else
	// insert an HTML element for a "head" start tag token with no attributes
	auto head_element = create_element(document(), "head");
	insert_element(head_element);

	// TODO
	// set the head element pointer to the newly created head element

	// switch the insertion mode to "in head"
	insertion_mode = InsertionMode::InHead;

	// reprocess the current token
	process_using_the_rules_for(insertion_mode, token);
}

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
// 13.2.6.4.4 The "in head" insertion mode
void Parser::handle_in_head(Token &token)
{
	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// insert the character
		insert_character(token);
		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		insert_comment(token);
		return;
	}

	// -> a start tag whose tag name is "html"
	if (token.is_start_tag() && token.tag_name() == "html")
	{
		// process the token using the rules for the "in body" insertion mode
		process_using_the_rules_for(InsertionMode::InBody, token);
		return;
	}

	// -> a start tag whose tag name is "meta"
	if (token.is_start_tag() && token.tag_name() == "meta")
	{
		// insert an HTML element for the token
		// immediately pop the current node off the stack of open elements
		auto head_element = create_element(document(), "head");
		insert_element(head_element);
		stack_of_open_elements.pop_back();

		// acknowledge the token's self-closing flag, if it is set
		// TODO - handle speculative parsing
		return;
	}

	// -> a start tag whose tag name is one of: "noframes", "style"
	if (token.is_start_tag() && is_one_of(token.tag_name(), {"noframes", "style"}))
	{
		parse_raw_text(token);
		return;
	}

	// -> a start tag whose tag name is "script"
	if (token.is_start_tag() && token.tag_name() == "script")
	{
		parse_raw_text(token);
		return;
	}

	// -> an end tag whose name is "head"
	if (token.is_end_tag() && token.tag_name() == "head")
	{
		// pop the current node (which will be the head element) off the stack of open elements
		stack_of_open_elements.pop_back();

		// switch the insertion mode to "after head"
		insertion_mode = InsertionMode::AfterHead;
		return;
	}

	// -> an end tag whose tag name is one of: "body", "html", "br"
	if (token.is_end_tag() && is_one_of(token.tag_name(), {"body", "html", "br"}))
	{
		// act as described in the "anything else" entry below
		goto anything_else;
	}

	// -> a start tag whose tag name is "head"
	// -> any other end tag
	if ((token.is_start_tag() && token.tag_name() == "head") || token.is_end_tag())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

anything_else:
	// -> anything else
	// pop the current node (which will be the head element) off the stack of open elements
	stack_of_open_elements.pop_back();

	// switch the insertion mode to "after head"
	insertion_mode = InsertionMode::AfterHead;

	// reprocess the token
	process_using_the_rules_for(insertion_mode, token);
}

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
// 13.2.6.4.5 The "in head noscript" insertion mode
void Parser::handle_in_head_noscript(Token &token) { }

// https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
// 13.2.6.4.6 The "after head" insertion mode
void Parser::handle_after_head(Token &token)
{
	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// insert the character
		insert_character(token);
		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		insert_comment(token);
		return;
	}

	// -> a DOCTYPE token
	if (token.is_doctype())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

	// -> a start tag whose tag name is "body"
	if (token.is_start_tag() && token.tag_name() == "body")
	{
		// insert an HTML element for the token
		auto body_element = create_element(document(), "body");
		insert_element(body_element);

		// TODO
		// set the frameset-ok flag to "not ok"

		// switch the insertion mode to "in body"
		insertion_mode = InsertionMode::InBody;
		return;
	}

	// -> an end tag whose tag name is one of: "body", "html", "br"
	if (token.is_end_tag() && is_one_of(token.tag_name(), {"body", "html", "br"}))
	{
		// act as described in the "anything else" entry below
		goto anything_else;
	}

	// -> a start tag whose tag name is "head"
	// -> any other end tag
	if ((token.is_start_tag() && token.tag_name() == "head") || token.is_end_tag())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

anything_else:
	// -> anything else
	// insert an HTML element for a "body" start tag token with no attributes
	auto body_element = create_element(document(), "body");
	insert_element(body_element);

	// switch the insertion mode to "in body"
	insertion_mode = InsertionMode::InBody;

	// reprocess the current token
	process_using_the_rules_for(insertion_mode, token);
}

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
// 13.2.6.4.7 The "in body" insertion mode
void Parser::handle_in_body(Token &token)
{
	// -> a character token that is U+0000 NULL
	// TODO

	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// TODO
		// reconstruct the active formatting elements, if any

		// insert the character
		insert_character(token);
		return;
	}

	// -> any other character token
	if (token.is_character())
	{
		// TODO
		// reconstruct the active formatting elements, if any

		// insert the character
		insert_character(token);

		// TODO
		// set the frameset-ok flag to "not ok"

		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		insert_comment(token);
		return;
	}

	// -> a DOCTYPE token
	if (token.is_doctype())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

	// -> a start tag whose tag name is "html"
	if (token.is_start_tag() && token.tag_name() == "html")
	{
		// parse error
		parse_error();

		// TODO
		// if there is a template element on the stack of open elements, then ignore the token
		// otherwise, for each attribute on the token, check to see if the attribute is already present on the top element of the stack of open elements. If it is not, add the attribute and its corresponding value to that element
		return;
	}

	// -> a start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
	// -> an end tag whose tag name is "template"
	if ((token.is_start_tag() &&
	     is_one_of(
	         token.tag_name(),
	         {"base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"})) ||
	    (token.is_end_tag() && token.tag_name() == "template"))
	{
		// process the token using the rules for the "in head" insertion mode
		process_using_the_rules_for(InsertionMode::InHead, token);
		return;
	}

	// -> a start tag whose tag name is "body"
	if (token.is_start_tag() && token.tag_name() == "body")
	{
		// parse error
		parse_error();

		// TODO
		// if the stack of open elements has only one node on it, if the second element on the stack of open elements is not a body element, or if there is a template element on the stack of open elements, then ignore the token. (fragment case or there is a template element on the stack)
		// otherwise, set the frameset - ok flag to "not ok"; then, for each attribute on the token, check to see if the attribute is already present on the body element (the second element) on the stack of open elements, and if it is not, add the attribute and its corresponding value to that element
		return;
	}

	// -> an end-of-file token
	if (token.is_eof())
	{
		// TODO
		// if the stack of template insertion modes is not empty, then process the token using the rules for the "in template" insertion mode
		// otherwise, follow these steps:
		// if there is a node in the stack of open elements that is not either a dd element, a dt element, an li element, an optgroup element, an option element, a p element, an rb element, an rp element, an rt element, an rtc element, a tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html element, then this is a parse error
		// stop parsing
		return;
	}

	// -> an end tag whose tag name is "body"
	if (token.is_end_tag() && token.tag_name() == "body")
	{
		// TODO
		// if the stack of open elements does not have a body element in scope, this is a parse error; ignore the token
		// otherwise, if there is a node in the stack of open elements that is not either a dd element, a dt element, an li element, an optgroup element, an option element, a p element, an rb element, an rp element, an rt element, an rtc element, a tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html element, then this is a parse error
		// switch the insertion mode to "after body"
		insertion_mode = InsertionMode::AfterBody;
		return;
	}

	// -> any other start tag
	if (token.is_start_tag())
	{
		// TODO
		// reconstruct the active formatting elements, if any

		// insert an HTML element for the token
		auto tag_data = token.as_tag_data();
		auto element = create_element(document(), tag_data.name);
		for (auto attribute : tag_data.attributes)
			element->add_attribute(attribute.first, attribute.second);

		insert_element(element);

		if (tag_data.self_closing)
			stack_of_open_elements.pop_back();

		return;
	}

	// -> any other end tag
	if (token.is_end_tag())
	{
		stack_of_open_elements.pop_back();
		return;
	}
}

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incdata
// 13.2.6.4.8 The "text" insertion mode
void Parser::handle_text(Token &token)
{
	// -> a character token
	if (token.is_character())
	{
		// insert the character
		insert_character(token);
		return;
	}

	// -> an end tag whose tag name is "script"
	if (token.is_end_tag() && token.tag_name() == "script")
	{
		// If the active speculative HTML parser is null and the JavaScript execution context stack is empty, then perform a microtask checkpoint
		// Let script be the current node (which will be a script element)
		auto script = current_node();

		// Pop the current node off the stack of open elements
		stack_of_open_elements.pop_back();

		// Switch the insertion mode to the original insertion mode
		insertion_mode = original_insertion_mode;

		// TODO
		// Let the old insertion point have the same value as the current insertion point. Let the insertion point be just before the next input character
		// Increment the parser's script nesting level by one
		// If the active speculative HTML parser is null, then prepare the script element script. This might cause some script to execute, which might cause new characters to be inserted into the tokenizer, and might cause the tokenizer to output more tokens, resulting in a reentrant invocation of the parser
		// Decrement the parser's script nesting level by one. If the parser's script nesting level is zero, then set the parser pause flag to false
		// Let the insertion point have the value of the old insertion point. (In other words, restore the insertion point to its previous value. This value might be the "undefined" value.)
		// At this stage, if the pending parsing-blocking script is not null, then:
		// If the script nesting level is not zero:
		// Set the parser pause flag to true, and abort the processing of any nested invocations of the tokenizer, yielding control back to the caller. (Tokenization will resume when the caller returns to the "outer" tree construction stage.)
		// The tree construction stage of this particular parser is being called reentrantly, say from a call to document.write()
		// Otherwise:
		// While the pending parsing-blocking script is not null:
		// Let the script be the pending parsing-blocking script
		// Set the pending parsing-blocking script to null
		// Start the speculative HTML parser for this instance of the HTML parser
		// Block the tokenizer for this instance of the HTML parser, such that the event loop will not run tasks that invoke the tokenizer
		// If the parser's Document has a style sheet that is blocking scripts or the script's ready to be parser-executed is false: spin the event loop until the parser's Document has no style sheet that is blocking scripts and the script's ready to be parser-executed becomes true
		// If this parser has been aborted in the meantime, return
		// This could happen if, e.g., while the spin the event loop algorithm is running, the Document gets destroyed, or the document.open() method gets invoked on the Document
		// Stop the speculative HTML parser for this instance of the HTML parser
		// Unblock the tokenizer for this instance of the HTML parser, such that tasks that invoke the tokenizer can again be run
		// Let the insertion point be just before the next input character
		// Increment the parser's script nesting level by one (it should be zero before this step, so this sets it to one)
		// Execute the script element the script
		// Decrement the parser's script nesting level by one. If the parser's script nesting level is zero (which it always should be at this point), then set the parser pause flag to false
		// Let the insertion point be undefined again

		// Execute the script
		document().execute_script_node(script);
		return;
	}

	// -> any other end tag
	if (token.is_end_tag())
	{
		// pop the current node off the stack of open elements
		stack_of_open_elements.pop_back();

		// switch the insertion mode to the original insertion mode
		insertion_mode = original_insertion_mode;
		return;
	}
}

void Parser::handle_in_table(Token &token) { }

void Parser::handle_in_table_body(Token &token) { }

void Parser::handle_in_row(Token &token) { }

void Parser::handle_in_cell(Token &token) { }

void Parser::handle_in_table_text(Token &token) { }

void Parser::handle_in_select_in_table(Token &token) { }

void Parser::handle_in_select(Token &token) { }

void Parser::handle_in_caption(Token &token) { }

void Parser::handle_in_column_group(Token &token) { }

void Parser::handle_in_template(Token &token) { }

// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-afterbody
// 13.2.6.4.19 The "after body" insertion mode
void Parser::handle_after_body(Token &token)
{
	// -> a character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
	if (token.is_character() && token.is_parser_whitespace())
	{
		// process the token using the rules for the "in body" insertion mode
		process_using_the_rules_for(InsertionMode::InBody, token);
		return;
	}

	// -> a comment token
	if (token.is_comment())
	{
		// insert a comment as the last child of the first element in the stack of open elements (the html element)
		insert_comment(token);
		return;
	}

	// -> a DOCTYPE token
	if (token.is_doctype())
	{
		// parse error. Ignore the token
		parse_error();
		return;
	}

	// -> a start tag whose tag name is "html"
	if (token.is_start_tag() && token.tag_name() == "html")
	{
		// process the token using the rules for the "in body" insertion mode
		process_using_the_rules_for(InsertionMode::InBody, token);
		return;
	}

	// -> an end tag whose tag name is "html"
	if (token.is_end_tag() && token.tag_name() == "html")
	{
		// TODO
		// if the parser was created as part of the HTML fragment parsing algorithm, this is a parse error; ignore the token. (fragment case)
		// otherwise, switch the insertion mode to "after after body"
		insertion_mode = InsertionMode::AfterAfterBody;
		return;
	}

	// -> an end-of-file token
	if (token.is_eof())
	{
		// stop parsing
		stop_parsing();
		return;
	}

	// -> anything else
	// parse error. Switch the insertion mode to "in body" and reprocess the token
	parse_error();
	insertion_mode = InsertionMode::InBody;
	process_using_the_rules_for(insertion_mode, token);
}

void Parser::handle_in_frameset(Token &token) { }

void Parser::handle_after_frameset(Token &token) { }

void Parser::handle_after_after_body(Token &token) { }

void Parser::handle_after_after_frameset(Token &token) { }

std::shared_ptr<Node> Parser::current_node()
{
	return stack_of_open_elements.back();
}

std::shared_ptr<Node> Parser::adjusted_current_node()
{
	return current_node();
}

/**
 * the appropriate place for inserting a node, optionally using a particular override target,
 * is the position in an element returned by running the following steps:
 * 
 * @ref https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
 */
std::shared_ptr<Node> Parser::insert_location()
{
	// 1. if there was an override target specified, then let target be the override target
	// otherwise, let target be the current node
	auto target = current_node();

	// 2. tetermine the adjusted insertion location using the first matching steps from the following list:
	// if foster parenting is enabled and target is a table, tbody, tfoot, thead, or tr element
	// ...

	// otherwise, let adjusted insertion location be inside target, after its last child
	auto adjusted_insertion_location = target;

	// 3. if the adjusted insertion location is inside a template element,
	// let it instead be inside the template element's template contents, after its last child (if any)

	// 4. return the adjusted insertion location.
	return adjusted_insertion_location;
}

/**
 * when the steps below require the user agent to insert an HTML element for a token,
 * the user agent must insert a foreign element for the token, in the HTML namespace.
 * 
 * @ref https://html.spec.whatwg.org/multipage/parsing.html#insert-an-html-element
 * @ref https://html.spec.whatwg.org/multipage/parsing.html#insert-a-foreign-element
 */
void Parser::insert_element(std::shared_ptr<Node> element)
{
	// 1. find the appropriate place to insert the element
	auto target = insert_location();

	// 2. let element be the result of creating an element for the token in the given namespace,
	// with the intended parent being the element in which the adjusted insertion location finds itself
	// https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token

	// 3. if it is possible to insert element at the adjusted insertion location, then:
	// a. if the parser was not created as part of the HTML fragment parsing algorithm,
	// then push a new element queue onto element's relevant agent's custom element reactions stack
	// b. insert element at the adjusted insertion location
	target->add_child(element);
	// c. if the parser was not created as part of the HTML fragment parsing algorithm,
	// then pop the element queue from element's relevant agent's custom element reactions stack, and invoke custom element reactions in that queue

	// 4. push element onto the stack of open elements so that it is the new current node
	stack_of_open_elements.push_back(element);
}

/**
 * when the steps below require the user agent to insert a character while
 * processing a token, the user agent must run the following steps:
 * 
 * @ref https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
 */
void Parser::insert_character(Token t)
{
	// 1. let data be the characters passed to the algorithm, or,
	// if no characters were explicitly specified, the character of the character token being processed.
	auto data = t.get_char();

	// 2. let the adjusted insertion location be the appropriate place for inserting a node.
	auto target = insert_location();

	// 3. if the adjusted insertion location is in a Document node, then return.

	// 4. if there is a Text node immediately before the adjusted insertion location, then append data to that Text node's data.
	auto previous_node = target->last_child();
	if (previous_node && previous_node->type() == NodeType::Text)
	{
		auto *text_element = dynamic_cast<Text *>(previous_node);
		text_element->append(data);
	}

	// otherwise, create a new Text node whose data is data and whose node document is the same as that of the element
	// in which the adjusted insertion location finds itself, and insert the newly created node at the adjusted insertion location.
	else
	{
		auto text_element = std::make_shared<Text>(data);
		target->add_child(text_element);
	}
}

/**
* When the steps below require the user agent to insert a comment while processing a comment token, optionally with an explicitly insertion position position,
* the user agent must run the following steps:
*
* @ref https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
* TODO: implement
*/
void Parser::insert_comment(Token &token) { }

/**
 * parsing elements that contain only text
 * 
 * @ref https://html.spec.whatwg.org/multipage/parsing.html#generic-raw-text-element-parsing-algorithm
*/
void Parser::parse_raw_text(Token t)
{
	// 1. insert an html element for the token
	auto element = create_element(document(), t.tag_name());
	insert_element(element);

	// 2. if the algorithm that was invoked is the generic raw text element parsing algorithm, switch the tokenizer to the RAWTEXT state;
	// otherwise the algorithm invoked was the generic RCDATA element parsing algorithm, switch the tokenizer to the RCDATA state
	tokenizer.set_state(Tokenizer::State::RAWTEXT);

	// 3. let the original insertion mode be the current insertion mode
	original_insertion_mode = insertion_mode;

	// 4. Then, switch the insertion mode to "text"
	insertion_mode = InsertionMode::Text;
}

void Parser::parse_error()
{
	fmt::print(stderr, "HTML Parse Error!\n");
}
}
