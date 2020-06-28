CXX := g++
C := gcc
INCLUDES := -I./includes
CXX_FLAGS := -Wall -std=c++17 -pthread
DEBUG := -g3
LINKER_OPTIONS := -lopenal -lvorbisfile
OBJECTS := test.o rpaudio.o
SRC := ./src

release: test.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(OBJECTS) -o RPtest $(LINKER_OPTIONS)
	make clean

test.o: rpaudio.o
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/test.cpp -o test.o $(LINKER_OPTIONS)

rpaudio.o:
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/rpaudio.cpp -o rpaudio.o $(LINKER_OPTIONS)

clean:
	-rm *.o
