CXX=mpic++ -w

EXE=squirrels

SQURL_DIR=../provided/
SQURL_SRC=$(SQURL_DIR)ran2.c $(SQURL_DIR)squirrel-functions.c
SQURL_OBJ=$(SQURL_SRC:.c=.o)

PROC_POOL_DIR=../processpool/
PROC_POOL_SRC=$(PROC_POOL_DIR)pool.c
PROC_POOL_OBJ=$(PROC_POOL_SRC:.c=.o)

SRC=actor.cpp master_actor.cpp
OBJ=$(SRC:.cpp=.o)


all: $(PROC_POOL_OBJ) $(OBJ)
	$(CXX) -o $(EXE) -I$(PROC_POOL_DIR) $(notdir $(PROC_POOL_OBJ)) $(OBJ) $(EXE).cpp

clean:
	rm -f *.o $(EXE)

.cpp.o:
	$(CXX) -c -I$(SQURL_DIR) -I$(PROC_POOL_DIR) -o $@ $<

.c.o:
	$(CXX) -c -o $(notdir $@) $<
