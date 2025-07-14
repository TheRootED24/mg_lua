local mg = require "mg_lua"
local threads = require "threads"
local thread = threads.thread;

local run = true
local SIGINT = 2;
local SIGTERM = 15;

local ERROR = 3
local OK = 4

-- homebrew sub-second sleep function
function sleep (ss)
	local sec = tonumber(os.clock() + ss)
	while (os.clock() < sec) do end
end

function thread_function(nt, nc)
	local to = thread.new(nt);
	local con = mg.connection.new(nc);
	local id = con.id;
	local mgr = mg.mgr.new(con:mgr());
	print("Thread Started ID: " .. id)

	-- simulate long procees time
	sleep(2);

	mg.wakeup(mgr, id, "hi", 3);
	print("thread complete ID: " .. id);
	thread.destroy(to);
	return OK
end

s_callback = function(c, ev, ev_data)
	if(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		if(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/fast"))) then
			--Single-threaded code path, for performance comparison
			--The /fast URI responds immediately
			mg.http.reply(mg.connection.ptr(c), 200, "Host: foo.com\r\n", "hi\r\n");
		else
			local t = thread.new();
			local ret = thread.attr.detachstate(t, true);
			ret = thread.create(t, "thread_function", mg.connection.ptr(c));
		end

	elseif(ev == MG_EV_WAKEUP) then
		local s = mg.str.to_str(ev_data);
		mg.http.reply(mg.connection.ptr(c), 200, "", string.format("Result: %s\n", s));
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
	mg.free_lua_states();
end

main(arg)