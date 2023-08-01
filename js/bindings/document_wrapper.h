#include "wrapper.h"

#include <memory>

#include "document/document.h"

namespace js
{
namespace bindings
{
class DocumentWrapper : public Wrapper
{
public:
	DocumentWrapper(Document *);

	Document &document() const { return *m_document; }
private:
	Document *m_document;
};
}
}
