all: ai 

ai: ai.c ai.h game.c game.h
	gcc -g game.c ai.c -o ai

.PHONY: all

