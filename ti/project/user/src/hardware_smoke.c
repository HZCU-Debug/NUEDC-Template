#include "zf_common_headfile.h"

#define BEEP (A14)

int main(void)
{
    clock_init(SYSTEM_CLOCK_80M);
    gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL);

    system_delay_ms(300);
    gpio_set_level(BEEP, GPIO_HIGH);
    system_delay_ms(100);
    gpio_set_level(BEEP, GPIO_LOW);
    system_delay_ms(100);
    gpio_set_level(BEEP, GPIO_HIGH);
    system_delay_ms(100);
    gpio_set_level(BEEP, GPIO_LOW);
    system_delay_ms(100);

    while(1)
    {
        system_delay_ms(5);
    }
}
