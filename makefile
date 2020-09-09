CXX := g++
CC := gcc
INCLUDES := -I./includes
CXX_FLAGS := -Wall -std=c++17 -pthread
CXX_FLAGS_LIB := -Wall -std=c++17 -pthread -shared
C_FLAGS := -Wall
DEBUG := -g3
EX := 
OS :=Linux
LINKER_OPTIONS :=
LIB :=
#FINAL_LINKER := -lopenal -lvorbisfile
#FINAL_LINKER := -lvorbis
OBJECTS := rpaudio.o commontools.o test.o
OBJECTS_LIB := rpaudio.o commontools.o
TEST_OBJECTS := rpaudio.o commontools.o rptest.o
#SO_DIRS := -L./includes/ogg -L./includes/libopenal -L./includes/vorbis
#SO_DIRS := -L./includes/libopenal/libopenal.so.1 -L./includes/vorbis/libvorbisfile.so.3
#SO_DIRS := -Wl,-rpath,./includes/libopenal -L./includes/libopenal -lopenal -Wl,-rpath,./includes/vorbis -L./includes/vorbis -lvorbisfile
#SO_DIRS :=
ifeq ($(OS), Linux)
LIBS := ./includes/libopenal/libopenal.so.1 ./includes/vorbisfile/libvorbisfile.so.3 ./includes/oggvorbis/libogg.so.0
SO_DIRS := -Wl,-rpath,./includes/libopenal -L./includes/libopenal -Wl,-rpath,./includes/vorbis -L./includes/vorbis -Wl,-rpath,./includes/vorbisfile -L./includes/vorbisfile -Wl,-rpath,./includes/oggvorbis -L./includes/oggvorbis 
ifeq ($(LIB), 1)
LIB_OPTIONS := -fPIC
SO_DIRS += -Wl,-rpath,./includes/libsndio -L./includes/libsndio
LIBS += ./includes/libsndio/libsndio.so.7.0
EX := .so
endif
endif

ifeq ($(OS), Windows)
SO_DIRS := -Wl,-rpath,./includes/libopenal -L./includes/libopenal -Wl,-Bdynamic -Wl,-rpath,./includes/vorbis -L./includes/vorbis -Wl,-rpath,./includes/vorbisfile -L./includes/vorbisfile -Wl,-rpath,./includes/oggvorbis -L./includes/oggvorbis
#LIBS := ./includes/vorbisfile/libvorbisfile.dll ./includes/oggvorbis/libogg.dll ./includes/vorbis/libvorbis.dll
FINAL_LINKER := -lOpenAL32 -logg -lvorbisfile-3
CXX_FLAGS +=
ifeq ($(LIB), 1)
LIB_OPTIONS := -fPIC
CXX_FLAGS_LIB := --shared
FINAL_LINKER += -lstdc++ -Wl,--out-implib,librpaudio.a 
CXX_FLAGS += -DBUILDING_EXAMPLE_DLL
EX := .dll
endif
endif
SRC := ./src


release: test.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(OBJECTS) $(LIBS) -o RPtest$(EX) $(SO_DIRS) $(FINAL_LINKER)
	make clean


lib: rpaudio.o
	$(CXX) $(CXX_FLAGS_LIB) $(INCLUDES) $(DEBUG) $(OBJECTS_LIB) $(LIBS) -o rpaudio$(EX) $(SO_DIRS) $(FINAL_LINKER)
	make clean

test: rptest.o
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(DEBUG) $(TEST_OBJECTS) $(LIBS) -o RPtest$(EX) $(SO_DIRS) $(LINKER_OPTIONS)
	make clean

rptest.o: rpaudio.o
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/rptest.cpp -o rptest.o $(LINKER_OPTIONS)

test.o: rpaudio.o 
	$(CXX) -c $(CXX_FLAGS) $(DEBUG) $(SRC)/test.cpp -o test.o $(LINKER_OPTIONS)

rpaudio.o: commontools.o
	$(CXX) -c $(LIB_OPTIONS) $(CXX_FLAGS) $(DEBUG) $(SRC)/rpaudio.cpp -o rpaudio.o $(LINKER_OPTIONS)

commontools.o:
	$(CXX) -c $(LIB_OPTIONS) $(CXX_FLAGS) $(DEBUG) $(SRC)/commontools.cpp -o commontools.o $(LINKER_OPTIONS)

clean:
	-rm *.o
