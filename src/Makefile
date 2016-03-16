CXX=mpic++ -w

EXE=squirrels

SQURL_DIR=../provided/
SQURL_SRC=$(SQURL_DIR)ran2.c $(SQURL_DIR)squirrel-functions.c
SQURL_OBJ=$(SQURL_SRC:.c=.o)

PROC_POOL_DIR=../processpool/
PROC_POOL_SRC=$(PROC_POOL_DIR)pool.c
PROC_POOL_OBJ=$(PROC_POOL_SRC:.c=.o)

ACTOR_DIR=./actors/
ACTOR_SRC=$(ACTOR_DIR)actor.cpp $(ACTOR_DIR)master_actor.cpp $(ACTOR_DIR)grid_actor.cpp $(ACTOR_DIR)squirrel_actor.cpp
ACTOR_OBJ=$(ACTOR_SRC:.cpp=.o)

all: $(PROC_POOL_OBJ) $(SQURL_OBJ) $(ACTOR_OBJ)
	$(CXX) -o $(EXE) -I$(PROC_POOL_DIR) $(notdir $(PROC_POOL_OBJ)) -I$(SQURL_DIR) $(notdir $(SQURL_OBJ)) -I$(ACTOR_DIR) $(notdir $(ACTOR_OBJ)) $(EXE).cpp

clean:
	rm -f *.o $(EXE)

.cpp.o:
	$(CXX) -c -I$(SQURL_DIR) -o $(notdir $@) $<

.c.o:
	$(CXX) -c -o $(notdir $@) $<
