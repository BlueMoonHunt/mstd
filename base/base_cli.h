#if !defined(BASE_CLI_H)
#define BASE_CLI_H

#define ANSI_SCREEN_CLEAR               "\033[2J\033[H"                                 /* Clear visible screen & move cursor to (1,1) */
#define ANSI_SCREEN_CLEAR_ONLY          "\033[2J"                                       /* Clear visible screen (cursor stays put) */
#define ANSI_SCREEN_CLEAR_ALL           "\033[3J\033[2J\033[H"                          /* Clear screen + scrollback history + home */
#define ANSI_SCREEN_CLEAR_BELOW         "\033[0J"                                       /* Clear from cursor to bottom of screen */
#define ANSI_SCREEN_CLEAR_ABOVE         "\033[1J"                                       /* Clear from cursor to top of screen */
#define ANSI_SCREEN_ALT_ON              "\033[?1049h"                                   /* Switch to clean secondary screen */
#define ANSI_SCREEN_ALT_OFF             "\033[?1049l"                                   /* Restore original terminal screen */
#define ANSI_SCREEN_RESET_HARD          "\033c"                                         /* Full terminal reset (clears everything & state) */
#define ANSI_SCREEN_RESET_SOFT          "\033[!p"                                       /* Reset terminal modes without clearing text */

#define ANSI_CURSOR_HIDE                "\033[?25l"                                     /* Hide cursor */
#define ANSI_CURSOR_SHOW                "\033[?25h"                                     /* Show cursor */
#define ANSI_CURSOR_HOME                "\033[H"                                        /* Move cursor to top-left (1,1) */
#define ANSI_CURSOR_SAVE                "\033[s"                                        /* Save current cursor position (SCO) */
#define ANSI_CURSOR_RESTORE             "\033[u"                                        /* Restore saved cursor position (SCO) */
#define ANSI_CURSOR_DEC_SAVE            "\0337"                                         /* Save cursor position & attributes (DEC) */
#define ANSI_CURSOR_DEC_RESTORE         "\0338"                                         /* Restore cursor position & attributes (DEC) */
#define ANSI_CURSOR_STYLE_DEFAULT       "\033[0 q"                                      /* Reset cursor shape to terminal default */
#define ANSI_CURSOR_BLOCK_BLINK         "\033[1 q"                                      /* Blinking block cursor */
#define ANSI_CURSOR_BLOCK               "\033[2 q"                                      /* Steady block cursor */
#define ANSI_CURSOR_UNDER_BLINK         "\033[3 q"                                      /* Blinking underline cursor */
#define ANSI_CURSOR_UNDER               "\033[4 q"                                      /* Steady underline cursor */
#define ANSI_CURSOR_BAR_BLINK           "\033[5 q"                                      /* Blinking bar (I-beam) cursor */
#define ANSI_CURSOR_BAR                 "\033[6 q"                                      /* Steady bar (I-beam) cursor */
#define ANSI_CURSOR_GOTO(r, c)          "\033[" #r ";" #c "H"                           /* Move to literal row r, col c (1-based) */
#define ANSI_CURSOR_UP(n)               "\033[" #n "A"                                  /* Move up by literal n rows */
#define ANSI_CURSOR_DOWN(n)             "\033[" #n "B"                                  /* Move down by literal n rows */
#define ANSI_CURSOR_RIGHT(n)            "\033[" #n "C"                                  /* Move right by literal n columns */
#define ANSI_CURSOR_LEFT(n)             "\033[" #n "D"                                  /* Move left by literal n columns */
#define ANSI_CURSOR_NEXT_LINE(n)        "\033[" #n "E"                                  /* Move down n lines and to column 1 */
#define ANSI_CURSOR_PREV_LINE(n)        "\033[" #n "F"                                  /* Move up n lines and to column 1 */
#define ANSI_CURSOR_COL(n)              "\033[" #n "G"                                  /* Move to literal column n on current row */
#define ANSI_CURSOR_ROW(n)              "\033[" #n "d"                                  /* Move to literal row n on current column */
#define ANSI_CURSOR_GOTO_FMT            "\033[%d;%dH"                                   /* Format string for runtime row, col */
#define ANSI_CURSOR_UP_FMT              "\033[%dA"                                      /* Format string for runtime up n rows */
#define ANSI_CURSOR_DOWN_FMT            "\033[%dB"                                      /* Format string for runtime down n rows */
#define ANSI_CURSOR_RIGHT_FMT           "\033[%dC"                                      /* Format string for runtime right n cols */
#define ANSI_CURSOR_LEFT_FMT            "\033[%dD"                                      /* Format string for runtime left n cols */
#define ANSI_CURSOR_COL_FMT             "\033[%dG"                                      /* Format string for runtime column n */
#define ANSI_CURSOR_ROW_FMT             "\033[%dd"                                      /* Format string for runtime row n */
#define ANSI_CURSOR_QUERY               "\033[6n"                                       /* Request cursor pos (replies \033[r;cR on stdin) */

