all: ssp routing
routing: routing.o
	g++ -std=c++0x routing.o -o routing

routing.o: routing.cpp
	g++ -std=c++0x -c routing.cpp

ssp: Dijkstra.o
	g++ -std=c++0x Dijkstra.o -o ssp

Dijkstra.o: Dijkstra.cpp
	g++ -std=c++0x -c Dijkstra.cpp

clean: 
	rm -rf *.o ssp routing
