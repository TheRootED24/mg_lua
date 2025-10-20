require "mg_types"

local mg = require "mg_lua"

local s_url = "mqtt://broker.hivemq.com:1883";
local s_sub_topic = "mg/+/test";	-- Publish topic
local s_pub_topic = "mg/clnt/test";	-- Subscribe topic
local s_qos = 1;			--MQTT QoS
local s_con;				-- MQTT Connection

local run = true
local SIGINT = 2;
local SIGTERM = 15;

MQTT = function(c, ev, ev_data)

	if (ev == MG_EV_OPEN) then
		local conn = mg.connection.new(c)
		mg.log.info(string.format("Open Connection for id: [%u]", conn:id()));
		conn:is_hexdumping(1);

	elseif(ev == MG_EV_ERROR) then
		local conn = mg.connection.new(c)
		mg.log.error(string.format("%u ERROR %s", conn:id(), tostring(ev_data)));

	elseif(ev == MG_EV_MQTT_OPEN) then
		local conn = mg.connection.new(c)
		local subt = mg.str.new(s_sub_topic);
		local pubt = mg.str.new(s_pub_topic);
		local data = mg.str.new("hello");

		mg.log.info(string.format("%u CONNECTED to %s", conn:id(), s_url));

		local sub_opts = mg.mqtt.opts.new()
		sub_opts:topic(subt:buf());
		sub_opts:qos(s_qos);

		mg.mqtt.sub(conn, sub_opts);
		mg.log.info(string.format("%u SUBSRCIBED to %s", conn:id(), subt:buf()));

		local pub_opts = mg.mqtt.opts.new()
		pub_opts:topic(pubt:buf());
		pub_opts:message(data:buf());
		pub_opts:qos(s_qos);
		pub_opts:retain(false);

		mg.mqtt.pub(conn, pub_opts);
		mg.log.info(string.format("%u PUBLISHED %s -> %s", conn:id(), data:buf(), pubt:buf()));

	elseif (ev == MG_EV_MQTT_MSG) then
		local conn = mg.connection.new(c)
		local id = conn:id();
		local mm = mg.mqtt.message.new(ev_data);
		mg.log.info(string.format("%u RECEIVED %s <- %s", id, mm:data(), mm:topic()));

	elseif (ev == MG_EV_CLOSE) then
		local conn = mg.connection.new(c)
		mg.log.info(string.format("CONNECTION [%u] CLOSED", conn:id()));
		s_con = nil;
	end
end

function timer(arg)
	local mgr = mg.mgr.new(arg);
	local opts = mg.mqtt.opts.new();
	opts:qos(s_qos);
	opts:topic(s_pub_topic);
	opts:version(4);
	opts:message("bye");

	if(s_con == nil) then
		s_con = mg.mqtt.connect(mgr, s_url, opts, "MQTT", mgr);
	end

end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

local function main(arg)
	local i = 1;
	while(i < #arg) do
		if(arg[i] == "-u") then
			s_url = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-p") then
			s_pub_topic = arg[i + 1];
			i = i + 2
		elseif(agr[i] == "-s") then
			s_sub_topic = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-v") then
			mg.log.log_set(arg[i + 1]);
			i = i + 2
		else
			mg.log.error(string.format("Unknown option: %s\n\nUsage: %s %s", arg[i], arg[0], "[-u mqtts://SERVER:PORT] [-p PUB_TOPIC] [-s SUB_TOPIC]\n[-v DEBUG_LEVEL]"));
		end
	end

	mg.handle_sig(SIGINT, "sig_handler"); --SIGINT
	mg.handle_sig(SIGTERM, "sig_handler") --SIGTERM

	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);

	mg.timer.add(mgr, 3000, MG_TIMER_REPEAT, MG_TIMER_RUN_NOW, "timer", mgr);

	while(run) do
		mg.mgr.poll(mgr, 1000);
	end

	mg.mgr.free(mgr);
	return;
end

main(arg);