CXXFLAGS += --std=c++17 -g


SOURCES = \
    main.cpp \
    vtdump.cpp

vtdump: CC = g++
vtdump: LDLIBS += -lm
vtdump: $(SOURCES:.cpp=.o)
