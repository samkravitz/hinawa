#pragma once

class Document;

namespace js
{
class Vm;

void prelude(Vm &, Document *document = nullptr);
}