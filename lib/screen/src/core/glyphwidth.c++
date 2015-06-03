#include "commons.h++"

extern "C" {
#include "wcwidth.c"
}
using namespace Scr;

std::bitset<(1<<17)*2> Scr::GlyphWidth::glyphWidth;

// unsigned long Scr::width(wchar_t c)
// {
// 	return mk_wcwidth(c);
// }

GlyphWidth::GlyphWidth()
{
	for(Uint i = 0;i<glyphWidth.size();i+=2) {
		int w = mk_wcwidth((wchar_t)(i>>1));
		if(w == 1) // make width 1 characters require only one lookup
			glyphWidth[i] = true;
		else if(w == 2) {
			glyphWidth[i] = false;
			glyphWidth[i+1] = true;
		}
		else {
			glyphWidth[i] = false;
			glyphWidth[i+1] = false;
		}
	}
}


