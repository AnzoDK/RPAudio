CXX := g++-9
CC := gcc
INCLUDES := -I./includes
CXX_FLAGS := -Wall -std=c++17 -pthread
C_FLAGS := -Wall
DEBUG := -g3
EX := 
#LINKER_OPTIONS := -lopenal -lvorbisfile
LINKER_OPTIONS :=
#FINAL_LINKER := -lopenal -lvorbisfile
FINAL_LINKER :=
OBJECTS := rpaudio.o commontools.o test.o
TEST_OBJECTS := rpaudio.o commontools.o rptest.o
#SO_DIRS := -L./includes/ogg -L./includes/libopenal -L./includes/vorbis
SO_DIRS := -Wl,-rpath,./includes/libopenal -L./includes/libopenal -Wl,-rpath,./includes/vorbis -L./includes/vorbis
#SO_DIRS := -L./includes/libopenal/libopenal.so.1 -L./includes/vorbis/libvorbisfile.so.3
#SO_DIRS := -Wl,-rpath,./includes/libopenal -L./includes/libopenal -lopenal -Wl,-rpath,./includes/vorbis -L./includes/vorbis -lvorbisfile
#SO_DIRS :=
LIBS := ./includes/libopenal/libopenal.so.1 ./includes/vorbis/libvorbisfile.so.3
SRC := ./src


release: test.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(OBJECTS) $(LIBS) -o RPtest$(EX) $(SO_DIRS) $(FINAL_LINKER)
	make clean

test: rptest.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(TEST_OBJECTS) $(LIBS) -o RPtest$(EX) $(SO_DIRS) $(LINKER_OPTIONS)
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
