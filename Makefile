
#C compiler
CC = g++

SRC = log.cpp grep.cpp

CC_FLAGS = -std=c++11

EXE = logger

release:$(SRC)
	$(CC)    $(SRC) $(CC_FLAGS) -o $(EXE) 

clean: $(SRC)
	rm -f $(EXE) $(EXE_X) $(EXE).linkinfo 
