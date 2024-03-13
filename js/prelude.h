#pragma once

class Document;

namespace js
{
class Vm;

#ifdef JS_BUILD_BINDINGS
void prelude(Vm &, Document *document = nullptr);
#else
void prelude(Vm &);
#endif
}