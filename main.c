#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void split2int(int*, char*, const char*);
void print_array(int**, int);
int* gen_gene(int);
void shuffle(int*, int);
int cal_score(int*);

char* input_file = "res/TSP100.txt"; 
FILE* fh;
int** Array;
int len;
int* genes[2];

const int child_count = 200;
const int max_time = 3000; //ms

int main()
{
  fh = fopen(input_file, "r");

  char length[10];
  int i, j;

  fgets(length, 10, fh);
  len = atoi(length);

  char line[99999];
  fgets(line, sizeof(line), fh);

  genes[0] = (int*)malloc(len * sizeof(int)); // score
  genes[1] = (int*)malloc(len * sizeof(int)); // gene

  int *pData;
  Array = (int **)malloc(len * sizeof(int *) + len * len * sizeof(int));
  for (i = 0, pData = (int *)(Array + len); i < len; i++, pData += len)
    Array[i] = pData;

  i = 0;
  while(fgets(line, sizeof(line), fh))
    split2int(Array[i++], line, "\t");

  srand(time(NULL));

  ////////////////////
  // start gene loop
  ////////////////////
  
  for(i = 0; i < child_count; i++)
  {
  }

  fclose(fh);
}

int* gen_gene(int size)
{
  int* gene = (int*)malloc(size * sizeof(int));
  int i;

  for(i = 0; i < size; i++)
    gene[i] = i;

  //for(i = 0; i < size; i++)
  //  gene[i] = rand() % (size-i);

  return gene;
}

int cal_score(int* gene)
{
  int i;
  int score = 0;

  for(i = 0; i < len - 1; i++)
    score += Array[gene[i]][gene[i+1]];
  score += Array[gene[len - 1]][gene[0]];

  return score;
}

void shuffle(int* ary, int size)
{
  int len = 5;
  int i, j, temp;

  for (i = size - 1; i > 0; i--) {
    j = rand() % i;
    temp = ary[i];
    ary[i] = ary[j];
    ary[j] = temp;
  }
}

void print_array(int **ary, int len)
{
  int i, j;
  for(i = 0; i < len; i++)
  {
    for(j = 0; j < len; j++)
      printf("%d ", ary[i][j]);

    printf("\n");
  }

}

void split2int(int *arr, char *str, const char *del) 
{
  char *s = strtok(str, del);

  int i = 0;
  while(s != NULL) 
  {
    arr[i++] = atoi(s);
    s = strtok(NULL, del);
  }
}
