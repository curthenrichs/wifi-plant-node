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
 *  GET : /moisture : Argument["documentation"=<bool>]
 *  GET : /brightness : Argument["documentation"=<bool>]
 *
 * Note that this module makes no guarantee to the actual state of the LED
 * controller.
 *
 * The REST API is inspired and devloped after the LED Controller's IR remote.
 * Thus the features on the remote are exposed as routes in the service.
 */

//==============================================================================
//                                    Libraries
//==============================================================================

#include "RestService.h"

#include "LEDStrip.h"
#include "NetworkConfig.h"
#include <ESP8266WebServer.h>

//==============================================================================
//                           Data Structure Declaration
//==============================================================================

/**
 * Store previous commanded value as per this instance of the server. However as
 * noted, state is not enforced since external influence on LEDs can occur.
 */
typedef struct Service_State_Cache {
  String moisture;    //! Last moisture read
  String brightness;  //! Last brightness read
  String uri;         //! Last valid URI visited
} Service_State_Cache_t;

//==============================================================================
//                           Private Function Prototypes
//==============================================================================

/**
 * Root and Routing path presents a message to user on what is avalaible through
 * the LED service API.
 */
static void _handle_root(void);
/**
 * Returns the cached state of the service to the client however this function
 * does not provide proof that the LED controller is in that state.
 */
static void _handle_cached_state(void);
/**
 * API GET, POST call to set raw byte data directly on the controller.
 */
static void _handle_raw(void);
/**
 * API GET, POST method to configure brightness on the LED controller.
 */
static void _handle_brightness(void);
/**
 * API GET, POST method to configure the power state of the LED controller
 */
static void _handle_power(void);
/**
 * API GET, POST method to set controller into a special function mode.
 */
static void _handle_functions(void);
/**
 * API GET, POST method to set a static color on the LED controller.
 */
static void _handle_colors(void);
/**
 * Not found handle will return the call to client if invalid.
 */
static void _handle_not_found(void);

//==============================================================================
//                               Private Attributes
//==============================================================================

/**
 * Web server that handles one client at a time to control IR LED
 */
static ESP8266WebServer server(REST_SERVICE_PORT);
/**
 * Cache of previous operations on service
 */
static Service_State_Cache_t state;

//==============================================================================
//                         Public Function Implementation
//==============================================================================

/**
 * Initialize IR LED controller REST service. Note that WIFI must be enabled
 * before calling this function.
 */
void rs_init(void){

  state.raw = "unknown";
  state.brightness = "unknown";
  state.power = "unknown";
  state.function = "unknown";
  state.color = "unknown";
  state.uri = "unknown";

  server.on("/",              HTTP_GET,   _handle_root);
  server.on("/routes",        HTTP_GET,   _handle_root);
  server.on("/moisture",   _handle_moisture);
  server.on("/brightness", _handle_brightness);
  server.onNotFound(_handle_not_found);
  server.begin();
}

/**
 * Periodically update server to service single client connected.
 */
void rs_update(void){
  server.handleClient();
}

/**
 * Stop REST server. This typically is called if Wifi is down.
 */
void rs_stop(void){
  server.close();
}

//==============================================================================
//                         Private Function Implementation
//==============================================================================

/**
 * Root and Routing path presents a message to user on what is avalaible through
 * the LED service API.
 */
static void _handle_root(void){
  String message = "IR Controlled LED Strip Web Service\n\n"
      "Routes:\n"
      "\t- / (GET) Arguments: none\n"
      "\t- /routes (GET) Arguments: none\n"
      "\t- /cached-state (GET) Arguments : none\n"
      "\t- /raw (GET) Arguments:[boolean] or none, (POST) Arguments:[byte]\n"
      "\t- /brightness (GET) Arguments:[boolean] or none, (POST) Arguments:[string]\n"
      "\t- /power (GET) Arguments:[boolean] or none, (POST) Arguments:[string]\n"
      "\t- /function (GET) Arguments:[boolean] or none, (POST) Arguments:[string]\n"
      "\t- /color (GET) Arguments:[boolean] or none, (POST) Arguments:[string]\n";

  server.send(200, "text/plain", message);
  state.uri = server.uri();
}

