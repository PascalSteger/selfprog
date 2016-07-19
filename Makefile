CFLAGS = -g -O0 #-fpermissive
CXXFLAGS = -g -O0 -std=c++11

all: nature

timestamp: timestamp.cpp timestamp.hpp
	g++ $(CXXFLAGS) -c timestamp.cpp

intelligence: intelligence.cpp intelligence.hpp
	g++ $(CXXFLAGS) -c intelligence.cpp

md5: md5.cpp md5.hpp
	g++ $(CXXFLAGS) -c md5.cpp

similarity: similarity.cpp similarity.hpp datastructures.hpp
	g++ $(CXXFLAGS) -c similarity.cpp

mysystem: mysystem.cpp timestamp.o mysystem.hpp timestamp.hpp datastructures.hpp
	g++ $(CXXFLAGS) -c mysystem.cpp

tests: tests.cpp tests.hpp mysystem.hpp
	g++ $(CXXFLAGS) -c tests.cpp

myfiles: myfiles.cpp myfiles.hpp
	g++ $(CXXFLAGS) -c myfiles.cpp

myrandom: myrandom.cpp myrandom.hpp
	g++ $(CXXFLAGS) -c myrandom.cpp

myoutput: myoutput.cpp myoutput.hpp
	g++ $(CXXFLAGS) -c myoutput.cpp

paths: paths.cpp paths.hpp
	g++ $(CXXFLAGS) -c paths.cpp

myparams: myparams.cpp myparams.hpp
	g++ $(CXXFLAGS) -c myparams.cpp

nature: nature.cpp tests.o timestamp.o intelligence.o similarity.o mysystem.o myfiles.o myrandom.o myoutput.o paths.o myparams.o md5.o datastructures.hpp debug.hpp
	g++ $(CXXFLAGS) nature.cpp paths.o timestamp.o intelligence.o similarity.o mysystem.o tests.o myfiles.o myrandom.o myoutput.o myparams.o md5.o -lssl -lcrypto -o nature

clean:
	rm -f *.o *.exe
