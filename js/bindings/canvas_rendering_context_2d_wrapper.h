#include "wrapper.h"

class CanvasRenderingContext2D;

namespace js
{
namespace bindings
{
class CanvasRenderingContext2DWrapper : public Wrapper
{
public:
	CanvasRenderingContext2DWrapper(CanvasRenderingContext2D *);

	CanvasRenderingContext2D &context() const { return *m_context; }
private:
	CanvasRenderingContext2D *m_context = nullptr;
};
}
}
