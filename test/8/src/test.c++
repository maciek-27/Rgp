#include<rexio/tk/toolkit.h++>
#include<iostream>
#include <fstream>
#include <sstream>
#include <rexio/glyphwidth.h++>
using namespace std;
using namespace Scr;
using namespace Scr::Tk;

static const wchar_t pointerSymbol = L'✛';

static const wchar_t longPointerSymbol = L'ー';

const DisplayStyle _INACTIVE_FRAME_COLOR(DisplayStyle(Fg::Black, Fg::Bright, 
													  Bg::Black));
const FrameStyle FRAMEDWINDOW_INACTIVE_FRAMESTYLE(
	_INACTIVE_FRAME_COLOR);

const DisplayStyle _ACTIVE_FRAME_COLOR(DisplayStyle(Fg::White, Fg::Bright, 
													  Bg::Black));
const FrameStyle FRAMEDWINDOW_ACTIVE_FRAMESTYLE(
	_ACTIVE_FRAME_COLOR);

class ActiveFrame : public FramedWindow
{
public:
	ActiveFrame(Uint _height, Uint _width) throw() 
		:FramedWindow(_height, _width, FRAMEDWINDOW_DEFAULT_STYLE, 
			FRAMEDWINDOW_INACTIVE_FRAMESTYLE) {		
		;
	}
	void OnFocus(FocusPolicy focustype) throw()  {
		SetFrameStyle(FRAMEDWINDOW_ACTIVE_FRAMESTYLE);
		FramedWindow::OnFocus(focustype);
	}
	void OnUnFocus(FocusPolicy focustype) throw() {
		SetFrameStyle(FRAMEDWINDOW_INACTIVE_FRAMESTYLE);
		FramedWindow::OnUnFocus(focustype);
	}
};

static const Fg::Color colors[8] = {Fg::White, Fg::Black, Fg::Red, Fg::Green, Fg::Yellow,
									Fg::Blue, Fg::Magenta, Fg::Cyan};

class ScreenEditor : public Widget
{	
	Uint width, height;
//	Uint cursorPos;
	Position cursorPos;
	bool cursorVisible;

	Key lastPut;

   
	struct Atom {
		Atom(wchar_t _ch = ' ', DisplayStyle _style = WIDGET_DEFAULT_STYLE) : ch(_ch), style(_style) {;}
		wchar_t ch;
		DisplayStyle style;		
	};

	std::vector<std::vector<Atom> > data;

	

public:
	class ColourSelect : public ActiveFrame {
		DisplayStyle current;

		class NullWidget : public Widget {
		public:
			NullWidget() : Widget(1, 1) { ; }
			void OnFocus(FocusPolicy focustype)throw() { ; }
		} nullWidget;
		Uint currentFg;
		Uint currentBg;

		enum {FgSelected, BgSelected} selected;

		
	public:		
		ColourSelect(Uint _width) : ActiveFrame(4, _width), currentFg(0), 
									currentBg(1), selected(FgSelected) {
			AddWidget(nullWidget);
			
		}
		DisplayStyle GetColor() {
			return DisplayStyle(colors[currentFg % (sizeof(colors)/sizeof(colors[0]))], 
								(currentFg >= (sizeof(colors)/sizeof(colors[0])))?Fg::Bright:Fg::Dark, (Bg::Color)((int)colors[currentBg] + 10));
		}
/*		void PassFocusRequest(FocusPolicy focustype) throw()
			{
				GetParent().PassFocusRequest(focustype);
				}*/

		void OnKeyDown(Key key) throw() {
			ActiveFrame::OnKeyDown(key);
			if(key.IsASpecialKey()) {
				if(key == Key::Down || key == Key::Up)
					selected = (selected == FgSelected)?BgSelected:FgSelected;
				else if(key == Key::Right) {
					if(selected == FgSelected)
						currentFg = (currentFg+1) % (2*sizeof(colors)/sizeof(colors[0]));
					else
						currentBg = (currentBg+1) % (sizeof(colors)/sizeof(colors[0]));
				}
				else if(key == Key::Left) {
					if(selected == FgSelected)
						currentFg = (currentFg == 0)?(2*sizeof(colors)/sizeof(colors[0]) - 1):(currentFg-1);
					else
						currentBg = (currentBg == 0)?(sizeof(colors)/sizeof(colors[0]) - 1):(currentBg-1);
				}
			}
		}

/*		void OnFocus(FocusPolicy focustype)throw() {
			;
		}

		void OnUnFocus(FocusPolicy focustype)throw() {
			GetParent().PassFocusRequest(focustype);
			}*/

