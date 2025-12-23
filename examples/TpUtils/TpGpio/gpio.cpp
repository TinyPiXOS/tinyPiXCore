#include "Hardware/TpGpio.h"

int example_out()
{
    TpGpio gpio(16);
    if (!gpio.open())
    {
        printf("gpio打开失败\n");
        return -1;
    }
    gpio.setDirection(TpGpio::OUTPUT);
    while (1)
    {
        gpio.setHeight();
        sleep(1);
        gpio.setLow();
        sleep(1);
    }
    gpio.close();
    return 0;
}

int example_in()
{
    TpGpio gpio(16);
    if (!gpio.open())
    {
        printf("gpio打开失败\n");
        return -1;
    }
    gpio.setDirection(TpGpio::INPUT);
    while (1)
    {
        if (gpio.getLevel())
        {
            printf("输入高电平\n");
        }
        else
        {
            printf("输入低电平\n");
        }
        sleep(1);
    }
    gpio.close();
    return 0;
}

int main()
{
    printf("example_out\n");
    example_out();
    printf("example_in\n");
    example_in();
}