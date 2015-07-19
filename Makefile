all: nature.exe

helpers.o: helpers.cpp helpers.hpp timestamp.o
	g++ -c helpers.cpp

timestamp.o: timestamp.cpp timestamp.hpp
	g++ -c timestamp.cpp

tests.o: tests.cpp tests.hpp helpers.hpp helpers.o timestamp.hpp timestamp.o
	g++ -c tests.cpp

nature.exe: nature.cpp tests.hpp tests.o helpers.o
	g++ nature.cpp tests.o helpers.o timestamp.o  -o nature.exe
