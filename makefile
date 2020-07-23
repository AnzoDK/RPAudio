CXX := g++
CC := gcc
INCLUDES := -I./includes
CXX_FLAGS := -Wall -std=c++17 -pthread
C_FLAGS := -Wall
DEBUG := -g3
EX := 
LINKER_OPTIONS := -lopenal -lvorbisfile
OBJECTS := test.o rpaudio.o commontools.o
TEST_OBJECTS := rpaudio.o commontools.o rptest.o
#SO_DIRS := -L./includes/ogg -L./includes/libopenal -L./includes/vorbis -Wl,-rpath=./includes/
SO_DIRS := 
SRC := ./src


release: test.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(OBJECTS) -o RPtest$(EX) $(SO_DIRS) $(LINKER_OPTIONS)
	make clean

test: rptest.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(TEST_OBJECTS) -o RPtest$(EX) $(SO_DIRS) $(LINKER_OPTIONS)
	make clean

rptest.o: rpaudio.o
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/rptest.cpp -o rptest.o $(SO_DIRS) $(LINKER_OPTIONS)

test.o: rpaudio.o 
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/test.cpp -o test.o $(SO_DIRS) $(LINKER_OPTIONS)

rpaudio.o: commontools.o
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/rpaudio.cpp -o rpaudio.o $(SO_DIRS) $(LINKER_OPTIONS)

commontools.o:
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/commontools.cpp -o commontools.o $(SO_DIRS) $(LINKER_OPTIONS)

clean:
	-rm *.o
