#include <stdio.h>

#define CITY_COUNT (15)

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
{ 0, 269, 82, 98, 65, 260, 211, 123, 149, 230, 105, 74, 126, 198, 134},
{269, 0, 226, 173, 206, 40, 105, 157, 166, 55, 191, 196, 207, 200, 150},
{ 82, 226, 0, 63, 79, 230, 202, 121, 76, 202, 36, 59, 45, 117, 77},
{ 98, 173, 63, 0, 48, 172, 139, 62, 68, 142, 40, 26, 75, 132, 40},
{ 65, 206, 79, 48, 0, 196, 148, 59, 114, 165, 77, 25, 110, 176, 88},
{260, 40, 230, 172, 196, 0, 71, 141, 180, 31, 196, 190, 218, 224, 156},
{211, 105, 202, 139, 148, 71, 0, 89, 174, 50, 175, 150, 204, 232, 142},
{123, 157, 121, 62, 59, 141, 89, 0, 121, 110, 102, 64, 136, 186, 86},
{149, 166, 76, 68, 114, 180, 174, 121, 0, 156, 44, 90, 42, 65, 36},
{230, 55, 202, 142, 165, 31, 50, 110, 156, 0, 169, 160, 193, 206, 130},
{105, 191, 36, 40, 77, 196, 175, 102, 44, 169, 0, 53, 35, 99, 41},
{ 74, 196, 59, 26, 25, 190, 150, 64, 90, 160, 53, 0, 86, 151, 65},
{126, 207, 45, 75, 110, 218, 204, 136, 42, 193, 35, 86, 0, 72, 63},
{198, 200, 117, 132, 176, 224, 232, 186, 65, 206, 99, 151, 72, 0, 101},
{134, 150, 77, 40, 88, 156, 142, 86, 36, 130, 41, 65, 63, 101, 0},
};

int main (int argc, char* argv[])
{
    char* route[CITY_COUNT];
    int taken[CITY_COUNT] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int index = 0;
    
    int visited = 0;
    while (visited != CITY_COUNT)
    {
        char* location = citynames[index];
        printf("%s --> \n", location);
        route[visited] = location;
        taken[index] = 1;
        visited += 1;

        int smallestVal = 9999;
        int smallestIndex = NULL; 
        for (int i=0 ; i<CITY_COUNT ; i++) {
            int possible = distances[index][i];
            if (taken[i] != 1 && possible < smallestVal)
            {
                smallestVal=possible;
                smallestIndex=i;
            }
        }

        index = smallestIndex;
    }

    for (int i=CITY_COUNT ; i>0 ; i--) {
        char* location = route[i-1];
        printf("<-- %s\n", location);
    }

    return 0;
}
