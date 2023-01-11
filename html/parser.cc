// parses an HTML document into a DOM tree according to
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

#include "parser.h"

#include <iostream>

#include "document/element.h"
#include "document/element_factory.h"
#include "document/node.h"
#include "document/text.h"
#include "token.h"

namespace html
{
static inline bool is_whitespace(char c)
{
	return c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == ' ';
}

Parser::Parser(Document &document) :
    m_document(document)
{ }

Document &Parser::parse(std::string const &input)
{
	tokenizer = Tokenizer(input);
	while (auto token = tokenizer.next())
	{
		// std::cout << token.to_string() << "\n";

	reprocess_token:
		switch (insertion_mode)
		{
			// 13.2.6.4.1 The "initial" insertion mode
			case InsertionMode::Initial:
				switch (token.type())
				{
					case Character:
						// ignore the character
						break;

					case Comment: break;

					case Doctype: break;

					default: insertion_mode = InsertionMode::BeforeHtml; goto reprocess_token;
				}
				break;

			// 13.2.6.4.2 The "before html" insertion mode
			case InsertionMode::BeforeHtml:
				switch (token.type())
				{
					case Doctype:
						// ignore the token
						break;

					case Comment: break;

					case Character:
					{
						auto c = token.get_char();
						// ignore the whitespace
						if (is_whitespace(c))
							break;

						goto before_html_anything_else;
						break;
					}

					case StartTag:
					{
						auto tag_name = token.tag_name();
						if (tag_name == "html")
						{
							auto html_element = std::make_shared<Element>("html");
							document().add_child(html_element);
							stack_of_open_elements.push_back(html_element);
							insertion_mode = InsertionMode::BeforeHead;
							break;
						}

						goto before_html_anything_else;
						break;
					}

					case EndTag:
					{
						auto name = token.tag_name();
						if (name == "head" || name == "body" || name == "html" || name == "br")
						{
							goto before_html_anything_else;
						}

						else
						{
							// ignore the token
							break;
						}
					}

					before_html_anything_else:
					default:
					{
						auto html_element = std::make_shared<Element>("html");
						document().add_child(html_element);
						stack_of_open_elements.push_back(html_element);
						insertion_mode = InsertionMode::BeforeHead;
						goto reprocess_token;
					}
				}
				break;

			// 13.2.6.4.3 The "before head" insertion mode
			case InsertionMode::BeforeHead:
				switch (token.type())
				{
					case Character:
					{
						auto c = token.get_char();
						if (is_whitespace(c))
						{
							// ignore the token
							break;
						}

						else
						{
							goto before_head_anything_else;
						}
						break;
					}

					case Comment: break;

					case Doctype:
						// ignore the token
						break;

					case StartTag:
					{
						auto tag_name = token.tag_name();
						if (tag_name == "html")
						{
							// process the token using the rules for the "in body" insertion mode
						}

						else if (tag_name == "head")
						{
							auto head_element = std::make_shared<Element>("head");
							insert_element(head_element);
							// TODO: set the head element pointer to the newly created head element
							insertion_mode = InsertionMode::InHead;
							break;
						}

						goto before_head_anything_else;
						break;
					}

					case EndTag:
					{
						auto name = token.tag_name();
						if (name == "head" || name == "body" || name == "html" || name == "br")
						{
							goto before_head_anything_else;
						}

						else
						{
							// ignore the token;
							break;
						}
						break;
					}

					before_head_anything_else:
					default:
						auto head_element = std::make_shared<Element>("head");
						insert_element(head_element);
						// TODO: set the head element pointer to the newly created head element
						insertion_mode = InsertionMode::InHead;
						goto reprocess_token;
				}
				break;

			// 13.2.6.4.4 The "in head" insertion mode
			case InsertionMode::InHead:
				switch (token.type())
				{
					case Character:
					{
						auto c = token.get_char();
						if (is_whitespace(c))
						{
							insert_character(token);
						}

						else
						{
							goto in_head_anything_else;
						}
						break;
					}

					case Comment:
						// insert_comment(token);
						break;

					case StartTag:
					{
						if (token.tag_name() == "noframes" || token.tag_name() == "style" ||
						    token.tag_name() == "title")
							parse_raw_text(token);

						else
							goto in_head_anything_else;
						break;
					}

					case EndTag:
					{
						if (token.tag_name() == "head")
						{
							stack_of_open_elements.pop_back();
							insertion_mode = InsertionMode::AfterHead;
						}

						else
							goto in_head_anything_else;

						break;
					}

					in_head_anything_else:
					default:
						// pop head element off stack of open elements
						stack_of_open_elements.pop_back();
						insertion_mode = InsertionMode::AfterHead;
						goto reprocess_token;
				}
				break;

			// 13.2.6.4.5 The "in head noscript" insertion mode
			case InsertionMode::InHeadNoScript: break;

			// 13.2.6.4.6 The "after head" insertion mode
			case InsertionMode::AfterHead:
				switch (token.type())
				{
					case Character:
					{
						auto c = token.get_char();
						if (is_whitespace(c))
						{
							insert_character(token);
						}

						else
							goto after_head_anything_else;
						break;
					}
					case StartTag:
					{
						auto tag_name = token.tag_name();
						if (tag_name == "body")
						{
							auto body_element = std::make_shared<Element>("body");
							insert_element(body_element);
							insertion_mode = InsertionMode::InBody;
							break;
						}

						break;
					}

					after_head_anything_else:
					default:
					{
						// create body start tag
						auto body_element = std::make_shared<Element>("body");
						insert_element(body_element);
						insertion_mode = InsertionMode::InBody;
					}
				}
				break;

			// 13.2.6.4.7 The "in body" insertion mode
			case InsertionMode::InBody:
				switch (token.type())
				{
					case Character: insert_character(token); break;

					case Comment: break;

					case Doctype: break;

					case StartTag:
					{
						auto tag_data = token.as_tag_data();
						auto element = create_element(tag_data.name);
						for (auto attribute : tag_data.attributes)
							element->add_attribute(attribute.first, attribute.second);

						insert_element(element);

						if (tag_data.self_closing)
							stack_of_open_elements.pop_back();

						else if (token.tag_name() == "area" || token.tag_name() == "br" ||
						         token.tag_name() == "embed" || token.tag_name() == "img" ||
						         token.tag_name() == "keygen" || token.tag_name() == "wbr")
						{
							stack_of_open_elements.pop_back();
						}
						break;
					}

					case EndTag:
					{
						auto tag_name = token.tag_name();
						if (tag_name == "body")
						{
							insertion_mode = InsertionMode::AfterBody;
							break;
						}

						else
						{
							stack_of_open_elements.pop_back();
						}
						break;
					}

					in_body_anything_else:
					default: insertion_mode = InsertionMode::BeforeHtml; goto reprocess_token;
				}
				break;

			// 13.2.6.4.8 The "text" insertion mode
			case InsertionMode::Text:
				switch (token.type())
				{
					case Character: insert_character(token); break;

					// TODO
					case Eof: break;

					case EndTag:
					{
						// TODO
						if (token.tag_name() == "script")
						{ }

						else
						{
							stack_of_open_elements.pop_back();
							insertion_mode = original_insertion_mode;
						}
						break;
					}
				}
				break;

			// 13.2.6.4.9 The "in table" insertion mode
			case InsertionMode::InTable: break;

			// 13.2.6.4.10 The "in table text" insertion mode
			case InsertionMode::InTableText: break;

			// 13.2.6.4.11 The "in caption" insertion mode
			case InsertionMode::InCaption: break;

			// 13.2.6.4.12 The "in column group" insertion mode
			case InsertionMode::InColumnBody: break;

			// 13.2.6.4.13 The "in table body" insertion mode
			case InsertionMode::InTableBody: break;

			// 13.2.6.4.14 The "in row" insertion mode
			case InsertionMode::InRow: break;

			// 13.2.6.4.15 The "in cell" insertion mode
			case InsertionMode::InCell: break;

			// 13.2.6.4.16 The "in select" insertion mode
			case InsertionMode::InSelect: break;

			// 13.2.6.4.17 The "in select in table" insertion mode
			case InsertionMode::InSelectInTable: break;

			// 13.2.6.4.18 The "in template" insertion mode
			case InsertionMode::InTemplate: break;

			// 13.2.6.4.19 The "after body" insertion mode
			case InsertionMode::AfterBody:
				switch (token.type())
				{
					case EndTag:
					{
						auto tag_name = token.tag_name();
						if (tag_name == "html")
						{
							insertion_mode = InsertionMode::AfterAfterBody;
							break;
						}

						else
						{
							goto after_body_anything_else;
						}
						break;
					}

					case Eof: return m_document;

					    after_body_anything_else:
					default:;
				}
				break;

			// 13.2.6.4.20 The "in frameset" insertion mode
			case InsertionMode::InFrameset: break;

			// 13.2.6.4.21 The "after frameset" insertion mode
			case InsertionMode::AfterFrameset: break;

			// 13.2.6.4.22 The "after after body" insertion mode
			case InsertionMode::AfterAfterBody: break;

			// 13.2.6.4.23 The "after after frameset" insertion mode
			case InsertionMode::AfterAfterFrameset: break;
		}
	}

	return m_document;
}

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
 * parsing elements that contain only text
 * 
 * @ref https://html.spec.whatwg.org/multipage/parsing.html#generic-raw-text-element-parsing-algorithm
*/
void Parser::parse_raw_text(Token t)
{
	// 1. insert an html element for the token
	auto element = std::make_shared<Element>(t.tag_name());
	insert_element(element);

	// 2. if the algorithm that was invoked is the generic raw text element parsing algorithm, switch the tokenizer to the RAWTEXT state;
	// otherwise the algorithm invoked was the generic RCDATA element parsing algorithm, switch the tokenizer to the RCDATA state
	tokenizer.set_state(Tokenizer::State::RAWTEXT);

	// 3. let the original insertion mode be the current insertion mode
	original_insertion_mode = insertion_mode;

	// 4. Then, switch the insertion mode to "text"
	insertion_mode = InsertionMode::Text;
}
}
