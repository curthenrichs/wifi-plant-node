/**
 * Station
 * @author Curt Henrichs
 * @date 1-23-2018
 *
 * Station module is responsable for initializing and maintaing a WiFi
 * connection. Additionally, this module will maintain all network services
 * for this device.
 *
 * For Wifi connection and management the WiFiManager library was selected.
 * https://github.com/tzapu/WiFiManager
 * This library preserves last configured wifi connection and hosts a captive
 * web service when not connected to a wifi in station mode.
 *
 * MDNS is an optional service for this device if configured in the network
 * configuration file. The following is pulled from the example documentation,
 *    - Install host software:
 *      - For Linux, install Avahi (http://avahi.org/).
 *      - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
 *      - For Mac OSX and iOS support is built in through Bonjour already.
 *
 * The LED service REST API can be found in the appropriate project module.
 */

#ifndef STATION_H
#define STATION_H

//==============================================================================
//                                 Libraries
//==============================================================================

#include "NetworkConfig.h"

//==============================================================================
//                         Public Function Prototypes
//==============================================================================

/**
 * Initialize Station which will connect to last configured Wifi router. If
 * no such router then block as an access point until user defines a router.
 * When connected to a router, start all services.
 */
void st_init(void);
/**
 * Update Station which will check current WiFi connection. If lost connection
 * then stop all services and attempt to reconnect. (Blocking until new
 * connection is established). Then restart all previously stopped services.
 * Else update the services normally.
 */
void st_update(void);

//==============================================================================

#endif
