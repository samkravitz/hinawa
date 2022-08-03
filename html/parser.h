#pragma once

#include "tokenizer.h"
#include "../document/dom.h"
#include "../document/element.h"

#include <memory>
#include <vector>

namespace html
{
#define INSERTION_MODES \
	MODE(Initial) \
	MODE(BeforeHtml) \
	MODE(BeforeHead) \
	MODE(InHead) \
	MODE(InHeadNoScript) \
	MODE(AfterHead) \
	MODE(InBody) \
	MODE(Text) \
	MODE(InTable) \
	MODE(InTableText) \
	MODE(InCaption) \
	MODE(InColumnBody) \
	MODE(InTableBody) \
	MODE(InRow) \
	MODE(InCell) \
	MODE(InSelect) \
	MODE(InSelectInTable) \
	MODE(InTemplate) \
	MODE(AfterBody) \
	MODE(InFrameset) \
	MODE(AfterFrameset) \
	MODE(AfterAfterBody) \
	MODE(AfterAfterFrameset) \

class Parser
{
public:
	Parser(std::string const &input) :
		tokenizer(input)
	{ }

	enum class InsertionMode
	{
#define MODE(mode) mode,
		INSERTION_MODES
#undef MODE
	};

	std::shared_ptr<Node> parse();

private:
	InsertionMode insertion_mode = InsertionMode::Initial;
	std::vector<std::shared_ptr<Node>> open_elements;
	Tokenizer tokenizer;

	std::shared_ptr<Node> current_node();
	std::shared_ptr<Node> adjusted_current_node();
	void insert_element(std::shared_ptr<Node>);
	void insert_character(Token);
	std::shared_ptr<Node> insert_location();
};
}
