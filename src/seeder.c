#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIDE_X 16384
#define SIDE_Y 8192

int main(int argc, char **argv)
{
  srand(time(NULL));

  FILE *f = fopen("../bin/main.dat", "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  long i = 0;
  while (i < SIDE_X * SIDE_Y)
  {
    putc(rand(), f);

    i++;
  }

  fclose(f);
}
