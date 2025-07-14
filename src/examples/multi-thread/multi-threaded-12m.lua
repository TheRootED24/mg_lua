local mg = require "mg_lua"

local threads = require "threads"
local thread = threads.thread;

local run = true
local SIGINT = 2;
local SIGTERM = 15;

local WORKING = 0
local SUSPENDED = 1
local COMPLETE = 2
local ERROR = 3
local OK = 4

local states = { "WORKING", "SUSPENEDED", "COMPLETE", "ERROR", "OK" }

function sleep (a)
	local sec = tonumber(os.clock() + a)
	while (os.clock() < sec) do
	end
	return 0;
end

function thread_function(nt, nc)
	local to = thread.new(nt);
	local conn = mg.connection.new(nc);

	local id = conn.id;
	local mgr = mg.mgr.new(conn:mgr());
	print("Thread Started")

	thread.sleep(3,0); -- simulate long procees time

	mg.wakeup(mgr, id, "hi", 3);
	print("ok")
	--return OK
end

CALLBACK = function(c, ev, ev_data)
	mg.connection.ptr(c):is_hexdumping(0);

	if (ev == MG_EV_OPEN and mg.connection.ptr(c):is_listening() == 1) then
		local t = thread.new();
		thread.create(t, "thread_function", c);
		print("here")
	elseif (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		if(mg.string.match(mg.str.new(hm.uri), mg.str.new("/websocket"))) then
			local conn = mg.connection.ptr(c);
			conn:data("WS", 2);
			print("here 2")
			mg.ws.upgrade(conn, hm);
		else
			local opts = mg.http.serve_opts.new(s_web_root);
			local conn = mg.connection.ptr(c);
			mg.http.serve_dir(conn, hm, opts);
		end

	elseif (ev == MG_EV_WS_MSG) then
		print("here 3")
		local wm = mg.ws.message.new(ev_data)
		local con = mg.connection.ptr(c);
		mg.ws.send(con, wm.data, wm.len, WEBSOCKET_OP_TEXT);
		--mg.iobuf.del(conn:recv(), 0, conn:recv("len)"));

	elseif(ev == MG_EV_WAKEUP) then
		print("here")
		local conn = mg.connection.new(c);
		local t = conn:mgr_conns()
		print("shit !!!!")
		--/* load all marked "WS" connection into conns table */--
		local conns = {}
			while(t) do
				print("here 4")
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
					mg.ws.send(v, wm.data, wm.len, WEBSOCKET_OP_TEXT);
			end
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(arg)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr)
	mg.log.log_set(MG_LL_DEBUG);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	mg.http.listen(mgr, "http://localhost:8000", "CALLBACK");
	mg.wakeup_init(mgr);

	while(run) do
		mg.mgr.poll(mgr, 100);
	end
	mg.mgr.free(mgr);
end

main(arg)