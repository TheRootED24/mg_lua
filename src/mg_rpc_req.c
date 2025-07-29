#include "mg_rpc_req.h"

/*
struct mg_rpc_req {
  struct mg_rpc **head;  // RPC handlers list head
  struct mg_rpc *rpc;    // RPC handler being called
  mg_pfn_t pfn;          // Response printing function
  void *pfn_data;        // Response printing function data
  void *req_data;        // Arbitrary request data
  struct mg_str frame;   // Request, e.g. {"id":1,"method":"add","params":[1,2]}
};
*/

int _mg_rpc_req_new (lua_State *L) {
	int nargs = lua_gettop(L);
	rpc_req * req = NULL;
	if(nargs > 1 ) {
		mg_rpc **head = (mg_rpc **)lua_topointer(L, 1);
		//mg_rpc *rpc = (mg_rpc *)lua_topointer(L, 2);
		mg_iobuf *io = (mg_iobuf*)lua_topointer(L, 2);
		//mg_iobuf_init(io, 16,512);
		//void *req_data = (void*) luaL_optstring(L, 2, NULL);
		const char *frame = luaL_checkstring(L, 3);

		req = (rpc_req *)lua_newuserdata(L, sizeof(rpc_req));
		memset(req, 0, sizeof(rpc_req));

		req->head = head;
		req->rpc = NULL;
		req->pfn = mg_pfn_iobuf;
		req->pfn_data = io;
		req->req_data = NULL;
		req->frame = mg_str(frame);
	}
	else
		req = (rpc_req*)lua_touserdata(L, 1);

	lua_pushlightuserdata(L, req);
	luaL_getmetatable(L, "LuaBook.rpc_request");
	lua_setmetatable(L, -2);

	return 1;  /* new userdatum is already on the stack */
};

rpc_req *check_rpc_req (lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.rpc_request");
	luaL_argcheck(L, ud != NULL, pos, "`rpc_request' expected");

	return(rpc_req*)ud;
};

static int _mg_rpc_req_frame (lua_State *L) {
	int nargs = lua_gettop(L);
	rpc_req *req = check_rpc_req(L, 1);
	if(nargs > 1)
		req->frame = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, req->frame.buf, req->frame.len);

	return 1;
};

static const struct luaL_reg rpc_req_lib_f [] = {
	{"new", 	_mg_rpc_req_new		},
	{NULL, NULL}
};

static const struct luaL_reg rpc_req_lib_m [] = {
	{"new",		_mg_rpc_req_new		},
	{"frame",	_mg_rpc_req_frame	},
	{NULL, NULL}
};

void  mg_open_mg_rpc_req (lua_State *L) {
	lua_getfield(L, -1, "rpc");
	lua_newtable(L);
	luaL_register(L, NULL, rpc_req_lib_m);
	lua_setfield(L, -2, "req");
	// mg_rpc_req
	luaL_newmetatable(L, "LuaBook.rpc_request");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, rpc_req_lib_m, 0);
	luaL_openlib(L, "mg_rpc_req", rpc_req_lib_f, 0);
	lua_pop(L, 2);
};
