#include "UltimaWindows.h"
#include "scheduler.h"
#include "semaphore.h"

#include <ncurses.h>
#include <pthread.h>
#include <string>

using namespace std;

int main()
{
	initscr(); // Start nCurses
	refresh();

	scheduler *sched = new scheduler();

	//UltimaWindows sched->sched_windows;

	sched->create_task("Task 0");
	WINDOW *W0 = sched->sched_windows.create_thread_window();
	sched->getProcessTable()->getLast()->window = W0;

	sched->create_task("Task 1");
	WINDOW *W1 = sched->sched_windows.create_thread_window();
	sched->getProcessTable()->getLast()->window = W1;

	sched->create_task("Task 2");
	WINDOW *W2 = sched->sched_windows.create_thread_window();
	sched->getProcessTable()->getLast()->window = W2;

	// WINDOW * WLog = sched->sched_windows.create_log_window();

	// WINDOW * WConsole = sched->sched_windows.create_console_window();

	char winBuff[256];

	//sched->create_lazy_task("Task 3");

	int input = -1;
	int thread_no = 1;
	int CPU_Quantum = 0;
	Queue<WINDOW *> win_queue;

	string t1Name, t2Name, t3Name;
	int t1ID, t2ID, t3ID;
	char t1State, t2State, t3State;
	bool switching = true;
	bool refresh_table = false;

	int i = 0;

	cbreak();										// disable line winBuffering
	noecho();										// disable automatic echo of characters read by getch(), wgetch()
	nodelay(sched->sched_windows.consoleWin, true); // nodelay causes getch to be a non-blocking call.
													// If no input is ready, getch returns ERR

	bool carryOver = false;
	bool gotInput;
	bool pause = false;
	bool printedDL = false;
	string semaphoreDump, tcbDump, ipcDump, memDump;
	int lengthSemDump = 0;
	//char outSemaphore[256];
	//char outTCB[256];
	int nextNewlineRewind = 0;
	int totalNewlineRewind = 0;
	int PIDtoKill;

	sched->start();

	while (input != 'q')
	{
		//switching = sched->yield();

		if (!pause && !sched->getDL())
		{
			if (switching)
			{
				refresh_table = true;
				sprintf(winBuff, " PID %d is starting\n", sched->getProcessTable()->getHead()->PID);
				sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			}
			else
			{
				sprintf(winBuff, " PID %d is running\n", sched->getProcessTable()->getHead()->PID);
				sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			}
		}

		if (refresh_table)
		{
			tcbDump = sched->dump();
			refresh_table = false;
		}

		if (!carryOver)
		{
			input = wgetch(sched->sched_windows.consoleWin);
		}
		else
		{
			carryOver = false;
		}

		gotInput = true;
		switch (input)
		{
		case 'a':
			memDump = sched->memory_manager->dump();
			for (int i = 0; i < memDump.length(); i++)
			{
				winBuff[i] = memDump[i];
			}
			winBuff[memDump.length()] = '\0';

			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);

			break;
		case 'd':
			tcbDump += "\n";
			for (int i = 0; i < tcbDump.length(); i++)
			{
				//outTCB[i] = tcbDump[i];
				winBuff[i] = tcbDump[i];
			}
			winBuff[tcbDump.length()] = '\0';
			//sprintf(winBuff, "%s", outTCB);
			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			//fill_n(outTCB, tcbDump.length(), '\0');
			break;

		case 's':
			semaphoreDump = sched->io->dump();
			//semaphoreDump += sched->getProcessTable()->getFirst()->my_mailbox->sema_mailbox->dump();
			semaphoreDump += sched->getProcessTable()->getFirst()->next->my_mailbox->sema_mailbox->dump();
			//semaphoreDump += sched->getProcessTable()->getFirst()->next->next->my_mailbox->sema_mailbox->dump();

			semaphoreDump += "\n";
			for (int i = 0; i < semaphoreDump.length(); i++)
			{
				winBuff[i] = semaphoreDump[i];
				//outSemaphore[i] = semaphoreDump[i];
			}
			winBuff[semaphoreDump.length()] = '\0';
			//sprintf(winBuff, "%s", outSemaphore);
			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			//fill_n(outSemaphore, semaphoreDump.length(), '\0');
			break;

		case 't':
			sched->sched_ipc->Message_Delete_All(sched->getProcessTable()->getThreadByPID(1));
			break;

		case 'm':
			ipcDump = sched->sched_ipc->ipc_dump();
			for (int iter = 0; iter * 255 - totalNewlineRewind < ipcDump.length(); iter++)
			{
				for (int i = 0; i < ipcDump.length() && i < 255; i++)
				{
					winBuff[i] = ipcDump[iter * 255 + i - totalNewlineRewind];
					nextNewlineRewind++;
					if (winBuff[i] == '\n')
					{
						nextNewlineRewind = 0;
					}
				}
				if (ipcDump.length() - iter * 256 > 0)
				{
					winBuff[255 - nextNewlineRewind] = '\0';
				}
				else
				{
					winBuff[ipcDump.length() - iter * 256 - nextNewlineRewind] = '\0';
				}
				totalNewlineRewind += nextNewlineRewind;
				//winBuff[ipcDump.length()] = '\0';
				sched->sched_windows.write_window(sched->sched_windows.messageWin, winBuff);
			}
			totalNewlineRewind = 0;
			nextNewlineRewind = 0;

			break;

		case 'u':
			sprintf(winBuff, " Looking for dead threads to remove, Please Wait... \n");
			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			//refresh();
			if (sched->cleanup())
			{
				sprintf(winBuff, " All dead threads have been removed from process table. \n");
				sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
				refresh_table = true;
			}
			else
			{
				sprintf(winBuff, " There are no dead threads in the process table. \n");
				sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			}
			break;

		case 'c':
			refresh();								 // Clear the entire screen (in case it is corrupted)
			wclear(sched->sched_windows.consoleWin); // Clear the WConsole window
			sched->sched_windows.write_window(sched->sched_windows.consoleWin, 1, 1, "Ultima # ");
			break;

			/*case '0':
			
			case '1':
			
			case '2':
				sched->destroy_task(input - '0');
				sched->io->up(input - '0');
				win_queue.Enqueue(sched->getProcessTable()->getThreadByPID(input - '0')->window);
				break;
			*/

		case 'n':
			if (!win_queue.isEmpty())
			{
				sched->create_task("New Task");
				sched->getProcessTable()->getLast()->window = win_queue.Front();
				wclear(win_queue.Front());
				mvwprintw(win_queue.Front(), 1, 2, "Starting Thread...\n");
				box(win_queue.Front(), 0, 0);
				wrefresh(win_queue.Front());
				win_queue.Dequeue();
				printedDL = false;
				refresh_table = true;
			}
			break;

		case 'k':
			PIDtoKill = 0;
			sprintf(winBuff, " Enter PID to kill.\n");
			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			sprintf(winBuff, " You typed: %c\n", input);
			sched->sched_windows.write_window(sched->sched_windows.consoleWin, winBuff);
			do
			{
				input = wgetch(sched->sched_windows.consoleWin);
				if (input != ERR && input >= '0' && input <= '9')
				{
					sprintf(winBuff, " You typed: %c\n", input);
					sched->sched_windows.write_window(sched->sched_windows.consoleWin, winBuff);
					PIDtoKill = PIDtoKill * 10 + input - '0';
					sprintf(winBuff, " Kill PID: %d? (y/n, 0-9 to add more digits)\n", PIDtoKill);
					sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
				}
			} while ((input >= '0' && input <= '9') || input == ERR);

			if (input == 'y' || input == '\n')
			{
				if (sched->destroy_task(PIDtoKill))
				{
					sched->io->up(PIDtoKill);
					if (sched->getProcessTable()->getWindowByPID(PIDtoKill))
					{
						win_queue.Enqueue(sched->getProcessTable()->getWindowByPID(PIDtoKill));
					}
					refresh_table = true;
					sprintf(winBuff, " PID %d has been killed.\n", PIDtoKill);
					sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
				}
				else
				{
					sprintf(winBuff, " No living task with PID: %d found in process table.\n", PIDtoKill);
					sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
				}
			}
			else
			{
				sprintf(winBuff, " Canceled killing thread.\n");
				sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			}
			break;

		case 'l':
			sched->create_lazy_task("Lazy New Task");
			printedDL = false;
			refresh_table = true;
			break;

		case 'p':
			pause = !pause;
			break;

		case 'h':
			sched->sched_windows.display_help(sched->sched_windows.consoleWin);
			break;

		case 'q':
			break;

		case ERR:
			gotInput = false;
			break;
		default:
			sprintf(winBuff, " This should never happen!\n");
			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			break;
		}
		//switching = sched->yield();
		if (gotInput && !carryOver)
		{
			sprintf(winBuff, " You typed: %c\n", input);
			sched->sched_windows.write_window(sched->sched_windows.consoleWin, winBuff);
		}
		wrefresh(sched->sched_windows.consoleWin);
		//switching = sched->yield();
		if (sched->getDL() && !printedDL)
		{ //deadlock check for debugging
			sprintf(winBuff, " Deadlock has occured.\n");
			sched->sched_windows.write_window(sched->sched_windows.logWin, winBuff);
			printedDL = true;
			refresh_table = true;
		}

		//sleep(1);
		switching = sched->yield();
	}

	endwin();
	delete sched;
	return 0;
}