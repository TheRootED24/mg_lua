#include "mg_mgr.h"

int newmgr (lua_State *L) {
	mg_mgr *mgr = (mg_mgr*)lua_newuserdata(L, sizeof(mg_mgr));
	luaL_getmetatable(L, "LuaBook.mg_mgr");
	lua_setmetatable(L, -2);
	if(!mgr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}


mg_mgr *checkmgr(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_mgr");
	luaL_argcheck(L, ud != NULL, 1, "`mg_mgr' expected");
	
	return (mg_mgr*)ud;
}

static int _mg_mgr_init(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	mg_mgr_init(mgr);

	return 1;
}

static int _mg_mgr_poll(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	int timeout = luaL_checkint(L, -1);
	mg_mgr_poll(mgr, timeout);

	return 0;
}

static int _mg_mgr_free(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	mg_mgr_free(mgr);

	return 0;
}

static const struct luaL_reg mgrlib_f [] = {
	{"new", 	newmgr	},
	{NULL, NULL}
};

static const struct luaL_reg mgrlib_m [] = {
	{"new", 		newmgr			},
	{"init",		_mg_mgr_init	},
	{"poll",		_mg_mgr_poll	},
	{"free",		_mg_mgr_free	},
	{NULL, NULL}
};

static void dumpstack (lua_State *L) {
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
}

void mg_open_mg_mgr (lua_State *L) {
	printf("START MG.MGR: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mgrlib_m);
	//dumpstack(L);
	lua_setfield(L, -2, "mgr");
	// mg_mgr
	luaL_newmetatable(L, "LuaBook.mg_mgr");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mgrlib_m, 0);
	luaL_openlib(L, "mg_mgr", mgrlib_f, 0);
	lua_pop(L, 2);
	printf("END MG.MGR: \n"); dumpstack(L);
}