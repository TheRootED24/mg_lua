#include "mg_mgr.h"

int _mg_mgr_new (lua_State *L) {
	mg_mgr *mgr = NULL;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		mgr = (mg_mgr*)lua_touserdata(L, 1);
		//lua_pushlightuserdata(L, mgr);
	}
	else {
		mgr = (mg_mgr*)lua_newuserdata(L, sizeof(mg_mgr));
		memset(mgr, 0 , sizeof(mg_mgr));
	}

	luaL_getmetatable(L, "LuaBook.mg_mgr");
	lua_setmetatable(L, -2);
	
	if(!mgr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_mgr *check_mg_mgr(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_mgr");
	luaL_argcheck(L, ud != NULL, 1, "`mg_mgr' expected");

	return (mg_mgr*)ud;
};

static int _mg_mgr_init(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	mg_mgr_init(mgr);

	return 1;
};

static int _mg_mgr_poll(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	int timeout = luaL_checkint(L, -1);
	mg_mgr_poll(mgr, timeout);

	return 0;
};

static int _mg_mgr_free(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	mg_mgr_free(mgr);

	return 0;
};

static int _mg_mgr_conns(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);

	if(NULL != mgr->conns)
	{
		mg_connection *conns = mgr->conns;
		lua_pushlightuserdata(L, conns);
	}
	else
		lua_pushnil(L);

	return 1;
};

static const struct luaL_reg mg_mgr_lib_f [] = {
	{"new", 	_mg_mgr_new	},
	{NULL, NULL}
};

static const struct luaL_reg mg_mgr_lib_m [] = {
	{"new", 	_mg_mgr_new	},
	{"conns", 	_mg_mgr_conns	},
	{"init",	_mg_mgr_init	},
	{"poll",	_mg_mgr_poll	},
	{"free",	_mg_mgr_free	},
	{NULL, NULL}
};

void mg_open_mg_mgr (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_mgr_lib_m);
	lua_setfield(L, -2, "mgr");
	// mg_mgr
	luaL_newmetatable(L, "LuaBook.mg_mgr");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_mgr_lib_m, 0);
	luaL_openlib(L, "mg_mgr", mg_mgr_lib_f, 0);
	lua_pop(L, 2);
};
