
CXX=g++
MPICXX=mpic++

EXE=squirrels

PROVIDED_DIR=../provided/

SRC=$(PROVIDED_DIR)ran2.c $(PROVIDED_DIR)squirrel-functions.c
OBJ=ran2.o squirrel-functions.o

all: $(OBJ)
	$(MPICXX) -o $(EXE) -I$(PROVIDED_DIR) $(OBJ) $(EXE).cpp

clean:
	rm -f *.o
	rm -f squirrels

%.o:
	$(CXX) -c -o $@ $(PROVIDED_DIR)$(@:.o=.c)
