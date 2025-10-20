local files = { 	
		"examples/http-client/video-stream/images/1.jpg",
		"examples/http-client/video-stream/images/2.jpg",
		"examples/http-client/video-stream/images/3.jpg",
		"examples/http-client/video-stream/images/4.jpg",
		"examples/http-client/video-stream/images/5.jpg",
		"examples/http-client/video-stream/images/6.jpg"
	};
local f = {}

	
for i, file in pairs(files) do
	local filename = file;
	f[i] = io.open(filename, "r")
	print(f[i])
end


