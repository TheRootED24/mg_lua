require "mg_types";
local mg = require "mg_lua";
local web_root = "examples/http-client/http-file-upload/web_root/"
local s_url = "http://0.0.0.0:8100";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

CALLBACK = function(c, ev, ev_data)
	if(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(ev_data);
		mg.log.info(string.format("New request to: [%s], body size: %u", hm:uri(), hm:body():len()));

		if(mg.string.match(mg.str.new(hm:uri()), mg.str.new("/upload"))) then
			local part = mg.http.part.new();
			local ofs = 0;
			print(part)
			ofs = mg.http.next_multipart(mg.str.new(hm:body()), ofs, part);
			print("here");
			while(ofs > 0) do
				mg.log.error(string.format("Chunk name: [%s] filename: [%s] length: %u bytes", part:name(), part:filename(), part:body():len()));
				ofs = mg.http.next_multipart(mg.str.new(hm:body()), ofs, part);
			end
			--s = mg.str.new(hm:body())
			--print(s:buf(), s:length())
			--print(hm.body);
			local conn = mg.connection.new(c);
			mg.http.reply(conn, 200, "", "Thank you!");
		else
			local conn = mg.connection.new(c);
			local opts = mg.http.serve_opts.new(web_root);
			mg.http.serve_dir(conn, ev_data, opts);
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(arg)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);
	mg.log.log_set(MG_LL_DEBUG);

	local conn = mg.http.listen(mgr, s_url, "CALLBACK");

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	while(run) do
		mg.mgr.poll(mgr, 50);
	end
	mg.mgr.free(mgr);
end

main(arg)