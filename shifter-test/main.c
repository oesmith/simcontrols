#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

void shifter_init(void);

int main(void)
{
  stdio_init_all();
  shifter_init();

  while (1) {
    adc_select_input(0);
    uint16_t x = adc_read();
    adc_select_input(1);
    uint16_t y = adc_read();
    bool r = gpio_get(22);

    printf("State: %d %d %d\n", x, y, r);

    sleep_ms(200);
  }
}

void shifter_init() {
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  gpio_init(22);
  gpio_set_dir(22, GPIO_IN);
}
