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

int new_rpc_req(lua_State *L) {
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
}

rpc_req *check_rpc_req (lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.rpc_request");
	luaL_argcheck(L, ud != NULL, 1, "`rpc_request' expected");

	return(rpc_req*)ud;
}

static int _mg_rpc_req_frame (lua_State *L) {
	int nargs = lua_gettop(L);
	rpc_req *req = check_rpc_req(L);
	if(nargs > 1)
		req->frame = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, req->frame.buf, req->frame.len);

	return 1;
}

/*
static int _mg_rpc_req_frame (lua_State *L) {
	int nargs = lua_gettop(L);
	rpc_req *req = check_rpc_req(L);
	lua_remove(L, 1);
	struct mg_str *s = check_mg_str(L);

	if(nargs > 2)
		req->frame = mg_str(luaL_checkstring(L, 3));

	s->buf = req->frame.buf;
	s->len = req->frame.len;

	return 1;
}
*/

/*static void dumpstack (lua_State *L) {
  int top=lua_gettop(L);
  for (int i = 1; i <= top; i++) {
    printf("%d\t%s\t", i, luaL_typename(L,i));
    switch (lua_type(L, i)) {
      case LUA_TNUMBER:
        printf("%g\n",lua_tonumber(L,i));
        break;
      case LUA_TSTRING:
        printf("%s\n",lua_tostring(L,i));
        break;
      case LUA_TBOOLEAN:
        printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
        break;
      case LUA_TNIL:
        printf("%s\n", "nil");
        break;
      default:
        printf("%p\n",lua_topointer(L,i));
        break;
    }
  }
}*/

static const struct luaL_reg rpc_req_lib_f [] = {
	{"new", 	new_rpc_req		},
	{NULL, NULL}
};

static const struct luaL_reg rpc_req_lib_m [] = {
	{"new",		new_rpc_req		},
	{"frame",	_mg_rpc_req_frame	},
	{NULL, NULL}
};

void  mg_open_mg_rpc_req (lua_State *L) {
	//printf("START MG.RPC.REQ \n");
	lua_getfield(L, -1, "rpc");
	//dumpstack(L);
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
	//printf("END MG.RPC.REQ:\n"); dumpstack(L);

}