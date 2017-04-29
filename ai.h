/**************************************************************************
 * 
 */


// a single brain has a score and an array of neuron weightings 
struct brain {
	int score;
	float *neurons;
};


// global options set from commandline
extern int NumOfBrains;
extern int NumOfGenerations;
extern int Mutate;


// the population is an array of brains
struct brain *population;


