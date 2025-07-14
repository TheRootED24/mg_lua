# mg_lua
mg_lua is a full set lua bindings for the Mongoose C Networking Library

# Usage Examples

Below are quick snippets that should give an idea how simple the API is and how easy it is to create applications with it.

Create a simple web server that serves a directory. The behavior of the HTTP server is specified by its event handler function:
```lua
local mg = require "mg_lua"   -- include the mg_lua library 

local web_root = ".";
-- HTTP server event handler function
function ev_handler(c, ev, ev_data) 
  if (ev == MG_EV_HTTP_MSG) then
    local hm = mg.http.message.new(ev_data);
    local opts = mg.http.serve_opts.new(web_root);
    mg.http.serve_dir(c, hm, opts);
  end
end

local function main(...) 
  local run = true
  local mgr = mg.mgr.new();  -- Declare event manager
  mg.mgr.init(mgr);  -- Initialise event manager
  mg.http.listen(mgr, "http://0.0.0.0:8000", "ev_handler");  -- Setup listener
  while(run) do         -- Run an infinite event loop
    mg.mgr.poll(mgr, 1000);
  end
  return 0;
end
```
HTTP server implements a REST API that returns current time. JSON formatting:
```lua
local mg = require "mg_lua"

function ev_handler(c, ev, ev_data)
  if (ev == MG_EV_HTTP_MSG) then
    local hm = mg.http.message.new(ev_data);
    
    if (mg.string.match(mg.str.new(hm.uri), mg.str.new("/api/time/get"))) then
      local conn = mg.connection.new(c);
      mg.http.reply(conn, 200, "", string.format("{%q:%q}\n", "time", tostring(os.date())));
    else
	    local conn = mg.connection.new(c);
        mg.http.reply(conn, 500, "",  string.format("{%q:%q}\n", "error", "Unsupported URI"));
    end
  end
end

local function main(...) 
  local run = true
  local mgr = mg.mgr.new();  -- Declare event manager
  mg.mgr.init(mgr);  -- Initialise event manager
  mg.http.listen(mgr, "http://0.0.0.0:8000", "ev_handler");  -- Setup listener
  while(run) do         -- Run an infinite event loop
    mg.mgr.poll(mgr, 1000);
  end
  return 0;
end
```
MQTT client that subscribes to a topic device1/rx and echoes incoming messages to device1/tx:
```lua
local mg = require "mg_lua"

local s_mqtt_url = "mqtt://broker.hivemq.com:1883";
local s_mqtt_conn = "NULL";
local run = true;
-- MQTT connection event handler function
function ev_handler(c, ev, ev_data)
  if (ev == MG_EV_OPEN) then
    local conn = mg.connection.new(c);
    mg.log.info(string.format("%d created, connecting to %s ...", conn.id, s_mqtt_url));
  elseif (ev == MG_EV_MQTT_OPEN) then
   local conn = mg.connection.new(c);
   local opts = mg.mqtt.opts.new();
   opts.qos = 1;
   opts.topic = "device1/rx";
   mg.mqtt.sub(conn, opts);
   mg.log.info(string.format("%d connected, subscribing to %s", conn.id, opts.topic));
  elseif (ev == MG_EV_MQTT_MSG) then
    local conn = mg.connection.new(c);
    local mm = mg.mqtt.message.new(ev_data);
    local pub_opts = mg.mqtt.opts.new();
    pub_opts.qos = 1;
    pub_opts.topic = "device1/tx";
    pub_opts.message = string.format("Received [%s] / [%s]", mm.topic, mm.data);
    mg.mqtt.pub(conn, pub_opts);
  elseif (ev == MG_EV_CLOSE) then
    local conn = mg.connection.new(c);
    mg.log.info(string.format("%d closing", conn.id));
    s_mqtt_conn = "NULL";
  end
end

-- Reconnection timer function. If we get disconnected, reconnect again
function timer_fn(args)
  local mgr = mg.mgr.new(args);
  
  if (s_mqtt_conn == "NULL") then
    local opts = mg.mqtt.opts.new();
    opts.clean = 0;
    s_mqtt_conn = mg.mqtt.connect(mgr, s_mqtt_url, opts, "ev_handler", mgr);
  end
end

function main(...)
  local mgr = mg.mgr.new();  -- Mongoose event manager. Holds all connections
  mgr = mg.mgr.init(mgr);  -- Initialise event manager
  mg.timer.add(mgr, 3000, MG_TIMER_REPEAT , MG_TIMER_RUN_NOW, "timer_fn", mgr);

  while(run) do
    mg.mgr.poll(mgr, 1000);  -- Infinite event loop
  end
  return 0;
end

```
