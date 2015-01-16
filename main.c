#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void split2int(int*, char*, const char*);
void print_array(int**, int);
int* gen_gene(int);
void shuffle(int*, int);
int cal_score(int*);
int compare(const void*, const void*);
int* crossover(int*, int*, float);

struct G{
  int score;
  int* gene;
};
typedef struct G gene_data;

char* input_file = "res/TSP100.txt"; 
FILE* fh;
int** Array;
int len;
gene_data* genes;

int* points;
const int child_count = 200;
const int max_time = 5; //s
const float reserve_rate = 0.3;
const float death_rate = 0.3;

int main()
{
  fh = fopen(input_file, "r");

  char length[10];
  int i, j;

  fgets(length, 10, fh);
  len = atoi(length);

  char line[99999];
  fgets(line, sizeof(line), fh);

  points = (int*)malloc(len * sizeof(int));
  for(i = 0; i < len; i++)
    points[i] = i;

  genes = (gene_data*)malloc(child_count * sizeof(gene_data));

  int *pData;
  Array = (int **)malloc(len * sizeof(int *) + len * len * sizeof(int));
  for (i = 0, pData = (int *)(Array + len); i < len; i++, pData += len)
    Array[i] = pData;

  i = 0;
  while(fgets(line, sizeof(line), fh))
    split2int(Array[i++], line, "\t");

  srand(time(NULL));
  int start_time = time(NULL);

  ////////////////////////////
  // gen first 
  ////////////////////////////
  for(i = 0; i < child_count; i++)
    genes[i].gene = gen_gene(len); 

  ////////////////////////////
  // ga loop
  ////////////////////////////

  int g = 1;
  int reserve_limit, death_limit;
  gene_data* next_genes;
  reserve_limit = child_count * reserve_rate;
  death_limit   = child_count * death_rate;
  while(1)
  {
    for(i = 0; i < child_count; i++)
      genes[i].score = cal_score(genes[i].gene); 

    qsort((void*)genes, child_count, sizeof(gene_data), compare);

    if(time(NULL) - start_time >= max_time)
      break;

    // next gen
    // next_genes = (gene_data*)malloc(child_count * sizeof(gene_data));
    // for(i = 0; i < child_count; i++)
    // {
    //   if(i >= death_limit)
    //     next_gen[i].gene = gen_gene(len);
    //   else if(i <= reserve_limit)
    //     next_gen[i].gene = genes[i].gene // TODO prevent repeat count score
    //     
    // }

  }

  // for(j = 0; j < len; j++)
  //   printf("%d ", genes[j].score);
  // printf("\n");

  fclose(fh);
}

int* crossover(int* a, int* b, float mut_rate)
{
  int* c;
  int i;
  c = (int*)malloc(len * sizeof(int));


}

int compare(const void* arg1, const void* arg2)
{
  return ((*(gene_data*)arg1).score - (*(gene_data*)arg2).score);
}

int* gen_gene(int size)
{
  int* gene = (int*)malloc(size * sizeof(int));
  int i;

  //for(i = 0; i < size; i++)
  //  gene[i] = i;

  for(i = 0; i < size; i++)
    gene[i] = rand() % (size-i);

  return gene;
}

int cal_score(int* gene)
{
  int i, score = 0, cur_point, last_point;
  int* points_copy;

  points_copy = (int*)malloc(len * sizeof(int));
  memcpy(points_copy, points, len * sizeof(int));

  last_point = gene[0];
  points_copy[last_point] = -1;
  for(i = 1; i < len; i++)
  {
    cur_point = gene[i];

    while(points_copy[cur_point] == -1)
      cur_point++;

    score += Array[last_point][cur_point];
    last_point = cur_point;
  }
  score += Array[cur_point][gene[0]];

  free(points_copy);
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
