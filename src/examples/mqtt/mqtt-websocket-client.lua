require "mg_types"
local mg = require "mg_lua"

local s_url = "ws://broker.hivemq.com:8000/mqtt";
local s_topic = "mg/test";

local SIGINT = 2;
local SIGTERM = 15;
local run = true;

MQTT_WS_CB = function (c, ev, ev_data)
	if(ev == MG_EV_ERROR) then
		mg.log.error(string.format("%s %s", c:fd(), tostring(ev_data)));
	elseif(ev == MG_EV_CONNECT) then
		local conn = mg.connection.new(c);
		if(c:is_tls() == 1) then
			local opts = mg.tls.opts.new();
			opts:ca(mg.unpack("/certs/ca.pem"));
			opts:name(mg.url:host(s_url));

			mg.tls.init(conn, opts);
		end
	elseif(ev == MG_EV_WS_OPEN) then
		mg.log.info("Connected to WS. Logging in to MQTT...");
		local conn = mg.connection.new(c);
		local opts = mg.mqtt.opts.new();

		opts:qos(1);
		opts:topic(s_topic);
		opts:message("goodbye");-- unpack the send iobuf into table "send"
		local len = conn:send("len");
		mg.mqtt.login(conn, opts);
		mg.ws.wrap(conn, conn:send("len") - len, WEBSOCKET_OP_BINARY);
		conn:is_hexdumping(1);
	elseif(ev == MG_EV_WS_MSG) then
		local conn = mg.connection.new(c);
		local version;
		if(conn:is_mqtt5() == 1) then
			version = 5;
		else
			version = 4;
		end
		local wm = mg.ws.message.new(ev_data);
		mg.log.info(string.format("GOT %d bytes WS msg", wm:len()));
		local mm = mg.mqtt.message.new();
		while(mg.mqtt.parse(mm, wm:data(), wm:len(), tonumber(version)) == MQTT_OK) do
			if(mm:cmd() == MQTT_CMD_CONNACK) then
				local conn = mg.connection.new(c);
				mg.util.call(conn, MG_EV_MQTT_OPEN, mm:ack())
				if(mm:ack() == 0) then
					local topic = s_topic;
					local data = "hello";
					local len = conn:send("len");

					mg.log.info(string.format("CONNECTED to %s", s_url));
					local sub_opts = mg.mqtt.opts.new();
					sub_opts:topic(topic);
					sub_opts:qos(1);
					mg.mqtt.sub(conn, sub_opts);
					len = mg.ws.wrap(conn, conn:send("len") - len, WEBSOCKET_OP_BINARY);
					mg.log.info(string.format("SUBSCRIBED to %s", topic));
					local pub_opts = mg.mqtt.opts.new();
					pub_opts:topic(topic);
					pub_opts:message(data);
					pub_opts:qos(1);
					pub_opts:retain(false);

					mg.mqtt.pub(conn, pub_opts);
					mg.log.info(string.format("PUBLISHED %s -> %s", data, topic));
					len = mg.ws.wrap(conn, conn:send("len") - len, WEBSOCKET_OP_BINARY);
				else
					mg.log.error(string.format("%u MQTT auth failed, code %d", conn:id(), mm:ack()));
					conn:is_draining(1);
				end
				break;
			elseif(mm:cmd() == MQTT_CMD_PUBLISH) then
				local conn = mg.connection.new(c);
				mg.log.debug(string.format("%u [%s] -> [%s]", conn:id(), mm:topic(), mm:data()));
				mg.log.info(string.format("RECEIVED %s <- %s", mm:data(), mm:topic()));
				conn:is_draining(1);
			end
			break;
		end
		wm:next(mm:dgram():len())
		--wm:data(wm:data():len() + mm:dgram_len());
		--wm:len(wm:len() + mm:dgram_len());

	elseif (ev == MG_EV_ERROR or ev == MG_EV_CLOSE) then
		run = false; -- Signal we're done
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(arg)

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	local mgr = mg.mgr.new();		-- Allocate a new mgr
	mgr = mg.mgr.init(mgr);			-- Initialise event manager
	mg.log.log_set(MG_LL_DEBUG);		-- Set log level
	local c = mg.ws.connect(mgr, s_url, "MQTT_WS_CB", "Sec-Websocket-Protocol: mqtt\r\n"); -- Create client

	while(run) do
		mg.mgr.poll(mgr, 1000);		-- Wait for echo
	end
	mg.mgr.free(mgr);		-- Deallocate resources
end

main(arg)