/**
 * Returns the cached state of the service to the client however this function
 * does not provide proof that the LED controller is in that state.
 */
static void _handle_cached_state(void){
  String message = "IR Controlled LED Strip Web Service\n\n";
  message += "Cached State:\n";
  message += "\traw: " + state.raw + "\n";
  message += "\tbrightness: " + state.brightness + "\n";
  message += "\tpower: " + state.power + "\n";
  message += "\tfunction: " + state.function + "\n";
  message += "\tcolor: " + state.color + "\n";
  message += "\turi: " + state.uri + "\n";

  server.send(200, "text/plain", message);
  state.uri = server.uri();
}

/**
 * API GET, POST call to set raw byte data directly on the controller.
 */
static void _handle_raw(void){
  String message = "";
  bool set = false;
  int cmd = 0;

  if(server.method() == HTTP_POST){ // transmit raw
    String raw = server.arg("raw");
    if(raw != ""){
      cmd = raw.toInt();
      if(cmd >= 0 && cmd < 256){
        set = true;
        message += "success";
      }else{
        message += "error: invalid argument type";
      }
    }else{
      message += "error: argument expected";
    }
  }else{ // report raw codes
    if(server.arg("documentation") == "true"){
      message += "IR Controlled LED Strip Web Service\n\n"
          "Raw command expects POST request with a single arguement. "
          "The contents of this argument will be a byte code from "
          "table below.\n\n"
          "    Hex Value | Name\n"
          "    ----------|----------------\n"
          "    x04       | Brightness-Down\n"
          "    x05       | Brightness-Up\n"
          "    x06       | Off\n"
          "    x07       | On\n"
          "    x08       | ~Green\n"
          "    x09       | ~Red\n"
          "    x0A       | ~Blue\n"
          "    x0B       | ~White\n"
          "    x0C       | ~Pea Green\n"
          "    x0D       | ~Orange\n"
          "    x0E       | ~Dark Orchid\n"
          "    x0F       | Flash Function\n"
          "    x10       | ~Cyan\n"
          "    x11       | ~Dark Yellow\n"
          "    x12       | ~Magenta\n"
          "    x13       | Fade Function\n"
          "    x14       | ~Light Blue\n"
          "    x15       | ~Yellow\n"
          "    x16       | ~Pink\n"
          "    x17       | Strobe Function\n"
          "    x18       | ~Sky Blue\n"
          "    x19       | ~Light Yellow\n"
          "    x1A       | ~Purple\n"
          "    x1B       | Smooth Function\n"
          "\n"
          "Special functions have a unique property depending if one send the"
          " command after it is already in the selected mode. The following lists"
          " describes this behavior.\n"
          "  - Pressing Flash once does same action as smooth\n"
          "  - Pressing Flash twice strobes between color transitions of flash 1.\n"
          "  - Pressing Strobe once strobes currently displayed color\n"
          "  - Pressing Strobe twice smoothly changes brightness of static color.\n"
          "  - Pressing fade once fades between all colors\n"
          "  - Pressing fade twice fades only an rgb single cycling them.\n"
          "  - Pressing smooth once transitions between all colors abruptly.\n"
          "  - Pressing smooth twice flashes only an rgb single cycling them.\n"
          "\n"
          "Brightness adjustment is measured in ticks. To move from brightest"
          " to least will take 9 ticks.\n"
          "\n"
          "Brightness adjustment will act as expected for static colors. However "
          "when running a special function the brightness adjustment will alter "
          "the transition speed of the current function.\n"
          "  - During Flash increases/decreases transition speed (9 ticks)\n"
          "  - During Strobe increases/decreases transition speed (9 ticks)\n"
          "  - During Fade increases/decreases transition speed (9 ticks)\n"
          "  - During Smooth increases/decreases transition speed (9 ticks)\n";
    }else{
      message += "raw: " + state.raw;
    }
  }

  server.send(200, "text/plain", message);
  state.uri = server.uri();

  // Transmit over IR LED
  if(set){
    led_send_value(cmd);
    state.raw = cmd;
  }
}

/**
 * API GET, POST method to configure brightness on the LED controller.
 */
