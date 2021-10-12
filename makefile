COMPILE = gcc
EXE = pthreads
SOURCE = pthreads.cpp
PTH = pthread

compile:
	$(COMPILE) -o $(EXE) $(SOURCE) -lm -lstdc++ -pthread
	
exe:
	./$(EXE)
