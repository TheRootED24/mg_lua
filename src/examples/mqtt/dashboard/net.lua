local mg = require "mg_lua";

--/* Globals Vars */--
local g_firmware_version = MQTT_FIRM_VER or "1.0.0";
local g_mqtt_server_url = MQTT_SERVER_URL or "mqtt://broker.hivemq.com:1883";
local g_root_topic = MQTT_ROOT_TOPIC or "mg_mqtt_dashboard";
local g_device_id = MQTT_DEV_ID or 0;

--/* Static Vars */--
local s_device_state = nil;
local s_qos = 1;		-- MQTT QoS
local s_conn = nil;		-- MQTT Client connection
local s_rpc = nil;		-- List of registered RPC methods

--/* Http Server */--
local s_listen_on = "http://localhost:8001";
local s_web_root = "examples/mqtt/dashboard/dashboard/";

--/* RPC List head */-- 
local rpc_head = mg.rpc.init_head();

-- DevState class
DevState = {}
DevState.__index = DevState;

function DevState.new(led_status, fw_version)
	local state = setmetatable({}, DevState);
	state.led = led_status or false;
	state.version = fw_version or nil;

	return state;
end

function rpc_state_set(req)
	local r = mg.rpc.req.new(req);
	local status = mg.json.get_bool(mg.str.ptr(r.frame), "$.params.led_status");
	s_device_state.led = tostring(status);

	mg.rpc.ok(req, "%s", "true");
end

function rpc_ota_upload(req)
	local r = mg.rpc.req.new(req);
	local ofs = mg.json.get_long(r.frame, "$.params.offset");
	local tot = mg.json.get_long(r.frame, "$.params.total");
	local len = 0;
	local buf = mg.json.get_b64(r.frame, "$.params.chunk", len);

	if(buf == nil) then
		mg.rpc.err(r, 1, "Error processing the binary chunk.");
	else
		if(ofs < 0 or tot < 0) then
			mg.rpc.err(r, 1, "offset and total not set");
		elseif(ofs == 0 and mg.ota_begin(tot) == false) then
			mg.rpc.err(r, 1, string.format("mg_ota_begin(%d) failed\n", tot));
		elseif(len > 0 and mg.ota_write(buf, len) == false) then
			mg.rpc.err(r, 1, string.format("mg_ota_write(%d) @%d failed\n", len, ofs));
			mg.ota_end();
		elseif(len == 0 and mg.ota_end() == false) then
			mg.rpc.err(r, 1, string.format("mg_ota_end() failed %d\n", tot));
		else
			mg.rpc.ok(r, "%s", "ok");
		end
	end
end

function set_device_id()
	local id = mg.fs.file.read(0, "/etc/machine-id")
	if(id and MQTT_DEV_ID == 0) then
		g_device_id = id:sub(1,14);
	end

	return g_device_id;
end

function subscribe(conn)
	local rx_topic = string.format("%s/%s/rx", g_root_topic, g_device_id);

	local sub_opts = mg.mqtt.opts.new();
	sub_opts.topic = rx_topic;
	sub_opts.qos = s_qos;

	mg.mqtt.sub(mg.connection.ptr(conn), sub_opts);

	mg.log.info(string.format("%d SUBSCRIBED to %s", mg.connection.ptr(conn):id(), rx_topic));
end

function publish_status(conn)
	local topic = string.format("%s/%s/status", g_root_topic, g_device_id);

	local message = string.format("{%q:%q,%q:{%q:%q,%q:%s,%q:%q}}",
									"method", "status.notify",
									"params",
									"status", "online",
									"led_status", tostring(s_device_state.led),
									"firmware_version", s_device_state.version
	);

	local pub_opts = mg.mqtt.opts.new();
	pub_opts.topic = topic;
	pub_opts.message = message;
	pub_opts.qos = s_qos;
	pub_opts.retain = true;

	mg.mqtt.pub(mg.connection.ptr(conn), pub_opts);
end

function publish_response(conn, iobuf)
	local topic = string.format("%s/%s/tx", g_root_topic, g_device_id);

	local pub_opts = mg.mqtt.opts.new();
	pub_opts.topic = topic;
	pub_opts.message = iobuf;
	pub_opts.qos = s_qos;

	mg.mqtt.pub(mg.connection.ptr(conn), pub_opts);