		void OnRedraw(Screen &screen) throw() {
			ActiveFrame::OnRedraw(screen);			
			
			screen << DisplayStyle(Fg::White,
								   selected == FgSelected?Fg::Bright:Fg::Dark,
								   Bg::Transparent);
			screen << Control::GotoYX(1, 1);			
			screen << "Foreground colour:    ";

			screen << Control::GotoYX(1, 24);
			
			for(Uint i = 0;i<sizeof(colors)/sizeof(colors[0]);++i) {
				if(i == currentFg) {
					if(colors[i] == Fg::Black)
						screen << DisplayStyle(Fg::White, Fg::Dark, Bg::Transparent) << "░░░";
					else
						screen << DisplayStyle(colors[i], Fg::Dark, Bg::Transparent) << "▒▒▒";
				}
				else
					screen << DisplayStyle(colors[i], Fg::Dark, Bg::Transparent) << "███";
			}
			for(Uint i = 0;i<sizeof(colors)/sizeof(colors[0]);++i) {
				if(i == currentFg-sizeof(colors)/sizeof(colors[0]))
					screen << DisplayStyle(colors[i], Fg::Bright, Bg::Transparent) << "▒▒▒";
				else
					screen << DisplayStyle(colors[i], Fg::Bright, Bg::Transparent) << "███";
			}
			screen << Control::GotoYX(2, 1); 

			screen << DisplayStyle(Fg::White,
								   selected == BgSelected?Fg::Bright:Fg::Dark,
								   Bg::Transparent);
			screen << "Background colour:    ";
			screen << Control::GotoYX(2, 24);

			for(Uint i = 0;i<sizeof(colors)/sizeof(colors[0]);++i) {
				if(i == currentBg) {
					if(colors[i] == Fg::Black)
						screen << DisplayStyle(Fg::White, Fg::Dark, Bg::Transparent) << "░░░";
					else
						screen << DisplayStyle(colors[i], Fg::Dark, Bg::Transparent) << "▒▒▒";
				}
				else
					screen << DisplayStyle(colors[i], Fg::Dark, Bg::Transparent) << "███";
			}

			screen << DisplayStyle(Fg::Transparent, Fg::Dark, Bg::Transparent);
			screen << "     Example: ";
			screen << current;
			screen << GetColor();
			screen << "$$$";
			screen << DisplayStyle(Fg::Transparent, Fg::Dark, Bg::Black);

		}
		friend class ScreenEditor;
	} colourSelector;


	ScreenEditor(Uint _height, Uint _width) : Widget(_height, _width),
											  width(_width), height(_height), cursorPos(0, 0),
											  data(height, std::vector<Atom>(width)),
											  colourSelector(_width)
		{
			Load(".editordump");
		}
	
	virtual void OnRedraw(Scr::Screen &screen) throw() {
		Widget::OnRedraw(screen);

		for(Uint i = 0;i<data.size();i++)
			for(Uint j = 0;j<data[i].size();j++) {
				screen << Control::GotoYX(i,j) << data[i][j].style << data[i][j].ch;
				if(Scr::width(data[i][j].ch) == 2) j++;
			}

		if(cursorVisible) {
			screen << Control::GotoYX(cursorPos.row, cursorPos.col);
			if(Scr::width(data[cursorPos.row][cursorPos.col].ch) == 2)				
				screen << longPointerSymbol;
			else
				screen << pointerSymbol;
		}
	}

	void OnFocus(FocusPolicy focustype)throw() {
		cursorVisible = true;
	}

	void OnUnFocus(FocusPolicy focustype)throw() {
		Widget::OnUnFocus(focustype);
		cursorVisible = false;
	}
	
