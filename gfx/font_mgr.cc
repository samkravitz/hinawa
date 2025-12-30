#include "font_mgr.h"
#include "ports/SkFontMgr_mac_ct.h"

namespace gfx
{
sk_sp<SkFontMgr> CreateFontMgr()
{
	return SkFontMgr_New_CoreText(nullptr);
}
}
