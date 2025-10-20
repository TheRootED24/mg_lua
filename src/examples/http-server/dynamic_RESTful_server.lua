require "mg_types" -- enums for mg network event types
local mg = require "mg_lua"
local s_web_root = "examples/http-server/web_root/";

-- callback functions must be global
CALLBACK = function(c, ev, ev_data)
	if( ev == MG_EV_HTTP_MSG ) then
		print("here")
		local hm = mg.http.message.new(ev_data)
		print(hm:body())
		if(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/api/f1"))) then
			local conn = mg.connection.new(c);
			mg.http.reply(conn, 200, "Content-Type: application/json\r\n", string.format("{\"result\": %d}", 123));
		elseif(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/api/sum"))) then
			local num1 = mg.json.get_num(mg.str.new(hm:body()), "$[0]")
			local num2 = mg.json.get_num(mg.str.new(hm:body()), "$[1]")

			if( num1 and num2 ) then
				local conn = mg.connection.new(c);
				mg.http.reply(c, 200, "Content-Type: application/json\r\n", string.format("{\"result\": %d}", num1 + num2));
			else
				local conn = mg.connection.new(c);
				mg.http.reply(c, 500, "", "Parameters missing\n");
			end
		else
			print("no here")
			local conn = mg.connection.new(c)
;			local opts = mg.http.serve_opts.new(s_web_root);
			mg.http.serve_dir(conn, hm, opts);
		end
	end
end

local function main(...)
	local mgr = mg.mgr.new() 	-- Event manager
	mgr = mg.mgr.init(mgr)		-- Initialize event manager

	-- Setup listener
	local conn = mg.http.listen(mgr, "localhost:8200", "CALLBACK")

	-- Start Event loop (~1 min)
	for i = 1, 60 do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr)
	return
end

main(...)
