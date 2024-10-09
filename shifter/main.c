#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "tusb.h"

const int X_LO = 1500;
const int X_HI = 2600;

const int Y_LO = 1000;
const int Y_HI = 3000;

void hid_task(void);
void shifter_init(void);

int main(void)
{
  board_init();
  tusb_init();
  adc_init();
  shifter_init();

  while (1)
  {
    tud_task();
    hid_task();
  }
}

void shifter_init() {
  adc_gpio_init(26);
  adc_gpio_init(27);
  gpio_init(22);
  gpio_set_dir(22, GPIO_IN);
}

uint8_t read_shifter() {
  adc_select_input(0);
  uint16_t x = adc_read();
  adc_select_input(1);
  uint16_t y = adc_read();
  bool r = gpio_get(22);

  if (x < X_LO) {
    if (y < Y_LO) return 2;
    else if (y > Y_HI) return 1;
  } else if (x > X_HI) {
    if (y < Y_LO) {
      return r ? 7 : 6;
    }
    else if (y > Y_HI) return 5;
  } else {
    if (y < Y_LO) return 4;
    else if (y > Y_HI) return 3;
  }
  return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t gear)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  uint8_t buttons = 0;
  if (gear > 0) {
    buttons = 1 << (gear - 1);
  }

  tud_hid_report(1, &buttons, sizeof(buttons));
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  static uint8_t last_gear = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  uint8_t gear = read_shifter();
  if (gear == last_gear) {
    return;
  }
  last_gear = gear;
  send_hid_report(gear);
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
