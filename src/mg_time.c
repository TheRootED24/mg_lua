#include "mg_time.h"

// uint64_t mg_millis(void);
static int _mg_time_millis(lua_State *L) {
	uint64_t t = mg_millis();
	lua_pushnumber(L, t);

	return 1;
};

// uint64_t mg_now(void);
static int _mg_time_now(lua_State *L) {
	uint64_t now = mg_now();
	lua_pushnumber(L, now);

	return 1;
};

static const struct luaL_reg mg_time_lib_m [] = {
	{"millis",	_mg_time_millis		},
	{"now",		_mg_time_now		},
	{NULL, NULL}
};

void mg_open_mg_time(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_time_lib_m);
	lua_setfield(L, -2, "time");
	// mg_time
	luaL_newmetatable(L, "LuaBook.mg_time");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
};
