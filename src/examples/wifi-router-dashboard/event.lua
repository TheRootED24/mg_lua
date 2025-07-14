-- Event Class
Event = {}
Event.__index = Event;

function Event.new()
	local ev = setmetatable({}, Event);

	return ev;
end

function Event:json()
	local json = string.format("{%q:%d,%q:%d,%q:%d,%q:%q}\n",
						"time",self.timestamp,
						"type",self.type,
						"prio",self.prio,
						"text",self.text
	);

	return json
end
-- End Event Class