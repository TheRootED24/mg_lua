-- User class
User = {}
User.__index = User;

function User.new(name, pass, token)
	local user = setmetatable({}, User);
	user.name = name or nil;
	user.pass = pass or nil;
	user.access_token = token or nil;

	return user;
end
-- user class

--[[
--require "ubus"
function User:login(u)
	local conn = ubus.connect()
	if not conn then
		error("Failed to connect to ubus")
	end
	if(u.name and u.pass) then
		local req = conn:call("session", "login", {{ username = u.name, password = u.pass, timeout = u.timeout }})
	
		if(req and req.ubus_rpc_session) then
			u.access_token = req.ubus_rpc_session;
			u.timeout = req.timeout;
			u.expires = req.expires;
		end
	elseif(u.name and u.access_token) then
		local req = conn:call("session", "list", {});
		
	end
end
-- End User]]--