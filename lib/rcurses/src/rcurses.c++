#include "rcurses.h"


static SCREEN* maincontext = NULL;

WINDOW* initscr()
{
	maincontext = new CursesRootWindow();
	stdscr = &(maincontext->GetScreenPtr());
	return stdscr;
}

int endwin()
{
	if(!maincontext)
		return ERR;
	delete maincontext;
	stdscr = NULL;
	return OK;
}

int mvwaddstr(WINDOW *win, int y, int x, const char *str)
{
	(*win) 
		<< DisplayStyle(Fg::White, Fg::Bright, Bg::Black)
		<<Control::GotoYX(y, x) << str;
	return OK;
}

int wrefresh(WINDOW *win)
{
	(*win) << Control::Refresh << Control::Clear;
	return OK;
}

int wgetch(WINDOW *win)
{
/*   	LocalScreen *lscr = dynamic_cast<LocalScreen *>(stdscr);
	

	if(lscr->TestForResize(lscr->input.FD())) // PRIVATE
	return KEY_RESIZE;*/
	return 10;
}

int resize_term(int lines, int columns)
{
	maincontext->OnResize(lines, columns);
	return OK;
}
