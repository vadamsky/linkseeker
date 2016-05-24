#############################################################################
# Makefile for building: udpnetobjectseeker
#############################################################################

MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT -fPIE $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W -D_REENTRANT -fPIC #-pthread -static $(DEFINES)
INCPATH       = -I. -I/usr/include -I/usr/include/boost
LINK          = g++
LIBS          = -L./lib -lboost_signals -lboost_system -lboost_thread #-pthread

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		udpnetobjectseeker.cpp \
		loaderfile.cpp 
OBJECTS       = main.o \
		udpnetobjectseeker.o \
		loaderfile.o
QMAKE_TARGET  = udpnetobjectseeker

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: main1.o main2.o libudpnetobjectseeker.so #libudpnetobjectseeker.a
	$(CXX) -o test1 main1.o -L. -ludpnetobjectseeker $(LIBS)
	$(CXX) -o test2 main2.o -L. -ludpnetobjectseeker $(LIBS)
#	$(CXX) -pthread -o binary main.o udpnetobjectseeker.o loaderfile.o $(LIBS)

main1.o: main1.cpp udpnetobjectseeker.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main1.o main1.cpp

main2.o: main2.cpp udpnetobjectseeker.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main2.o main2.cpp

libudpnetobjectseeker.a: udpnetobjectseeker.o loaderfile.o udpnetobjectseekerworker.o
	ar cqs libudpnetobjectseeker.a udpnetobjectseeker.o udpnetobjectseekerworker.o loaderfile.o #libboost_thread.a libboost_signals.a libboost_system.a
	ranlib libudpnetobjectseeker.a

libudpnetobjectseeker.so: udpnetobjectseeker.o loaderfile.o udpnetobjectseekerworker.o
	$(CXX) -shared -o libudpnetobjectseeker.so udpnetobjectseeker.o udpnetobjectseekerworker.o loaderfile.o #libboost_thread.a libboost_signals.a libboost_system.a

udpnetobjectseeker.o: udpnetobjectseeker.cpp udpnetobjectseeker.h \
		udpnetobjectseekerworker.h \
		udpsender.h \
		loaderfile.h \
		udpreceiver.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -L/usr/lib -lboost_thread -lboost_thread-mt -lboost_system -lboost_signals -o udpnetobjectseeker.o udpnetobjectseeker.cpp

udpnetobjectseekerworker.o: udpnetobjectseekerworker.cpp udpnetobjectseekerworker.h \
		udpsender.h \
		loaderfile.h \
		udpreceiver.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -L/usr/lib -lboost_thread -lboost_thread-mt -lboost_system -lboost_signals -o udpnetobjectseekerworker.o udpnetobjectseekerworker.cpp

loaderfile.o: loaderfile.cpp loaderfile.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o loaderfile.o loaderfile.cpp

clean:
	rm -f *.o *.so *.a test*

install:
	install libudpnetobjectseeker.so /usr/lib
	install udpnetobjectseeker.h /usr/include

uninstall:
	rm -rf /usr/lib/libudpnetobjectseeker.so
	rm -rf /usr/include/udpnetobjectseeker.h
