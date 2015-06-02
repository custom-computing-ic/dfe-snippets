#include <iostream>

#include "../include/HardwareAPI.h"

int main(void)
{

    std::cout << "calling hardwareAPI(" << "1): " << hardwareAPI(1) << std::endl;
    std::cout << "calling hardwareAPI(" << "10): " << hardwareAPI(10) << std::endl;

    return 0;
}
