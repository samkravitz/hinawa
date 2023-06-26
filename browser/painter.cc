#include "painter.h"

#include "SkCanvas.h"

namespace browser
{
Painter::Painter(SkCanvas* canvas) :
    canvas(canvas)
{ }
}