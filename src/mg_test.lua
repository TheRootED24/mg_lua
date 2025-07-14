#!/usr/bin/env lua

mg = require "mg_lua"
local mgr = mg.mgr.new()
mgr = mg.mgr.init(mgr)

local MG_EV_ERROR		= 0   --// Error                        char *error_message
local MG_EV_OPEN		= 1   --// Connection created           NULL
local MG_EV_POLL		= 2   --// mg_mgr_poll iteration        uint64_t *uptime_millis
local MG_EV_RESOLVE		= 3   --// Host name is resolved        NULL
local MG_EV_CONNECT		= 4   --// Connection established       NULL
local MG_EV_ACCEPT		= 5   --// Connection accepted          NULL
local MG_EV_TLS_HS		= 6   --// TLS handshake succeeded      NULL
local MG_EV_READ		= 7   --// Data received from socket    long *bytes_read
local MG_EV_WRITE		= 8   --// Data written to socket       long *bytes_written
local MG_EV_CLOSE		= 9   --// Connection closed            NULL
local MG_EV_HTTP_HDRS	= 10  --// HTTP headers                 struct mg_http_message *
local MG_EV_HTTP_MSG	= 11  --// Full HTTP request/response   struct mg_http_message *
local MG_EV_WS_OPEN		= 12  --// Websocket handshake done     struct mg_http_message *
local MG_EV_WS_MSG		= 13  --// Websocket msg, text or bin   struct mg_ws_message *
local MG_EV_WS_CTL		= 14  --// Websocket control msg        struct mg_ws_message *
local MG_EV_MQTT_CMD	= 15  --// MQTT low-level command       struct mg_mqtt_message *
local MG_EV_MQTT_MSG	= 16  --// MQTT PUBLISH received        struct mg_mqtt_message *
local MG_EV_MQTT_OPEN	= 17  --// MQTT CONNACK received        int *connack_status_code
local MG_EV_SNTP_TIME	= 18  --// SNTP time received           uint64_t *epoch_millis
local MG_EV_WAKEUP		= 19  --// mg_wakeup() data received    struct mg_str *data
local MG_EV_USER		= 20  --// Starting ID for user events

local WEBSOCKET_OP_CONTINUE = 0
local WEBSOCKET_OP_TEXT 	= 1
local WEBSOCKET_OP_BINARY 	= 2
local WEBSOCKET_OP_CLOSE 	= 8
local WEBSOCKET_OP_PING 	= 9
local WEBSOCKET_OP_PONG 	= 10

json = "{\"a\":\"b:c\"}"
resp = mg.json.get_token(json, "$.a")
print(resp)
s = mg.str.new("192.168.1.1")
print(s:buf(),s:len())
addr = mg.addr.new()
atn = mg.string.aton(s, addr)


print(addr:ip())

test = function(c, ev, ev_data)
	--print(c,ev,ev_data)
	conn = mg.connection.new(c)
	if (ev == MG_EV_OPEN) then
		conn:is_hexdumping(0)
	elseif (ev == MG_EV_HTTP_MSG) then
		hm = mg.http.message.new(ev_data)
		if(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/websocket"))) then
			mg.ws.upgrade(conn, hm)
		elseif (mg.string.match(mg.str.new(hm:uri()), mg.str.new(("/rest")))) then
			mg.http.reply(conn, 200, "", string.format("{\"result\": %d}\n", 123))
		else
			opts = mg.http.serve_opts.new();
			mg.http.serve_dir(conn, hm, opts);
		end
	elseif (ev == MG_EV_WS_MSG) then
		wm = mg.ws.message.new(ev_data)
		mg.ws.send(conn, wm:data(), wm:len(), WEBSOCKET_OP_TEXT);
	end

end

test2 = function(c, ev, ev_data)
	print(c,ev,ev_data)
	conn = mg.connection.new(c)
	print("connect")
	print(conn:id())
end

local conn = mg.http.listen(mgr, "0.0.0.0:8200", "test")
print(conn:id())

for i = 1, 60 do
	mgr:poll(1000)
end

mgr:free()
print(resp)