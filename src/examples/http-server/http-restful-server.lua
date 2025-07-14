local mg = require "mg_lua"

local s_web_root = "examples/http-server/";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

local function proto_type(t)
	if(t.is_udp == 1) then
		return "UDP"
	else
		return "TCP"
	end
end

local function conn_type(t)
	if(t.is_listening == 1) then
		return("LISTENING")
	elseif(t.is_accepted == 1) then
		return("ACCEPTED")
	else
		return("CONNECTED")
	end
end

local s_tls_ca = [[
-----BEGIN CERTIFICATE-----
MIIBFTCBvAIJAMNTFtpfcq8NMAoGCCqGSM49BAMCMBMxETAPBgNVBAMMCE1vbmdv
b3NlMB4XDTI0MDUwNzE0MzczNloXDTM0MDUwNTE0MzczNlowEzERMA8GA1UEAwwI
TW9uZ29vc2UwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASuP+86T/rOWnGpEVhl
epsRMAoGCCqGSM49BAMCA0gAMEUCIQCwb2CxuAKm51s81S6BIoy1IcandXSohnqs
us64BAA7QgIgGGtUrpkgFSS0oPBlCUG6YPHFVw42vTfpTC0ySwAS0M4=
-----END CERTIFICATE-----]]


local s_tls_cert = [[
-----BEGIN CERTIFICATE-----
MIIBMTCB2aADAgECAgkAluqkgeuV/zUwCgYIKoZIzj0EAwIwEzERMA8GA1UEAwwI
TW9uZ29vc2UwHhcNMjQwNTA3MTQzNzM2WhcNMzQwNTA1MTQzNzM2WjARMQ8wDQYD
VQQDDAZzZXJ2ZXIwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASo3oEiG+BuTt5y
ZRyfwNr0C+SP+4M0RG2pYkb2v+ivbpfi72NHkmXiF/kbHXtgmSrn/PeTqiA8M+mg
BhYjDX+zoxgwFjAUBgNVHREEDTALgglsb2NhbGhvc3QwCgYIKoZIzj0EAwIDRwAw
RAIgTXW9MITQSwzqbNTxUUdt9DcB+8pPUTbWZpiXcA26GMYCIBiYw+DSFMLHmkHF
+5U3NXW3gVCLN9ntD5DAx8LTG8sB
-----END CERTIFICATE-----]]


local s_tls_key =[[
-----BEGIN EC PRIVATE KEY-----
MHcCAQEEIAVdo8UAScxG7jiuNY2UZESNX/KPH8qJ0u0gOMMsAzYWoAoGCCqGSM49
AwEHoUQDQgAEqN6BIhvgbk7ecmUcn8Da9Avkj/uDNERtqWJG9r/or26X4u9jR5Jl
4hf5Gx17YJkq5/z3k6ogPDPpoAYWIw1/sw==
-----END EC PRIVATE KEY-----]]


local function hdr_size(hdrs)
	local size = 0;
	for i = 1, 30 do
		if(hdrs[i].name ~= "") then 
			size = size + 1
		else
			break;
		end
	end
	return size;
end

CALLBACK = function(c, ev, ev_data)
	if (ev == MG_EV_ACCEPT and mg.connection.ptr(c):is_tls() == 1 ) then
		local opts = mg.tls.opts.new();
		--opts.ca = s_tls_ca;
		opts.cert = s_tls_cert;
		opts.key = s_tls_key;

		mg.tls.init(c, opts);
	end

	if (ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		if(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/stats"))) then
			--local size = hdr_size(hm.headers)
			--print("NUM HEADERS: "..size)

			--for i = 1, size do
			--	print(i,hm.headers[i].name,hm.headers[i].value)
			--end

			local con = mg.connection.new(c);
			mg.printf(con, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg.http.printf_chunk(con, "ID\tPROTO\t\tTYPE\t\tLOCAL\t\tREMOTE\n");
			local payload;
			local t = mg.connection.new((con:mgr_conns()));
			while(t.ctx) do
				payload = string.format("%u\t%s\t\t%s\t%s\t\t%s\n", t.id, proto_type(t), conn_type(t), t.loc_ip, t.rem_ip)
				mg.http.printf_chunk(con, payload)
				t = mg.connection.new((t:next()));
			end
			mg.http.printf_chunk(con, "");  --Don't forget the last empty chunk

		elseif(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/f2/*"))) then
			local uri = hm.uri
			local con = mg.connection.ptr(c);
			mg.http.reply(con, 200, "", string.format("{%q: %s}\n", "result", uri));
		else
			local con = mg.connection.ptr(c);
			local opts = mg.http.serve_opts.ptr(s_web_root);
			mg.http.serve_dir(con, hm, opts);
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(arg)
	local mgr = mg.mgr.new() 	-- Event manager
	mgr = mg.mgr.init(mgr)		-- Initialize event manager
	mg.log.log_set(MG_LL_DEBUG);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	-- Setup listener
	mg.http.listen(mgr, "http://0.0.0.0:8000", "CALLBACK")
	mg.http.listen(mgr, "https://0.0.0.0:8443", "CALLBACK")

	-- Start Event loop 
	while(run) do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr);
	mg.free_lua_states();
end

main(arg)