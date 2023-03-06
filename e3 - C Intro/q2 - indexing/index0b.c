#include <stdio.h>

#define SIZE (10)

int main (int argc, char* argv[])
{
    int values[SIZE];
    for (int i=0 ; i<SIZE ; i++) {
        values[i] = i;
        printf("%u: %d\n", i, values[i]);
    }
    return 0;
}
