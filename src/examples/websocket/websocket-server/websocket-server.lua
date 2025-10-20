require "mg_types"
local mg = require "mg_lua"

local s_listen_on = "ws://localhost:8600";
local s_web_root = ".";
local s_ca_path = "ca.pem";
local s_cert_path = "cert.pem";
local s_key_path = "key.pem";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

CALLBACK = function(c, ev, ev_data)
	if (ev == MG_EV_OPEN) then

		local conn = mg.connection.new(c);
		conn:is_hexdumping(0);

	elseif (ev == MG_EV_HTTP_MSG) then

		local hm = mg.http.message.new(ev_data);

		if(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/websocket"))) then
			local conn = mg.connection.new(c);
			conn:data("WS", 2);
			mg.ws.upgrade(conn, hm);
		elseif (mg.string.match(mg.str.new(hm:uri()), mg.str.new(("/rest")))) then
			local conn = mg.connection.new(c);
			mg.http.reply(conn, 200, "", string.format("{%q: %d}\n", "result", 123))
		else
			local opts = mg.http.serve_opts.new(s_web_root);
			local conn = mg.connection.new(c);
			mg.http.serve_dir(conn, hm, opts);
		end

	elseif (ev == MG_EV_WS_MSG) then

		local conn = mg.connection.new(c);
		-- NOTE mg.ws.messages.new() must be called after gathering the connections
		-- currently, conns are popped from stack upon call to message.new() ...
		local t = conn:mgr_conns()
		--/* load all marked "WS" connection into conns table */--
		local conns = {}
			while(t) do
				--print(t:id(), t:loc_ip(), t:rem_ip())
				if(t:data() == "WS" ) then
					conns[ #conns + 1 ] = t;
				end
			t = t:next();
			end

		local wm = mg.ws.message.new(ev_data)
		--/* broadcast to marked "WS" connections */--
		if(#conns > 1) then
			for k,v in pairs(conns) do
					mg.ws.send(v, wm:data(), wm:len(), WEBSOCKET_OP_TEXT);
			end
		else
			--/* single connection reply */--
			mg.ws.send(conn, wm:data(), wm:len(), WEBSOCKET_OP_TEXT);
		end

	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local usage = [[
		Usage: %s OPTIONS
		 ******************** OPTIONS ********************
		-ca 	PATH  	Path to the CA file	default: %s
		-cert 	PATH	Path to the CERT file	default: %s
		-key 	PATH	Path to the KEY file	default: %s
		-url 	URL	Listen on URL		default: %s
]]

local function main(arg)
	local i = 1;
	while(i < #arg) do
		if(arg[i] == "-url") then
			s_listen_on = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-ca") then
			s_ca_path = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-cert") then
			s_cert_path = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-key") then
			s_key_path = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-v") then
			mg.log.log_set(arg[i + 1]);
			i = i + 2
		else
			local payload = string.format("\n\n" .. usage,  arg[0] or "??", s_ca_path, s_cert_path, s_key_path, s_listen_on)
			mg.log.error(payload);
			return;
		end
	end

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);
	print(string.format("Starting WS listener on %s/websocket\n", s_listen_on));
	mg.http.listen(mgr, s_listen_on, "CALLBACK");

	while(run) do
		mg.mgr.poll(mgr, 1000);
	end

	mg.mgr.free(mgr);
end


main(arg)

