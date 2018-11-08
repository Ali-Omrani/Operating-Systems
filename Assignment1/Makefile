
all: main

main: main.o worker.o
	g++ -o main main.o worker.o

main.o: main.cpp worker.o
	g++ -c main.cpp

worker.o: worker.h worker.cpp
	g++ -c worker.cpp

clean:
	rm *.o main
