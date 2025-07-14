local mg = require "mg_lua"

MQTT_SERVER = "mqtt://broker.hivemq.com:1883";
MQTT_PUBLISH_TOPIC = "mg/my_device";
MQTT_SUBSCRIBE_TOPIC = "mg/#";

local run = true
local SIGINT = 2;
local SIGTERM = 15;

local s_http_addr = "http://localhost:8000";
local s_web_root = "examples/webui-plain/web_root/";
local json_header = "Content-Type: application/json\r\n";

local s_config = { url = "0.0.0.0", pub = "", sub = ""};

local conf_resp = string.format("{%q:%q,%q:%q,%q:%q}\n",
							"url", s_config.url,
							"pub", s_config.pub,
							"sub", s_config.sub
);

function update_config(json)
	if(json) then
		s_config.url = mg.json.get_str(json, "$.url");
		s_config.pub = mg.json.get_str(json, "$.pub");
		s_config.sub = mg.json.get_str(json, "$.sub");
	end
end

function fn(c, ev, ev_data)
	if (ev == MG_EV_OPEN and mg.connection.ptr(c):is_listening()) then
		s_config.url = MQTT_SERVER;
		s_config.pub = MQTT_PUBLISH_TOPIC;
		s_config.sub = MQTT_SUBSCRIBE_TOPIC;
	elseif(ev == MG_EV_HTTP_MSG) then
		local hm = mg.http.message.new(hm);
		local body = hm.body;

		if(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/config/get"))) then
			mg.http.reply(mg.connection.ptr(c), 200, json_header, conf_resp);
		elseif(mg.string.match(mg.str.ptr(hm.uri), mg.str.ptr("/api/config/set"))) then
			local json = mg.str.ptr(body);
			update_config(json);

			mg.http.reply(mg.connection.ptr(c), 200, "", "ok\n");
		else
			local opts = mg.http.serve_opts.ptr(s_web_root);
			mg.http.serve_dir(mg.connection.ptr(c), hm, opts);
		end
	end
end

function sig_handler(sig)
	if(sig == SIGINT or sig == SIGTERM) then
		run = false
	end
end

function main(...)
	local mgr = mg.mgr.new();
	mgr = mg.mgr.init(mgr);

	mg.handle_sig(SIGINT, "sig_handler");	--SIGINT
	mg.handle_sig(SIGTERM, "sig_handler");	--SIGTERM

	mg.http.listen(mgr, s_http_addr, "fn");

	while(run) do
		mg.mgr.poll(mgr, 1000);
	end

	mg.mgr.free(mgr);
	mg.free_lua_states();
end

main(...);