#include "curses.h"

WINDOW *stdscr;

int main(void)
{
	stdscr = initscr();
	
	int c;
	while( (c = wgetch(stdscr)) != '\0' ) {
//		if(c == KEY_RESIZE)
//			resize_term( );

		mvwaddstr(stdscr, 20, 0, "DUPA");
		mvwaddstr(stdscr, 24, 5, "DUPA");
		wrefresh(stdscr);
	}
	
	
	endwin();
}
