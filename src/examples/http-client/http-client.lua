local mg = require "mg_lua"

local s_url = "http://info.cern.ch/"

CALLBACK = function(c, ev, ev_data)
	if(ev == MG_EV_CONNECT) then
		local host = mg.str.new(mg.url.host(s_url))
		local uri = mg.str.new(mg.url.uri(s_url))
		local conn = mg.connection.new(c);

		-- Send request
		mg.printf(conn, string.format("GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", uri.buf, host.buf))
	end

	if(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		print(hm.message);
	end
end

function main(...)
	local mgr = mg.mgr.new()
	mgr = mg.mgr.init(mgr)

	mg.http.connect(mgr, s_url, "CALLBACK")

	-- Start Event loop (~1 min)
	for i = 1, 8 do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr)
	mg.free_lua_states();
	return

end

main(...)