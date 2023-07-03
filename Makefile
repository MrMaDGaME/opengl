CXX      := g++
CXXFLAGS :=
LDFLAGS  := -lGL -lglfw -lglut -lGLEW -lSOIL

all: flappy.o main.o
	$(CXX) $(CXXFLAGS) flappy.cpp main.cpp $(LDFLAGS) -o flappy

clean:
	rm -f *.o flappy

.PHONY: clean
