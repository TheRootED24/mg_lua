local mg = require "mg_lua"

require "examples.dashboard.certs"
require "examples.dashboard.config"
require "examples.dashboard.event"
require "examples.dashboard.user"

-- define maximums
MAX_DEVICE_NAME = 40
MAX_EVENTS_NO = 400
MAX_EVENT_TEXT_SIZE = 10
EVENTS_PER_PAGE = 20

-- uci config 
local uci_conf = {
	conf = "mg_conf",
	cpath = "examples/dashboard/etc/config",
	sec = "settings"
};
local settings = Config.new(uci_conf);

local s_web_root = "examples/dashboard/web_root/";
local HTTP_URL = string.format("http://%s:8000", arg[1] or "localhost");
local HTTPS_URL = string.format("https://%s:8443", arg[1] or "localhost");

local json_header = "Content-Type: application/json\r\nCache-Control: no-cache\r\n"

--fetch user pass from hm
function authenticate(hm)
	local users = {
		{name = "admin", pass = "admin", access_token = "admin_token"},
		{name = "user1", pass = "user1", access_token = "user1_token"},
		{name = "user2", pass = "user2", access_token = "user2_token"}
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

	return nil;
end

function ui_event_next(no, ev)
	if(no < 0 or no >= MAX_EVENTS_NO) then return 0 end;
	math.randomseed((no + 1));
	ev.type = math.random(4) -1;
	ev.prio = math.random(3) -1;
	ev.timestamp = (os.time())
			- 	86400 * 1000 				-- one day back
				+	no * 300 * 1000 		-- 5min between alerts 
					+	1000 * math.random(300) -- randomize event time
						/	1000
	;
	ev.text = string.format("event#%d", (no + 1)):sub(1, MAX_EVENT_TEXT_SIZE);

	return (no + 1), ev;
end

function print_events(pageno)
	local ev = Event.new();
	local no = (pageno - 1) * EVENTS_PER_PAGE;
	local last = no + EVENTS_PER_PAGE;
	local len, sep, json = 0, "", "";

	no, ev = ui_event_next(no, ev);

	while( no ~= 0 and no <= last) do
		if(ev) then
			local new_json = ev:json();
			json = json .. sep .. new_json;
			sep = ",";
			len = len + json:len();
			no, ev = ui_event_next(no, ev);
		end

	end

	local resp = string.format("{%q:[%s], %q:%d}\n", "arr", json, "totalCount", MAX_EVENTS_NO);

	return len, resp;
end

function handel_events_get(conn, hm)
	local body = mg.http.message.ptr(hm):body();
	local pageno = mg.json.get_long(mg.str.ptr(body), "$.page");
	local len, resp = print_events(pageno or 1);

	if(len >= 0) then
		mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
	end
end

function handle_settings_set(conn, hm)
	local body = mg.http.message.ptr(hm):body();
	local ok, message = "true", "Success";
	local log_enabled = mg.json.get_bool(mg.str.ptr(body), "$.log_enabled");
	local log_level = mg.json.get_long(mg.str.ptr(body), "$.log_level");
	local brightness = mg.json.get_long(mg.str.ptr(body), "$.brightness");
	local device_name = mg.json.get_str(mg.str.ptr(body), "$.device_name");

	if(type(log_enabled) == "boolean") then
		settings:set("log_enabled", tostring(log_enabled));
	else
		ok = "false";
		message = "Failed";
	end
	if(type(brightness) == "number") then
		settings:set("brightness", brightness);
	else
		ok = "false";
		message = "Failed";
	end
	if(type(log_level) == "number") then
		settings:set("log_level", log_level);
	else
		ok = "false";
		message = "Failed";
	end
	if(type(device_name) == "string") then
		settings:set("device_name", device_name);
	else
		ok = "false";
		message = "Failed";
	end

	local resp = string.format("{%q:%s,%q:%q}", "status", ok, "message", message);

	mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
end

function handle_settings_get(conn)
	local payload = string.format("{%q:%s,%q:%d,%q:%d,%q:%q}\n",
					"log_enabled",tostring(settings:get("log_enabled")),
					"log_level",settings:get("log_level"),
					"brightness",settings:get("brightness"),
					"device_name", settings:get("device_name")
				);
	if(conn and payload) then
		mg.http.reply(mg.connection.ptr(conn), 200, json_header, payload);
	end
end

function handle_debug(conn, hm)
	local level = mg.json.get_long(mg.str.ptr(hm:body()), "$.level") or MG_LL_DEBUG;
	mg.log.set(level);

	local resp = string.format("Debug level set to %d\n", level);

	if(conn and resp) then
		mg.http.reply(mg.connection.ptr(conn), 200, "", resp);
	end
end

function print_int_arr(points)
	local len = #points
	local sep, pts = "", "";
	local resp = "["

	for k,v in pairs(points) do
		resp = resp.. sep .. v;
		sep = ","
	end
	resp = resp .. "]";

	return len, resp
end

function handle_stats_get(conn)
	local points = { 21, 22, 22, 19, 18, 20, 23, 23, 22, 22, 22, 23, 22 };
	local len, pts = print_int_arr(points);

	local resp = string.format("{%q:%d,%q:%d,%q:%s}\n",
					"temperature", 21,
						"humidity", 67,
							"points", pts
	);

	if(conn and resp) then
		mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
	end
end

function handle_firmware_upload(conn, hm)
	local name, offset, total = "\0", "\0", "\0";
	local name_size, offset_size, total_size = 64, 20, 20;
	local ofs, tot = -1, -1;
	
	local data = mg.str.ptr(hm:body());
	local query = mg.str.ptr(hm:query());

	mg.http.get_var(query, "name", name, name_size);
	mg.http.get_var(query, "offset", offset, offset_size);
	mg.http.get_var(query, "total", total, total_size);

	mg.log.info("Files %s, offset %s, len %d", name, offset, data:len())
end

function timer_sntp_fn(param)
	local mgr = mg.mgr.new(param);
	mg.sntp.connect(mgr, "udp://time.google.com:123");
end

fn = function(c, ev, ev_data)
	if (ev == MG_EV_ACCEPT) then
		if(mg.connection.ptr(c):is_tls() == 1) then
			local opts = mg.tls.opts.new();
			--opts.ca = s_tls_ca; -- two-way
			opts.cert = s_tls_cert;
			opts.key = s_tls_key;
			mg.tls.init(c, opts);
		end

	elseif(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.ptr(ev_data);
		local u = authenticate(hm);

		if(u == nil and mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/#"))) then
			local con = mg.connection.ptr(c);
			mg.http.reply(con, 403, "", "Not Authorized\n");
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/login"))) then
			local con = mg.connection.ptr(c);
			handle_login(con, u);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/logout"))) then
			local con = mg.connection.ptr(c);
			handle_logout(con);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/debug"))) then
			local con = mg.connection.ptr(c);
			handle_debug(con, hm);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/stats/get"))) then
			local con = mg.connection.ptr(c);
			handle_stats_get(con);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/events/get"))) then
			local con = mg.connection.ptr(c);
			handel_events_get(con, hm);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/settings/get"))) then
			local con = mg.connection.ptr(c);
			handle_settings_get(con);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/settings/set"))) then
			local con = mg.connection.ptr(c);
			handle_settings_set(con, hm);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/firmware/upload"))) then
			local con = mg.connection.ptr(c);
			handle_firmware_upload(con, hm);
		else
			-- Serve static files
			local con = mg.connection.ptr(c);
			local opts = mg.http.serve_opts.ptr(s_web_root);
			mg.http.serve_dir(con, hm, opts);

			--mg.log.debug("%d %s %s -> %s", con:id(), hm:method(), hm:uri(), "ToDO");
		end
	end
end

local function get_cookie_info(con)
	local cookie_name, is_sec = nil, nil;
	local max_time = (3600 * 24);

	if(con:is_tls() == 1) then
		cookie_name = "access_token";
		is_sec = "Secure; ";
	else
		cookie_name = "access_token";
		is_sec = "";
	end

	return cookie_name, is_sec, max_time;
end

function handle_login(con, u)
	local cookie_name, is_sec, max_time = get_cookie_info(con);
	local cookie = string.format("Set-Cookie:%s=%s;Path=/;%sHttpOnly;SameSite=Lax;Max-Age=%d\r\n", cookie_name, u.access_token or "123456", is_sec, max_time);

	if(con and cookie and u.name) then
		mg.http.reply(con, 200, cookie, string.format("{%q:%q}", "user", u.name));
	end
end

function handle_logout(con)
	local cookie_name, is_sec, max_age = get_cookie_info(con);
		local cookie = string.format("Set-Cookie:%s=;Path=/;Expires=Thu, 01 Jan 1970 00:00:00 UTC;%sHttpOnly;Max-Age=%d\r\n", cookie_name, is_sec, max_age);
		mg.http.reply(con, 200, cookie, "true\n");
end

function web_init(mgr)
  	settings:set("name", "My Device");
 	mg.http.listen(mgr, HTTP_URL, "fn");
  	mg.http.listen(mgr, HTTPS_URL, "fn");
  	mg.timer.add(mgr, 3600 * 1000, MG_TIMER_RUN_NOW ,MG_TIMER_REPEAT, "timer_sntp_fn", mgr);
end
