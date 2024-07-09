

#include <stdio.h>
int integers() {
    //unsigned int number = 4294967296; //2147483647*2;
    int number = 2147483647;
    //char letter = 'l';

    printf("%u \n", number);

    number = number +1;
    printf(" after adding 1 to the number %d \n", number);
    number = number +1;
    printf("again after adding 1 to the number %d \n", number);
    //the number will be 4, as 4 bytes
    return 0;
}

