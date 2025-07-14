local mg = require "mg_lua";

local s_listen_on = "http://localhost:8000";
local s_web_root = "examples/huge-response/web_root"

local DATA_SIZE = 10000;
local CHUNK_SIZE = 100;

local run = true
local SIGINT = 2;
local SIGTERM = 15;

local s_data = {};
local s_version = 0;

local function getparam(hm, json_path)
	local body = mg.http.message.ptr(hm):body();
	local dv = mg.json.get_num(mg.str.ptr(body), json_path);

	return dv or 0;
end

local function print_data(start)
	-- lua tables start at index 1
	if(start < 1 ) then
		start = 1;
	end

	local max = start + CHUNK_SIZE;
	-- prevent overflow
	if(max > DATA_SIZE) then
		max = DATA_SIZE;
	end

	local sep, next, data = "", "", "";
	-- format next chunk
	while(start < max) do
		next = string.format("%s%d", sep, s_data[start])
		sep = ","
		data = data .. next;
		start = start + 1;
	end

	return data;
end

function fn(c, ev, ev_data)
	if (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);

		if(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/data"))) then
			local headers = "content-type: text/json\r\n";
			local start = getparam(hm, "$.start") or 0;
			local version = getparam(hm, "$.version") or 0;

			mg.log.debug(hm.body);
			
			-- Version mismatch: s_data has changed while client fetches it
			-- Tell client to restart
			if(version > 0 and version ~= s_version) then
				mg.http.reply(mg.connection.ptr(c), 200, headers, string.format("{%q:%q, %q:%d}",
														"error","wrong version",
														"version", version
				));

			else
				-- Return data, up to CHUNK_SIZE elements
				mg.http.reply(mg.connection.ptr(c), 200, headers, string.format("{%q:%d,%q:%d,%q:[%s]}",
															"version", s_version,
															"start", start,
															"data", print_data(start)
				));
			end
		else
			-- Serve static files
			local conn = mg.connection.ptr(c);
			local opts = mg.http.serve_opts.ptr(s_web_root);
			mg.http.serve_dir(conn, hm, opts);
		end
	end
end

function timer_fn(args)
	for i = 1, DATA_SIZE do
		s_data[i] = string.format("%d", math.random(9999999));
	end
	s_version = s_version + 1;
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(...)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);
	math.randomseed(os.time());

	mg.handle_sig(SIGINT, "sig_handler");	-- SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	-- SIGTERM

	mg.timer.add(mgr, 5000, MG_TIMER_REPEAT, MG_TIMER_RUN_NOW, "timer_fn", mgr);
	mg.http.listen(mgr, s_listen_on, "fn");

	mg.log.info(string.format("Listening on %s", s_listen_on));

	while(run) do
		mg.mgr.poll(mgr, 1000);
	end

	mg.mgr.free(mgr);
	mg.free_lua_states();

	return 0;
end

main(...);