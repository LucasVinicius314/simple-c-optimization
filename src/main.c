#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.141592

#define SIDE_X 2048
#define SIDE_Y 2048
#define C_SIZE 256

struct timespec tv;

long GetTimeDiff(clock_t t1, clock_t t2)
{
  return ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
}

unsigned char ReadElement(FILE *p)
{
  unsigned char element;
  if (1 != fread(&element, 1, 1, p))
  {
    printf("Corrupt data file.\n");
    exit(-1);
  }
  else
  {
    return element;
  }
}

// #1 - Separando funções de calcular seno e cosseno.

double DetCos(unsigned char element)
{
  // #2 - Cache no cálculo de cosseno.

  static float cache[C_SIZE];

  int a = cache[element];

  if (a != 0)
  {
    return a;
  }

  cache[element] = cos(2.0 * PI * element / 360.0);

  return cache[element];
}

double DetSin(unsigned char element)
{
  // #3 - Cache no cálculo de seno.

  static float cache[C_SIZE];

  int a = cache[element];

  if (a != 0)
  {
    return a;
  }

  cache[element] = sin(2.0 * PI * element / 360.0);

  return cache[element];
}

// #4 - Separando funções de calcular par e ímpar.

float DetEven(unsigned char element, float *C)
{
  // #5 - Cache no cálculo de par.

  static float cache[C_SIZE];

  int a = cache[element];

  if (a != 0)
  {
    return a;
  }

  cache[element] = 2.0 * DetSin(element) + exp(DetCos(element));

  return cache[element];
}

float DetEvenWrapper(unsigned char element, float *C)
{
  // #6 - Wrapper do DetEven para evitar a repetição do quadrado.

  static float cache[C_SIZE];

  int a = cache[element];

  if (a != 0)
  {
    return a;
  }

  cache[element] = pow(DetEven(element, C), 2);

  return cache[element];
}

float DetOdd(unsigned char element, float *C)
{
  // #7 - Cache no cálculo de ímpar.

  static float cache[C_SIZE];

  int a = cache[element];

  if (a != 0)
  {
    return a;
  }

  cache[element] = exp(DetSin(element)) * C[element];

  return cache[element];
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

  for (j = 0; j < rows; j++)
  {
    int t = j * cols; // #8: Isolando a multiplicação do offset.

    for (i = 0; i < cols; i++)
    {
      *(M + t + i) = ReadElement(data_file);
    }
  }

  fclose(data_file);

  if (!(Q = (float *)malloc((long)rows * cols * sizeof(float))))
  {
    printf("Not enough memory for output matrix.\n");
    exit(-1);
  }

  // Checkpoint.

  for (i = 0; i < C_SIZE; i++)
  {
    C[i] = 0.0;
  }

  for (i = 0; i < cols; i++)
  {
    for (j = 0; j < rows; j++)
    {
      C[*(M + j * cols + i)]++;
    }
  }

  for (i = 0; i < C_SIZE; i++)
  {
    C[i] = (C[i] > 0) ? log(C[i]) : 0.0;
  }

  for (i = 0; i < cols; i++)
  {
    for (j = 0; j < rows; j++)
    {
      element = *(M + j * cols + i);

      if ((element % 2) == 0)
      {
        *(Q + j * cols + i) = DetEvenWrapper(element, C);
      }
      else
      {
        *(Q + j * cols + i) = DetOdd(element, C);
      }
    }
  }

  // Checkpoint.

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
