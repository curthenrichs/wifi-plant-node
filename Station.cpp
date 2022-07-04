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

//==============================================================================
//                                 Libraries
//==============================================================================

#include "Station.h"

#include "RestService.h"

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#if ST_USE_MDNS
#include <ESP8266mDNS.h>
#endif

//==============================================================================
//                         Private Function Prototypes
//==============================================================================

/**
 * Attempt to connect to last configured Wifi network. If failed then block as
 * an access point until connection is established.
 * @return boolean that should always be true (legacy)
 */
static bool _connect(void);
/**
 * Start all services for the project:
 *    - MDNS if enabled
 *    - LED Service REST API
 */
static void _start_services(void);
/**
 * Update all services for the project:
 *    - MDNS if enabled
 *    - LED Service REST API
 */
static void _update_services(void);
/**
 * Stop all services that require polling stop on Wifi failure
 *    - LED Service REST API
 */
static void _stop_services(void);

//==============================================================================
//                        Public Function Implementation
//==============================================================================

/**
 * Initialize Station which will connect to last configured Wifi router. If
 * no such router then block as an access point until user defines a router.
 * When connected to a router, start all services.
 */
void st_init(void){
  bool success = _connect();
  if(success){
    _start_services();
  }
}

/**
 * Update Station which will check current WiFi connection. If lost connection
 * then stop all services and attempt to reconnect. (Blocking until new
 * connection is established). Then restart all previously stopped services.
 * Else update the services normally.
 */
void st_update(void){
  if(WiFi.status() != WL_CONNECTED){
    _stop_services();
    bool success = _connect();
    if(success){
      _start_services();
    }
  }else{
    _update_services();
  }
}

//==============================================================================
//                        Private Function Implementation
//==============================================================================

/**
 * Attempt to connect to last configured Wifi network. If failed then block as
 * an access point until connection is established.
 * @return boolean that should always be true (legacy)
 */
static bool _connect(void){
  WiFiManager wifiManager;
  wifiManager.autoConnect("Wifi_Plant_Node_AP",ST_CONFIGURATION_DEFAULT_PASSWORD);
  return true;
}

/**
 * Start all services for the project:
 *    - MDNS if enabled
 *    - LED Service REST API
 */
static void _start_services(void){
  rs_init();
}

/**
 * Update all services for the project:
 *    - MDNS if enabled
 *    - LED Service REST API
 */
static void _update_services(void){
  rs_update();
}

/**
 * Stop all services that require polling stop on Wifi failure
 *    - LED Service REST API
 */
static void _stop_services(void){
  rs_stop();
}
