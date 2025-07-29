
local mg = require "mg_lua"
require "mg_types"

local s_web_root = "examples/http-server/";

local function proto(t)
	if(t:is_udp() == 1) then
		return "UDP"
	else
		return "TCP"
	end
end

local function ptype(t)
	if(t:is_listening() == 1) then
		return("LISTENING")
	elseif(t:is_accepted() == 1) then
		return "ACCEPTED"
	else
		return("CONNECTED")
	end
end

local s_tls_ca = [[
"-----BEGIN CERTIFICATE-----\n",
"MIIBFTCBvAIJAMNTFtpfcq8NMAoGCCqGSM49BAMCMBMxETAPBgNVBAMMCE1vbmdv\n",
"b3NlMB4XDTI0MDUwNzE0MzczNloXDTM0MDUwNTE0MzczNlowEzERMA8GA1UEAwwI\n",
"TW9uZ29vc2UwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASuP+86T/rOWnGpEVhl\n",
"epsRMAoGCCqGSM49BAMCA0gAMEUCIQCwb2CxuAKm51s81S6BIoy1IcandXSohnqs\n",
"us64BAA7QgIgGGtUrpkgFSS0oPBlCUG6YPHFVw42vTfpTC0ySwAS0M4=\n",
"-----END CERTIFICATE-----\n"]]


local s_tls_cert = [[
"-----BEGIN CERTIFICATE-----\n",
"MIIBMTCB2aADAgECAgkAluqkgeuV/zUwCgYIKoZIzj0EAwIwEzERMA8GA1UEAwwI\n",
"TW9uZ29vc2UwHhcNMjQwNTA3MTQzNzM2WhcNMzQwNTA1MTQzNzM2WjARMQ8wDQYD\n",
"VQQDDAZzZXJ2ZXIwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASo3oEiG+BuTt5y\n",
"ZRyfwNr0C+SP+4M0RG2pYkb2v+ivbpfi72NHkmXiF/kbHXtgmSrn/PeTqiA8M+mg\n",
"BhYjDX+zoxgwFjAUBgNVHREEDTALgglsb2NhbGhvc3QwCgYIKoZIzj0EAwIDRwAw\n",
"RAIgTXW9MITQSwzqbNTxUUdt9DcB+8pPUTbWZpiXcA26GMYCIBiYw+DSFMLHmkHF\n",
"+5U3NXW3gVCLN9ntD5DAx8LTG8sB\n",
"-----END CERTIFICATE-----\n"]]


local s_tls_key =[[
"-----BEGIN EC PRIVATE KEY-----",
"MHcCAQEEIAVdo8UAScxG7jiuNY2UZESNX/KPH8qJ0u0gOMMsAzYWoAoGCCqGSM49\n",
"AwEHoUQDQgAEqN6BIhvgbk7ecmUcn8Da9Avkj/uDNERtqWJG9r/or26X4u9jR5Jl\n",
"4hf5Gx17YJkq5/z3k6ogPDPpoAYWIw1/sw==\n",
"-----END EC PRIVATE KEY-----\n"]]


CALLBACK = function(c, ev, ev_data)

	if(ev == MG_EV_ACCEPT and c:is_tls() == 1 ) then
		local opts = mg.tls.opts.new();
		opts:ca(s_tls_ca);
		opts:cert(s_tls_cert);
		opts:key(s_tls_key);
		mg.tls.init(c, opts);
	end

	if (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);

		if(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/api/stats"))) then
			local conn = mg.connection.new(c);

			mg.printf(conn, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg.http.printf_chunk(conn, "ID\tPROTO\t\tTYPE\t\tLOCAL\t\tREMOTE\n");
			local payload;
			local t = conn:mgr()
			while(t) do
				payload = string.format("%u\t%s\t\t%s\t%s\t\t%s\n", t:id(), proto(t), ptype(t), t:loc_ip(), t:rem_ip())
				mg.http.printf_chunk(conn, payload)
				t = t:next();
			end
			mg.http.printf_chunk(conn, "");  --Don't forget the last empty chunk

		elseif(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/api/f2/*"))) then
			mg.http.reply(c, 200, "", string.format("{%q: %s}\n", "result", hm:uri()));
		else
			local conn = mg.connection.new(c);
			local opts = mg.http.serve_opts.new(s_web_root);
			mg.http.serve_dir(c, hm, opts);
		end
	end
end

local function main(...)
	local mgr = mg.mgr.new() 	-- Event manager
	mgr = mg.mgr.init(mgr)		-- Initialize event manager

	-- Setup listener
	local con = mg.http.listen(mgr, "http://0.0.0.0:8000", "CALLBACK")
	local con2 = mg.http.listen(mgr, "https://0.0.0.0:8443", "CALLBACK")

	-- Start Event loop (~1 min)
	for i = 1, 60 do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr)
	return
end

main(...)