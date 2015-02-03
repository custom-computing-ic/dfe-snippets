#include <stdio.h>

#include "../include/HardwareAPI.h"

int main(void)
{

    printf("calling hardwareAPI(%d) = %d\n", 1, hardwareAPI(1));
    printf("calling hardwareAPI(%d) = %d\n", 10, hardwareAPI(10));

    return 0;
}
