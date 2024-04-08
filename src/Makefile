CXX = g++
CXXFLAGS = -std=c++11 -w

.PHONY: all clean

all: assembler

assembler: assembler.o
	$(CXX) $(CXXFLAGS) -o assembler assembler.o

assembler.o: assembler.cpp
	$(CXX) $(CXXFLAGS) -c assembler.cpp

clean:
	$(RM) assembler.o assembler

run:
	./assembler input.txt output.txt

check_version:
	$(CXX) $(CXXFLAGS) -dM -E - < /dev/null | grep __cplusplus | awk '{print $3}' | xargs test 201103L -le