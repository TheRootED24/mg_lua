require "mg_types"
mg = require "mg_lua"

callback  = function(c, ev, ev_data)
	if( ev == MG_EV_HTTP_MSG ) then
		local opts = mg.http.serve_opts.new(".") -- Serve local directory
		mg.http.serve_dir(c, ev_data, opts)
	end
end


function main(...)
	local mgr = mg.mgr.new() 	-- Event manager
	mgr = mg.mgr.init(mgr)		-- Initialize event manager

	-- Setup listener
	local conn = mg.http.listen(mgr, "0.0.0.0:8200", "callback")

	-- Start Event loop (~1 min)
	for i = 1, 60 do
		mg.mgr.poll(mgr, 1000)
	end

	-- Cleanup
	mg.mgr.free(mgr)
	return
end

main(...)