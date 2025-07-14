local mg = require "mg_lua"

local body = mg.str.ptr('{"log_enabled":true,"log_level":1,"brightness":52,"device_name":"My Device"}')
local log_enabled = mg.json.get_bool(body, "$.log_enabled");
local log_level = mg.json.get_long(body, "$.log_level");
local brightness = mg.json.get_long(body, "$.brightness");
local device_name = mg.json.get_str(body, "$.device_name");

print(log_enabled)
print(log_level)
print(brightness)
print(device_name)
