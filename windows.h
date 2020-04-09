#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>
#include <stdarg.h>
#include <unistd.h>


class UltimaWindows{
    private:
        int head_height;
        int head_width;
        int console_log_height;
        int console_width;
        int log_width;
        int start_x;
        int start_y;
        int thread_height;
        int thread_width;
        int log_console_y;
        int max_x;
        int max_y;
        int index;

    public:
        WINDOW* headingWin;
        WINDOW* logWin;
        WINDOW* consoleWin;
        UltimaWindows();
        WINDOW *create_window(int height, int width, int starty, int startx);
        WINDOW* create_thread_window();
        WINDOW* create_heading_win();
        WINDOW* create_log_window();
        WINDOW* create_console_window();
        void write_window(WINDOW * Win, const char* text);
        void write_window(WINDOW * Win, int y, int x, const char* text);
        void display_help(WINDOW * Win);

};

#endif
