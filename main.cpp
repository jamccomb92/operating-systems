#include "windows.h"
#include "scheduler.h"
#include "semaphore.h"

#include <ncurses.h>
#include <pthread.h>
#include <string>
// #include <bits/stdc++.h>

using namespace std;

int main(){
    initscr(); // Start nCurses
    refresh();
	
	scheduler* sched = new scheduler();

    UltimaWindows my_Windows;

    // WINDOW * Heading_Win = my_Windows.create_heading_win();

	sched->create_task("Task 0");
    WINDOW * thread_1 = my_Windows.create_thread_window();
	sched->process_table->getHead()->next->my_window = thread_1;
	
	sched->create_task("Task 1");
    WINDOW * thread_2 = my_Windows.create_thread_window();
	sched->process_table->getHead()->next->my_window = thread_2;
	
	sched->create_task("Task 2");
    WINDOW * thread_3 = my_Windows.create_thread_window();
	sched->process_table->getHead()->next->my_window = thread_3;
	
    /*WINDOW * thread_3 = my_Windows.create_thread_window();

    WINDOW * thread_4 = my_Windows.create_thread_window();

    WINDOW * thread_5 = my_Windows.create_thread_window();

    WINDOW * thread_6 = my_Windows.create_thread_window();

    WINDOW * thread_7 = my_Windows.create_thread_window();*/

    WINDOW * log_win = my_Windows.create_log_window();

    WINDOW * console = my_Windows.create_console_window();
    
    char buff[256];



    int input = -1;
    int thread_no = 1;
    int CPU_Quantum = 0;
	Queue<WINDOW *> win_queue;
	
	string t1Name, t2Name, t3Name;
	int t1ID, t2ID, t3ID;
	char t1State, t2State, t3State;
	bool checkstop;
	
	/*for(int i = 0; i < 1000; i++){
		
		sprintf(buff, " Task-1 running #%d\n", i);
		wprintw(thread_1, buff);
		
		wprintw(thread_2, " Task-2 running #%d\n", i);
		
		box(thread_1, 0,0);
		box(thread_2, 0,0);
		
		wrefresh(thread_1);
		wrefresh(thread_2);
		
		sleep(0.1);
	}*/
	int i = 0;
	
	cbreak(); // disable line buffering
    noecho(); // disable automatic echo of characters read by getch(), wgetch() 
	nodelay(console, true); // nodelay causes getch to be a non-blocking call.
                                // If no input is ready, getch returns ERR
								
	bool carryOver = false;
	string semaphoreDump, tcbDump;
	int lengthSemDump = 0;
	char buffer[256];

	while(input != 'q'){
		//sched->yield();
		if(sched->getDL()){
			cout<<"DEADLOCK HAS OCCURED\n";
			sleep(5);
			//input = 'q';
		}
	
		
		checkstop = sched->yield();
		
		if(!carryOver)
			input = wgetch(console);
		else
			carryOver = false;
		
		
		switch(input)
		{
			case 'd':	
			
				// t1Name = sched->process_table->getHead()->name;
				// t1ID = sched->process_table->getHead()->PID;
				// t1State = sched->process_table->getHead()->state;
				
				// t2Name = sched->process_table->getHead()->next->name;
				// t2ID = sched->process_table->getHead()->next->PID;
				// t2State = sched->process_table->getHead()->next->state;
				
				// t3Name = sched->process_table->getHead()->next->next->name;
				// t3ID = sched->process_table->getHead()->next->next->PID;
				// t3State = sched->process_table->getHead()->next->next->state;

				tcbDump = sched->dump();
				for( int i = 0; i < tcbDump.length(); i++){
					buffer[i] = tcbDump[i];
				}

				if(!checkstop){
					sprintf(buff, "%s", buffer);
					my_Windows.write_window(log_win, buff);
				}
				else{
					carryOver = true;
				}
				break;
			case 's':
				semaphoreDump = sched->io->dump();

				for( int i = 0; i < semaphoreDump.length(); i++){
					buffer[i] = semaphoreDump[i];
				}
				// strcpy(outSemaphore, semaphoreDump.c_str());
				sprintf(buff, "%s", buffer);
				my_Windows.write_window(log_win, buff);
				break;
				
			case 'u' :
				sched->cleanup();
				break;

			case 'c':
				refresh(); // Clear the entire screen (in case it is corrupted) 
                wclear(console); // Clear the Console window 
                my_Windows.write_window(console, 1, 1, "Ultima # ");
                break;
				
			case '0':
			
			case '1':
			
			case '2':
				sprintf(buff, " You typed: %c\n", input);
				my_Windows.write_window(console, buff);
				sched->destroy_task(input - '0');
				sched->io->up(input - '0');
				win_queue.Enqueue(sched->process_table->getbyPID(input - '0')->my_window);
				break;
				
			case 'n':
				if(!win_queue.isEmpty()){
					sched->create_task("New Task");
					sched->process_table->getHead()->next->my_window = win_queue.Front();
					refresh();
					wclear(win_queue.Front());
					win_queue.Dequeue();
				}
				break;
			
			case 'l':
				//if(!win_queue.isEmpty()){
					sched->create_lazy_task("Lazy New Task");
					/*sched->process_table->getHead()->next->my_window = win_queue.Front();
					refresh();
					wclear(win_queue.Front());
					win_queue.Dequeue();*/
				//}
				break;
			case 'h':
				my_Windows.display_help(console);
				break;
				
			case ERR:
				break;
			default:
				sprintf(buff, "This should never happen!");
				my_Windows.write_window(log_win, buff);
				break;
		}
		
		wrefresh(console);
		
		
		
		/*sprintf(buff, " Task-0 running #%d\n", i);
		wprintw(thread_1, buff);
		
		sprintf(buff, " Task-1 running #%d\n", i);
		wprintw(thread_2, buff);
		
		sprintf(buff, " Task-2 running #%d\n", i);
		wprintw(thread_3, buff);
		
		box(thread_1, 0,0);
		box(thread_2, 0,0);
		box(thread_3, 0,0);
		
		wrefresh(thread_1);
		wrefresh(thread_2);
		wrefresh(thread_3);
		
		sleep(0.1);
		
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

		wrefresh(thread_1);
		wrefresh(thread_2);
		wrefresh(my_Windows.consoleWin);
        thread_no = 1;
        CPU_Quantum++;
		i++;*/
    

    }

    endwin();
	return 0;
}