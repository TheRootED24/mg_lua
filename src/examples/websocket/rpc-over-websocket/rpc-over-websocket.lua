require "mg_types"
local mg =- require "mg_lua"

local s_listen_on = "ws://localhost:8000";
local s_web_root = "web_root";
local s_rpc_head = nil;


function  rpc_sum(r) {
 local a, b  = 0.0, 0.0;
 
  mg.json.get_num(r:frame(), "$.params[0]", a);
  mg.json.get_num(r:frame(), "$.params[1]", b);
  mg.rpc.ok(r, "%g", a + b);
}