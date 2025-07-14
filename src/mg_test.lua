#!/usr/bin/env lua

require "mg_types"
mg = require "mg_lua"

test = function(c, ev, ev_data)
	conn = mg.connection.new(c)
	local done = false;
	if (ev == MG_EV_OPEN) then
		conn:is_hexdumping(0)
	elseif (ev == MG_EV_HTTP_MSG) then
		local t = conn:mgr()

		while(t) do
			print("ID: " .. t:id())
			print("is accepted: " .. t:is_accepted())
			print("is listening: " ..t:is_listening())
			print("local ip: ".. t:loc_ip())
			print("local ip w/port: " .. t:loc_port())
			print("remote ip: ".. t:rem_ip())
			print("remote ip w/port: " .. t:rem_port())
			t = t:next()
		end

		local hm = mg.http.message.new(ev_data)

		if(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/websocket"))) then
			mg.ws.upgrade(conn, hm)
		elseif (mg.string.match(mg.str.new(hm:uri()), mg.str.new(("/rest")))) then
			mg.http.reply(conn, 200, "", string.format("{%q: %d}\n", "result", 123))
		else
			local opts = mg.http.serve_opts.new();
			mg.http.serve_dir(conn, hm, opts);
		end

	elseif (ev == MG_EV_WS_MSG) then
		local wm = mg.ws.message.new(ev_data)
		mg.ws.send(conn, wm:data(), wm:len(), WEBSOCKET_OP_TEXT);
	end

end

local mgr = mg.mgr.new()
mgr = mg.mgr.init(mgr)

local co = mg.http.listen(mgr, "0.0.0.0:4600", "test")
print(co:id())

for i = 1, 60 do
	--mgr:poll(1000)
	mg.mgr.poll(mgr, 1000)
end

--mgr:free()
mg.mgr.free(mgr)
