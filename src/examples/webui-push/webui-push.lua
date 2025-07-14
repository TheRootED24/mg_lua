local mg = require "mg_lua";

local RAND_MAX = 32767;

local s_listen_on = "http://localhost:8000";
local s_web_root = "examples/webui-push/web_root";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

function fn(c, ev, ev_data)
	if (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		
		if(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/watch"))) then
			local conn = mg.connection.new(c);
			conn.data = "WS";
			mg.ws.upgrade(conn, hm);
		else
			local opts = mg.http.serve_opts.ptr(s_web_root);
			mg.http.serve_dir(mg.connection.ptr(c), hm, opts);
		end
	end
end

function push(m, name, data)
	local mgr = mg.mgr.new(m);
	local conn = mg.connection.new((mgr:conns()));

	while(conn.ctx) do
		if(conn.data == "WS") then
			mg.ws.printf(conn, WEBSOCKET_OP_TEXT, "%s", string.format("{%q:%q,%q:%q}",
												"name", name,
												"data", data)
			);
		end
		conn = mg.connection.new((conn:next()));
	end
end

function timer_fn(args)
	local mgr = mg.mgr.new(args);
	local buf = string.format("[%d, %d]", os.time(), math.random(1,100) );

	push(mgr, "metrics", buf);
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(...)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);
	mg.log.log_set(MG_LL_DEBUG);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	mg.timer.add(mgr, 2000, MG_TIMER_REPEAT, "timer_fn", mgr);
	mg.http.listen(mgr, s_listen_on, "fn");

	while(run) do
		mg.mgr.poll(mgr, 500)
	end

	mg.mgr.free(mgr);
	mg.free_lua_states();
end

main(...);