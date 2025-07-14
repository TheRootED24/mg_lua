local mg = require "mg_lua";

local run = true;
local SIGINT = 2;
local SIGTERM = 15;
local fnum = 1;

local files = {
	"examples/http-client/video-stream/images/1.jpg",
	"examples/http-client/video-stream/images/2.jpg",
	"examples/http-client/video-stream/images/3.jpg",
	"examples/http-client/video-stream/images/4.jpg",
	"examples/http-client/video-stream/images/5.jpg",
	"examples/http-client/video-stream/images/6.jpg"
};

local function next_file()
	if(fnum >= #files ) then
		fnum = 1;
	else
		fnum = fnum + 1;
	end
end

local hdr = "HTTP/1.0 200 OK\r\nCache-Control: no-cache\r\nPragma: no-cache\r\nExpires: Thu, 01 Dec 1994 16:00:00 GMT\r\nContent-Type: multipart/x-mixed-replace; boundary=--foo\r\n\r\n";

server_callback = function (c, ev, ev_data)
	if(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		if(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/video1"))) then
			local conn = mg.connection.ptr(c);
			conn:data("S", 1);
			mg.printf(conn, string.format("%s", hdr));
		else
			local conn = mg.connection.new(c);
			local opts = mg.http.serve_opts.ptr("examples/http-client/video-stream/web_root");
			mg.http.serve_dir(conn, hm, opts);
		end
	end
end

local function broadcast_mjpeg_frame(args)
	local mgr = mg.mgr.new(args);
	local conn = mg.connection.ptr(mgr:conns());
	local data = mg.fs.file.read(0, files[fnum])

	while(conn) do
		if(data and conn:data() == "S") then
			local payload = string.format("--foo\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", data:len());
			mg.printf(conn, payload);
			mg.send(conn, data, data:len());
			mg.send(conn, "\r\n", 2);
		end
		conn = conn:next();
	end
	next_file();
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

function timer_callback(args)
	broadcast_mjpeg_frame(args);
end

local function main(arg)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);

	local c = mg.http.listen(mgr, "http://localhost:8000", "server_callback");
	mg.timer.add(mgr, 200, MG_TIMER_REPEAT, "timer_callback", mgr);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	while(run) do
		mg.mgr.poll(mgr, 50);
	end
	mg.mgr.free(mgr);
	mg.free_lua_states();
end

main(arg)