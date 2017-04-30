#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "ai.h"



// global settings from commandline
int NumOfBrains = 1000;
int NumOfGenerations = 100;
int Mutate = 1;			// percentage of mutate
int CreateFlag = 0;		// do we need to create new population?



// process the commandline and set global options
int options(int argc, char **argv)
{
    int tmp;

    while ((tmp = getopt(argc, argv, "cg:n:m:")) != -1)
	switch (tmp) {
	case 'c':
	    CreateFlag = 1;
	    break;
	case 'g':
	    NumOfGenerations = atoi(optarg);
	    break;
	case 'n':
	    NumOfBrains = atoi(optarg);
	    break;
	case 'm':
	    Mutate = atoi(optarg);
	    break;

	case '?':
	    if (optopt == 'g')
		fprintf(stderr, "Option -%c requires an argument.\n",
			optopt);
	    else if (optopt == 'n')
		fprintf(stderr, "Option -%c requires an argument.\n",
			optopt);
	    else if (isprint(optopt))
		fprintf(stderr, "Unknown option `-%c'.\n", optopt);
	    else
		fprintf(stderr, "Unknown option character `\\x%x'.\n",
			optopt);
	    return 1;
	default:
	    abort();
	}
}



/* creates a population of random brains */
void CreateBrains()
{
    Population = malloc(NumOfBrains * sizeof(struct Brain));

    for (int i = 0; i < NumOfBrains; i++) {
	(Population + i)->Score = 0;
	(Population + i)->NumLayers = 6;
	(Population + i)->SizeLayer =
	    malloc((Population + i)->NumLayers * sizeof(int));
	(Population + i)->SizeLayer[0] = 192;
	(Population + i)->SizeLayer[1] = 128;
	(Population + i)->SizeLayer[2] = 128;
	(Population + i)->SizeLayer[3] = 64;
	(Population + i)->SizeLayer[4] = 32;
	(Population + i)->SizeLayer[5] = 4;

	int TotalNeurons = 0;
	for (int t = 0; t < (Population + i)->NumLayers; t++) {
	    TotalNeurons = TotalNeurons + (Population + i)->SizeLayer[t];
	}
	(Population + i)->Neuron = malloc(TotalNeurons * sizeof(bool));

	int TotalSynapse = 0;
	for (int t = 0; t < (Population + i)->NumLayers; t++) {
	    TotalSynapse =
		TotalSynapse +
		((Population + i)->SizeLayer[t] * (Population +
						   i)->SizeLayer[t + 1]);
	}
	(Population + i)->NumSynapse = TotalSynapse;
	(Population + i)->Synapse = malloc(TotalSynapse * sizeof(float));

	for (int t = 0; t <= TotalSynapse; t++) {
	    *((Population + i)->Synapse + t) = (rand() % 1024) - 512;
	}
    }
}



void SaveBrains()
{
    for (int i = 0; i < NumOfBrains; i++) {
	FILE *file_handel;
	char FileName[15];
	sprintf(FileName, "brain_%d.bin", i);
	int TotalSynapse = (Population + i)->NumSynapse;
	int NumLayers = (Population + i)->NumLayers;
	file_handel = fopen(FileName, "wb");
	fwrite("Br", 1, 2, file_handel);
	fwrite(&NumLayers, 1, sizeof(int), file_handel);
	fwrite((Population + i)->SizeLayer, 1, NumLayers * sizeof(int),
	       file_handel);
	fwrite(&TotalSynapse, 1, sizeof(int), file_handel);
	fwrite((Population + i)->Synapse, 1,
	       (TotalSynapse * sizeof(float)), file_handel);
	fclose(file_handel);
    }
}



int main(int argc, char **argv)
{
    options(argc, argv);
    if (CreateFlag == 1) {
	CreateBrains();
	SaveBrains();
    } else {
	printf("run brains\n");
    }
}