#define ANSI_LINE_CLEAR                 "\r\033[2K"                                     /* Erase entire line and return cursor to col 1 */
#define ANSI_LINE_CLEAR_ONLY            "\033[2K"                                       /* Erase entire line (cursor stays at current col) */
#define ANSI_LINE_CLEAR_EOL             "\033[0K"                                       /* Erase from cursor to end of line */
#define ANSI_LINE_CLEAR_BOL             "\033[1K"                                       /* Erase from start of line to cursor */
#define ANSI_LINE_INSERT(n)             "\033[" #n "L"                                  /* Insert literal n blank lines at cursor */
#define ANSI_LINE_DELETE(n)             "\033[" #n "M"                                  /* Delete literal n lines at cursor */
#define ANSI_LINE_WRAP_ON               "\033[?7h"                                      /* Enable automatic line wrapping at right edge */
#define ANSI_LINE_WRAP_OFF              "\033[?7l"                                      /* Disable automatic line wrapping (clips text) */

#define ANSI_CHAR_INSERT(n)             "\033[" #n "@"                                  /* Insert literal n blank chars at cursor */
#define ANSI_CHAR_DELETE(n)             "\033[" #n "P"                                  /* Delete literal n chars at cursor */
#define ANSI_CHAR_ERASE(n)              "\033[" #n "X"                                  /* Erase literal n chars (replace with spaces) */

#define ANSI_SCROLL_UP(n)               "\033[" #n "S"                                  /* Scroll screen up by literal n lines */
#define ANSI_SCROLL_DOWN(n)             "\033[" #n "T"                                  /* Scroll screen down by literal n lines */
#define ANSI_SCROLL_REGION(t, b)        "\033[" #t ";" #b "r"                           /* Lock scrolling to rows t..b (1-based) */
#define ANSI_SCROLL_RESET               "\033[r"                                        /* Reset scroll region to full screen */
#define ANSI_SCROLL_UP_FMT              "\033[%dS"                                      /* Format string for runtime scroll up n */
#define ANSI_SCROLL_DOWN_FMT            "\033[%dT"                                      /* Format string for runtime scroll down n */
#define ANSI_SCROLL_REGION_FMT          "\033[%d;%dr"                                   /* Format string for runtime scroll region t..b */

#define ANSI_LINK(url, text)            "\033]8;;" url "\033\\" text "\033]8;;\033\\"   /* Clickable literal hyperlink */
#define ANSI_LINK_START(url)            "\033]8;;" url "\033\\"                         /* Open literal hyperlink block */
#define ANSI_LINK_END                   "\033]8;;\033\\"                                /* Close hyperlink block */
#define ANSI_LINK_FMT                   "\033]8;;%s\033\\%s\033]8;;\033\\"              /* Format string for runtime URL & text */

#define ANSI_TITLE(str)                 "\033]0;" str "\007"                            /* Set terminal window & tab title (literal) */
#define ANSI_TITLE_TAB(str)             "\033]1;" str "\007"                            /* Set tab/icon title only (literal) */
#define ANSI_TITLE_WIN(str)             "\033]2;" str "\007"                            /* Set window title only (literal) */
#define ANSI_TITLE_FMT                  "\033]0;%s\007"                                 /* Format string for runtime window & tab title */

