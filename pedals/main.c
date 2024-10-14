#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/adc.h"
#include "tusb.h"

#define NUM_PEDALS 3
#define SAMPLES_PER_REPORT 10
#define INTERVAL_MILLIS 1

void hid_task(void);
void pedals_init(void);

int main(void)
{
  board_init();
  tusb_init();
  adc_init();
  pedals_init();

  uint32_t start_ms = board_millis();

  while (1)
  {
    tud_task();

    if (board_millis() - start_ms >= INTERVAL_MILLIS) {
      start_ms += INTERVAL_MILLIS;
      hid_task();
    }
  }
}

void pedals_init() {
  adc_gpio_init(26);
  adc_gpio_init(27);
  adc_gpio_init(28);
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define CLAMP(x, low, high) ({\
  __typeof__(x) __x = (x); \
  __typeof__(low) __low = (low);\
  __typeof__(high) __high = (high);\
  __x > __high ? __high : (__x < __low ? __low : __x);\
  })

int8_t read_pedal(uint8_t pedal) {
  static uint16_t pedalMins[NUM_PEDALS] = { 800, 800, 800 };
  static uint16_t pedalMaxs[NUM_PEDALS] = { 3600, 3600, 3600 };

  adc_select_input(pedal);
  uint16_t raw = adc_read();
  pedalMins[pedal] = MIN(raw, pedalMins[pedal]);
  pedalMaxs[pedal] = MAX(raw, pedalMaxs[pedal]);
  uint16_t deadZone = (pedalMaxs[pedal] - pedalMins[pedal]) / 10;
  uint16_t low = pedalMins[pedal] + deadZone;
  uint16_t high = pedalMaxs[pedal] - deadZone;
  uint16_t pos = CLAMP(raw, low, high);
  return (int8_t) map(pos, low, high, 127, -127);
}

void hid_task(void)
{
  static int8_t sample_count = 0;
  static int8_t samples[NUM_PEDALS][SAMPLES_PER_REPORT] = {0};

  for (int i = 0; i < NUM_PEDALS; i++) {
    samples[i][sample_count] = read_pedal(i);
  }
  sample_count += 1;

  if (sample_count < SAMPLES_PER_REPORT) return;
  sample_count = 0;

  int8_t report[NUM_PEDALS];
  for (int i = 0; i < NUM_PEDALS; i++) {
    int16_t avg = 0;
    for (int j = 0; j < SAMPLES_PER_REPORT; j++) {
      avg += samples[i][j];
    }
    report[i] = avg / SAMPLES_PER_REPORT;
  }

  if (tud_hid_ready()) {
    tud_hid_report(1, report, sizeof(report));
  }
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) report;
  (void) len;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}
