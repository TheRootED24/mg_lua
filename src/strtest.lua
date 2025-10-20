local mg = require "mg_lua"

local function load_string(s)
    s.buf = "New String";
    s.len =  s.buf:len();

    return s;
end

local s = mg.str.newt();
s.buf = "test1";

local s2 = mg.str.newt("test2");

local s3 = mg.str.newt("test3", 5);

local s4 = mg.str.newt(s3);


print(s.buf, s.len);
print(s2.buf, s2.len);
print(s3.buf, s3.len);
print(s4.buf, s4.len);

local s5 = mg.str.newt();
s5 = load_string(s5);

print(s5.buf, s5.len);