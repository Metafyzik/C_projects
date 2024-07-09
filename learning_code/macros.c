
#include <stdio.h>
#define custom_macro 55

int macros() {

    printf("custom_macro: %d", custom_macro);

    //__DATE__ and __TIME__ are predefined macros
    printf("Date: %s \n", __DATE__);
    printf("Time: %s \n", __TIME__);



    return 0;
}
