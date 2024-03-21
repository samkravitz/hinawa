#pragma once

#include "document/document.h"
#include "tokenizer.h"

#include <memory>
#include <vector>

namespace html
{
#define INSERTION_MODES   \
	MODE(Initial)         \
	MODE(BeforeHtml)      \
	MODE(BeforeHead)      \
	MODE(InHead)          \
	MODE(InHeadNoScript)  \
	MODE(AfterHead)       \
	MODE(InBody)          \
	MODE(Text)            \
	MODE(InTable)         \
	MODE(InTableText)     \
	MODE(InCaption)       \
	MODE(InColumnBody)    \
	MODE(InTableBody)     \
	MODE(InRow)           \
	MODE(InCell)          \
	MODE(InSelect)        \
	MODE(InSelectInTable) \
	MODE(InTemplate)      \
	MODE(AfterBody)       \
	MODE(InFrameset)      \
	MODE(AfterFrameset)   \
	MODE(AfterAfterBody)  \
	MODE(AfterAfterFrameset)

class Parser
{
public:
	Parser() = delete;

	static Document parse(const std::string &, Url base_url = {});

	enum class InsertionMode
	{
#define MODE(mode) mode,
		INSERTION_MODES
#undef MODE
	};

	Document &document() { return m_document; }

private:
	Parser(Document &);

	Document run(const std::string &input);
	InsertionMode insertion_mode = InsertionMode::Initial;
	InsertionMode original_insertion_mode = InsertionMode::Initial;
	std::vector<std::shared_ptr<Node>> stack_of_open_elements;
	Tokenizer tokenizer;
	Document m_document;

	std::shared_ptr<Node> current_node();
	std::shared_ptr<Node> adjusted_current_node();
	void insert_element(std::shared_ptr<Node>);
	void insert_character(Token);
	std::shared_ptr<Node> insert_location();
	void parse_raw_text(Token);
};
}
