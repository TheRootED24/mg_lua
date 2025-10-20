local mg = require "mg_lua"


--[[ **** mg_json_get() **** ]]

-- Create a json string: { "a": 1, "b": [2, 3] }
--[[
buf = mg_mprintf("{ %m: %d, %m: [%d, %d] }",
                       MG_ESC("a"), 1,
                       MG_ESC("b"), 2, 3);
]]--
local buf = string.format("{ \"%s\": %d, \"%s\": [%d, %d] }", "a", 1, "b", 2, 3);
print(buf);
--[[
struct mg_str json = mg_str(buf);
int offset, length;
]]
local json = mg.str.new(buf);
local offset, length = 0,0;
--[[
// Lookup "$", which is the whole JSON. Can be used for validation
offset = mg_json_get(json, "$", &length);    // offset = 0, length = 23
]]
offset, length = mg.json.get(json, "$");
print(offset, length);
--[[
// Lookup attribute "a". Point to value "1"
offset = mg_json_get(json, "$.a", &length);  // offset = 7, length = 1
]]
offset, length = mg.json.get(json, "$.a");
print(offset, length);
--[[
// Lookup attribute "b". Point to array [2, 3]
offset = mg_json_get(json, "$.b", &length);  // offset = 15, length = 6
]]
offset, length = mg.json.get(json, "$.b");
print(offset, length);
--[[
// Lookup attribute "b[1]". Point to value "3"
offset = mg_json_get(json, "$.b[1]", &length); // offset = 19, length = 1
]]
offset, length = mg.json.get(json, "$.b[1]");
print(offset, length);

--[[ **** mg_json_get_tok() **** ]]
local json1 = mg.str.new("{\"a\":\"b:c\"}")
local resp = mg.json.get_tok(json1, "$.a")
print(resp,"here")



--[[ **** mg_json_get_num() **** ]]

--[[
double d = 0.0;
mg_json_get_num(mg_str("[1,2,3]", "$[1]", &d));     // d == 2
mg_json_get_num(mg_str("{\"a\":1.23}", "$.a", &d)); // d == 1.23

]]

local d = 0.00;
d = mg.json.get_num(mg.str.new("[1,2,3]"), "$[1]");
print(d);

d = mg.json.get_num(mg.str.new("{\"a\":1.23}"), "$.a");
print(d);



--[[ **** mg_json_get_bool() **** ]]

--[[
bool b = false;
mg_json_get_bool(mg_str("[123]", "$[0]", &b));   // Error. b remains to be false
mg_json_get_bool(mg_str("[true]", "$[0]", &b));  // b is true
]]

local b = false;
b = mg.json.get_bool(mg.str.new("[123]"), "$[0]");
print(b);

b = mg.json.get_bool(mg.str.new("[true]"), "$[0]");
print(b);



--[[ **** mg_json_get_long() ****]]

--[[
long a = mg_json_get_long(mg_str("[123]", "$a", -1));   // a = -1
long b = mg_json_get_long(mg_str("[123]", "$[0]", -1)); // b = 123
]]

local l = -1;
l = mg.json.get_long(mg.str.new("[123]"), "$a", l);
print(l);

l = mg.json.get_long(mg.str.new("[123]"), "$[0]", l);
print(l);



--[[ **** mg_json_get_str() ****]]

--[[
struct mg_str json = mg_str("{\"a\": \"hi\"}");  // json = {"a": "hi"}
char *str = mg_json_get_str(json, "$.a");        // str = "hi"
mg_free(str);
]]

local str, len = mg.json.get_str(mg.str.new("{\"a\": \"hi\"}"), "$.a");
print(str, len); -- no need to free, lua GC will take care of it



--[[ **** mg_json_get_hex() **** ]]

--[[
struct mg_str json = mg_str("{\"a\": \"6869\"}"); // json = {"a": "6869"}
char *str = mg_json_get_hex(json, "$.a", NULL);   // str = "hi"
mg_free(str);
]]
local hx, len = mg.json.get_hex(mg.str.new("{\"a\": \"6869\"}"), "$.a", 1);
print(hx,len); -- no need to free, lua GC will take care of it



--[[ **** mg_json_get_b64() **** ]]

--[[
struct mg_str json = mg_str("{\"a\": \"YWJj\"}"); // json = {"a": "YWJj"}
char *str = mg_json_get_b64(json, "$.a", NULL);   // str = "abc"
mg_free(str);

]]

local b64 =  mg.json.get_b64(mg.str.new("{\"a\": \"YWJj\"}"), "$.a", 1);
print(b64);




--[[ **** mg_json_unescape() **** ]]

--[[
struct mg_str str = mg_str("{\"a\": \"b\\u0063d\"}"); // escaped json = {"a": "b\u0063d"}
char json[20];
bool result = mg_json_unescape(str, result, 20);    // json = {"a": "bcd"}
]]

local bool, ustr =  mg.json.unescape(mg.str.new("{\"a\": \"b\\u0063d\"}"), 19);
print(bool, ustr);




--[[ **** mg_json_next() **** ]]

--[[
struct mg_str key, val, obj = mg_str("{\"a\": [true], \"b\": 12345}");
size_t ofs = 0;
while ((ofs = mg_json_next(obj, ofs, &key, &val)) > 0) {
  printf("%.*s -> %.*s\n", (int) key.len, key.buf, (int) val.len, val.buf);
}
]]

local obj = mg.str.newt("{\"a\": [true], \"b\": 12345}");
local key = mg.str.newt();
local val = mg.str.newt();

local ofs = mg.json.next(obj, 0, key, val);
while(ofs > 0) do
    print(key.buf, val.buf, ofs)
    ofs = mg.json.next(obj, ofs, key, val);
end

--[[
json = "{\"a\":\"b:c\"}"
resp = mg.json.get_token(json, "$.a")
print(resp)
s = mg.str.new("192.168.1.1")
print(s:buf(),s:len())
addr = mg.addr.new()
atn = mg.string.aton(s, addr)


print(addr:ip())
]]