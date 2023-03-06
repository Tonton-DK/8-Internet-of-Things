#include <stdio.h>

#define CITY_COUNT (15)

typedef int bool;
#define TRUE  (1)
#define FALSE (0)

char* citynames[CITY_COUNT] = {
    "Esbjerg",
    "Helsingør",
    "Herning",
    "Horsens",
    "Kolding",
    "København",
    "Næstved",
    "Odense",
    "Randers",
    "Roskilde",
    "Silkeborg",
    "Vejle",
    "Viborg",
    "Aalborg",
    "Århus",
};

// source: https://dk.afstand.org
int distances[CITY_COUNT][CITY_COUNT] = {
    {  0, 269,  82,  98,  65, 260, 211, 123, 149, 230, 105,  74, 126, 198, 134},
    {269,   0, 226, 173, 206,  40, 105, 157, 166,  55, 191, 196, 207, 200, 150},
    { 82, 226,   0,  63,  79, 230, 202, 121,  76, 202,  36,  59,  45, 117,  77},
    { 98, 173,  63,   0,  48, 172, 139,  62,  68, 142,  40,  26,  75, 132,  40},
    { 65, 206,  79,  48,   0, 196, 148,  59, 114, 165,  77,  25, 110, 176,  88},
    {260,  40, 230, 172, 196,   0,  71, 141, 180,  31, 196, 190, 218, 224, 156},
    {211, 105, 202, 139, 148,  71,   0,  89, 174,  50, 175, 150, 204, 232, 142},
    {123, 157, 121,  62,  59, 141,  89,   0, 121, 110, 102,  64, 136, 186,  86},
    {149, 166,  76,  68, 114, 180, 174, 121,   0, 156,  44,  90,  42,  65,  36},
    {230,  55, 202, 142, 165,  31,  50, 110, 156,   0, 169, 160, 193, 206, 130},
    {105, 191,  36,  40,  77, 196, 175, 102,  44, 169,   0,  53,  35,  99,  41},
    { 74, 196,  59,  26,  25, 190, 150,  64,  90, 160,  53,   0,  86, 151,  65},
    {126, 207,  45,  75, 110, 218, 204, 136,  42, 193,  35,  86,   0,  72,  63},
    {198, 200, 117, 132, 176, 224, 232, 186,  65, 206,  99, 151,  72,   0, 101},
    {134, 150,  77,  40,  88, 156, 142,  86,  36, 130,  41,  65,  63, 101,   0},
};

static inline bool contains (int sequence[], int depth, int value) {
  for (int j=1; j<depth ; j++)
    if (sequence[j]==value)
      return TRUE;
  
  return FALSE;
}

void solve (int sequence[], unsigned int* distance, int depth, int sequence_tmp[], int distance_tmp) {
  if (depth==CITY_COUNT) {
    distance_tmp += distances[sequence_tmp[depth-1]][0];
    if (distance_tmp<*distance) {
      for (int i=1 ; i<CITY_COUNT ; i++)
        sequence[i] = sequence_tmp[i];
      *distance = distance_tmp;
    }
  } else {
    for (int i=1 ; i<CITY_COUNT ; i++) {
      if (!contains(sequence_tmp, depth, i)) {
        int step_length = distances[sequence_tmp[depth-1]][i];
        
        // prune search tree
        if (distance_tmp+step_length>=*distance) continue;
        
        sequence_tmp[depth] = i;
        solve(sequence, distance, depth+1, sequence_tmp, distance_tmp+step_length);
      }
    }
  }
}

int main (int argc, char* argv[]) {
  // print out city names
  for (int i=0 ; i<CITY_COUNT ; i++) {
    printf("City %2d: %s\n", i, citynames[i]);
  }
  printf("\n");
  
  // print out distance matrix
  printf("Distance matrix:\n");
  for (int y=0 ; y<CITY_COUNT ; y++) {
    printf(" ");
    for (int x=0 ; x<CITY_COUNT ; x++) {
      printf(" %3d", distances[y][x]);
    }
    printf("\n");
  }
  printf("\n");
  
  // solve
  int sequence[CITY_COUNT];
  unsigned int distance = -1;
  int sequence_tmp[CITY_COUNT];
  int distance_tmp = 0;
  sequence[0] = 0;
  sequence_tmp[0] = 0;
  solve(sequence, &distance, 1, sequence_tmp, distance_tmp);
  
  // print solution
  printf("Shortest loop (total distance: %u km):\n", distance);
  for (int i=0 ; i<CITY_COUNT ; i++)
    printf("- %s\n", citynames[sequence[i]]);
  printf("- %s\n", citynames[sequence[0]]);
  
  return 0;
}
