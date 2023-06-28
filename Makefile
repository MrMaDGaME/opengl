CXX      := g++
CXXFLAGS :=
LDFLAGS  := -lGL -lglfw

all: flappy.o
	$(CXX) $(CXXFLAGS) flappy.cpp $(LDFLAGS) -o flappy

clean:
	rm -f *.o flappy

.PHONY: clean
