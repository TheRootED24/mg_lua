require "mg_types"
local mg = require "mg_lua"

local s_ca_path = "ca.pem";
local s_url = "ws://localhost:8600/websocket";
local run = true;

usage = [[
		Usage: %s OPTIONS
		 ******************** OPTIONS ********************
		-ca 	PATH  	Path to the CA file	default: %s
		-url 	URL	Listen on URL		default: %s
]]

WS_CALLBACK = function (c, ev, ev_data)
	if (ev == MG_EV_OPEN) then
		local conn = mg.connection.new(c);
		conn:is_hexdumping(0);
	elseif (ev == MG_EV_ERROR) then
		-- On error, log error message
		mg.log.error(string.format("%s", tostring(ev_data)))
	elseif (ev == MG_EV_WS_OPEN) then
		-- When websocket handshake is successful, send message
		mg.ws.send(c, "hello", 5, WEBSOCKET_OP_TEXT);
	elseif (ev == MG_EV_WS_MSG) then
		-- When we get echo response, print it
		local wm = mg.ws.message.new(ev_data);
		print(string.format("GOT ECHO REPLY: [ %s ] Length [ %d ]\n", wm:data(), wm:len()));
		local conn = mg.connection.new(c);
		conn:is_closing(1);  -- close connection
	elseif (ev == MG_EV_ERROR or ev == MG_EV_CLOSE) then
		run = false; -- Signal we're done
	end
end

local function main(arg)
	local i = 1;
	while(i < #arg) do
		if(arg[i] == "-ca") then
			s_ca_path = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-url") then
			s_url = arg[i + 1];
			i = i + 2
		else
			local payload = string.format(usage,  arg[0] or "progname", s_ca_path, s_url)
			mg.log.error(payload);
			return;
		end
	end

	local mgr = mg.mgr.new();				-- Allocate a new mgr
	mgr = mg.mgr.init(mgr);					-- Initialise event manager
	mg.log.log_set(MG_LL_VERBOSE);				-- Set log level
	local c = mg.ws.connect(mgr, s_url, "WS_CALLBACK"); 	-- Create client 

	while(run) do
		mg.mgr.poll(mgr, 1000);				-- Wait for echo
	end
	mg.mgr.free(mgr);					-- Deallocate resources
end

main(arg)