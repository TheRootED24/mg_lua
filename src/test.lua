require "luasocket"

function download(host,file)
	print(host,file)
end

function receive(connection)
	connection:timeout(0) -- do not block
	local s, status = connection:receive(2 ^ 10)

	if status == "timeout" then
		coroutine.yield(connection)
	end

	return s, status
end

 threads = {}    -- list of all live threads

function get (host, file)
	-- create coroutine
	local co = coroutine.create(function ()
		download(host, file)
	end)
	-- insert it in the list
	table.insert(threads, co)
end

function dispatcher ()
	while true do
		local n = table.getn(threads)
		if n == 0 then break end  -- no more threads to run
		for i=1,n do
			local status, res = coroutine.resume(threads[i])
			if not res then  -- thread finished its task?
				table.remove(threads, i)
				break
			end
		end
	end
end

host = "www.w3.org"
get(host, "/TR/html401/html40.txt")
get(host,"/TR/2002/REC-xhtml1-20020801/xhtml1.pdf")
get(host,"/TR/REC-html32.html")
get(host,"/TR/2000/REC-DOM-Level-2-Core-20001113/DOM2-Core.txt")

dispatcher()   -- main loop