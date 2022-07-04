/**
 * RestService
 * @author Curt Henrichs
 * @date 1-21-18
 *
 * REST API and single client server module to expose LED controller
 * functionality to the network. The functionality is broken down to setting
 * state of the controller either through a raw byte command or sending
 * a message to a valid route. The other functionality is server specific such
 * as displaying documentation, routing, and state cache.
 *
 * The following is the HTTP REST API for the LED Service
 *  GET : / : Argument[none]
 *  GET : /routes : Argument[none]
 *  GET : /cached-state : Argument[none]
 *
 * The following is the HTTP REST API for the LED Controller
 *  GET, POST : /raw : Argument["raw"=<byte>,"documentation"=<bool>]
 *  GET, POST : /brightness : Argument["brightness"=<string>,"documentation"=<bool>]
 *  GET, POST : /power : Argument["power"=<string>,"documentation"=<bool>]
 *  GET, POST : /function : Argument["function"=<string>,"documentation"=<bool>]
 *  GET, POST : /color :  Argument["color"=<string>,"documentation"=<bool>]
 */

#ifndef RESTSERVICE_H
#define RESTSERVICE_H

//==============================================================================
//                           Public Function Prototypes
//==============================================================================

/**
 * Initialize IR LED controller REST service. Note that WIFI must be enabled
 * before calling this function.
 */
void rs_init(void);
/**
 * Periodically update server to service single client connected.
 */
void rs_update(void);
/**
 * Stop REST server. This typically is called if Wifi is down.
 */
void rs_stop(void);

//==============================================================================

#endif
