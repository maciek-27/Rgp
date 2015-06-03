#ifndef __TEXTBOX_H__
#define __TEXTBOX_H__
#include <rexio/tk/window.h++>
#include <string>
#include <vector>

namespace MUD { namespace Aux {
		class TextBox : public Scr::Tk::Window
{

	// each string is single word (set of non-whitespace characters)
	// or '\n' denoting new line. text is formatted automatically
	std::vector<std::wstring> text;
public:
	TextBox(Scr::Uint _height,	Scr::Uint _width,
				  const Scr::DisplayStyle& _style):
		Window(_height, _width,_style)
		{

		}
	virtual void OnRedraw(Scr::Screen& screen)throw();
	void ClearText()
		{
			text.clear();
		}
	void AppendText(const std::wstring & text);
	~TextBox()throw() {}
};}}

#endif
