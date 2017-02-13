CXX=clang++

CXXFLAGS+=-std=c++14
CXXFLAGS+=-Werror
CXXFLAGS+=-Wall
CXXFLAGS+=-Wextra

LDLIBS+=-lstdc++
LDLIBS+=-lboost_program_options
LDLIBS+=-lboost_system

ifdef RELEASE
 CXXFLAGS+=-O3
else
 CXXFLAGS+=-g3
endif
	
%: %.cpp

sources=$(wildcard *.cpp)
objects=$(patsubst %.cpp,%.o,$(sources))

gpack: $(objects)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $@ $(objects)


.PHONY: clean
clean:
	$(RM) gpack *.o
