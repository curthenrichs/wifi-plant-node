/**
 * window_led_service
 * @author Curt Henrichs
 * @date 1-23-2018
 *
 * Window LED Service is an ESP8266 based home automation device that interfaces
 * a strip of LEDs with a REST API.
 *
 * Inspiration derived from preserving LED strip's IR remote controller module
 * attached to the LED strip but also a desire not to use the IR remote control.
 * Thus an microcontroller with WiFi was needed to connect to the network and
 * translate commands into an IR stream that is blasted at the LED strip
 * controller.
 *
 * This project consists of a REST API, IR Remote Blaster, and a WiFi Management
 * server. See program modules for details.
 */

//==============================================================================
//                                 Libraries
//==============================================================================

#include "HardwareConfig.h"
#include "Station.h"

#include <elapsedMillis.h>

//==============================================================================
//                        Constant and Macro Definitions
//==============================================================================

#define STATUS_LED_BLINK_TIME (500)   //! 1/2 LED flash rate

//==============================================================================
//                             Private Attributes
//==============================================================================

/**
 * Time counter which will act as the timer that periodically toggles status
 * LED according to the defined BLINK_TIME
 */
static elapsedMillis status_led_blink_etime = 0;

//==============================================================================
//                                   MAIN
//==============================================================================

/**
 * Main setup will configure hardware dependecies before connecting to Wifi. If
 * connected then the REST service will start, else application will wait in
 * AP mode for new configuration or until manually reset.
 */
void setup(void) {
  hwcfig_init();
  st_init();
}

/**
 * Main loop will maintain wifi station and LED service. Additionally a status
 * LED will be flashed periodically to indicate to a user that the device is
 * still operating correctly.
 *
 * If Wifi is lost during operation, device will block execution. During this
 * time the Wifi will be configred as in AP mode serving a configuration page.
 * Device will exit block when reconfigured or manually reset and connects to
 * Wifi.
 */
void loop(void) {

  // update wifi station (which will update service)
  st_update();

  // blink status LED to indicate server still operating
  if(status_led_blink_etime >= STATUS_LED_BLINK_TIME){
    status_led_blink_etime -= STATUS_LED_BLINK_TIME;
    digitalWrite(STATUS_LED_PIN,!digitalRead(STATUS_LED_PIN));
  }
}
