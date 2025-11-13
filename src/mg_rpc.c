#include "mg_rpc.h"

static struct mg_rpc *s_rpc_head = NULL;

int _mg_rpc_new (lua_State *L) {
	mg_rpc *rpc = (mg_rpc *)lua_newuserdata(L, sizeof(mg_rpc));
	memset(rpc, 0, sizeof(mg_rpc));

	luaL_getmetatable(L, "LuaBook.mg_rpc");
	lua_setmetatable(L, -2);
	if(!rpc)
		lua_pushnil(L);

	return 1 ;  /* new userdatum is already on the stack */
};

mg_rpc *check_mg_rpc (lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_rpc");
	luaL_argcheck(L, ud != NULL, pos, "`mg_rpc' expected");

	return(mg_rpc*)ud;
};

static int _rpc_head_init(lua_State *L) {
	mg_rpc **head = &s_rpc_head;

	lua_pushlightuserdata(L, head);

	return 1;
};

static int _mg_rpc_method (lua_State *L) {
	int nargs = lua_gettop(L);
	mg_rpc *rpc = check_mg_rpc(L, 1);
	if(nargs > 1) {
		rpc->method = mg_str(luaL_checkstring(L, 2)); // changed from 1 to 2 !!

		return 0;
	}
	lua_pushlstring(L, rpc->method.buf, rpc->method.len);

	return 1;
};

// void mg_rpc_add(struct mg_rpc **head, struct mg_str method_pattern, void (*handler)(struct mg_rpc_req *), void *handler_data);
static int _mg_rpc_add(lua_State *L) {
	mg_rpc **head = (mg_rpc**)lua_topointer(L, 1);
	const char *cb = luaL_checkstring(L, 2);
	struct mg_str method_pattern = mg_str(cb);

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
	rpc_req *req = check_rpc_req(L, 1);
	mg_rpc_list(req);

	return 0;
};

// void mg_rpc_ok(struct mg_rpc_req *, const char *fmt, ...);
static int _mg_rpc_ok(lua_State *L) {
	rpc_req *req = check_rpc_req(L, 1);
	const char *fmt = luaL_checkstring(L, 2);
	const char * argstr = luaL_checkstring(L, 3);

	mg_rpc_ok(req, fmt, argstr);

	return 0;
};

static int _mg_rpc_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		printf("Key: %s\n cannot be set !!\n", key);
	}

	return 0;
};

static int _mg_rpc_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_rpc_req_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_rpc_newt(lua_State * L) {
	_mg_rpc_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_pushcfunction(L, _rpc_head_init);
	lua_setfield(L, -2, "init_head");

	lua_pushcfunction(L, _mg_rpc_add);
	lua_setfield(L, -2, "add");

	lua_pushcfunction(L, _mg_rpc_del);
	lua_setfield(L, -2, "del");

	lua_pushcfunction(L, _mg_rpc_ok);
	lua_setfield(L, -2, "ok");

	lua_pushcfunction(L, _mg_rpc_process);
	lua_setfield(L, -2, "process");

	lua_pushcfunction(L, _mg_rpc_list);
	lua_setfield(L, -2, "list");

	lua_pushcfunction(L, _mg_rpc_method);
	lua_setfield(L, -2, "method");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_rpc_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_rpc_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};


static const struct luaL_reg mg_rpc_lib_m [] = {
	{"ptr",			_mg_rpc_new	},
	{"new",		_mg_rpc_newt	},
	{"init_head",		_rpc_head_init	},
	{"add",			_mg_rpc_add	},
	{"del",			_mg_rpc_del,	},
	{"ok",			_mg_rpc_ok	},
	{"process", 		_mg_rpc_process	},
	{"list",		_mg_rpc_list	},
	{"method",		_mg_rpc_method	},
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
	// open sub-modules
	mg_open_mg_rpc_req(L);
	lua_pop(L, 1);
};
