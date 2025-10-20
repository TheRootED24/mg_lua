local mg = require "mg_lua"


local io = mg.iobuf.newt();
--io:init(32,128);
io:add(0,"test",4);
print(io:getlen())
local cio = mg.iobuf.newt(io);
print(io:getbuf(0))
print(cio.buf)
print(io:getsize())
io:resize(10);
print(io:getsize())
io:free()


local s = mg.str.newt()
s.buf = "test"

print(s.buf, s.len)
--print(cio.getbuf)