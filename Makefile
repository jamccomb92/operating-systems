ultima: main.o tcb.o scheduler.o semaphore.o UltimaWindows.o ipc.o memoryManager.o
	g++ -std=c++11 main.o tcb.o scheduler.o semaphore.o UltimaWindows.o ipc.o memoryManager.o -lpthread -lncurses -o ultima

main.o: main.cpp scheduler.h UltimaWindows.h
	g++ -std=c++11 -c main.cpp

semaphore.o: semaphore.h semaphore.cc scheduler.h queue.h
	g++ -std=c++11 -c semaphore.cc

scheduler.o: scheduler.h scheduler.cc tcb.h semaphore.h thread.h ipc.h UltimaWindows.h memoryManager.h
	g++ -std=c++11 -c scheduler.cc
	
ipc.o: thread.h ipc.h ipc.cc message.h message_type.h scheduler.h
	g++ -std=c++11 -c ipc.cc

tcb.o: tcb.h tcb.cc thread.h mailbox.h
	g++ -std=c++11 -c tcb.cc
	
UltimaWindows.o: UltimaWindows.h UltimaWindows.cc
	g++ -std=c++11 -c UltimaWindows.cc

memoryManager.o: memoryManager.h memoryManager.cc listNode.h
	g++ -std=c++11 -c memoryManager.cc 

clean:
	rm *.o ultima
