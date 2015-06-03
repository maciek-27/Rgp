#ifndef __RCURSES_H__
#define __RCURSES_H__

#ifdef __cplusplus
#include "../lib/toolkit/include/rootwindow.h++"
#include "../lib/screen/include/localscreen.h++"
using namespace Scr;
using namespace Scr::Tk;

#define KEY_RESIZE 666

class CursesRootWindow : public Connection
{
 public:
	CursesRootWindow()throw() : Connection(std::cin, std::cout) {; };
	
	Screen& GetScreenPtr() {
		return (*screen);
	}

	CursesRootWindow(std::istream& _input, std::ostream& _output)throw() :
		Connection(_input, _output) {; };

	~CursesRootWindow()throw() {;};
};

#else
typedef struct Screen Screen;
typedef struct CursesRootWindow CursesRootWindow;
#endif

#undef ERR
#define ERR (-1)

#undef OK
#define OK (0)


#ifdef __cplusplus
extern "C" {
#endif
	typedef Screen WINDOW;
	typedef CursesRootWindow SCREEN;
	
	extern WINDOW *stdscr;
	
	WINDOW* initscr(void);
	int endwin(void);
	int mvwaddstr(WINDOW *win, int y, int x, const char *str);
	int wrefresh(WINDOW *win);
	int wgetch(WINDOW *win);
	int resize_term(int lines, int columns);
	
#ifdef __cplusplus
}
#endif

#endif /* __RCURSES_H__ */