end

function timer_reconnect(args)
	local mgr = mg.mgr.new(args);
	if(s_conn == nil) then
		local topic = string.format("%s/%s/status", g_root_topic, g_device_id);
		local message = string.format("{%q:%q,%q:{%q:%q}}", "method", "status.notify", "params","status", "offline");
		
		local opts = mg.mqtt.opts.new();
		opts.clean = true;
		opts.qos = s_qos;
		opts.topic = topic;
		opts.version = 4;
		opts.keepalive = MQTT_KEEPALIVE_SEC;
		opts.retain = true;
		opts.message = message;

		s_conn = mg.mqtt.connect(mgr, g_mqtt_server_url, opts, "fn", mgr);
	end
end

function timer_ping(args)
	mg.mqtt.send_header(mg.connection.ptr(s_conn), MQTT_CMD_PINGREQ, 0, 0);
end

function timer_sntp(args)
	local hourly_timer = 0;
	local t1, t2 = mg.time.now(), mg.time.millis();

	if(t1 < (t2 + 3600) or mg.timer.expired(hourly_timer, 3600000, t2)) then
		mg.sntp.connect(mg.mgr.new(args), "udp://time.google.com:123");
	end
end

function fn(c, ev, ev_data)
	if(ev == MG_EV_OPEN) then
		mg.log.info(string.format("%d CREATED", mg.connection.ptr(c):id()));
	elseif(ev == MG_EV_CONNECT) then
		mg.log.info(string.format("Device %s is connected", g_device_id));
	elseif(ev == MG_EV_ERROR) then
		mg.log.error(string.format("%d ERROR %s", mg.connection.ptr(c):id(), tostring(ev_data)));
	elseif(ev == MG_EV_MQTT_OPEN) then
		mg.log.info(string.format("%d CONNECTED to %s", mg.connection.ptr(c):id(), g_mqtt_server_url));
		subscribe(mg.connection.ptr(c));
		publish_status(mg.connection.ptr(c));
	elseif(ev == MG_EV_MQTT_MSG) then
		local mm = mg.mqtt.message.new(ev_data);
		local data, topic = mm.data, mm.topic;
		local io = mg.iobuf.new()
		local req = mg.rpc.req.ptr(rpc_head, io, data);

		mg.rpc.process(req);

		mg.log.info(string.format("%d RECEIVED %s <- %s", mg.connection.ptr(c):id(), data, topic));

		if(io.buf and io.len > 0) then
			publish_response(mg.connection.ptr(c), io.buf);
	 		publish_status(mg.connection.ptr(c));
		end

		mg.iobuf.free(io);

	elseif(ev == MG_EV_CLOSE) then
		mg.log.info(string.format("%d CLOSED", mg.connection.ptr(c):id()));
		s_conn = nil;
	elseif(ev == MG_EV_HTTP_MSG) then
		local opts = mg.http.serve_opts.ptr(s_web_root);
		mg.http.serve_dir(mg.connection.ptr(c), ev_data, opts);
	end
end

function web_init(g_mgr)
	local mgr = mg.mgr.new(g_mgr);
	local ping_interval_ms = MQTT_KEEPALIVE_SEC * 1000 - 500;
	local flags = MG_TIMER_REPEAT;
	g_device_id = set_device_id();

	-- Initialize device state
	s_device_state = DevState.new(false, g_firmware_version);

	-- Configure JSON-RPC functions we're going to handle
	mg.rpc.add(rpc_head, "rpc_state_set");
	mg.rpc.add(rpc_head, "rpc_ota_upload");

	-- Start a http listner to serve the dashboard
	mg.http.listen(mgr, s_listen_on, "fn");

	-- add the timers
	mg.timer.add(mgr, 3000, flags, MG_TIMER_RUN_NOW, "timer_reconnect", mgr);
	mg.timer.add(mgr, ping_interval_ms, flags, "timer_ping", mgr);
	mg.timer.add(mgr, 2000, flags, "timer_sntp", mgr);
end

function web_free()
	mg.rpc.del(rpc_head); --free rpc list
end