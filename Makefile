ultima: main.o tcb.o scheduler.o semaphore.o windows.o
	g++ -std=c++11 main.o tcb.o scheduler.o semaphore.o windows.o -lpthread -lncurses -o ultima

main.o: main.cpp scheduler.h windows.h
	g++ -std=c++11 -c main.cpp

semaphore.o: semaphore.h semaphore.cc scheduler.h queue.h
	g++ -std=c++11 -c semaphore.cc

scheduler.o: scheduler.h scheduler.cc tcb.h semaphore.h thread.h
	g++ -std=c++11 -c scheduler.cc

tcb.o: tcb.h tcb.cc thread.h
	g++ -std=c++11 -c tcb.cc
	
windows.o: windows.h windows.cc
	g++ -std=c++11 -c windows.cc

clean:
	rm *.o ultima