#define ANSI_TERM_BELL                  "\007"                                          /* Trigger terminal audible beep or visual flash */
#define ANSI_TERM_PASTE_ON              "\033[?2004h"                                   /* Enable bracketed paste (\033[200~ ... \033[201~) */
#define ANSI_TERM_PASTE_OFF             "\033[?2004l"                                   /* Disable bracketed paste mode */
#define ANSI_TERM_FOCUS_ON              "\033[?1004h"                                   /* Report focus gain (\033[I) and loss (\033[O) */
#define ANSI_TERM_FOCUS_OFF             "\033[?1004l"                                   /* Disable focus reporting */
#define ANSI_TERM_MOUSE_CLICK_ON        "\033[?1000h"                                   /* Report mouse button press/release */
#define ANSI_TERM_MOUSE_CLICK_OFF       "\033[?1000l"                                   /* Disable mouse click reporting */
#define ANSI_TERM_MOUSE_DRAG_ON         "\033[?1002h"                                   /* Report mouse clicks and drag movement */
#define ANSI_TERM_MOUSE_DRAG_OFF        "\033[?1002l"                                   /* Disable mouse drag reporting */
#define ANSI_TERM_MOUSE_ANY_ON          "\033[?1003h"                                   /* Report all mouse movement (even without click) */
#define ANSI_TERM_MOUSE_ANY_OFF         "\033[?1003l"                                   /* Disable all-motion mouse reporting */
#define ANSI_TERM_MOUSE_SGR_ON          "\033[?1006h"                                   /* Enable SGR extended mouse coordinates (>223 cols) */
#define ANSI_TERM_MOUSE_SGR_OFF         "\033[?1006l"                                   /* Disable SGR mouse coordinate mode */
#define ANSI_TERM_QUERY_DEVICE          "\033[c"                                        /* Query terminal device attributes (DA1) */

#define ANSI_FONT_RESET                 "\033[0m"                                       /* Reset all font styles and colors to default */
#define ANSI_FONT_BOLD                  "\033[1m"                                       /* Bold or increased font intensity */
#define ANSI_FONT_DIM                   "\033[2m"                                       /* Faint or decreased font intensity */
#define ANSI_FONT_ITALIC                "\033[3m"                                       /* Italic font style */
#define ANSI_FONT_UNDERLINE             "\033[4m"                                       /* Single underline */
#define ANSI_FONT_BLINK_SLOW            "\033[5m"                                       /* Slow blink (< 150 per min) */
#define ANSI_FONT_BLINK_FAST            "\033[6m"                                       /* Rapid blink (>= 150 per min, rare) */
#define ANSI_FONT_INVERT                "\033[7m"                                       /* Swap foreground and background colors */
#define ANSI_FONT_CONCEAL               "\033[8m"                                       /* Conceal text (e.g., password prompts) */
#define ANSI_FONT_STRIKETHROUGH         "\033[9m"                                       /* Strikethrough line across text */
#define ANSI_FONT_UNDERLINE_DOUBLE      "\033[21m"                                      /* Double underline */
#define ANSI_FONT_OVERLINE              "\033[53m"                                      /* Horizontal line above characters */
#define ANSI_FONT_RESET_INTENSITY       "\033[22m"                                      /* Reset bold and dim intensity to normal */
#define ANSI_FONT_RESET_ITALIC          "\033[23m"                                      /* Reset italic style */
#define ANSI_FONT_RESET_UNDERLINE       "\033[24m"                                      /* Reset single and double underline */
#define ANSI_FONT_RESET_BLINK           "\033[25m"                                      /* Reset slow and fast blink */
#define ANSI_FONT_RESET_INVERT          "\033[27m"                                      /* Reset inverted foreground/background */
#define ANSI_FONT_RESET_CONCEAL         "\033[28m"                                      /* Reset concealed/hidden text */
#define ANSI_FONT_RESET_STRIKETHROUGH   "\033[29m"                                      /* Reset strikethrough */
#define ANSI_FONT_RESET_OVERLINE        "\033[55m"                                      /* Reset overline */

