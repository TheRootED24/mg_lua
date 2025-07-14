local mg = require "mg_lua";

require "examples.mqtt.dashboard.net";

MQTT_DASHBOARD = 1;
MQTT_KEEPALIVE_SEC = 60;
MQTT_SERVER_URL = "mqtt://broker.hivemq.com:1883";
MQTT_ROOT_TOPIC = "mg_mqtt_dashboard";
MQTT_FIRM_VER = "1.0.0";
MQTT_DEV_ID = 0;

local SIGINT = 2;
local SIGTERM = 15;
local run, signo = true, 0;

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false;
		signo = sig;
	end
end

local usage = [[
		Usage: %s OPTION/S
		 ******************** OPTIONS ********************
		-u 	URL  	Set url			default: %s
		-i 	ID	Set device id		default: %s
		-t 	TOPIC	Set the root topic	default: %s
		-f 	VERSION	Set firmware version	default: %d

		-v 	LOG	Set mg log verbosity
]];

function main(arg)
	local i = 1;
	local log_set = false;
	while(i < #arg) do
		if(arg[i] == "-u") then
			MQTT_SERVER_URL = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-i") then
			MQTT_DEV_ID = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-t") then
			MQTT_ROOT_TOPIC = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-f") then
			MQTT_FIRM_VER = arg[i + 1];
			i = i + 2
		elseif(arg[i] == "-v") then
			log_set = true;
			mg.log.log_set(arg[i + 1]);
			i = i + 2
		else
			local payload = string.format("\n\n" .. usage,  arg[0] or "??", g_mqtt_server_url, g_device_id, g_root_topic, s_listen_on, g_firmware_version)
			mg.log.error(payload);
			return;
		end
	end

	local mgr = mg.mgr.new();

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	if(not log_set) then
		mg.log.log_set(MG_LL_DEBUG);
	end

	mgr:init();	-- initialize the mgr
	web_init(mgr);	-- initialize web

	while(run) do
		mg.mgr.poll(mgr, 50);
	end

	web_free(); 		-- free up web resources
	mg.mgr.free(mgr); 	-- free the manager
	mg.free_lua_states(); 	-- free lua states .. must be called last !!

	mg.log.info(string.format("Exiting on signal %d", signo))

end

main(arg);