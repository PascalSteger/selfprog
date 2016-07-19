CFLAGS = -g -O0 #-fpermissive
CXXFLAGS = -g -O0 -std=c++11

all: nature

timestamp.o: timestamp.cpp timestamp.hpp
	g++ $(CXXFLAGS) -c timestamp.cpp

intelligence.o: intelligence.cpp intelligence.hpp
	g++ $(CXXFLAGS) -c intelligence.cpp

md5.o: md5.cpp md5.hpp
	g++ $(CXXFLAGS) -c md5.cpp

similarity: similarity.cpp similarity.hpp datastructures.hpp
	g++ $(CXXFLAGS) -c similarity.cpp

mysystem: mysystem.cpp timestamp.o mysystem.hpp timestamp.hpp datastructures.hpp
	g++ $(CXXFLAGS) -c mysystem.cpp

nature: nature.cpp timestamp.o intelligence.o similarity.o mysystem.o datastructures.hpp debug.hpp
	g++ $(CXXFLAGS) nature.cpp timestamp.o intelligence.o similarity.o mysystem.o -lssl -lcrypto -o nature

clean:
	rm -f *.o *.exe
