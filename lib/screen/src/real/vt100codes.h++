/*! \file vt100codes.h++
  \brief VT100 terminal control macros.
  Contains macro for cursor positioning, attribute setting, character
  sets etc. Used by Scr::VT100Compatible class
*/

/* General setup */
#define               RESET_DEVICE "\033c" 
//! enable line wrapping
#define           ENABLE_LINE_WRAP "\x1b[7h"
//! disable it
#define          DISABLE_LINE_WRAP "\x1b[7l"

/* Scrolling options. Note: there is no way to disable scrolling */
//! Whole screen is scrolled on SCROLL_UP/SCROLL_DOWN
#define       SCROLL_ENTIRE_SCREEN "\x1b[r"
//! Only rows from A to B are scrolled on  SCROLL_UP/SCROLL_DOWN, anything above A or below B is not scrolled
#define  SCROLL_SCREEN_REGION(A,B) "\x1b["<< (A) << ';' << (B) << 'r'

//! scroll up
#define                  SCROLL_UP "\x1b[M"
//! scroll down
#define                SCROLL_DOWN "\x1b[D"

//! make cursor invisible - xterm
#define                HIDE_CURSOR "\x1b[?25l"

//! restore it -xterm
#define                SHOW_CURSOR "\x1b[?25h"

/* Absolute cursor positioning. */
//! Set cursor position  to left-top position
#define                CURSOR_HOME "\x1b[H"
//! Set cursor position to specific y/x (note: y = 1..height, x = 1..width)
#define             CURSOR_YX(y,x) "\x1b["<< (y) << ';' << (x) << 'H'
/* Relative cursor positioning. */
//! move cursor one position up
#define                  CURSOR_UP "\x1b[A"
//! move cursor n positions up
#define              CURSOR_UP_(n) "\x1b["<< (n) <<'A'
//! move cursor one position down
#define                CURSOR_DOWN "\x1b[B"
//! move cursor n positions down
#define            CURSOR_DOWN_(n) "\x1b["<< (n) <<'B'
//! move cursor one position forward
#define             CURSOR_FORWARD "\x1b[C"
//! move cursor n positions forward
#define         CURSOR_FORWARD_(n) "\x1b["<< (n) <<'C'
//! move cursor one position backward
#define            CURSOR_BACKWARD "\x1b[D"
//! move cursor n positions backward
#define        CURSOR_BACKWARD_(n) "\x1b["<< (n) <<'D'
/* Unsave restores position after last save. */
//! One cursor position may be saved
#define                SAVE_CURSOR "\x1b[s"
//! and restored
#define              UNSAVE_CURSOR "\x1b[u"

/* Erase screen. */
//! Erase whole screen
#define                      ERASE "\x1b[2J"
//! same as above
#define               ERASE_SCREEN ERASE
//! erase above cursor
#define                   ERASE_UP "\x1b[1J"
//! erase below cursor
#define                 ERASE_DOWN "\x1b[J"

/* Erase line. */
//! erase current line
#define                 ERASE_LINE "\x1b[K"
//! erase current line left from the cursor
#define        ERASE_START_OF_LINE "\x1b[1K"
//! erase current line right from the cursor
#define          ERASE_END_OF_LINE "\x1b[K"

/* a = one of following 23 attributes*/
//! set specific attribute
#define                SET_ATTR(a) "\x1b["<<a<<'m'
//! if you have to set more attributes, separate them by  <<';'<<
#define                   AND_ATTR <<';'<<
/*generalattributes (0-8 without 3 and 6) */
//!resets terminal defaults
#define                 ATTR_RESET 0 
//!sets brighter fg color
#define                ATTR_BRIGHT 1 
//!turns off bright (sets darker fg color) note: not supported by most of platforms
#define                   ATTR_DIM 2 
//!turns on text underline (not supported by MS Windows)
#define            ATTR_UNDERSCORE 4 
//!turns on blink (Not supported by MS Windows, most of other implementations incompatible)
#define                 ATTR_BLINK 5 
//! Inverts bg and fg color (incompatible implementation on MS windows)*/
#define               ATTR_REVERSE 7 

#define                ATTR_HIDDEN 8 /*???*/

/*Foreground (text) colours*/
#define             FG_COLOR_BLACK 30
#define               FG_COLOR_RED 31
#define             FG_COLOR_GREEN 32
#define            FG_COLOR_YELLOW 33
#define              FG_COLOR_BLUE 34
#define           FG_COLOR_MAGENTA 35
#define              FG_COLOR_CYAN 36
#define             FG_COLOR_WHITE 37

/*Background colors*/
#define             BG_COLOR_BLACK 40
#define               BG_COLOR_RED 41
#define             BG_COLOR_GREEN 42
#define            BG_COLOR_YELLOW 43
#define              BG_COLOR_BLUE 44
#define           BG_COLOR_MAGENTA 45
#define              BG_COLOR_CYAN 46
#define             BG_COLOR_WHITE 47

/* Character Set settings*/
#define                   CS_UK_G0 "\x1b(A"/*G Zero, not oh*/
#define                   CS_UK_G1 "\x1b)A"
//! Select UK character set
#define                      CS_UK CS_UK_G0 
#define                   CS_US_G0 "\x1b(B"
#define                   CS_US_G1 "\x1b)B"
//! Select US character set
#define                      CS_US CS_US_G0
/* alt character set including frames etc */
#define                  CS_ALT_G0 "\x1b(0"
#define                  CS_ALT_G1 "\x1b)0"
//! Select one of alt character set to use frames etc
#define                     CS_ALT CS_ALT_G0

/* ALT character set symbols (i.e. if CS_ALT is set, then you can use
 * frames etc.) */
#define                  ALT_BLANK '_'
#define                ALT_DIAMOND '`' /*no windows equiv.*/
#define           ALT_CHECKERBOARD 'a'
#define         ALT_HORIZONTAL_TAB 'b'
#define              ALT_FORM_FEED 'c'
#define                 ALT_RETURN 'd'
#define              ALT_LINE_FEED 'e'
#define                 ALT_DEGREE 'f'
#define             ALT_PLUS_MINUS 'g'
#define               ALT_NEW_LINE 'h'
#define           ALT_VERTICAL_TAB 'i'
#define            ALT_LOWER_RIGHT 'j'
#define            ALT_UPPER_RIGHT 'k'
#define             ALT_UPPER_LEFT 'l'
#define             ALT_LOWER_LEFT 'm'
#define               ALT_CROSSING 'n'
#define        ALT_HORIZONTAL_LINE 'q'
#define                 ALT_LEFT_T 't'
#define                ALT_RIGHT_T 'u'
#define               ALT_BOTTOM_T 'v'
#define                  ALT_TOP_T 'w'
#define          ALT_VERTICAL_LINE 'x'
#define          ALT_LESS_OR_EQUAL 'y'
#define       ALT_GREATER_OR_EQUAL 'z'
#define                     ALT_PI '{'  /*no windows equiv.*/
#define              ALT_NOT_EQUAL '|'
#define               ALT_UK_POUND '}'
#define           ALT_CENTERED_DOT '~'

/*VT100*/

// interesting extensions

//! resize entire vscreen (xterm, konsole)
#define         RESIZE_SCREEN(A,B) "\x1b[8;"<< (A) << ";"<<(B) << "t"
