#include "framedwindow.h++"

using namespace Scr;
using namespace Scr::Tk;

FramedWindow::FramedWindow(Uint _height, Uint _width,
						   const DisplayStyle& _style,
						   const FrameStyle& _frameStyle)throw()
	: FramedWindowBase<Window>(_height, _width, _style,
							   _frameStyle)
{
	;
}
