#include "mg_time.h"

// uint64_t mg_millis(void);
static int _mg_time_millis(lua_State *L) {
	lua_pushnumber(L, luaL_checknumber(L, mg_millis()));
	return 1;
}

// uint64_t mg_now(void);
static int _mg_time_now(lua_State *L) {
	lua_pushnumber(L, luaL_checknumber(L, mg_now()));
	return 1;
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

static const struct luaL_reg mg_time_lib_m [] = {
	{"millis",	_mg_time_millis		},
	{"now",		_mg_time_now		},
	{NULL, NULL}
};

void mg_open_mg_time(lua_State *L) {
	//printf("START MG.TIME: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_time_lib_m);
	lua_setfield(L, -2, "time");
	// mg_time
	luaL_newmetatable(L, "LuaBook.mg_time");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	//printf("END MG.TIME: \n"); dumpstack(L);
}