CFLAGS = -g -O0 #-fpermissive
CXXFLAGS = -g -O0 -std=c++11

all: nature

timestamp.o: timestamp.cpp timestamp.hpp
	g++ $(CXXFLAGS) -c timestamp.cpp

intelligence.o: intelligence.cpp intelligence.hpp
	g++ $(CXXFLAGS) -c intelligence.cpp

md5.o: md5.cpp md5.hpp
	g++ $(CXXFLAGS) -c md5.cpp

nature: nature.cpp timestamp.o intelligence.o
	g++ $(CXXFLAGS) nature.cpp timestamp.o intelligence.o -lssl -lcrypto -o nature

clean:
	rm -f *.o *.exe
