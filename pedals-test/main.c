#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

void pedals_init(void);

int main(void)
{
  stdio_init_all();
  pedals_init();

  while (1) {
    adc_select_input(0);
    uint16_t x = adc_read();
    adc_select_input(1);
    uint16_t y = adc_read();
    adc_select_input(2);
    uint16_t z = adc_read();

    printf("State: %d %d %d\n", x, y, z);

    sleep_ms(200);
  }
}

void pedals_init() {
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  adc_gpio_init(28);
}
