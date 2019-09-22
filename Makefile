CXXFLAGS += --std=c++17 -g

all: scat effect

scat: scat.o

SOURCES = \
    canoe.cpp \
    chargen.cpp \
    effect.cpp \
    figlet.cpp \
    goto.cpp \
    lptest.cpp \
    main.cpp \
    maze.cpp \
    options.cpp \
    screen.cpp \
    wipe.cpp

chargen.cpp effect.cpp screen.cpp wipe.cpp: \
	coord.h

canoe.cpp chargen.cpp effect.cpp figlet.cpp lptest.cpp main.cpp \
maze.cpp options.cpp wipe.cpp: \
	effect.h

goto.cpp screen.cpp wipe.cpp: \
	goto.h

effect.cpp screen.cpp wipe.cpp: \
	screen.h

effect: CC = g++
effect: LDLIBS += -lm
effect: $(SOURCES:.cpp=.o)
