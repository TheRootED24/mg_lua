local mg = require "mg_lua"

require "examples.wifi-router-dashboard.certs"
require "examples.wifi-router-dashboard.config"
require "examples.wifi-router-dashboard.device"
require "examples.wifi-router-dashboard.event"
require "examples.wifi-router-dashboard.user"

-- define maximums
MAX_DEVICE_NAME = 40
MAX_EVENTS_NO = 400
MAX_EVENT_TEXT_SIZE = 10
EVENTS_PER_PAGE = 20

local s_devices = {

	{
		dev_name = "esspressif",
		mac = "00:11:22:33:44:51",
		ip_addr = "192.168.1.1/24",
		speed = 1000,
		connected_to = "Ethernet",
		lease_time_left = 1000,
		last_seen = "13h20m ago"
	},

	{
		dev_name = "windows11",
		mac = "00:11:22:33:44:52",
		ip_addr = "192.168.1.2/24",
		speed = 200,
		connected_to = "Wifi 2.4 GHz",
		lease_time_left = 4140,
		last_seen = "23sec ago"
	},

	{
		dev_name = "iRobot-2",
		mac = "00:11:22:33:44:53",
		ip_addr = "192.168.1.3/24",
		speed = 600,
		connected_to = "Wifi 5GHz",
		lease_time_left = 1141,
		last_seen = "20m ago"
	},
};

local s_events = {
    {type = 0, prio = 0, text = "here goes event 1"},
    {type = 1, prio = 2, text = "event 2..."},
    {type = 2, prio = 1, text = "another event"},
    {type = 1, prio = 1, text = "something happened!"},
    {type = 2, prio = 0, text = "once more..."},
    {type = 2, prio = 0, text = "more again..."},
    {type = 1, prio = 1, text = "oops. it happened again"},
};

local s_boot_timestamp = 0;  -- Updated by SNTP

-- uci config 
local uci_conf = {
	conf = "mg_conf",
	cpath = "examples/wifi-router-dashboard/etc/config",
	sec = "dhcp"
};
local dhcp = Config.new(uci_conf);

local s_web_root = "examples/wifi-router-dashboard/web_root/";
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
	math.randomseed((no + 1));
	ev.type = s_events[no].type; --math.random(4) -1;
	ev.prio = s_events[no].prio; --math.random(3) -1;
	ev.timestamp = (os.time())
			- 	86400 * 1000 				-- one day back
				+	no * 300 * 1000 		-- 5min between alerts 
					+	1000 * math.random(300) -- randomize event time
						/	1000
	;
	ev.text = s_events[no].text;

	return ev;
end

function print_events()
	local len, sep, json = 0, "", "";
	for no = 1, #s_events do
		local ev = Event.new();
		ev = ui_event_next(no, ev);
		if(ev) then
			local new_json = ev:json();
			json = json .. sep .. new_json;
			sep = ",";
			len = len + json:len();
		end
	end

	local resp = string.format("[%s]", json);

	return len, resp;
end

function handel_events_get(conn)
	local len, resp = print_events();
	
	if(len >= 0) then
		mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
	end
end

function print_devices()
	local json, sep, len = "", "", 0;

	for no = 1, #s_devices do
		local dev = Device.new(s_devices[no]);

		if(dev.name) then
			local new_json = dev:json();
			json = json .. sep .. new_json;
			sep = ",";
			len = len + json:len();
		end

	end

	local resp = string.format("[%s]", json);

	return len, resp;
end

function handle_devices_get(conn)
	local len, resp = print_devices();

	if(conn and resp) then
		mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
	end
end

