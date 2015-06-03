#include "inputbox.h++"
#include "throw.h++"

using namespace Scr;
using namespace Scr::Tk;

#define fillend() \
if(textOffset + curChars  < text.length()) { \
	Uint w = GlyphWidth::Get(text[textOffset + curChars]); \
	while(curCols + w <= GetWidth() &&					   \
		  textOffset + curChars < text.length()) { \
		curCols += w; \
		curChars++; \
		w = GlyphWidth::Get(text[textOffset + curChars]); \
	} \
}

#define incoffset() { \
Uint w = GlyphWidth::Get(text[textOffset]); \
curCols -= w; \
cursorPos -= w; \
curChars--; \
charPos--; \
textOffset++; \
}

Inputbox::Inputbox(Uint _width, const std::wstring& _text,
				   const DisplayStyle& _style,
				   const DisplayStyle& _activeStyle,
				   const InputboxStyle& _inputboxStyle)
	throw() :
	ActiveWidget(1, _width, _style, _activeStyle),  
	cursorPos(0), charPos(0), curCols(0), curChars(0), textOffset(0),
	inputboxStyle(_inputboxStyle),maxLength(0)
{
	SetText(_text);
	SetMinHeight(1);
	SetMaxHeight(1);
	SetMinWidth(3); // at least one char and a cursor
}

Inputbox::Inputbox(const std::wstring& _text,
				   const DisplayStyle& _style,
				   const DisplayStyle& _activeStyle,
				   const InputboxStyle& _inputboxStyle
	)throw() :
	ActiveWidget(_style, _activeStyle),
	cursorPos(0), charPos(0), curCols(0), curChars(0), textOffset(0),
	inputboxStyle(_inputboxStyle),maxLength(0)
{
	SetText(_text);
	SetMaxHeight(1);
	SetMinHeight(1);
	SetMinWidth(3); // at least one char and a cursor
}

void Inputbox::OnRedraw(Screen& screen)throw()
{
	screen << (active?activeStyle:style) << Control::Clear
			<< Control::GotoYX (0, 0);
	
	if(active)
	{
		if(inputboxStyle.cursorStyle.GetFgColor () == Fg::System &&
				inputboxStyle.cursorStyle.GetBgColor () == Bg::System)
		{
			
			screen.ShowCursor ();
			
			screen.ForceCursorPosition (Position (0, cursorPos));
			
			std::wstring temp = text.substr (textOffset, curChars);
			screen << temp;
			
		}
		else
		{ // software cursor
			std::wstring temp;
			if(charPos)
			{
				temp = text.substr (textOffset, charPos);
				screen << temp;
			}
			
			screen << inputboxStyle.cursorStyle;
			screen << Control::GotoYX (0, cursorPos); // should it be needed?
			if(textOffset + charPos < text.length ())
			{
				screen << text[textOffset + charPos];
				screen <<
				Control::GotoYX (0, cursorPos +
								 GlyphWidth::Get(text[textOffset + charPos]));
				// the above width calculatation shouldn't be needed!!
			}
			else
				screen << ' ';
			screen << activeStyle;
			
			if(curChars > charPos + 1)
			{
				temp = text.substr (textOffset + charPos+1, curChars - (charPos + 1));
				screen << temp;
			}
		}
	}
	else
	{
		std::wstring temp = text.substr (textOffset, curChars);
		screen << temp;
	}
}

void Inputbox::SetOffset(Uint _textOffset)throw(OffsetOutOfRange)
{
	if(_textOffset >= text.length())
		THROW(OffsetOutOfRange);

	textOffset = _textOffset;
	curChars = 0;
	curCols = 0;
	charPos = 0;
	cursorPos = 0;
	fillend();
}

Uint Inputbox::GetOffset()throw()
{
	return textOffset;
}

void Inputbox::SetText(const std::wstring& _text)throw()
{
	textOffset = 0;
	curChars = 0;
	curCols = 0;
	charPos = 0;
	cursorPos = 0;
	text = _text;
	fillend();
}

const std::wstring& Inputbox::GetText()throw()
{
	return text;
}

void Inputbox::SetMaxLength(Uint _maxLength)throw()
{
	maxLength = _maxLength;
}

Uint Inputbox::GetMaxLength()throw()
{
	return maxLength;
}

