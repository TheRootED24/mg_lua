local mg = require "mg_lua";

local s_listening_url = "http://0.0.0.0:8000";
local s_web_root = "examples/webui-login/web_root/";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

-- User Class
User = {}
User.__index = User;

function User.new(name, pass, token)
	local user = setmetatable({}, User);
	user.name = name or nil;
	user.pass = pass or nil;
	user.access_token = token or nil;

	return user;
end
-- end user

local function get_user(hm)
	local users = {
		{name = "admin", pass = "pass0", access_token = "admin_token"},
		{name = "user1", pass = "pass1", access_token = "user1_token"},
		{name = "user2", pass = "pass2", access_token = "user2_token"}
	};
	local u = User.new();
	u.name, u.pass = mg.http.creds(hm);
	if(u.name or u.pass) then
		for i=1, #users do
			if(u.name and u.pass) then
				if(u.name == users[i].name ) then
					return users[i];
				end
			elseif(not u.name and u.pass) then
				if(u.pass == users[i].access_token) then
					return users[i];
				end
			end
		end
	end

	return u;
end

CALLBACK = function(c, ev, ev_data)
	if(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.ptr(ev_data);

		local user = get_user(hm);
		if(not user and mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/#"))) then
			local con = mg.connection.ptr(c);
			mg.http.reply(con, 403, "", string.format("%s\n", "Denied"));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/data"))) then
			local con = mg.connection.ptr(c);
			mg.http.reply(con, 200, "Content-Type: application/json\r\n", string.format("{%q:%q,%q:%q}", "text", "Hello!", "data", "somedata"));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/login"))) then
			local con = mg.connection.ptr(c);
			mg.http.reply(con, 200, "Content-Type: application/json\r\n", string.format("{%q:%q,%q:%q}", "user", user.name, "token", user.access_token))
		else
			local con = mg.connection.ptr(c);
			local opts = mg.http.serve_opts.new(s_web_root);
			mg.http.serve_dir(con, ev_data, opts);
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(...)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);

	mg.handle_sig(SIGINT, "sig_handler");	-- SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	-- SIGTERM

	mg.http.listen(mgr, s_listening_url, "CALLBACK");

	while(run) do
		mg.mgr.poll(mgr, 500)
	end

	mg.mgr.free(mgr);
	mg.free_lua_states();
end

main(...)