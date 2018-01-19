/**************************************************************************
 * 
 */


// a single brain has a score and an array of neurons and weightings 
struct Brain
{
  int Score;			//score of brain
  int NumLayers;		// how many layers of neurons
  int Header_Size;		// how Bytes make up the header 
  int *SizeLayer;		// how many neurons at layer
  bool **Neurons;		// collection of Neurons
  short int **Synapses;		// weighting between neurons
};



// global options set from commandline
extern int NumOfBrains;
extern int NumOfGenerations;
extern int Mutate;


// the population is an array of brains
struct Brain *Population;

void Think (struct Brain *A);
