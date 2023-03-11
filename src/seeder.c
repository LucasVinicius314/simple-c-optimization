#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIDE 4096

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
  while (i < SIDE * SIDE)
  {
    putc(rand(), f);

    i++;
  }

  fclose(f);
}
