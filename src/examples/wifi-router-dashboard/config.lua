require "uci"

-- Config Class
Config = {}
Config.__index = Config;

function Config.new(conf)
	local cfg = setmetatable({}, Config);
	cfg.path = conf.cpath or nil;
	cfg.conf = conf.conf or nil;
	cfg.sec = conf.sec or nil;

	return cfg;
end

function Config:get(opt)
	if(opt and type(opt) == "string") then
		local uci = uci.cursor(self.path);
		self[opt] =  uci:get(self.conf, self.sec, opt) or nil;
	end

	return self[opt];
end

function Config:set(opt, val)
	local new_val;
	if(val and type(opt) == "string") then
		if(type(val) == "boolean") then
			new_val = tostring(val);
		else
			new_val = val;
		end

		if(new_val) then
			local uci = uci.cursor(self.path);
			self[opt] = val;
			uci:set(self.conf, self.sec, opt, new_val);
			uci:commit(self.conf);
			return 0;
		end
	end
	return 1;
end
-- End Config Class