#define ANSI_CLIPBOARD_COPY(b64)        "\033]52;c;" b64 "\007"                         /* Copy base64 string to system clipboard (OSC 52) */
#define ANSI_CLIPBOARD_COPY_FMT         "\033]52;c;%s\007"                              /* Format string for runtime base64 clipboard copy */

#define ANSI_PROGRESS_CLEAR             "\033]9;4;0;0\007"                              /* Clear taskbar/tab progress indicator (OSC 9;4) */
#define ANSI_PROGRESS_NORMAL(pct)       "\033]9;4;1;" #pct "\007"                       /* Set taskbar/tab progress bar (0-100 literal) */
#define ANSI_PROGRESS_ERROR(pct)        "\033]9;4;2;" #pct "\007"                       /* Set red error progress bar (0-100 literal) */
#define ANSI_PROGRESS_INDETERMINATE     "\033]9;4;3;0\007"                              /* Set indeterminate/pulsing progress bar */
#define ANSI_PROGRESS_PAUSED(pct)       "\033]9;4;4;" #pct "\007"                       /* Set yellow paused progress bar (0-100 literal) */
#define ANSI_PROGRESS_NORMAL_FMT        "\033]9;4;1;%d\007"                             /* Format string for runtime taskbar progress (0-100) */

#define ANSI_TAB_NEXT                   "\033[1I"                                       /* Move cursor forward to next hardware tab stop */
#define ANSI_TAB_PREVIOUS               "\033[1Z"                                       /* Move cursor backward to previous hardware tab stop */
#define ANSI_TAB_SET_STOP               "\033H"                                         /* Set a hardware tab stop at current column */
#define ANSI_TAB_CLEAR_CURRENT          "\033[0g"                                       /* Clear hardware tab stop at current column */
#define ANSI_TAB_CLEAR_ALL              "\033[3g"                                       /* Clear all hardware tab stops */

