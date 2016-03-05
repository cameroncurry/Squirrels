CXX=g++
MPICXX=mpic++

EXE=squirrels

PROVIDED_DIR=../provided/

SRC=$(PROVIDED_DIR)ran2.c $(PROVIDED_DIR)squirrel-functions.c squirrel_actor.cpp
OBJ = $(addsuffix .o,$(basename $(SRC)))

x=$(subst $(PROVIDED_DIR),,$(OBJ))

all: $(OBJ)
	$(MPICXX) -o $(EXE) -I$(PROVIDED_DIR) $(notdir $(OBJ)) $(EXE).cpp

.cpp.o:
	$(CXX) -c -I$(PROVIDED_DIR) -o $@ $<

.c.o:
	$(CXX) -c -o $(notdir $@) $<
