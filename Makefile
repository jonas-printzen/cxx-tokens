# Predictable shell behavior
.SHELLFLAGS=-cuef -o pipefail
SHELL=/bin/bash

# Files to work with
HEADERS:=$(wildcard src/*.hpp)
SOURCES:=$(wildcard src/*.cpp)

what:
	@echo "toks:   Optimized release-build"
	@echo "toks_g: Debug build"

toks: ${SOURCES:%.cpp=%.o}
	${CXX} -o $@ $^

#==== Release-compile ====
%.o: %.cpp ${HEADERS}
	g++ -std=c++20 -O3 -Wall -o $@ -Isrc -c $<

#==== Debug-compile ====
toks_g: ${SOURCES:%.cpp=%_g.o}
	${CXX} -g -o $@ $^

%_g.o: %.cpp ${HEADERS}
	g++ -std=c++20 -g -fno-inline -Wall -o $@ -Isrc -c $<

src/toks.o,src/toks_g.o: src/solution.icpp

clean:
	rm -f toks toks_g
	find src -name '*.o' -delete
