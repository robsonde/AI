# AI
AI engine for 2048

# Introduction
This is a basic AI neural network genetic algorithm toy.
The aim is to solve the game 2048


# How to use

create a new population of 1000 brains 
> ./ai -c -n 1000

run 100 generarions for the 1000 brains with a mutate of 5%
> ./ai -n 1000 -g 100 -m 5



# Brain Files
Two byte header of "Br"
Four byte int , number of layers
A list of four byte ints, each is the size of a layer
Four byte int, the total number of Synapses
A list of four byte floats, each if the weighting of a Synapse



# How the neural network works


