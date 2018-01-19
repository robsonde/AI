#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "ai.h"
#include "game.h"


// global settings from commandline
int NumOfBrains = 100;		// how many brains in population
int NumToKeep = 20;		// how many to keep per generation
int NumToRegenerate = 20;	// how many to keep per generation
int NumOfGames = 10;		// how many games to average
int Mutate = 1;			// percentage of mutate
int CreateFlag = 0;		// create new population?
int HeaderSize = 0; 		// Size of file header


struct BrainScore
{
  int Number;	//Index to which Brain file
  int Score;	//Score of given Brain
};



// process the commandline and set global options
int options (int argc, char **argv) {
  int tmp;

  while ((tmp = getopt (argc, argv, "cg:n:m:k:r:")) != -1)
    switch (tmp)
      {
      case 'c':
        CreateFlag = 1;
        break;
      case 'g':
        NumOfGames = atoi (optarg);
        break;
      case 'n':
        NumOfBrains = atoi (optarg);
        break;
      case 'm':
        Mutate = atoi (optarg);
        break;
      case 'k':
        NumToKeep = atoi (optarg);
        break;
      case 'r':
        NumToRegenerate = atoi (optarg);
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
  A->Header_Size=(A->NumLayers*4)+10;
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
}





void RandomizeOneBrain (struct Brain *A) {
  A->Score=0;
  for (int t = 0; t < (A->NumLayers - 1); t++)
  {
    for (int i = 0; i < (A->SizeLayer[t] * A->SizeLayer[t + 1]); i++)
    {
      *(*(A->Synapses + t) + i) = (rand () % 1024) - 512;
    }
  }
}






void SaveOneBrain (struct Brain *A, int i) {
  FILE *file_handel;
  char FileName[15];
  sprintf (FileName, "brain_%d.bin", i);
  int NumLayers = A->NumLayers;
  file_handel = fopen (FileName, "wb");
  fwrite ("Br", 1, 2, file_handel);
  fwrite (&A->Score, 1, sizeof (int), file_handel);
  fwrite (&NumLayers, 1, sizeof (int), file_handel);

  fwrite (A->SizeLayer, 1, NumLayers * sizeof (int), file_handel);

  for (int t = 0; t < (A->NumLayers - 1); t++)
  {
    int Size_Of_Layer=(A->SizeLayer[t] * A->SizeLayer[t + 1]);
    fwrite (*(A->Synapses + t), 1, Size_Of_Layer * sizeof (short int), file_handel);
  }
  fclose (file_handel);
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

  fread (&A->Score, 1, sizeof (int), file_handel);
  fread (&A->NumLayers, 1, sizeof (int), file_handel);
  fread (A->SizeLayer, 1, A->NumLayers * sizeof (int), file_handel);

  for (int t = 0; t < (A->NumLayers - 1); t++)
  {
    int Size_Of_Layer=(A->SizeLayer[t] * A->SizeLayer[t + 1]);
    fread (*(A->Synapses + t), 1, Size_Of_Layer * sizeof (short int), file_handel);
  }

  fclose (file_handel);
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
  struct BrainScore A = *(struct BrainScore*) a;
  struct BrainScore B = *(struct BrainScore*) b;

  int Foo = B.Score - A.Score;
  return Foo;
}




void MixBrains (int A, int B, int C, int Mutate) {
  char FileName[15];
  char TempByte;
  short int TempA;
  short int TempB;
  short int TempC;

  FILE *A_file_handel;
  sprintf (FileName, "brain_%d.bin", A);
  A_file_handel = fopen (FileName, "rb");

  FILE *B_file_handel;
  sprintf (FileName, "brain_%d.bin", B);
  B_file_handel = fopen (FileName, "rb");

  FILE *C_file_handel;
  sprintf (FileName, "brain_%d.bin", C);
  C_file_handel = fopen (FileName, "wb");

  for (int i = 0; i < HeaderSize; i++)
  {
  fread (&TempByte, 1, 1, A_file_handel);
  fread (&TempByte, 1, 1, B_file_handel);
  fwrite (&TempByte, 1, 1, C_file_handel);
  }

  int BytesRead=sizeof(short int);
  while(BytesRead==sizeof(short int) )
  {
    BytesRead = fread (&TempA, 1, sizeof(short int), A_file_handel);
    BytesRead = fread (&TempB, 1, sizeof(short int), B_file_handel);

    if((rand () % 1024)>512)
    {
      TempC=TempA;
    }
    else
    {
      TempC=TempB;
    }

    if((rand () % 100)<Mutate)
    {
      TempC=(rand () % 1024) - 512;
    }

    if (BytesRead !=0)
    {
      fwrite (&TempC, 1, sizeof(short int), C_file_handel);
    }
  }

  fclose (A_file_handel);
  fclose (B_file_handel);
  fclose (C_file_handel);
}





void NextGeneration(struct BrainScore *Population) {
  qsort (Population, NumOfBrains, sizeof (struct BrainScore), cmpfunc);
  printf ("Best Score: %d\n", (Population)->Score);

  // mix some brains
  for (int i = 0; i < (NumOfBrains-NumToKeep); i++)
  {
    int A = (Population+(rand () % NumToKeep))->Number;
    int B = (Population+(rand () % NumToKeep))->Number;
    int C = (Population+(NumOfBrains-i))->Number; 

    if (i<NumToRegenerate)
    {
      MixBrains (A, B, C, 100);
    }
    else
    {
      MixBrains (A, B, C, Mutate);
    }
  }
}




int main (int argc, char **argv) {
srand(time(0));
  struct Brain *NewBrain;
  NewBrain = malloc (sizeof(struct Brain));
  CreateOneBrain(NewBrain);
  HeaderSize=NewBrain->Header_Size;

  options (argc, argv);

  if (CreateFlag == 1)
  {
    for (int i = 0; i < NumOfBrains; i++)
    {
      RandomizeOneBrain(NewBrain);
      SaveOneBrain(NewBrain,i);
    }
    printf ("Made %d brains\n", NumOfBrains);
  }
  else
  {
  struct BrainScore *Population;
  Population = malloc (sizeof(struct BrainScore) * NumOfBrains);

    for (int i = 0; i < NumOfBrains; i++)
    {
      LoadOneBrain(NewBrain,i);
      NewBrain->Score=0;

      for (int t = 0; t < NumOfGames; t++)
      {
        Play(NewBrain);
      }

      (Population+i)->Number=i;
      (Population+i)->Score=NewBrain->Score;
      SaveOneBrain(NewBrain,i);
    }
    printf ("Run %d brains\n", NumOfBrains);
    NextGeneration(Population);
  }
}