void Inputbox::OnKeyDown(Key key)throw()
{	
	Uint w;
	if(key.IsASpecialKey ())
	{
		switch(key.GetSpecialKey ())
		{
			case Key::Home:
				SetOffset (0);
				break;
			case Key::End:
				// if the input ending is on the sight
				if(textOffset + curChars <= text.length () &&
						curCols != GetWidth ())
				{
					charPos = curChars;
					cursorPos = curCols;
					break;
				}
				
				// otherwise put as much characters as can fit, counting
				// from the end
				{
					Uint totalw = 0, i = text.length ();
					curChars = 0;
					while(i--)
					{
						totalw += GlyphWidth::Get(text[i]);
						curChars++;
						if(totalw > GetWidth () - 1)
						{
							totalw -= GlyphWidth::Get(text[i]);
							curChars--;
							textOffset = i + 1;
							curCols = totalw;
							cursorPos = curCols;
							charPos = curChars;
							break;
						}
					}
				}
				break;
			case Key::Backspace:
				RexIOLog (LogLevelModerate) << "AA Backspace ";
				
				if(textOffset || charPos) // not at the beginning
				{
					Uint w = GlyphWidth::Get(text[textOffset + charPos - 1]);
					text.erase (textOffset + charPos - 1, 1);
					
					if(charPos)
					{
						cursorPos -= w;
						charPos--;
						curCols -= w;
						curChars--;
						
						fillend ()
					}
					else
						textOffset--;
				}
				break;
			case Key::Delete:
				if(textOffset + charPos != text.length ()) // not at the end
				{
					Uint w = GlyphWidth::Get(text[textOffset + charPos]);
					text.erase (textOffset + charPos, 1);
					
					curCols -= w;
					curChars--;
					
					if(charPos == curChars)
					{ // last character was deleted
						
						if(GlyphWidth::Get(text[textOffset + curChars]) > w)
						{
							// the deleted character was 1-width, but the
							// next in line is 2-width, make place for it
							incoffset ();
						}
					}
					fillend ();
				}
				break;
			case Key::Right:
				if(textOffset + charPos != text.length ())
				{// not at the end
					w = GlyphWidth::Get(text[textOffset + charPos]);
					
					cursorPos += w; // move cursor
					charPos++;
					
					if(charPos == curChars) // if cursor at the end
					{
						w = GlyphWidth::Get(text[textOffset + charPos]);
						
						curCols += w; // add char to the end
						curChars++;
						
						// trim from the beginning, until fits
						while(curCols >= GetWidth () )
						{
							incoffset ();
						}
						
						// possible place for one char left at the end
						fillend ();
					}
				}
				break;
			case Key::Left:
				if(charPos)
				{
					cursorPos -= GlyphWidth::Get(text[textOffset + charPos - 1]);
					charPos--;
				}
				else if(!charPos && textOffset)
				{ // cursor at the beginning
					// add to the front
					textOffset--;
					curChars++;
					curCols += GlyphWidth::Get(text[textOffset]);
					while(curCols > GetWidth () )
					{  // overflow
						// trim from the end, until ok
						curCols -= GlyphWidth::Get(text[textOffset + curChars - 1]);
						curChars--;
					}
				}
				break;
			default:
				break;
		}
		return;
	}
	if(maxLength && text.length () == maxLength) // limit reached?
		return;
	
	if (key<' ' or key == 0x7f) //   special ASCII (not a key);
		return;
	w = GlyphWidth::Get(key);
	
	text.insert (textOffset + charPos, 1, key);
	cursorPos += w;
	charPos++;
	curCols += w;
	curChars++;
	
	if(curChars == charPos)
	{ // cursor is at the end
		while(curCols > GetWidth () -1)
		{   // overflow
			// trim from the beginning, until ok. Also leave one space for
			// the cursor
			incoffset ();
		}
	}
	else
	{ // cursor somewhere in the middle, preserve textOffset
		// current highlighted char's width
		Uint curw = GlyphWidth::Get(text[textOffset + charPos]);
		
		// the cursor is almost at the end
		// (also considering current highlighted width)
		while(cursorPos >= GetWidth () + 1 - curw)
		{
			
			// results in the offset change, make place
			incoffset ();
		}
		
		while(curCols > GetWidth () )
		{  // overflow
			// trim from the end, until ok
			
			curCols -= GlyphWidth::Get(text[textOffset + curChars - 1]);
			curChars--;
		}
		
		// there is one possible additional place for a character at the end
		fillend ();
	}
}

Inputbox::~Inputbox()throw() {;}
