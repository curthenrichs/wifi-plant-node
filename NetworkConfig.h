/**
 * NetworkConfig
 * @author Curt Henrichs
 * @date 1-23-2018
 *
 * Network Configuration file is intented to capture all build parameters for
 * the station and API layers. Additionally, network specific constants should
 * be located here to reduce conflict. (Such as another server being hosted on
 * the same port)
 */

#ifndef NETWORKCONFIG_H
#define NETWORKCONFIG_H

//==============================================================================
//                       Constant and Macro Definitions
//==============================================================================

#define REST_SERVICE_PORT (80) //! REST API port is default for browser access

#define ST_CONFIGURATION_DEFAULT_PASSWORD "password" //! default for AP mode

//==============================================================================

#endif
