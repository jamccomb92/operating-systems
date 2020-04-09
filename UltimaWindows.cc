#include "UltimaWindows.h"


/************* UltimaWindow Constructor **********************
Input: NULL
Output: Heading window across the top of the screen
with set width and height
*************************************************************/

UltimaWindows::UltimaWindows(){
    head_height = 11;
    head_width = 98;
    console_log_height = 20;
    console_width = 32;
    log_width = 65;
    max_x = 100;
    max_y = 82;
    start_x = 2;
    start_y = 11;
    thread_height = 15;
    thread_width = 32;
    log_console_y = 0;
    message_start_y = 47;
    message_start_x = 2;
    message_width = 98;
    message_height = 30;
    headingWin = create_heading_win();
    consoleWin = create_console_window();
    logWin = create_log_window();
    messageWin = create_message_window();
}
/************* UltimaWindow Constructor **********************
Input: height, widthm, start_x, start_y
Output: created window with specified input parramters
*************************************************************/
WINDOW * UltimaWindows::create_window(int height, int width, int starty, int startx)
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
WINDOW* UltimaWindows::create_thread_window(){
    WINDOW* Win;

    Win = newwin(thread_height, thread_width, start_y, start_x);
    scrollok(Win, TRUE);    // allow scrolling
    scroll(Win);            
    box(Win, 0, 0);         // box around the window
    mvwprintw(Win, 1, 2, "Starting Thread...\n"); // write to the window

    wrefresh(Win);          // refresh


    start_x += thread_width + 1;    //space in between thread windows
    
    // check if next window will exceed our max and put onto next line if so
    if(start_x+thread_width > max_x){
        start_x = 2;
        start_y += thread_height;
    }

    return Win;
}
/**************** Create Heading Window **************************
Input: None
Output: heading window 
*****************************************************************/
WINDOW* UltimaWindows::create_heading_win(){
    WINDOW * Heading_Win = newwin(head_height, head_width, 0, 2);
    box(Heading_Win, 0,0);      // draw box
    mvwprintw(Heading_Win, 2,28, "ULTIMA 2.0 (Spring 2019)");

    mvwprintw(Heading_Win, 4, 2, "Starting ULTIMA 2.0.....");
    mvwprintw(Heading_Win, 5, 2, "Starting Thread 1....");
    mvwprintw(Heading_Win, 6, 2, "Starting Thread 2....");
    mvwprintw(Heading_Win, 7, 2, "Starting Thread 3....");
    mvwprintw(Heading_Win, 9, 2, "Press 'q' or Ctrl-C to exit the program..."); 
    mvwprintw(Heading_Win, 4, 65, " --- Console Commands ---");
    mvwprintw(Heading_Win, 5, 65, "p -- (Un)Pause Logging Window");
    mvwprintw(Heading_Win, 6, 65, "h -- Display Help");
    mvwprintw(Heading_Win, 7, 65, "q -- Quit the Program");

    wrefresh(Heading_Win); // changes to the window are refreshed

    headingWin = Heading_Win;

    return Heading_Win;
}
/**************** Create Log Window ******************************
Input: none
Output: Log Window
*****************************************************************/
WINDOW* UltimaWindows::create_log_window(){
    // check if start_x is at a new line
    // the y start for the console and log will be the same as start y
    // otherwise we will move the log and console to the next approppriate y position
    // if(start_x == 2){
    //     log_console_y = start_y;
    // }
    // else{
    //     log_console_y = start_y + thread_height;
    // }
    log_console_y = 27;

    WINDOW * Log_Win = create_window(console_log_height, log_width, log_console_y, 2);
    write_window(Log_Win, 1, 5, ".......Log....\n");

    logWin = Log_Win;

    return Log_Win;
}
/**************** Create Console Window **************************
Input: none
Output: Console Window
*****************************************************************/
WINDOW* UltimaWindows::create_console_window(){
    // check if start_x is at a new line
    // the y start for the console and log will be the same as start y
    // otherwise we will move the log and console to the next approppriate y position
    // same as previous, just in case
    // if(start_x == 2){
    //     log_console_y = start_y;
    // }
    // else{
    //     log_console_y = start_y + thread_height;
    // }

    log_console_y = 27;
    WINDOW * Console_Win = create_window(console_log_height,console_width, log_console_y, 68);
    write_window(Console_Win, 1, 1, "....Console....\n");
    write_window(Console_Win, 2, 1, "Ultima # ");

    consoleWin = Console_Win;

    return Console_Win;
}

WINDOW* UltimaWindows::create_message_window(){

    WINDOW * Message_Win = create_window(message_height,message_width, message_start_y, message_start_x);
    write_window(Message_Win, 1, 1, " ------ Message Window ------\n");

    messageWin = Message_Win;

    return Message_Win;
}

/**************** Write Window **********************************
Input: Window pointer to write to and specified text to write (char *)
Output: refreshed window with inputted text
*****************************************************************/
void UltimaWindows::write_window(WINDOW * Win, const char* text)
{
    wprintw(Win, text);         // put text into correct window
    box(Win, 0 , 0);            // maintain integrity of the box
    wrefresh(Win);              // draw the window
}
/**************** Write Window **********************************
Input: Window pointer to write to, x and y positions, and specified 
text to write (char *)
Output: refreshed window with inputted text at given position
*****************************************************************/
void UltimaWindows::write_window(WINDOW * Win, int y, int x, const char* text)
{        
    mvwprintw(Win, y, x, text); // put text into correct window
    box(Win, 0 , 0);            //keep box integrity
    wrefresh(Win);              // Draw the window

}
/**************** Display Help **********************************
Input: Window pointer to write to
Output: Help diagram in the inputted window
*****************************************************************/
void UltimaWindows::display_help(WINDOW * Win)
{
    wclear(Win);
    write_window(Win, 1, 1, "----------- Help -----------");
    write_window(Win, 2, 1, "k= Kill thread");
    write_window(Win, 3, 1, "p= Pause and unpause live feed");
    write_window(Win, 4, 1, "d= Dump TCB Table");
    write_window(Win, 5, 1, "s= Dump Semaphore");
    write_window(Win, 6, 1, "u= Remove Dead Tasks");
    write_window(Win, 7, 1, "n= New task in OPEN window");
    write_window(Win, 8, 1, "l= New task WITHOUT window");
    write_window(Win, 9, 1, "c= clear screen");
    write_window(Win, 10, 1, "h= help screen");
    write_window(Win, 11, 1, "q= Quit\n");
}

