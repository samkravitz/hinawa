#include "dom.h"
#include "element.h"
#include "node.h"
#include "text.h"

Dom::Dom(std::shared_ptr<Node> root) :
	root(root)
{

}

// creates a toy test document
Dom::Dom()
{
	auto root = std::make_shared<Element>("html");
	auto body = std::make_shared<Element>("body");
	auto text = std::make_shared<Text>("Hello World!");

	body->add_child(text);
	root->add_child(body);
	root = root;
}
