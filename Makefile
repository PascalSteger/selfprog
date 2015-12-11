CFLAGS = #-fpermissive

all: nature.exe

helpers.o: helpers.cpp helpers.hpp timestamp.o
	g++ $(CFLAGS) -c helpers.cpp

timestamp.o: timestamp.cpp timestamp.hpp
	g++ $(CFLAGS) -c timestamp.cpp

tests.o: tests.cpp tests.hpp helpers.hpp helpers.o timestamp.hpp timestamp.o
	g++ $(CFLAGS) -c tests.cpp

intelligence.o: intelligence.cpp intelligence.hpp
	g++ $(CFLAGS) -c intelligence.cpp

md5.o: md5.cpp md5.hpp
	g++ $(CFLAGS) -c md5.cpp

test.exe: test.cpp
	g++ $(CFLAGS) test.cpp -o test.exe

nature.exe: nature.cpp tests.hpp tests.o intelligence.o helpers.o helpers.hpp md5.o
	g++ $(CFLAGS) nature.cpp tests.o helpers.o timestamp.o intelligence.o -lssl -o nature.exe

clean:
	rm -f *.o *.exe