static void _handle_brightness(void){
  String message = "";
  bool set = false;
  int cmd = 0;

  if(server.method() == HTTP_POST){ // update brightness
    String brightness = server.arg("brightness");
    if(brightness != ""){
      if(brightness == "up"){
        state.brightness = brightness;
        set = true;
        cmd = LED_BRIGHTNESS_UP;
      }else if(brightness == "down"){
        state.brightness = brightness;
        set = true;
        cmd = LED_BRIGHTNESS_DOWN;
      }else{
        message += "error: argument does not match expected";
      }
    }else{
      message += "error: argument expected";
    }

    if(set){
      message += "success";
    }

  }else{ // report brightness adjust mechanism
    if(server.arg("documentation") == "true"){
      message += "IR Controlled LED Strip Web Service\n\n"
          "Brightness command expects POST request with a single arguement. "
          "The contents of this argument will be a string enumeration from "
          "table below.\n\n"
          "   String | Behavior\n"
          "   -------|----------------------------------\n"
          "   up     | Shifts LED brightness up a step  \n"
          "   down   | Shifts LED brightness down a step\n"
          "\n"
          "Brightness adjustment is measured in ticks. To move from brightest"
          " to least will take 9 ticks.\n"
          "\n"
          "Brightness adjustment will act as expected for static colors. However "
          "when running a special function the brightness adjustment will alter "
          "the transition speed of the current function.\n"
          "  - During Flash increases/decreases transition speed (9 ticks)\n"
          "  - During Strobe increases/decreases transition speed (9 ticks)\n"
          "  - During Fade increases/decreases transition speed (9 ticks)\n"
          "  - During Smooth increases/decreases transition speed (9 ticks)\n";
    }else{
      message += "brightness: " + state.brightness;
    }
  }

  server.send(200, "text/plain", message);
  state.uri = server.uri();

  // Transmit over IR LED
  if(set){
    led_send_value(cmd);
    state.raw = cmd;
  }
}

/**
 * API GET, POST method to configure the power state of the LED controller
 */
static void _handle_power(void){
  String message = "";
  bool set = false;
  int cmd = 0;

  if(server.method() == HTTP_POST){ // update power state
    String power = server.arg("power");
    if(power != ""){
      if(power == "on"){
        state.power = power;
        set = true;
        cmd = LED_ON_CMD;
      }else if(power == "off"){
        state.power = power;
        set = true;
        cmd = LED_OFF_CMD;
      }else{
        message += "error: argument does not match expected";
      }
    }else{
      message += "error: argument expected";
    }

    if(set){
      message += "success";
    }
  }else{ // report power information
    if(server.arg("documentation") == "true"){
      message += "IR Controlled LED Strip Web Service\n\n"
          "Power command expects POST request with a single arguement. "
          "The contents of this argument will be a string enumeration from "
          "table below.\n\n"
          "   String | Behavior\n"
          "   -------|-------------------------------------\n"
          "   on     | Commands LED controller to ON state \n"
          "   off    | Commands LED controller to OFF state\n";
    }else{
      message += "power: " + state.power;
    }
  }

  server.send(200, "text/plain", message);
  state.uri = server.uri();

  // Transmit over IR LED
  if(set){
    led_send_value(cmd);
    state.raw = cmd;
  }
}

/**
 * API GET, POST method to set controller into a special function mode.
 */
