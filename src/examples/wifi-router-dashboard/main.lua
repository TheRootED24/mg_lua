local mg = require "mg_lua";
local net = require "examples.wifi-router-dashboard.net"

local run = true;
local SIGINT = 2;
local SIGTERM = 15;
local SIGPIPE = 13

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
		sig_num = sig;
	end
end

local function main(arg)
	local mgr = mg.mgr.new();
	
	-- signals	
	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	mg.log.log_set(MG_LL_DEBUG);
	mgr = mg.mgr.init(mgr);

	web_init(mgr);

	while (run) do
		mg.mgr.poll(mgr, 500);
	end
	
	mg.mgr.free(mgr);
	mg.log.info(string.format("Exiting on Sinal %d\n", sig_num));
	mg.free_lua_states();
	return 0;

end

main(...)