#define ANSI_COLOR_RESET                "\033[0m"                                       /* Reset all colors and font attributes */
#define ANSI_COLOR_FG_DEFAULT           "\033[39m"                                      /* Reset foreground color to terminal default */
#define ANSI_COLOR_BG_DEFAULT           "\033[49m"                                      /* Reset background color to terminal default */
#define ANSI_COLOR_UNDERLINE_DEFAULT    "\033[59m"                                      /* Reset underline color to foreground color */
#define ANSI_COLOR_FG_BLACK             "\033[30m"                                      /* Standard black foreground */
#define ANSI_COLOR_FG_RED               "\033[31m"                                      /* Standard red foreground */
#define ANSI_COLOR_FG_GREEN             "\033[32m"                                      /* Standard green foreground */
#define ANSI_COLOR_FG_YELLOW            "\033[33m"                                      /* Standard yellow foreground */
#define ANSI_COLOR_FG_BLUE              "\033[34m"                                      /* Standard blue foreground */
#define ANSI_COLOR_FG_MAGENTA           "\033[35m"                                      /* Standard magenta foreground */
#define ANSI_COLOR_FG_CYAN              "\033[36m"                                      /* Standard cyan foreground */
#define ANSI_COLOR_FG_WHITE             "\033[37m"                                      /* Standard white foreground */
#define ANSI_COLOR_FG_BRIGHT_BLACK      "\033[90m"                                      /* High-intensity black (gray) foreground */
#define ANSI_COLOR_FG_BRIGHT_RED        "\033[91m"                                      /* High-intensity red foreground */
#define ANSI_COLOR_FG_BRIGHT_GREEN      "\033[92m"                                      /* High-intensity green foreground */
#define ANSI_COLOR_FG_BRIGHT_YELLOW     "\033[93m"                                      /* High-intensity yellow foreground */
#define ANSI_COLOR_FG_BRIGHT_BLUE       "\033[94m"                                      /* High-intensity blue foreground */
#define ANSI_COLOR_FG_BRIGHT_MAGENTA    "\033[95m"                                      /* High-intensity magenta foreground */
#define ANSI_COLOR_FG_BRIGHT_CYAN       "\033[96m"                                      /* High-intensity cyan foreground */
#define ANSI_COLOR_FG_BRIGHT_WHITE      "\033[97m"                                      /* High-intensity white foreground */
#define ANSI_COLOR_BG_BLACK             "\033[40m"                                      /* Standard black background */
#define ANSI_COLOR_BG_RED               "\033[41m"                                      /* Standard red background */
#define ANSI_COLOR_BG_GREEN             "\033[42m"                                      /* Standard green background */
#define ANSI_COLOR_BG_YELLOW            "\033[43m"                                      /* Standard yellow background */
#define ANSI_COLOR_BG_BLUE              "\033[44m"                                      /* Standard blue background */
#define ANSI_COLOR_BG_MAGENTA           "\033[45m"                                      /* Standard magenta background */
#define ANSI_COLOR_BG_CYAN              "\033[46m"                                      /* Standard cyan background */
#define ANSI_COLOR_BG_WHITE             "\033[47m"                                      /* Standard white background */
#define ANSI_COLOR_BG_BRIGHT_BLACK      "\033[100m"                                     /* High-intensity black (gray) background */
#define ANSI_COLOR_BG_BRIGHT_RED        "\033[101m"                                     /* High-intensity red background */
#define ANSI_COLOR_BG_BRIGHT_GREEN      "\033[102m"                                     /* High-intensity green background */
#define ANSI_COLOR_BG_BRIGHT_YELLOW     "\033[103m"                                     /* High-intensity yellow background */
#define ANSI_COLOR_BG_BRIGHT_BLUE       "\033[104m"                                     /* High-intensity blue background */
#define ANSI_COLOR_BG_BRIGHT_MAGENTA    "\033[105m"                                     /* High-intensity magenta background */
#define ANSI_COLOR_BG_BRIGHT_CYAN       "\033[106m"                                     /* High-intensity cyan background */
#define ANSI_COLOR_BG_BRIGHT_WHITE      "\033[107m"                                     /* High-intensity white background */
#define ANSI_COLOR_FG_256(id)           "\033[38;5;" #id "m"                            /* 256-color palette foreground (literal 0-255) */
#define ANSI_COLOR_BG_256(id)           "\033[48;5;" #id "m"                            /* 256-color palette background (literal 0-255) */
#define ANSI_COLOR_UNDERLINE_256(id)    "\033[58;5;" #id "m"                            /* 256-color palette underline (literal 0-255) */
#define ANSI_COLOR_FG_256_FMT           "\033[38;5;%dm"                                 /* Format string for runtime 256-color foreground */
#define ANSI_COLOR_BG_256_FMT           "\033[48;5;%dm"                                 /* Format string for runtime 256-color background */
#define ANSI_COLOR_UNDERLINE_256_FMT    "\033[58;5;%dm"                                 /* Format string for runtime 256-color underline */
#define ANSI_COLOR_FG(r, g, b)          "\033[38;2;" #r ";" #g ";" #b "m"               /* 24-bit TrueColor foreground (literal 0-255) */
#define ANSI_COLOR_BG(r, g, b)          "\033[48;2;" #r ";" #g ";" #b "m"               /* 24-bit TrueColor background (literal 0-255) */
#define ANSI_COLOR_UNDERLINE(r, g, b)   "\033[58;2;" #r ";" #g ";" #b "m"               /* 24-bit TrueColor underline (literal 0-255) */
#define ANSI_COLOR_FG_FMT               "\033[38;2;%d;%d;%dm"                           /* Format string for runtime TrueColor foreground */
#define ANSI_COLOR_BG_FMT               "\033[48;2;%d;%d;%dm"                           /* Format string for runtime TrueColor background */
#define ANSI_COLOR_UNDERLINE_FMT        "\033[58;2;%d;%d;%dm"                           /* Format string for runtime TrueColor underline */

#endif /* BASE_CLI_H */