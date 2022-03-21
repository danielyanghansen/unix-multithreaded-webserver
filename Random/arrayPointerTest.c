#include <stdio.h>

typedef struct something {
    int *p_arr;
} something;


int main () {
    int arr[5] = {1,2,4,5,6};

    something* st = (something *){arr};

    something st2 = *st;
    
    int arr2[] = st2.p_arr;
    for (int i = 0; i < sizeof(arr2); i++) {
        printf("%d", arr2[i]);
    }
}

//???