#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "ai.h"



// global settings from commandline
int NumOfBrains=1000;
int NumOfGenerations=100;
int Mutate=1;


// process the commandline and set global options
int options(int argc, char**argv)
{
int tmp;

while ((tmp = getopt (argc, argv, "g:n:m:")) != -1)
    switch (tmp)
      {
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
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (optopt == 'n')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
}



int main(int argc, char**argv)
{ 
options(argc, argv);
printf("Number of brains: %d\n", NumOfBrains);
printf("Number of generations: %d\n", NumOfGenerations);
printf("Number of mutate: %d\n", Mutate);
}

