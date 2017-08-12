#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "ai.h"
#include "game.h"

// global settings from commandline
int NumOfBrains = 1000;	// how many brains in population
int NumToKeep = 400;	// how many to keep per generation
int NumOfGenerations = 100;// how many generations to run
int Mutate = 1;		// percentage of mutate
int CreateFlag = 0;	// create new population?



// process the commandline and set global options
int options (int argc, char **argv) {
  int tmp;

  while ((tmp = getopt (argc, argv, "cg:n:m:")) != -1)
    switch (tmp)
      {
      case 'c':
	CreateFlag = 1;
	break;
      case 'g':
	NumOfGenerations = atoi (optarg);
	break;
      case 'n':
	NumOfBrains = atoi (optarg);
	break;
      case 'm':
	Mutate = atoi (optarg);
	break;

      case '?':
	if (optopt == 'g')
	  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (optopt == 'n')
	  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (isprint (optopt))
	  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	else
	  fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
	return 1;
      default:
	abort ();
      }
}



/* creates a random brain */
void CreateOneBrain (struct Brain *A) {
  A->Score = 0;
  A->NumLayers = 10;
  A->SizeLayer = malloc (A->NumLayers * sizeof (int));
  A->SizeLayer[0] = 256;
  A->SizeLayer[1] = 2048;
  A->SizeLayer[2] = 2048;
  A->SizeLayer[3] = 1024;
  A->SizeLayer[4] = 1024;
  A->SizeLayer[5] = 512;
  A->SizeLayer[6] = 256;
  A->SizeLayer[7] = 128;
  A->SizeLayer[8] = 64;
  A->SizeLayer[9] = 4;

  A->Neurons = malloc (A->NumLayers * sizeof (bool *));

  for (int t = 0; t < A->NumLayers; t++)
    {
      *(A->Neurons + t) = malloc (A->SizeLayer[t] * sizeof (bool));
    }

  A->Synapses = malloc (A->NumLayers * sizeof (short int *));

  for (int t = 0; t < (A->NumLayers - 1); t++)
    {
      *(A->Synapses + t) =
	malloc ((A->SizeLayer[t] * A->SizeLayer[t + 1]) * sizeof (short int));
    }

  for (int t = 0; t < (A->NumLayers - 1); t++)
    {
      for (int i = 0; i < (A->SizeLayer[t] * A->SizeLayer[t + 1]); i++)
	{
	  *(*(A->Synapses + t) + i) = (rand () % 1024) - 512;
	}
    }
}



/* creates a population of random brains */
void CreateBrains () {
  for (int i = 0; i < NumOfBrains; i++)
    {
      CreateOneBrain ((Population + i));
    }
}



void SaveOneBrain (struct Brain *A, int i) {
  FILE *file_handel;
  char FileName[15];
  sprintf (FileName, "brain_%d.bin", i);
  int TotalSynapse = A->NumSynapse;
  int NumLayers = A->NumLayers;
  file_handel = fopen (FileName, "wb");
  fwrite ("Br", 1, 2, file_handel);
  fwrite (&NumLayers, 1, sizeof (int), file_handel);
  fwrite (A->SizeLayer, 1, NumLayers * sizeof (int), file_handel);

  for (int t = 0; t < (A->NumLayers - 1); t++)
    {
      int Num = (A->SizeLayer[t] * A->SizeLayer[t + 1]);
      fwrite (*(A->Synapses + t), 1, (Num * sizeof (short int)), file_handel);
    }
  fclose (file_handel);
}



void SaveBrains () {
  for (int i = 0; i < NumOfBrains; i++)
    {
      SaveOneBrain ((Population + i), i);
    }
}



void LoadOneBrain (struct Brain *A, int i) {
  FILE *file_handel;
  char FileName[15];
  char Signture[2];
  int NumLayers = 0;
  sprintf (FileName, "brain_%d.bin", i);
  file_handel = fopen (FileName, "rb");
  if (file_handel == NULL)
    {
      fprintf (stderr, "Error opening file: %s \n", FileName);
      exit (1);
    }
  // FIXME we read but dont check.
  fread (&Signture, 1, 2, file_handel);

  fread (&A->NumLayers, 1, sizeof (int), file_handel);
  A->SizeLayer = malloc (A->NumLayers * sizeof (int));
  fread (A->SizeLayer, 1, A->NumLayers * sizeof (int), file_handel);

  A->Neurons = malloc (A->NumLayers * sizeof (bool *));
  for (int t = 0; t < A->NumLayers; t++)
    {
      *(A->Neurons + t) = malloc (A->SizeLayer[t] * sizeof (bool));
    }

  A->Synapses = malloc (A->NumLayers * sizeof (short int *));

  for (int t = 0; t < (A->NumLayers - 1); t++)
    {
      *(A->Synapses + t) =
	malloc ((A->SizeLayer[t] * A->SizeLayer[t + 1]) * sizeof (short int));
    }

  for (int t = 0; t < (A->NumLayers - 1); t++)
    {
      int Num = (A->SizeLayer[t] * A->SizeLayer[t + 1]);
      fread (*(A->Synapses + t), 1, (Num * sizeof (short int)), file_handel);
    }

  fclose (file_handel);
}



