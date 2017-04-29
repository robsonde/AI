all: ai 

ai: ai.c ai.h 
	gcc -g ai.c -o ai

.PHONY: all

