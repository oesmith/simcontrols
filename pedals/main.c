#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/adc.h"
#include "tusb.h"

#define NUM_PEDALS 3

void hid_task(void);
void pedals_init(void);

int main(void)
{
  board_init();
  tusb_init();
  adc_init();
  pedals_init();

  while (1)
  {
    tud_task();
    hid_task();
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

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report()
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  int8_t pedals[NUM_PEDALS];
  for (int i = 0; i < NUM_PEDALS; i++) {
    pedals[i] = read_pedal(i);
  }

  tud_hid_report(1, pedals, sizeof(pedals));
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  send_hid_report();
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) report;
  (void) len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}
