local mg = require "mg_lua"

G_callback  = function(c, ev, ev_data)
	if( ev == MG_EV_HTTP_MSG ) then
		local opts = mg.http.serve_opts.new(".") -- Serve local directory
		mg.http.serve_dir(c, ev_data, opts)
	end
end

local function main(...)
	local mgr = mg.mgr.new() 	-- Event manager
	mgr = mg.mgr.init(mgr)		-- Initialize event manager

	-- Setup listener
	mg.http.listen(mgr, "0.0.0.0:8000", "G_callback")

	-- Start Event loop (~20 sec)
	for i = 1, 20 do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr)
	mg.free_lua_states();
	return
end

main(...)