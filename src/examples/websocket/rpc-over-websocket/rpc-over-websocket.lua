local mg = require "mg_lua"

local s_listen_on = "ws://192.168.1.1:8000";
local s_web_root = "examples/websocket/rpc-over-websocket/web_root/";

local rpc_head;

local function init_head()
	rpc_head = mg.rpc.init_head();
end

local run = true
local SIGINT = 2;
local SIGTERM = 15;

function rpc_sum(req)
	local a, b = 0.0, 0.0;
	local r = mg.rpc.req.ptr(req);

	a = mg.json.get_num(mg.str.ptr(r:frame()), "$.params[0]");
	b = mg.json.get_num(mg.str.ptr(r:frame()), "$.params[1]");
	mg.rpc.ok(req, "%s",  string.format("%g", a + b));
end

function rpc_mul(req)
	local a, b  = 0.0, 0.0;
	local r = mg.rpc.req.ptr(req);

	a = mg.json.get_num(mg.str.ptr(r:frame()), "$.params[0]");
	b =  mg.json.get_num(mg.str.ptr(r:frame()), "$.params[1]");
	mg.rpc.ok(r, "%s", string.format("%g", a * b));
end

CALLBACK = function(c, ev, ev_data)
	if(ev == MG_EV_OPEN) then
		local conn = mg.connection.new(c);
		conn.is_hexdumping = 0;
	elseif(ev == MG_EV_WS_OPEN) then
		local conn = mg.connection.new(c);
		--conn.ctx:data("WS", 2); -- mark websocket connections with "WS"  ( optional syntax )
		conn.data = "WS"; -- mark websocket connections with "WS"
	elseif(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.ptr(ev_data);
		if(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/websocket"))) then
			local conn = mg.connection.ptr(c);
			-- Upgrade to websocket. From now on, a connection is a full-duplex
			-- Websocket connection, which will receive MG_EV_WS_MSG events.
			mg.ws.upgrade(conn, hm);
		else
			-- Serve static files
			local conn = mg.connection.ptr(c);
			local opts = mg.http.serve_opts.new(s_web_root);
			mg.http.serve_dir(conn, ev_data, opts);
		end
	elseif(ev == MG_EV_WS_MSG) then
		local wm = mg.ws.message.ptr(ev_data);
		local io = mg.iobuf.ptr();
		--local io_ok = mg.iobuf.init(io, 32,512);
		local req = mg.rpc.req.ptr(rpc_head, io, wm:data());

		mg.rpc.process(req);

		if(io.buf) then
			local conn = mg.connection.ptr(c);
			 mg.ws.send(conn, io:buf(), io:len(), WEBSOCKET_OP_TEXT);
		end
		mg.iobuf.free(io);
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

function timer(arg)
	local mgr = mg.mgr.new(arg);
	local conn = mg.connection.ptr(mgr:conns());

	while(conn) do
		if(conn:data() == "WS") then
			mg.ws.printf(conn, WEBSOCKET_OP_TEXT, "%s", string.format("{%q:%q,%q:[%d,%d,%d]}", "method", "notification1", "params", 1,2,3));
		end
		conn = conn:next();
	end
end

local function main(...)
	init_head(); 				-- init the mg_rpc list head
	local mgr = mg.mgr.new()
	mgr = mg.mgr.init(mgr);
	mg.log.log_set(MG_LL_DEBUG);

	mg.handle_sig(SIGINT, "sig_handler");	-- SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	-- SIGTERM

	mg.timer.add(mgr, 5000, MG_TIMER_REPEAT, "timer", mgr); -- add option to specify flags

	mg.rpc.add(rpc_head, "rpc_sum");
	mg.rpc.add(rpc_head, "rpc_mul");
	--mg.rpc.add(rpc_head, "mg.rpc.list", rpc_head);

	local con = mg.http.listen(mgr, s_listen_on, "CALLBACK");

	while(run) do
		mg.mgr.poll(mgr, 1000);
	end
	mg.mgr.free(mgr);
	mg.rpc.del(rpc_head);
end

main(...);