static void _handle_functions(void){
  String message = "";
  bool set = false;
  int cmd = 0;

  if(server.method() == HTTP_POST){ // update function state
    String function = server.arg("function");
    if(function != ""){
      if(function == "flash"){
        state.function = function;
        set = true;
        cmd = LED_FLASH_CMD;
      }else if(function == "strobe"){
        state.function = function;
        set = true;
        cmd = LED_STROBE_CMD;
      }else if(function == "fade"){
        state.function = function;
        set = true;
        cmd = LED_FADE_CMD;
      }else if(function == "smooth"){
        state.function = function;
        set = true;
        cmd = LED_SMOOTH_CMD;
      }else{
        message += "error: argument does not match expected";
      }
    }else{
      message += "error: argument expected";
    }

    if(set){
      message += "success";
    }
  }else{ // report function information
    if(server.arg("documentation") == "true"){
      message += "IR Controlled LED Strip Web Service\n\n"
          "Function command expects POST request with a single arguement. "
          "The contents of this argument will be a string enumeration from "
          "table below.\n\n"
          "   String | Behavior\n"
          "   -------|--------------------------------------------\n"
          "   flash  | Flash a subset of preselected colors (Note)\n"
          "   strobe | Strobe last static color selected (Note)   \n"
          "   fade   | Fade last static color selected (Note)     \n"
          "   smooth | Smooth last static color selected (Note)   \n"
          "\n"
          "Special functions have a unique property depending if one send the"
          " command after it is already in the selected mode. The following lists"
          " describes this behavior.\n"
          "  - Pressing Flash once does same action as smooth\n"
          "  - Pressing Flash twice strobes between color transitions of flash 1.\n"
          "  - Pressing Strobe once strobes currently displayed color\n"
          "  - Pressing Strobe twice smoothly changes brightness of static color.\n"
          "  - Pressing fade once fades between all colors\n"
          "  - Pressing fade twice fades only an rgb single cycling them.\n"
          "  - Pressing smooth once transitions between all colors abruptly.\n"
          "  - Pressing smooth twice flashes only an rgb single cycling them.\n"
          "\n"
          "When running a special function the brightness adjustment will alter"
          "the transition speed of the current function.\n"
          "  - During Flash increases/decreases transition speed (9 ticks)\n"
          "  - During Strobe increases/decreases transition speed (9 ticks)\n"
          "  - During Fade increases/decreases transition speed (9 ticks)\n"
          "  - During Smooth increases/decreases transition speed (9 ticks)\n";
    }else{
      message += "function: " + state.function;
    }
  }

  server.send(200, "text/plain", message);
  state.uri = server.uri();

  // Transmit over IR LED
  if(set){
    led_send_value(cmd);
    state.raw = cmd;
  }
}

/**
 * API GET, POST method to set a static color on the LED controller.
 */
static void _handle_colors(void){
  String message = "";
  bool set = false;
  int cmd = 0;

  if(server.method() == HTTP_POST){ // update color state
    String color = server.arg("color");
    if(color != ""){
      if(color == "white"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_WHITE;
      }else if(color == "red"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_RED;
      }else if(color == "orange"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_ORANGE;
      }else if(color == "dark-yellow"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_DARK_YELLOW;
      }else if(color == "yellow"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_YELLOW;
      }else if(color == "light-yellow"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_LIGHT_YELLOW;
      }else if(color == "green"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_GREEN;
      }else if(color == "pea-green"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_PEA_GREEN;
      }else if(color == "cyan"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_CYAN;
      }else if(color == "light-blue"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_LIGHT_BLUE;
      }else if(color == "sky-blue"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_SKY_BLUE;
      }else if(color == "blue"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_BLUE;
      }else if(color == "dark-orchid"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_DARK_ORCHID;
      }else if(color == "purple"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_PURPLE;
      }else if(color == "magenta"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_MAGENTA;
      }else if(color == "pink"){
        state.color = color;
        set = true;
        cmd = LED_COLOR_PINK;
      }else{
        message += "error: argument does not match expected";
      }
    }else{
      message += "error: argument expected";
    }

    if(set){
      message += "success";
    }
  }else{ // report color information
    if(server.arg("documentation") == "true"){
      message += "IR Controlled LED Strip Web Service\n\n"
          "Color command expects POST request with a single arguement. "
          "The contents of this argument will be a string enumeration from "
          "list below.\n\n"
          "   - white\n"
          "   - red\n"
          "   - orange\n"
          "   - dark-yellow\n"
          "   - yellow\n"
          "   - light-yellow\n"
          "   - green\n"
          "   - pea-green\n"
          "   - cyan\n"
          "   - light-blue\n"
          "   - sky-blue\n"
          "   - blue\n"
          "   - dark-orchid\n"
          "   - purple\n"
          "   - magenta\n"
          "   - pink\n";
    }else{
      message += "color: " + state.color;
    }
  }

  server.send(200, "text/plain", message);
  state.uri = server.uri();

  // Transmit over IR LED
  if(set){
    led_send_value(cmd);
    state.raw = cmd;
  }
}

/**
 * Not found handle will return the call to client if invalid.
 */
static void _handle_not_found(void){
  String message = "404: Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for (uint8_t i=0; i<server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}

	server.send (404,"text/plain",message);
}
