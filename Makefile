CXX      := g++
CXXFLAGS :=
LDFLAGS  := -lGL -lglfw -lglut -lGLEW -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSOIL

all: flappy.o
	$(CXX) $(CXXFLAGS) flappy.cpp $(LDFLAGS) -o flappy

clean:
	rm -f *.o flappy

.PHONY: clean
