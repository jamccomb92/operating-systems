#include <iostream>
#include <pthread.h>    // Needed for using the pthread library
#include <assert.h>
#include <time.h>
#include <unistd.h> 
#include <ncurses.h>    
#include <stdarg.h>     
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
// #include "semaphore.cpp"

// extern semaphore sema;
class windows{
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
        windows(){
            head_height = 11;
            head_width = 132;
            console_log_height = 15;
            console_width = 30;
            log_width = 60;
            max_x = 136;
            max_y = 82;
            start_x = 2;
            start_y = 11;
            thread_height = 20;
            thread_width = 32;
            log_console_y = 0;
            WINDOW* headingWin = create_heading_win();
        }

        /**************** Create Window **********************************
        Input: height, width, start_y, start_x of a new window
        Output: window with specified inputs
        *****************************************************************/
        WINDOW *create_window(int height, int width, int starty, int startx)
        {
            // sema.down();
            WINDOW *Win;
            Win = newwin(height, width, starty, startx);
            scrollok(Win, TRUE);            // Allow scrolling of the window
            scroll(Win);                    // scroll the window
            box(Win, 0 , 0);                // 0, 0 gives default characters 
                                            // for the vertical and horizontal lines
            wrefresh(Win);                  // draw the window
            // sema.up();
            return Win;
        }
        /**************** Create Thread Window ***************************
        Input: None
        Output: Window with specified height and width from the class
        *****************************************************************/
        WINDOW* create_thread_window(){
            WINDOW* Win;

            Win = newwin(thread_height, thread_width, start_y, start_x);
            scrollok(Win, TRUE);
            scroll(Win);
            box(Win, 0, 0);
            mvwprintw(Win, 1, 2, "Starting Thread...");

            wrefresh(Win);

            start_x += thread_width + 1;
            if(start_x+thread_width > max_x){
                start_x = 2;
                start_y += thread_height;
            }

            return Win;
        }
        /**************** Create Heading Window **************************
        Input: none
        Output: heading window 
        *****************************************************************/
        WINDOW* create_heading_win(){
            WINDOW * Heading_Win = newwin(head_height, head_width, 0, 2);
            box(Heading_Win, 0,0);
            mvwprintw(Heading_Win, 2,28, "ULTIMA 2.0 (Spring 2019)");

            mvwprintw(Heading_Win, 4, 2, "Starting ULTIMA 2.0.....");
            mvwprintw(Heading_Win, 5, 2, "Starting Thread 1....");
            mvwprintw(Heading_Win, 6, 2, "Starting Thread 2....");
            mvwprintw(Heading_Win, 7, 2, "Starting Thread 3....");
            mvwprintw(Heading_Win, 9, 2, "Press 'q' or Ctrl-C to exit the program..."); 
            wrefresh(Heading_Win); // changes to the window are refreshed

            headingWin = Heading_Win;

            return Heading_Win;
        }
        /**************** Create Log Window ******************************
        Input: none
        Output: Log Window
        *****************************************************************/
        WINDOW* create_log_window(){
            if(start_x == 2){
                log_console_y = start_y;
            }
            else{
                log_console_y = start_y + thread_height;
            }

            WINDOW * Log_Win = create_window(console_log_height, log_width, log_console_y, 2);
            write_window(Log_Win, 1, 5, ".......Log....\n");

            logWin = Log_Win;

            return Log_Win;
        }
        /**************** Create Console Window **************************
        Input: none
        Output: Console Window
        *****************************************************************/
        WINDOW* create_console_window(){
            if(start_x == 2){
                log_console_y = start_y;
            }
            else{
                log_console_y = start_y + thread_height;
            }
            WINDOW * Console_Win = create_window(console_log_height,console_width, log_width, 106);
            write_window(Console_Win, 1, 1, "....Console....\n");
            write_window(Console_Win, 2, 1, "Ultima # ");

            consoleWin = Console_Win;

            return Console_Win;
        }
        /**************** Write Window **********************************
        Input: Window pointer to write to and specified text to write (char *)
        Output: refreshed window with inputted text
        *****************************************************************/
        void write_window(WINDOW * Win, const char* text)
        { 
            // sema.down();
            wprintw(Win, text);
            box(Win, 0 , 0);
            wrefresh(Win);              // draw the window
            // sema.up();
        }
        /**************** Write Window **********************************
        Input: Window pointer to write to, x and y positions, and specified 
        text to write (char *)
        Output: refreshed window with inputted text at given position
        *****************************************************************/
        void write_window(WINDOW * Win, int y, int x, const char* text)
        {        
            // sema.down();
            mvwprintw(Win, y, x, text);
            box(Win, 0 , 0);
            wrefresh(Win);              // Draw the window
            // sema.up();
        }

};


int main(){

    initscr(); // Start nCurses
    refresh();

    windows my_Windows;

    // WINDOW * Heading_Win = my_Windows.create_heading_win();


    WINDOW * thread_1 = my_Windows.create_thread_window();

    WINDOW * thread_2 = my_Windows.create_thread_window();

    WINDOW * thread_3 = my_Windows.create_thread_window();

    WINDOW * thread_4 = my_Windows.create_thread_window();

    WINDOW * thread_5 = my_Windows.create_thread_window();

    WINDOW * thread_6 = my_Windows.create_thread_window();

    WINDOW * thread_7 = my_Windows.create_thread_window();

    WINDOW * log_win = my_Windows.create_log_window();

    WINDOW * console = my_Windows.create_console_window();

    my_Windows.write_window(my_Windows.logWin, "  Hi there Justin and Kamil..  _____ \n");
    my_Windows.write_window(my_Windows.logWin, "                              |_____|__ \n");
    my_Windows.write_window(my_Windows.logWin, "                              | ^_^ |\n");
    my_Windows.write_window(my_Windows.logWin, "                              |  O  |  \n");
    my_Windows.write_window(my_Windows.logWin, "                              ///////\n");
    
    char buff[256];



    int input = -1;
    int thread_no = 1;
    int CPU_Quantum = 0;

    while(input != 'q'){
        input = wgetch(my_Windows.headingWin);

        sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_1, buff);
        thread_no++;

        sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_2, buff);
        thread_no++;

        sprintf(buff, " \n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_3, buff);
        thread_no++;

        sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_4, buff);
        thread_no++;

       sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_5, buff);
        thread_no++;   

       sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_6, buff);
        thread_no++;

        sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        my_Windows.write_window(thread_7, buff);

        // sprintf(buff, "\n Task-%d running #%d", thread_no, CPU_Quantum);
        // my_Windows.write_window(test, buff);

        thread_no = 1;
        CPU_Quantum++;
    

    }

    endwin();
    return 0;
}
