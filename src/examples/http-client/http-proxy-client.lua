require "mg_types"
local mg = require "mg_lua"

local run = true;
local SIGINT = 2;
local SIGTERM = 15;

CALLBACK = function(c, ev, ev_data)
	local conn = mg.connection.new(c);
	local url = conn:fn_data();
	local connected = false;

	if (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		print(string.format("%s", hm:message()));
		return 0;
	elseif(ev == MG_EV_CONNECT) then
		local host = mg.str.new(mg.url.host(url));
		local conn = mg.connection.new(c);

		if(mg.url:is_ssl(url)) then
			local opts = mg.tls.opts.new();
			opts:ca("/cerst/ca.perm");
			opts:name(host:buf());
			mg.tls.init(conn, opts);
		end

		conn:is_hexdumpinmg(0);
		mg.printf(conn, "%s", string.format("CONNECT %s:%u HTTP/1.1\r\nHost: %s:%u\r\n\r\n",
							host:buf(), mg.url.port(url), host:buf(), mg.url.port(url)));

	elseif (not connected and ev == MG_EV_READ) then
		local hm = mg.http.message.new();
		local io = conn:recv()
		local n = mg.http.parse(io:buf(), hm);

		if(n > 0) then
			local conn = mg.connection.new(c);
			local host = mg.url.host(url);
			connected = true;
			mg.log.debug(string.format("Connected to proxy, status: %s", hm:uri()));

			mg.iobuf.del(io, 0, n);
			mg.printf(conn, string.format("GET %s HTTP/1.0\r\nHost: %.*s\r\n\r\n", mg.url.uri(url), host));
		end

	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(arg)
	print(#arg);
	if not (#arg == 2) then
		print(string.format("Usage: %s PROXY_URL URL\n", arg[0] or "??"));
		return 1;
	end

	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);
	local c = mg.http.connect(mgr, arg[1], arg[2]);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	while(run) do
		mg.mgr.poll(mgr,1000);
	end
	mg.mgr.free(mgr);

end

main(arg)