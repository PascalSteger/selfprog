CFLAGS = -g -O0 #-fpermissive
CXXFLAGS = -g -O0 -std=c++11

all: nature.exe

timestamp.o: timestamp.cpp timestamp.hpp
	g++ $(CXXFLAGS) -c timestamp.cpp

tests.o: tests.cpp tests.hpp timestamp.hpp timestamp.o
	g++ $(CXXFLAGS) -c tests.cpp

intelligence.o: intelligence.cpp intelligence.hpp
	g++ $(CXXFLAGS) -c intelligence.cpp

md5.o: md5.cpp md5.hpp
	g++ $(CXXFLAGS) -c md5.cpp

test.exe: test.cpp
	g++ $(CXXFLAGS) test.cpp -o test.exe

nature.exe: nature.cpp tests.hpp tests.o intelligence.o md5.o
	g++ $(CXXFLAGS) nature.cpp tests.o timestamp.o intelligence.o -lssl -o nature.exe

clean:
	rm -f *.o *.exe
