#include "mg_rpc.h"

static struct mg_rpc *s_rpc_head = NULL;

int new_mg_rpc (lua_State *L) {
	mg_rpc *rpc = (mg_rpc *)lua_newuserdata(L, sizeof(mg_rpc));
	memset(rpc, 0, sizeof(mg_rpc));

	luaL_getmetatable(L, "LuaBook.mg_rpc");
	lua_setmetatable(L, -2);
	if(!rpc)
		lua_pushnil(L);

	return 1 ;  /* new userdatum is already on the stack */
};

mg_rpc *check_mg_rpc (lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_rpc");
	luaL_argcheck(L, ud != NULL, 1, "`mg_rpc' expected");

	return(mg_rpc*)ud;
};

static int _rpc_head_init(lua_State *L) {
	mg_rpc **head = &s_rpc_head;

	//s_rpc_head = head;
	lua_pushlightuserdata(L, head);

	return 1;
};

static int _rpc_method (lua_State *L) {
	int nargs = lua_gettop(L);
	mg_rpc *rpc = check_mg_rpc(L);
	if(nargs > 1)
		rpc->method = mg_str(luaL_checkstring(L, 1));

	lua_pushlstring(L, rpc->method.buf, rpc->method.len);

	return 1;
};

// void mg_rpc_add(struct mg_rpc **head, struct mg_str method_pattern, void (*handler)(struct mg_rpc_req *), void *handler_data);
static int _mg_rpc_add(lua_State *L) {
	mg_rpc **head = (mg_rpc**)lua_topointer(L, 1);
	const char *cb = luaL_checkstring(L, 2);
	struct mg_str method_pattern = mg_str(cb);
	//lua_settop(L, 0);
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;
	GL->fn_data = NULL;

	mg_rpc_add(head, method_pattern, lua_rpc_cb, GL);

	return 0;
};

// void mg_rpc_del(struct mg_rpc **head, void (*handler)(struct mg_rpc_req *));
static int _mg_rpc_del(lua_State *L) {
	mg_rpc **head = (mg_rpc**)lua_topointer(L, 1);
	//void(*handler)(rpc_req*) f = (rpc_req*)lua_topointer(L, 2);
		mg_rpc_del(head, NULL);

	return 0;
};

// void mg_rpc_process(struct mg_rpc_req *req);
static int _mg_rpc_process(lua_State *L) {
	rpc_req *req = (rpc_req*) lua_touserdata(L, 1);
	mg_rpc_process(req);

	return 0;
};

// void mg_rpc_list(struct mg_rpc_req *r);
static int _mg_rpc_list(lua_State *L) {
	rpc_req *req = check_rpc_req(L);
	mg_rpc_list(req);

	return 0;
};

// void mg_rpc_ok(struct mg_rpc_req *, const char *fmt, ...);
static int _mg_rpc_ok(lua_State *L) {
	rpc_req *req = check_rpc_req(L);
	const char *fmt = luaL_checkstring(L, 2);
	const char * argstr = luaL_checkstring(L, 3);

	mg_rpc_ok(req, fmt, argstr);

	return 0;
};

static const struct luaL_reg mg_rpc_lib_m [] = {
	{"new",			new_mg_rpc	},
	{"init_head",		_rpc_head_init	},
	{"add",			_mg_rpc_add	},
	{"del",			_mg_rpc_del,	},
	{"ok",			_mg_rpc_ok	},
	{"process", 		_mg_rpc_process	},
	{"list",		_mg_rpc_list	},
	{"method",		_rpc_method	},
	{NULL, NULL}
};

void mg_open_mg_rpc(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_rpc_lib_m);
	lua_setfield(L, -2, "rpc");
	// mg_rpc
	luaL_newmetatable(L, "LuaBook.mg_rpc");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	// open syb-modules
	mg_open_mg_rpc_req(L);
	lua_pop(L, 1);
};