void LoadBrains () {
  for (int i = 0; i < NumOfBrains; i++)
    {
      LoadOneBrain ((Population + i), i);
    }
}



void ThinkLayer (bool * Upper_Layer, int Upper_size,
		 bool * Lower_Layer, int Lower_size,
		 short int *weight) {
  int x = 0;
  for (int Lower_index = 0; Lower_index < Lower_size; Lower_index++)
    {
      x = 0;
      for (int Upper_index = 0; Upper_index < Upper_size; Upper_index++)
	{
	  short int Temp_Weight =
	    *(weight + (Lower_size * Upper_index) + Lower_index);
	  x = x + Upper_Layer[Upper_index] * Temp_Weight;
	}
      if (x > 0)
	Lower_Layer[Lower_index] = true;
      else
	Lower_Layer[Lower_index] = false;
    }
}



void Think (struct Brain *A) {
  for (int i = 0; i < (A->NumLayers - 1); i++)
    {
      bool *UpperLayer = *(A->Neurons + i);
      bool *LowerLayer = *(A->Neurons + i + 1);
      int UpperSize = *(A->SizeLayer + i);
      int LowerSize = *(A->SizeLayer + i + 1);
      short int *Weights = *(A->Synapses + i);
      ThinkLayer (UpperLayer, UpperSize, LowerLayer, LowerSize, Weights);
    }
}



int cmpfunc (const void *a, const void *b) {
  struct Brain A = *(struct Brain *) a;
  struct Brain B = *(struct Brain *) b;
  int Foo = B.Score - A.Score;
  return Foo;
}




void MixBrains (struct Brain *A, struct Brain *B,
		struct Brain *C, int Mutate) {
  for (int t = 0; t < (C->NumLayers - 1); t++)
    {
      for (int i = 0; i < (C->SizeLayer[t] * C->SizeLayer[t + 1]); i++)
	{
	  if ((rand () % 1024) > 512)
	    {
	      *(*(C->Synapses + t) + i) = *(*(A->Synapses + t) + i);
	    }
	  else
	    {
	      *(*(C->Synapses + t) + i) = *(*(B->Synapses + t) + i);
	    }
	  if ((rand () % 100) < (Mutate))
	    {
	      *(*(C->Synapses + t) + i) = (rand () % 1024) - 512;
	    }
	}
    }
}





void NextGeneration () {
  qsort (Population, NumOfBrains, sizeof (struct Brain), cmpfunc);

  printf ("Best Score: %d\n", (Population)->Score);

  // Create next generation from best "NumToKeep" brains.
  for (int f = (NumOfBrains - 1); f > NumToKeep; f = f - 1)
    {
      struct Brain *A = (Population + (rand () % NumToKeep));
      struct Brain *B = (Population + (rand () % NumToKeep));
      struct Brain *C = (Population + f);
      MixBrains (A, B, C, Mutate);
    }

  // Create 10 random brains.
  for (int f = 0; f < 10; f++)
    {
      struct Brain *A = (Population + (rand () % 9));
      struct Brain *B = (Population + (rand () % 9));
      struct Brain *C = (Population + NumToKeep + f);
      MixBrains (A, B, C, 99); //very high mutate
    }
}



int main (int argc, char **argv) {
  options (argc, argv);
  Population = malloc (NumOfBrains * sizeof (struct Brain));

  if (CreateFlag == 1)
    {
      CreateBrains ();
      SaveBrains ();
      printf ("Made %d brains\n", NumOfBrains);
    }
  else
    {
      LoadBrains ();
      printf ("Loaded %d brains\n", NumOfBrains);
      printf ("Ready to run brains\n");

      for (int g = 0; g < NumOfGenerations; g++)
	{

	  for (int i = 0; i < NumOfBrains; i++)
	    {
	      Play ((Population + i));
	    }
	  NextGeneration ();
	}
      SaveBrains ();
    }
}
