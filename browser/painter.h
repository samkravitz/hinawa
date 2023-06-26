#pragma once

class SkCanvas;

namespace browser
{
class Painter
{
public:
	Painter(SkCanvas*);

private:
	SkCanvas *canvas;
};
}