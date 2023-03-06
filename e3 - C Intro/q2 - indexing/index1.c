#include <stdio.h>

#define SIZE (10)

int main (int argc, char* argv[])
{
    int a[SIZE];
    int* b = &a[0];
    b = b-1;
    
    for (int i=0 ; i<SIZE ; i++) {
        a[i] = i;
        //printf("%u: %d\n", i, b[i]);
    }

    for (int i=1 ; i<=SIZE ; i++) {
        //b[i] = i;
        printf("%u: %d\n", i, b[i]);
    }
    
    return 0;
}
