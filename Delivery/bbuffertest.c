#include "bbuffer.h"
#include "bbuffer.c"


int main() {
    printf("Starting buffer test\n");
    const BNDBUF *bndbuf = bb_init(5);
    printf("Buffer successfully created\n");
    int first = bb_add(bndbuf,8);
    int second = bb_add(bndbuf,7);
    int third = bb_add(bndbuf,5);
    int fourth = bb_add(bndbuf,10);
    int fifht = bb_add(bndbuf,11);
    printf("Attempdet adding values to the buffer\n");
    printf("The first value should be 8 and is %i , the fifth value should be 11 and is %i \n",first,fifht);
    printf("Attempting adding to full que\n");
    int overload = bb_add(bndbuf,9);
    printf("Last and first value should remain the same %i and %i \n", first, fifht);
    printf("Should return 8 and returns :%i \n", bb_get(bndbuf));
    printf("Now the next value is 7 ;%i \n",bb_get(bndbuf));
    int sixth = bb_add(bndbuf, 12);
    int seventh = bb_add(bndbuf,13);
    for (int i = 0; i < 6; i++) {

        printf("Return value %i\n", bb_get(bndbuf));
    }

    printf("Buffer field bufsiz: %d", bndbuf->bufsiz);
    bb_del(bndbuf);

    printf("deleted buffer");
    
    /*
    if (!bb_get(bndbuf)) {
    printf("Exception happened here this works fine\n");
    } else {
     printf("Ooopsie buffer still alive");
    }
    */
   


    return 0;
}