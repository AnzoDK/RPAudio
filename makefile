CXX := g++
CC := gcc
INCLUDES := -I./includes
CXX_FLAGS := -Wall -std=c++17 -pthread
C_FLAGS := -Wall
DEBUG := -g3
EX := ""
LINKER_OPTIONS := -lopenal -lvorbisfile
OBJECTS := test.o rpaudio.o commontools.o
SRC := ./src


release: test.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(OBJECTS) -o RPtest$(EX) $(LINKER_OPTIONS)
	make clean

test: rptest.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(OBJECTS) -o RPtest$(EX) $(LINKER_OPTIONS)
	make clean

rptest.o: rpaudio.o
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/rptest.cpp -o rptest.o $(LINKER_OPTIONS)

test.o: rpaudio.o 
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/test.cpp -o test.o $(LINKER_OPTIONS)

rpaudio.o: commontools.o
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/rpaudio.cpp -o rpaudio.o $(LINKER_OPTIONS)

commontools.o:
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/commontools.cpp -o commontools.o $(LINKER_OPTIONS)

clean:
	-rm *.o
