#Makefile for AI assignment
#David Chan, Dec 2014

#File setup options
SOURCES=Eval.cpp Minimax.cpp Move.cpp Agent.cpp ChineseCheckersBoard.cpp main.cpp
EXECUTABLE=Agent

#Compiler Options
CC=clang++
CFLAGS=-c -std=c++11 -Weverything -Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-sign-conversion -Wno-unused-parameter -Os
LDFLAGS=-pthread
OBJECTS=$(SOURCES:.cpp=.o)


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
