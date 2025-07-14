-- MG LOG LEVEL ENUMS
MG_LL_NONE		= 0 -- Disable logging
MG_LL_ERROR		= 1 -- Log errors only
MG_LL_INFO		= 2 -- Log errors and info messages
MG_LL_DEBUG		= 3 -- Log errors, info and debug messages
MG_LL_VERBOSE		= 4 -- Log everything, and more

-- MG MGR EVENT (EV) ENUMS
MG_EV_ERROR		= 0   --// Error                        char *error_message
MG_EV_OPEN		= 1   --// Connection created           NULL
MG_EV_POLL		= 2   --// mg_mgr_poll iteration        uint64_t *uptime_millis
MG_EV_RESOLVE		= 3   --// Host name is resolved        NULL
MG_EV_CONNECT		= 4   --// Connection established       NULL
MG_EV_ACCEPT		= 5   --// Connection accepted          NULL
MG_EV_TLS_HS		= 6   --// TLS handshake succeeded      NULL
MG_EV_READ		= 7   --// Data received from socket    long *bytes_read
MG_EV_WRITE		= 8   --// Data written to socket       long *bytes_written
MG_EV_CLOSE		= 9   --// Connection closed            NULL
MG_EV_HTTP_HDRS		= 10  --// HTTP headers                 struct mg_http_message *
MG_EV_HTTP_MSG		= 11  --// Full HTTP request/response   struct mg_http_message *
MG_EV_WS_OPEN		= 12  --// Websocket handshake done     struct mg_http_message *
MG_EV_WS_MSG		= 13  --// Websocket msg, text or bin   struct mg_ws_message *
MG_EV_WS_CTL		= 14  --// Websocket control msg        struct mg_ws_message *
MG_EV_MQTT_CMD		= 15  --// MQTT low-level command       struct mg_mqtt_message *
MG_EV_MQTT_MSG		= 16  --// MQTT PUBLISH received        struct mg_mqtt_message *
MG_EV_MQTT_OPEN		= 17  --// MQTT CONNACK received        int *connack_status_code
MG_EV_SNTP_TIME		= 18  --// SNTP time received           uint64_t *epoch_millis
MG_EV_WAKEUP		= 19  --// mg_wakeup() data received    struct mg_str *data
MG_EV_USER		= 20  --// Starting ID for user events

-- MG WEBSOCKET OP CODE ENUMS
WEBSOCKET_OP_CONTINUE	= 0
WEBSOCKET_OP_TEXT	= 1
WEBSOCKET_OP_BINARY	= 2
WEBSOCKET_OP_CLOSE	= 8
WEBSOCKET_OP_PING	= 9
WEBSOCKET_OP_PONG	= 10

-- MG FILESYTEM ENUMS
MG_FS_READ		= 1
MG_FS_WRITE		= 2
MG_FS_DIR		= 4

-- FILESYTEM TYPE ENUMS
MG_FS_POSIX 		= 0
MG_FS_FAT		= 1
MG_FS_PACKED		= 2

-- MG TIMER ENUMS
MG_TIMER_ONCE		= 0
MG_TIMER_REPEAT 	= 1
MG_TIMER_RUN_NOW	= 2

-- MQTT CMD TYPES
MQTT_CMD_CONNECT	= 1
MQTT_CMD_CONNACK	= 2
MQTT_CMD_PUBLISH	= 3
MQTT_CMD_PUBACK		= 4
MQTT_CMD_PUBREC		= 5
MQTT_CMD_PUBREL		= 6
MQTT_CMD_PUBCOMP 	= 7
MQTT_CMD_SUBSCRIBE	= 8
MQTT_CMD_SUBACK 	= 9
MQTT_CMD_UNSUBSCRIBE	= 10
MQTT_CMD_UNSUBACK	= 11
MQTT_CMD_PINGREQ	= 12
MQTT_CMD_PINGRESP	= 13
MQTT_CMD_DISCONNECT	= 14


MQTT_OK			= 0
MQTT_INCOMPLETE		= 1
MQTT_MALFORMED		= 2


MG_TIMER_ONCE		= 0
MG_TIMER_REPEAT		= 1
MG_TIMER_RUN_NOW	= 2