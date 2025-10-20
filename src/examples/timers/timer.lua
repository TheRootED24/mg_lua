--[[
// Example Websocket server with timers. This is a simple Websocket echo
// server, which sends a message to all connected clients periodically,
// using timer API.
]]--

local mg = require "mg_lua"

local s_listen_on = "http://localhost:8000";
local s_web_root = "examples/timers/web_root"

local run = true;
local SIGINT = 2;
local SIGTERM = 15;

s_callback = function(c, ev, ev_data)
	if (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);

		if(mg.string.match(mg.str.new(hm.uri), mg.str.new("/websocket"))) then
				local conn = mg.connection.new(c);
				conn.data = "W";
				mg.ws.upgrade(conn, hm);
		else
			local opts = mg.http.serve_opts.new(s_web_root);
			local conn = mg.connection.new(c);
			mg.http.serve_dir(conn, hm, opts);
		end
	elseif(ev == MG_EV_WS_MSG) then
		local conn = mg.connection.new(c);

		if(conn.data == "W") then
			local wm = mg.ws.message.new(ev_data)
			mg.ws.send(conn, wm.data, wm.len, WEBSOCKET_OP_TEXT);
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

function timer(arg)
	local mgr = mg.mgr.new(arg);
	local conn = mg.connection.new(mgr:conns());

	while(conn) do
		if(conn.data == "W") then
			mg.ws.send(conn, "hi", 2, WEBSOCKET_OP_TEXT);
		end
		conn = conn:next();
	end
end

local function main(arg)
	local mgr = mg.mgr.new()
	mgr = mg.mgr.init(mgr);
	mg.log.log_set(MG_LL_DEBUG);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	mg.timer.add(mgr, 1000, MG_TIMER_REPEAT, "timer", mgr); -- add option to specify flags

	local con = mg.http.listen(mgr, s_listen_on, "s_callback");

	while(run) do
		mg.mgr.poll(mgr, 500);
	end
	mg.mgr.free(mgr);

end

main(arg);