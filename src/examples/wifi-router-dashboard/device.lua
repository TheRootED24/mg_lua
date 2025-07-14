-- Device class
Device = {}
Device.__index = Device;

function Device.new(device)
	local dev = setmetatable({}, Device);
	dev.name = device.dev_name or nil;
	dev.mac = device.mac or nil;
	dev.ip_addr = device.ip_addr or nil;
	dev.speed = device.speed or nil;
	dev.conn_to = device.connected_to or nil;
	dev.lease_time = device.lease_time_left or nil;
	dev.last_seen = device.last_seen or nil;

	return dev;
end

function Device:json()
local json = string.format("{%q:%q,%q:%q,%q:%q,%q:%d,%q:%q,%q:%d,%q:%q}",
									"dev_name",self.name,
									"mac",self.mac,
									"ip",self.ip_addr,
									"speed",self.speed,
									"connected_to",self.conn_to,
									"lease_time_left",self.lease_time,
									"last_seen",self.last_seen
	);

	return json
end

-- Device class