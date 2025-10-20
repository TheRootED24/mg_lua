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
	--thread.destroy(to);
	--return OK
end

s_callback = function(c, ev, ev_data)
	--print(ev)
	if(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		if(mg.string.match(mg.str.new(hm.uri), mg.str.new("/fast"))) then
			--Single-threaded code path, for performance comparison
			--The /fast URI responds immediately
			local conn = mg.connection.new(c);
			mg.http.reply(conn, 200, "Host: foo.com\r\n", "hi\r\n");
		else
			local conn = mg.connection.new(c);
			local t = thread.new();
			local ret = thread.attr.detachstate(t, true);
			ret = thread.create(t, "thread_function", conn.ctx);
		end

	elseif(ev == MG_EV_WAKEUP) then
		--local conn = mg.connection.new(c);
		local s = mg.str.to_str(ev_data);
		local conn = mg.connection.new(c);
		mg.http.reply(conn, 200, "", string.format("Result: %s\n", s));
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

	mg.http.listen(mgr, "http://localhost:8000", "s_callback");
	mg.wakeup_init(mgr);

	while(run) do
		mg.mgr.poll(mgr, 100);
	end
	mg.mgr.free(mgr);
end

main(arg)