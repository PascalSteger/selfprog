CFLAGS = -g -O0 #-fpermissive
CXXFLAGS = -g -O0 -std=c++11

all: nature helloworld helloworld64 readwritefile putch devurandom min_devurandom

putch: putch.asm
	nasm -f elf putch.asm


devurandom: devurandom.asm
	nasm -f elf64 devurandom.asm -o devurandom.o
	gcc -o devurandom devurandom.o

min_devurandom: min_devurandom.asm
	nasm -f elf64 min_devurandom.asm -o min_devurandom.o
	ld min_devurandom.o -o min_devurandom
# gcc -o min_devurandom min_devurandom.o

readwritefile: readwritefile.asm
	nasm -f elf64 readwritefile.asm -o readwritefile.o
	ld readwritefile.o -o readwritefile

helloworld: helloworld.asm
	nasm -f elf32 helloworld.asm -o helloworld.o
	gcc -m32 -o helloworld helloworld.o

helloworld64: helloworld64.asm
	nasm -f elf64 helloworld64.asm -o helloworld64.o
	gcc -o helloworld64 helloworld64.o

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

debug: debug.cpp debug.hpp
	g++ $(CXXFLAGS) -c debug.cpp

nature: nature.cpp tests.o timestamp.o intelligence.o similarity.o mysystem.o myfiles.o myrandom.o myoutput.o paths.o myparams.o md5.o debug.o datastructures.hpp debug.hpp
	g++ $(CXXFLAGS) nature.cpp paths.o timestamp.o intelligence.o similarity.o mysystem.o tests.o myfiles.o myrandom.o myoutput.o myparams.o md5.o debug.o -lssl -lcrypto -o nature

clean:
	rm -f *.o *.exe
