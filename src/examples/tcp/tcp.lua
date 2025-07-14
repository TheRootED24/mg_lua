local mg = require "mg_lua";

local s_lsn = "tcp://localhost:8765";   -- Listening address
local s_conn = "tcp://localhost:8765";  -- Connect to address

local run = true;
local SIGINT = 2;
local SIGTERM = 15;

--local iter = 0;
--local client = nil;

local c_res = { i = 0, c = nil };

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

-- CLIENT event handler
function cfn(c, ev, ev_data)
	if(mg.connection.ptr(c):is_listening() == 0) then
		if(ev == MG_EV_OPEN) then
			mg.log.info("CLIENT has been initialized");
		elseif(ev == MG_EV_CONNECT) then
			mg.log.info("CLIENT connected");
			c_res.i = 1; -- do something
		elseif(ev == MG_EV_READ) then
			local conn = mg.connection.new(c);
			mg.log.info(string.format("CLIENT got data: %s", conn:recv("buf")));
			conn:recv("len", 0); -- Tell Mongoose we've consumed data
		elseif(ev == MG_EV_CLOSE) then
			mg.log.info("CLIENT disconnected")
			mg.connection.ptr(c):is_closing(1);
			c_res.c = nil;
		elseif(ev == MG_EV_ERROR) then
			mg.log.info(string.format("CLIENT error: %s", tostring(ev_data)));
		elseif(ev == MG_EV_POLL and c_res.i ~= 0) then
			c_res.i = c_res.i + 1;
			if(c_res.i  == 50) then
				local msg = "Hi, there";
				mg.send(mg.connection.ptr(c), msg, msg:len());
				mg.log.info("CLIENT sent data")
			end
			-- alternate [] syntax
			if(c_res["i"] == 100) then
				mg.connection.ptr(c):is_draining(1);
			end

		end
	end
end

function sfn(c, ev, ev_data)
	if (ev == MG_EV_OPEN and mg.connection.ptr(c):is_listening() == 1) then
		mg.log.info("SERVER is listening");
	elseif(ev == MG_EV_ACCEPT) then
		mg.log.info("SERVER accepted a connection");
		if (mg.url.is_ssl(s_lsn) == 1 ) then
			local opts = mg.tls.opts.new();
			opts.ca = s_tls_ca;
			opts.cert = s_tls_cert;
			opts.key = s_tls_key;

			mg.tls.init(c, opts);
		end
	elseif(ev == MG_EV_READ) then
		local conn = mg.connection.new(c);
		mg.log.info(string.format("SERVER got data: %s", conn:recv("buf")));
		mg.send(conn, conn:recv("buf"), conn:recv("len")); 
		conn:recv("len", 0); -- Tell Mongoose we've consumed data
	elseif(ev == MG_EV_CLOSE) then
		mg.log.info("SERVER disconnected");
	elseif(ev == MG_EV_ERROR) then
		mg.log.info(string.format("SERVER error: %s", tostring(ev_data)));
	end
end

function timer_fn(args)
	local mgr = mg.mgr.new(args);
	if(c_res.c == nil) then
		c_res.i = 0;
		c_res.c = mg.connection.ptr((mg.connect(mgr, s_conn, "cfn")));

		if(c_res.c) then
			mg.log.info(string.format("CLIENT connecting"));
		else
			mg.log.info(string.format("CLIENT failed"));
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(...)
	local mgr = mg.mgr.new()
	mgr = mg.mgr.init(mgr);

	mg.handle_sig(SIGINT, "sig_handler");	-- SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	-- SIGTERM

	mg.log.log_set(MG_LL_INFO); -- Set log level

	mg.timer.add(mgr, 15000, MG_TIMER_REPEAT, MG_TIMER_RUN_NOW, "timer_fn", mgr); -- start the timer

	local c = mg.listen(mgr, s_lsn, "sfn") -- Start the listner
	if(c == nil) then
		mg.log.info("SERVER cant' open a connection");
		return 0;
	end

	while(run) do
		mg.mgr.poll(mgr, 100);
	end

	mg.mgr.free(mgr);
	mg.free_lua_states();

	return 0;
end

main(...);