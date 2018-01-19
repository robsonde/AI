#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "ai.h"

#define bits 12

int board[4][4];
int boardChanged;
int Score;


void
Board2Brain (struct Brain *A)
{
  for (int y = 0; y < 4; y++)
    {
      for (int x = 0; x < 4; x++)
	{
	  for (int n = 0; n < bits; n++)
	    {
	      *(*(A->Neurons) + ((y * 4 + x) * bits + n)) =
		! !(board[x][y] & (1 << n));
	    }
	}
    }
}





int
Decide (struct Brain *A)
{
  for (int out_index = 0; out_index < *(A->SizeLayer + (A->NumLayers - 1));
       out_index++)
    {
      if (*(*(A->Neurons + (A->NumLayers - 1)) + out_index) == true)
	return out_index;
    }
  return 5;
}







void
InitBoard ()
{
  for (int y = 0; y < 4; y++)
    {
      for (int x = 0; x < 4; x++)
	{
	  board[x][y] = 0;
	}
    }
}





void
AddTile ()
{
  int Count = 0;
  int new = 0;
  while (new == 0)
    {
      Count = Count + 1;
      int x = rand () % 4;
      int y = rand () % 4;
      if (board[x][y] == 0)
	{
	  board[x][y] = 2;
	  new = 1;
	}
      if (Count == 200)
	{
	  boardChanged = 0;
	  return;
	}
    }
}





void
PushUp ()
{
  for (int x = 0; x < 4; x++)
    {
      for (int i = 0; i < 4; i++)
	{
	  if (board[x][3] == 0)
	    {
	      board[x][3] = board[x][2];
	      board[x][2] = 0;
	      if (board[x][3] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[x][2] == 0)
	    {
	      board[x][2] = board[x][1];
	      board[x][1] = 0;
	      if (board[x][2] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[x][1] == 0)
	    {
	      board[x][1] = board[x][0];
	      board[x][0] = 0;
	      if (board[x][1] != 0)
		{
		  boardChanged = 1;
		}
	    }
	}

    }
}








void
PushDown ()
{
  for (int x = 0; x < 4; x++)
    {
      for (int i = 0; i < 4; i++)
	{
	  if (board[x][0] == 0)
	    {
	      board[x][0] = board[x][1];
	      board[x][1] = 0;
	      if (board[x][0] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[x][1] == 0)
	    {
	      board[x][1] = board[x][2];
	      board[x][2] = 0;
	      if (board[x][1] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[x][2] == 0)
	    {
	      board[x][2] = board[x][3];
	      board[x][3] = 0;
	      if (board[x][2] != 0)
		{
		  boardChanged = 1;
		}
	    }
	}

    }
}






void
PushLeft ()
{
  for (int y = 0; y < 4; y++)
    {
      for (int i = 0; i < 4; i++)
	{
	  if (board[0][y] == 0)
	    {
	      board[0][y] = board[1][y];
	      board[1][y] = 0;
	      if (board[0][y] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[1][y] == 0)
	    {
	      board[1][y] = board[2][y];
	      board[2][y] = 0;
	      if (board[1][y] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[2][y] == 0)
	    {
	      board[2][y] = board[3][y];
	      board[3][y] = 0;
	      if (board[2][y] != 0)
		{
		  boardChanged = 1;
		}
	    }
	}

    }
}









void
PushRight ()
{
  for (int y = 0; y < 4; y++)
    {
      for (int i = 0; i < 4; i++)
	{
	  if (board[3][y] == 0)
	    {
	      board[3][y] = board[2][y];
	      board[2][y] = 0;
	      if (board[3][y] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[2][y] == 0)
	    {
	      board[2][y] = board[1][y];
	      board[1][y] = 0;
	      if (board[2][y] != 0)
		{
		  boardChanged = 1;
		}
	    }

	  if (board[1][y] == 0)
	    {
	      board[1][y] = board[0][y];
	      board[0][y] = 0;
	      if (board[1][y] != 0)
		{
		  boardChanged = 1;
		}
	    }
	}

    }
}





void
AddVertical ()
{
  for (int x = 0; x < 4; x++)
    {
      if (board[x][0] == board[x][1])
	{
	  board[x][0] = board[x][0] + board[x][1];
	  Score = Score + board[x][0];
	  board[x][1] = 0;
	  if (board[x][0] != 0)
	    {
	      boardChanged = 1;
	    }
	}

      if (board[x][1] == board[x][2])
	{
	  board[x][1] = board[x][1] + board[x][2];
	  Score = Score + board[x][1];
	  board[x][2] = 0;
	  if (board[x][1] != 0)
	    {
	      boardChanged = 1;
	    }
	}

      if (board[x][2] == board[x][3])
	{
	  board[x][2] = board[x][2] + board[x][3];
	  Score = Score + board[x][2];
	  board[x][3] = 0;
	  if (board[x][2] != 0)
	    {
	      boardChanged = 1;
	    }
	}
    }

}





void
AddHorozontal ()
{
  for (int y = 0; y < 4; y++)
    {
      if (board[0][y] == board[1][y])
	{
	  board[0][y] = board[0][y] + board[1][y];
	  Score = Score + board[0][y];
	  board[1][y] = 0;
	  if (board[0][y] != 0)
	    {
	      boardChanged = 1;
	    }
	}

      if (board[1][y] == board[2][y])
	{
	  board[1][y] = board[1][y] + board[2][y];
	  Score = Score + board[1][y];
	  board[2][y] = 0;
	  if (board[1][y] != 0)
	    {
	      boardChanged = 1;
	    }
	}

      if (board[2][y] == board[3][y])
	{
	  board[2][y] = board[2][y] + board[3][y];
	  Score = Score + board[2][y];
	  board[3][y] = 0;
	  if (board[2][y] != 0)
	    {
	      boardChanged = 1;
	    }
	}
    }

}






void
DoDown ()
{
  PushDown ();
  AddVertical ();
  PushDown ();
  if (boardChanged == 1)
    {
      AddTile ();
    }
}


void
DoUp ()
{
  PushUp ();
  AddVertical ();
  PushUp ();
  if (boardChanged == 1)
    {
      AddTile ();
    }
}




void
DoLeft ()
{
  PushLeft ();
  AddHorozontal ();
  PushLeft ();
  if (boardChanged == 1)
    {
      AddTile ();
    }
}


void
DoRight ()
{
  PushRight ();
  AddHorozontal ();
  PushRight ();
  if (boardChanged == 1)
    {
      AddTile ();
    }
}





void
Do_Game_Loop (struct Brain *A)
{
  Board2Brain (A);
  Think (A);
  int move = Decide (A);
  boardChanged = 0;
  if (move == 0)
    DoRight ();
  if (move == 1)
    DoLeft ();
  if (move == 2)
    DoUp ();
  if (move == 3)
    DoDown ();
}








void Play (struct Brain *A)
{
  Score=A->Score;
  srand (time(NULL));
  InitBoard ();
  AddTile ();
  AddTile ();
  boardChanged = 1;
  while (boardChanged == 1)
    {
      Do_Game_Loop (A);
    }
  A->Score = Score;
}
