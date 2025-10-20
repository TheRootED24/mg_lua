local mg = require "mg_lua"
local s_web_root = "examples/http-server/web_root/";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

-- callback functions must be global
CALLBACK = function(c, ev, ev_data)
	if( ev == MG_EV_HTTP_MSG ) then
		local hm = mg.http.message.new(ev_data)
		if(mg.string.match(mg.str.new(hm.uri), mg.str.new("/api/f1"))) then
			local conn = mg.connection.new(c);
			mg.http.reply(conn, 200, "Content-Type: application/json\r\n", string.format("{\"result\": %d}", 123));
		elseif(mg.string.match(mg.str.new(hm.uri), mg.str.new("/api/sum"))) then
			local num1 = mg.json.get_num(mg.str.new(hm.body), "$[0]")
			local num2 = mg.json.get_num(mg.str.new(hm.body), "$[1]")

			if( num1 and num2 ) then
				local conn = mg.connection.new(c);
				mg.http.reply(conn, 200, "Content-Type: application/json\r\n", string.format("{\"result\": %d}", num1 + num2));
			else
				local conn = mg.connection.new(c);
				mg.http.reply(conn, 500, "", "Parameters missing\n");
			end
		else
			local conn = mg.connection.new(c);
			local opts = mg.http.serve_opts.new(s_web_root);
			mg.http.serve_dir(conn, hm, opts);
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(...)
	local mgr = mg.mgr.new() 	-- Event manager
	mgr = mg.mgr.init(mgr)		-- Initialize event manager

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	-- Setup listener
	local conn = mg.http.listen(mgr, "localhost:8000", "CALLBACK")

	-- Start Event loop (~1 min)
	while(run) do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr)
	return
end

main(...)
