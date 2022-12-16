#include "document.h"
#include "element.h"
#include "node.h"
#include "text.h"

Document::Document(std::shared_ptr<Node> root) :
    m_root(root)
{ }
