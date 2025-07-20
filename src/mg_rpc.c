#include "mg_rpc.h"

void dummy2(struct mg_rpc_req *r) {
	printf("rpc address: %p\n", r);
}
/*
int new_mg_rpc_head (lua_State *L) {
	mg_rpc **head = (mg_rpc **)lua_newuserdata(L, sizeof(mg_rpc*));
	luaL_getmetatable(L, "LuaBook.mg_rpc_head");
	lua_setmetatable(L, -2);
	return 1 ;  // new userdatum is already on the stack
}
*/
int new_mg_rpc (lua_State *L) {
	mg_rpc *rpc = (mg_rpc *)lua_newuserdata(L, sizeof(mg_rpc));
	luaL_getmetatable(L, "LuaBook.mg_rpc");
	lua_setmetatable(L, -2);
	if(!rpc) lua_pushnil(L);

	return 1 ;  /* new userdatum is already on the stack */
}

mg_rpc *check_mg_rpc (lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_rpc");
	luaL_argcheck(L, ud != NULL, 1, "`mg_rpc' expected");
	return(mg_rpc*)ud;
}

/*
mg_rpc **check_mg_rpc_head (lua_State *L) {
	void **ud = luaL_checkudata(L, 1, "LuaBook.mg_rpc_head");
	luaL_argcheck(L, ud != NULL, 1, "`mg_rpc_head' expected");
	return(mg_rpc**)ud;
}
*/

static int _rpc_method (lua_State *L) {
	int nargs = lua_gettop(L);
	mg_rpc *rpc = check_mg_rpc(L);
	if(nargs > 1)
		rpc->method = mg_str(luaL_checkstring(L, 1));

	lua_pushlstring(L, rpc->method.buf, rpc->method.len);
	return 1;
}

// void mg_rpc_add(struct mg_rpc **head, struct mg_str method_pattern, void (*handler)(struct mg_rpc_req *), void *handler_data);
static int _mg_rpc_add(lua_State *L) {
	mg_rpc *head = check_mg_rpc(L);
	struct mg_str method_pattern = mg_str(luaL_checkstring(L, 2));
	mg_rpc_add(&head, method_pattern, dummy2, NULL);
	return 0;
}

// void mg_rpc_del(struct mg_rpc **head, void (*handler)(struct mg_rpc_req *));
static int _mg_rpc_del(lua_State *L) {
	mg_rpc *head = check_mg_rpc(L);
	//rpc_req *f = (rpc_req*)lua_topointer(L, 2);
	bool dhead = lua_toboolean(L, luaL_checkint(L, 3));

	mg_rpc_del(&head, dummy2);

	if(dhead)
		mg_rpc_del(&head, NULL);

	return 0;
}

// void mg_rpc_process(struct mg_rpc_req *req);
static int _mg_rpc_process(lua_State *L) {
	rpc_req *req = check_rpc_req(L);
	mg_rpc_process(req);

	return 0;
}

// void mg_rpc_list(struct mg_rpc_req *r);
static int _mg_rpc_list(lua_State *L) {
	rpc_req *req = check_rpc_req(L);
	mg_rpc_list(req);

	return 0;
}

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

static const struct luaL_reg mg_rpc_lib_m [] = {
	{"new",			new_mg_rpc	},
	{"add",			_mg_rpc_add	},
	{"del",			_mg_rpc_del,	},
	{"process", 		_mg_rpc_process	},
	{"list",		_mg_rpc_list	},
	{"method",		_rpc_method	},
	{NULL, NULL}
};

void mg_open_mg_rpc(lua_State *L) {
	//printf("START MG.RPC: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_rpc_lib_m);
	lua_setfield(L, -2, "rpc");
	// mg_rpc
	luaL_newmetatable(L, "LuaBook.mg_rpc");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);

	mg_open_mg_rpc_req(L);
	lua_pop(L, 1);
	//printf("END MG.RPC: \n"); dumpstack(L);
}