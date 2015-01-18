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

// char* input_file = "res/TSP10.txt"; 
char input_file[999];
FILE* fh;
int** Array; // map
int len; // gene length
gene_data* genes;

int* points;
int child_count = 200;
int max_time = 5; //s
const float reserve_rate = 0.3;
const float death_rate = 0.3;
float mut_rate = 0.4;

int main()
{
  ////////////////////////////
  // input interface 
  ////////////////////////////
  int temp;
  float temp_f;
  char* temp_s;
  printf("Input the test file : ");
  scanf("%s", input_file);

  printf("Input the gene amount (enter 0 to set default 200) : ");
  scanf("%d", &temp);
  if(temp > 0)
    child_count = temp;
  else if(temp < 0)
  {
    printf("\nwrong number!!\n");
    return 0;
  }

  printf("\nInput the max time (s) (enter 0 to set default 5) : ");
  scanf("%d", &temp);
  if(temp > 0)
    max_time = temp;
  else if(temp < 0)
  {
    printf("\nwrong number!!\n");
    return 0;
  }

  printf("\nInput the mutation rate (0.0, 1.0] (enter 0 to set default 0.4) : ");
  scanf("%f", &temp_f);
  if(temp_f <= 1 && temp_f > 0)
    mut_rate = temp_f;
  else if(temp_f < 0)
  {
    printf("\nwrong number!!\n");
    return 0;
  }

  ////////////////////////////
  // init 
  ////////////////////////////
  fh = fopen(input_file, "r");

  char length[99999];
  int i, j;

  fgets(length, sizeof(length), fh);
  len = atoi(length);

  char line[99999];

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
  // gen first generation
  ////////////////////////////
  for(i = 0; i < child_count; i++)
    genes[i].gene = gen_gene(len); 

  ////////////////////////////
  // ga loop
  ////////////////////////////
  int g = 1; // counter
  int reserve_limit, death_limit;
  int* father, mother;
  gene_data* next_gen;
  reserve_limit = child_count * reserve_rate;
  death_limit   = child_count * (1 - death_rate);
  while(1)
  {
    // calculate gene score (distance)
    for(i = 0; i < child_count; i++)
      genes[i].score = cal_score(genes[i].gene); 

    // sort gene by score from small to big
    qsort((void*)genes, child_count, sizeof(gene_data), compare);
    printf("%d\n", genes[0].score);

    // set break point
    if(time(NULL) - start_time >= max_time)
      break;

    // next gen
    next_gen = (gene_data*)malloc(child_count * sizeof(gene_data));
    for(i = 0; i < child_count; i++)
    {
      if(i >= death_limit)
        next_gen[i].gene = gen_gene(len);
      else if(i <= reserve_limit)
        next_gen[i].gene = genes[i].gene; // TODO prevent repeat count score
      else
      {
        father = genes[rand() % reserve_limit].gene;
        mother = genes[rand() % child_count  ].gene;
        next_gen[i].gene = crossover(father, mother, mut_rate);
      }
    }
  
    free(genes);
    genes = next_gen;

    g++;
  }
  

  ////////////////////////////
  // output
  ////////////////////////////
  printf("\nAfter %d generations\n", g);
  printf("Distance : %d\n", genes[0].score);
  printf("Path : ");
  int cur_point, last_point;
  int* points_copy;

  points_copy = (int*)malloc(len * sizeof(int));
  memcpy(points_copy, points, len * sizeof(int));

  last_point = genes[0].gene[0];
  points_copy[last_point] = -1;
  printf("%d ", last_point);
  int relate_position;
  for(i = 1; i < len; i++)
  {
    cur_point = 0;
    relate_position = genes[0].gene[i];

    while(1)
    {
      if(points_copy[cur_point] != -1)
        relate_position--;

      if(relate_position < 0)
        break;

      cur_point++;
    }

    printf(",%d ", cur_point);

    Array[last_point][cur_point];
    points_copy[cur_point] = -1;
    last_point = cur_point;
  }
  printf("\n");

  free(points_copy);

  // for(j = 0; j < len; j++)
  //   printf("%d ", genes[j].score);
  // printf("\n");

  fclose(fh);

  system("PAUSE");
}

////////////////////////////////
// gene crossover & mutation
int* crossover(int* a, int* b, float mut_rate)
{
  int* c;
  int i;
  float rnd;
  c = (int*)malloc(len * sizeof(int));

  for(i = len - 1; i >= 0; i--)
  {
    rnd = (rand() % 100) / 100;
    if(rnd < mut_rate)
      c[i] = rand() % (len - i);
    else if(rnd > 0.5 + mut_rate/2)
      c[i] = a[i]; // choose farther
    else
      c[i] = b[i]; // choose mother
  }

  return c;
}

int compare(const void* arg1, const void* arg2)
{
  return ((*(gene_data*)arg1).score - (*(gene_data*)arg2).score);
}

////////////////////////////////
// random generate gene 
// 基因的編碼意義為：
// * 第一碼 = 第一步要經過路徑點清單中的第幾個點
// * 第二碼 = 拔掉第一步經過的點之後，要經過路徑點清單中的第幾個點
// * 第三碼 = 拔掉第二步經過的點之後，要經過路徑點清單中的第幾個點
// * 以下類推
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

////////////////////////////////
// calculate score (distance)
int cal_score(int* gene)
{
  int i, score = 0, cur_point, last_point;
  int* points_copy;

  points_copy = (int*)malloc(len * sizeof(int));
  memcpy(points_copy, points, len * sizeof(int));

  last_point = gene[0];
  points_copy[last_point] = -1;
  int relate_position;
  for(i = 1; i < len; i++)
  {
    cur_point = 0;
    relate_position = gene[i];

    while(1)
    {
      if(points_copy[cur_point] != -1)
        relate_position--;

      if(relate_position < 0)
        break;

      cur_point++;
    }

    score += Array[last_point][cur_point];
    points_copy[cur_point] = -1;
    last_point = cur_point;
  }

  score += Array[cur_point][gene[0]];

  free(points_copy);
  return score;
}

// none using
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

////////////////////////////////
// split char array to int array by "\t"
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
