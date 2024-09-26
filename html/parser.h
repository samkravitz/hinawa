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
	MODE(InColumnGroup)   \
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
	Parser() = default;

	static void parse(const std::string &, Document &);

	enum class InsertionMode
	{
#define MODE(mode) mode,
		INSERTION_MODES
#undef MODE
	};

	Document &document() { return *m_document; }

private:
	Parser(Document &);

	void run(const std::string &input);
	void process_using_the_rules_for(InsertionMode, Token &);

	void handle_initial(Token &);
	void handle_before_html(Token &);
	void handle_before_head(Token &);
	void handle_in_head(Token &);
	void handle_in_head_noscript(Token &);
	void handle_after_head(Token &);
	void handle_in_body(Token &);
	void handle_after_body(Token &);
	void handle_after_after_body(Token &);
	void handle_text(Token &);
	void handle_in_table(Token &);
	void handle_in_table_body(Token &);
	void handle_in_row(Token &);
	void handle_in_cell(Token &);
	void handle_in_table_text(Token &);
	void handle_in_select_in_table(Token &);
	void handle_in_select(Token &);
	void handle_in_caption(Token &);
	void handle_in_column_group(Token &);
	void handle_in_template(Token &);
	void handle_in_frameset(Token &);
	void handle_after_frameset(Token &);
	void handle_after_after_frameset(Token &);

	InsertionMode insertion_mode = InsertionMode::Initial;
	InsertionMode original_insertion_mode = InsertionMode::Initial;
	std::vector<std::shared_ptr<Node>> stack_of_open_elements;
	Tokenizer tokenizer;
	Document *m_document = nullptr;

	std::shared_ptr<Node> current_node();
	std::shared_ptr<Node> adjusted_current_node();
	void insert_element(std::shared_ptr<Node>);
	void insert_character(Token);
	void insert_comment(Token &);
	std::shared_ptr<Node> insert_location();
	void parse_raw_text(Token);
	void parse_error();
	void stop_parsing() {};
};
}
