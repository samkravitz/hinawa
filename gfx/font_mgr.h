#pragma once

#include "SkFontMgr.h"

namespace gfx
{
// Creates a platform-specific font manager
sk_sp<SkFontMgr> CreateFontMgr();
}
