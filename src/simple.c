#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.141592

#define SIDE_X 16384
#define SIDE_Y 8192
#define C_SIZE 256

struct timespec tv;

long GetTimeDiff(clock_t t1, clock_t t2)
{
  return ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
}

unsigned char ReadElement(FILE *p) // >> 2
{
  unsigned char element;
  if (1 != fread(&element, 1, 1, p)) // 2
  {
    printf("Corrupt data file.\n");
    exit(-1);
  }
  else
  {
    return (element);
  }
}

void DetSinCos(unsigned char element, float *sin_element, float *cos_element) // >> 10
{
  *sin_element = sin(2.0 * PI * element / 360.0); // 5
  *cos_element = cos(2.0 * PI * element / 360.0); // 5
}

void DetOutput(unsigned char element, float *C, float *out_even, float *out_odd) // >> 20
{
  float sin_element, cos_element; // 2

  DetSinCos(element, &sin_element, &cos_element); // >> 10

  *out_even = 2.0 * sin_element + exp(cos_element); // 4
  *out_odd = exp(sin_element) * C[element];         // 4
}

int main(int argc, char **argv)
{
  FILE *data_file;
  int rows, cols, i, j;
  unsigned char element, *M;
  float out_even, out_odd, *Q, C[C_SIZE];

  /***** Comecar a medir o tempo aqui *****/

  clock_t then = clock();

  // Flag para controlar a entrada maual ou automática.
  if (0)
  {
    if (argc < 4)
    {
      printf("Sintaxe: %s file_name rows cols\n", argv[0]);
      exit(-1);
    }

    if (NULL == (data_file = fopen(argv[1], "rb")))
    {
      printf("File not found.\n");
      exit(-1);
    }

    rows = atoi(argv[2]);
    cols = atoi(argv[3]);
  }
  else
  {
    if (NULL == (data_file = fopen("../bin/main.dat", "rb")))
    {
      printf("File not found.\n");
      exit(-1);
    }

    rows = SIDE_X;
    cols = SIDE_Y;
  }

  if (!(M = (unsigned char *)malloc((long)rows * cols)))
  {
    printf("Not enough memory to read file.\n");
    exit(-1);
  }

  for (j = 0; j < rows; j++) // N >> 2NM
  {
    for (i = 0; i < cols; i++) // M
    {
      *(M + j * cols + i) = ReadElement(data_file); // 2
    }
  }

  fclose(data_file);

  if (!(Q = (float *)malloc((long)rows * cols * sizeof(float))))
  {
    printf("Not enough memory for output matrix.\n");
    exit(-1);
  }

  for (i = 0; i < C_SIZE; i++) // >> 256
  {
    C[i] = 0.0;
  }

  for (i = 0; i < cols; i++) // N >> 2NM
  {
    for (j = 0; j < rows; j++) // M
    {
      element = *(M + j * cols + i); // 1
      C[element]++;                  // 1
    }
  }

  for (i = 0; i < C_SIZE; i++) // >> 256 * 4 >> 1024
  {
    C[i] = (C[i] > 0) ? log(C[i]) : 0.0;
  }

  for (i = 0; i < cols; i++) // N >> 22NM
  {
    for (j = 0; j < rows; j++) // M
    {
      element = *(M + j * cols + i);

      DetOutput(element, C, &out_even, &out_odd); // 20

      if ((element % 2) == 0) // >> 2
      {
        *(Q + j * cols + i) = pow(out_even, 2); // 2
      }
      else
      {
        *(Q + j * cols + i) = out_odd; // 1
      }
    }
  }

  clock_t now = clock();

  long diff = difftime(now, then);

  printf("%d ms", diff);

  /**** Medir o tempo até aqui *****/

  /**** Saída apenas para verificação, não deve ser medida ****/

  // for (i = 0; i < cols; i++)
  // {
  //   for (j = 0; j < rows; j++)
  //   {
  //     printf("%f ", *(Q + j * cols + i));
  //   }
  // }
}
