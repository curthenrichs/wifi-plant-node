/**
 * HardwareConfig
 * @author Curt Henrichs
 * @date 4-7-2022
 *
 * Hardware configuration file captures hardware pinout and design as to inform
 * the application of the hardware layer. Note that this is not a device driver
 * rather this is merely a location to place physical dependencies for drivers.
 *
 * Additionally, any hardware designs external to the processor should be noted
 * in this documentation.
 *    - Reset pin attached to button, normally pulled high. (this is on module)
 *    - Status LED attached in series with a 1 kOhm resistor to 3V3 (active low)
 *    - 1 kOhm resistor pullup on Boot 0 and Boot 2 (these should be on module)
 *    - 1 kOhm resistor pulldown on Boot 15 (these should be on module)
 * 
 *    - Analog moisture sensor is connected to a voltage divider feeding into
 *      a non-inverting op-amp buffer set with 3v3 rail and gnd.
 */

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

//==============================================================================
//                                 Libraries
//==============================================================================

#include <Arduino.h>

//==============================================================================
//                                Device Pinout
//==============================================================================

#define MOISTURE_SENSOR     (A0)       //! Main sensor for data logging
#define STATUS_LED_PIN      (D0)       //! Status LED to note active functioning

#define BOOT_0_PIN          (D3)       //! Vcc for flash run, GND for program
#define BOOT_2_PIN          (D4)       //! Always Vcc (via external pullup)
#define BOOT_15_PIN         (D8)       //! Always GND (via external pulldown)

//==============================================================================
//                       Hardware Initialization Routine
//==============================================================================

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

//==============================================================================

#endif /*HARDWARE_CONFIG_H*/