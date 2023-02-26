#include <stdio.h>
#include <stdlib.h>

#define LENGTH (12)

int main (int argc, char* argv[]) {
    int* values = (int*) malloc(LENGTH*sizeof(int));

    printf("int size: %d\n", sizeof(int));
    printf("long size: %d\n", sizeof(long));
    printf("long long size: %d\n", sizeof(long long));

    for (int i=0 ; i<LENGTH ; i++) {
        values[i] = i;
    }

    for (int i=0 ; i<LENGTH/2 ; i++) {
        printf("%u: %d\n", i, (int)(((long long*)values)[i]));
    }

    return 0;
}