	void PutCharacter(Key key) {
		if(Scr::width(key) == 2 && cursorPos.col == data[0].size()-1)
			return;
		bool waswide = (Scr::width(data[cursorPos.row][cursorPos.col].ch) == 2);
		bool nextwide = (cursorPos.col+1 != data[0].size()-1 &&
						 Scr::width(data[cursorPos.row][cursorPos.col+1].ch) == 2);
		
		if(nextwide) {
			data[cursorPos.row][cursorPos.col+1].ch = ' ';
			data[cursorPos.row][cursorPos.col+2].ch = ' ';
		}
		data[cursorPos.row][cursorPos.col].ch = key;
		data[cursorPos.row][cursorPos.col].style = colourSelector.GetColor();		
		if(waswide)
			data[cursorPos.row][cursorPos.col+1].ch = ' ';

		cursorPos.col += Scr::width(key);
		if(cursorPos.col >= data[0].size()) {
			cursorPos.col = 0;
			cursorPos.row++;
			if(cursorPos.row == data.size()) {
				cursorPos.row=0;
			}

		}
		
	}
	void OnKeyUp(Key key)throw() {
		// nie dziala?
	}
	void OnKeyDown(Key key)throw() {
		Widget::OnKeyDown(key);
		if(key.IsASpecialKey()) {		   
			switch(key.GetSpecialKey()) {
				case Key::Up:
					if(cursorPos.row == 0)
						cursorPos.row = data.size() - 1;
					else
						cursorPos.row--;
					break;
				case Key::Down:
					cursorPos.row++;
					cursorPos.row %= data.size();
					break;
				case Key::Left:
					if(cursorPos.col == 0)
						cursorPos.col = data[0].size() - 1;
					else
						cursorPos.col--;
					if(cursorPos.col > 0)
						if(Scr::width(data[cursorPos.row][cursorPos.col-1].ch) == 2)
							cursorPos.col--;

					break;
				case Key::Right:
					if(Scr::width(data[cursorPos.row][cursorPos.col].ch) == 2)
						cursorPos.col++;
					cursorPos.col++;
					cursorPos.col %= data[0].size();
					break;
				case Key::Enter:
					PutCharacter(lastPut);
					break;
				default:
					break;
			}
		}
		// don't allow setting a wide character at the last column
		else 
		{			
			lastPut = key;
			PutCharacter(key);
		}
//		RedrawRequest();
	}
	Uint GetMinHeight() const throw() {
		
		return height;
	}
	Uint GetMinWidth() const throw() {
		return width;
	}
	Uint GetMaxHeight() const throw() {
		return height;
	}
	Uint GetMaxWidth() const throw() {
		return width;
	}
	void Load(const std::string &fileName) {
		std::ifstream file(fileName.c_str(), std::ios::in);
		
		file.read((char*)&height, sizeof(height));
		file.read((char*)&width, sizeof(width));
		for(Uint i = 0;i<height;i++)
			for(Uint j = 0;j<width;j++)
				file.read((char*)&(data[i][j]), sizeof(data[i][j]));

	}
	void Save(const std::string &fileName) {
		std::ofstream file(fileName.c_str(), std::ios::out);
		
		file.write((char*)&height, sizeof(height));
		file.write((char*)&width, sizeof(width));
		for(Uint i = 0;i<height;i++)
			for(Uint j = 0;j<width;j++)
				file.write((char*)&(data[i][j]), sizeof(data[i][j]));

	}

	virtual ~ScreenEditor() throw() {
		Save(".editordump");
	}

};


class EditorApp : public RootWindow
{
public:
//	VerticalGroup group;
//	VerticalGroup group2;

    Label tytulAplikacji;
	ScreenEditor editor;
	ActiveFrame frame;
//	HorizontalScrollbar scrollbar;

//	Selectbox select;

    EditorApp()throw();	
    ~EditorApp()throw();
	
	void OnRedraw(Scr::Screen &screen)throw();
	void OnResize()throw();
	void OnKeyDown(Key key)throw() {
		RootWindow::OnKeyDown(key);
		if(key == 'q')
			Exit(1);
	}
	void OnStart() throw() {
		RedrawRequest();
	}

} * app;


EditorApp::EditorApp()throw()
    : RootWindow(std::cin,std::cout), tytulAplikacji("ALABAMA"), 
	  editor(24, 80), frame(26, 82) {

//	AddWidget(tytulAplikacji);

	frame.AddWidget(editor);
	AddWidget(editor.colourSelector);
	editor.colourSelector.SetPosition(25, 1);
	AddWidget(frame);
//	editor.SetPosition(20, 24);
}

void EditorApp::OnResize()throw()
{

	RootWindow::OnResize();	
}

void EditorApp::OnRedraw(Scr::Screen& screen)throw()
{
	RootWindow::OnRedraw(screen);
/*	screen << DisplayStyle(Fg::Red, Fg::Bright, Bg::Black)
		   << Control::GotoYX(1,5) << "Dupa"
		   << Control::GotoYX(1,15) << "<1,15"
		   << Control::GotoYX(2,5) << "こんいちは"
		   << Control::GotoYX(2,8) << "こ"
		   << Control::GotoYX(2,15) << "<2,15"

		;
	
	screen << DisplayStyle(Fg::Black, Fg::Dark, Bg::Yellow)
		   << Control::GotoYX(2,3) << "w";
	
	screen.GotoYX(GetHeight()/2+7, GetWidth()/2-3);
	screen.Rectangle(L'ち',Size(3,3));
	screen.GotoYX(GetHeight()/2+3, GetWidth()/2-3);
	screen.Rectangle(L'j',Size(3,3));
	screen.Refresh();*/
}

EditorApp::~EditorApp()throw()
{

}

int main (int argc,char ** argv)
{
	app = new EditorApp;	
	int result = app->Start(argc, argv);	
	delete app;
	return result;
}
/*end of main function of program*/ 
