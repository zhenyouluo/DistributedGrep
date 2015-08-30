
#C compiler
CC = g++

SRC = log.cpp

CC_FLAGS = -pthread -lz

EXE = logger

release:$(SRC)
	$(CC)    $(SRC) $(CC_FLAGS) -o $(EXE) 

clean: $(SRC)
	rm -f $(EXE) $(EXE_X) $(EXE).linkinfo 
