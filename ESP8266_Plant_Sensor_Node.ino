/**

https://www.ti.com/lit/ds/symlink/cd74hc4051.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1653256681850&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fcd74hc4051

https://www.analog.com/media/en/technical-documentation/data-sheets/OP282_482.pdf

*/

#include <WiFiManager.h>


#define RESET_BTN_PIN       D5
#define EXTRN_LED_PIN       D0
#define ANALOG_MOISTURE_PIN A0
#define AMUX_SLCT_0_PIN     D1
#define AMUX_SLCT_1_PIN     D2
#define AMUX_SLCT_2_PIN     D3

#define BOOT_0_PIN          (D3)       //! Vcc for flash run, GND for program
#define BOOT_2_PIN          (D4)       //! Always Vcc (via external pullup)
#define BOOT_15_PIN         (D8)       //! Always GND (via external pulldown)

/**
 * Initialize hardware pins as defined in the device pinout
 */
static inline void hwcfig_init(void) {
  pinMode(MOISTURE_SENSOR, INPUT);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  pinMode(BOOT_0_PIN, INPUT);
  pinMode(BOOT_2_PIN, INPUT);
  pinMode(BOOT_15_PIN, INPUT);
}


void setup() {
    Serial.begin(9600);

}

void loop() {

}