function handle_dhcp_set(conn, hm)
	local body = mg.http.message.ptr(hm):body();
	local ok, message = "true", "Success";
	local enabled = mg.json.get_bool(mg.str.ptr(body), "$.enabled");
	local address_begin = mg.json.get_long(mg.str.ptr(body), "$.address_begin");
	local address_end = mg.json.get_long(mg.str.ptr(body), "$.address_end");
	local lease_time_sec = mg.json.get_long(mg.str.ptr(body), "$.lease_time_sec");

	if(type(enabled) == "boolean") then
		dhcp:set("enabled", tostring(enabled));
	else
		ok = "false";
		message = "Failed";
	end
	if(type(address_begin) == "number") then
		dhcp:set("address_begin", address_begin);
	else
		ok = "false";
		message = "Failed";
	end
	if(type(address_end) == "number") then
		dhcp:set("address_end", address_end);
	else
		ok = "false";
		message = "Failed";
	end
	if(type(lease_time_sec) == "number") then
		dhcp:set("lease_time_sec", lease_time_sec);
	else
		ok = "false";
		message = "Failed";
	end

	local resp = string.format("{%q:%s,%q:%q}", "status", ok, "message", message);

	mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
end

function handle_dhcp_get(conn)
	local payload = string.format("{%q:%s,%q:%d,%q:%d,%q:%d}",
					"enabled",tostring(dhcp:get("enabled")),
					"address_begin",dhcp:get("address_begin"),
					"address_end",dhcp:get("address_end"),
					"lease_time_sec", dhcp:get("lease_time_sec")
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

	return resp
end

function handle_stats_get(conn)
	local us = { 124, 123, 7, 7, 8, 8, 125, 125, 124, 125, 134, 145 };
	local ds = { 430, 310, 56, 57, 68, 450, 470, 410, 420, 480, 490, 455}
	local upload_speed, download_speed = print_int_arr(us), print_int_arr(ds);
	
	local resp = string.format("{%q:%d,%q:%d,%q:%d,%q:%s,%q:%s}",
									"connected_devices", 7,
									"uploaded", 2187,
									"downloaded", 17934,
									"upload_speed", upload_speed,
									"download_speed",download_speed
	);

	if(conn and resp) then
		mg.http.reply(mg.connection.ptr(conn), 200, json_header, resp);
	end
end

function timer_sntp_fn(param)
	local mgr = mg.mgr.new(param);
	mg.sntp.connect(mgr, "udp://time.google.com:123");
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

fn = function(c, ev, ev_data)
	if (ev == MG_EV_ACCEPT) then
		if(mg.connection.ptr(c):is_tls() == 1) then
			local opts = mg.tls.opts.new();
			opts.cert = s_tls_cert;
			opts.key = s_tls_key;
			mg.tls.init(c, opts);
		end

	elseif(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.ptr(ev_data);
		local u = authenticate(hm);

		if(u == nil and mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/#"))) then
			mg.http.reply(mg.connection.ptr(c), 403, "", "Not Authorized\n");
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/login"))) then
			handle_login(mg.connection.ptr(c), u);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/logout"))) then
			handle_logout(mg.connection.ptr(c));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/debug"))) then
			handle_debug(mg.connection.ptr(c), hm);
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/stats/get"))) then
			handle_stats_get(mg.connection.ptr(c));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/events/get"))) then
			handel_events_get(mg.connection.ptr(c));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/devices/get"))) then
			handle_devices_get(mg.connection.ptr(c));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/dhcp/get"))) then
			handle_dhcp_get(mg.connection.ptr(c));
		elseif(mg.string.match(mg.str.ptr(hm:uri()), mg.str.ptr("/api/dhcp/set"))) then
			handle_dhcp_set(mg.connection.ptr(c), hm);
		else
			-- Serve static files
			local opts = mg.http.serve_opts.ptr(s_web_root);
			mg.http.serve_dir(mg.connection.ptr(c), hm, opts);
			
			
		end
		--/* uncomment for debugging use */--
		--local con = mg.connection.ptr(c);
		--local buf = con:send("buf");

		--mg.log.debug(string.format("%d %s %s -> %s", con:id(), mg.http.message.ptr(hm):method(), mg.http.message.ptr(hm):uri(), tostring(buf):gsub("%s", " ")));
	end
end

function web_init(mgr)
	mg.http.listen(mgr, HTTP_URL, "fn");
	mg.http.listen(mgr, HTTPS_URL, "fn");
	mg.timer.add(mgr, 3600 * 1000, MG_TIMER_RUN_NOW ,MG_TIMER_REPEAT, "timer_sntp_fn", mgr);
end
