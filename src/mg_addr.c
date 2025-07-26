#include "mg_addr.h"

// MG_ADDR USERDATUM
int newaddr (lua_State *L) {
	mg_addr *addr;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		addr = (mg_addr*)lua_touserdata(L, 1);
	 }
	else
		addr = (mg_addr*)lua_newuserdata(L, sizeof(mg_addr));

	luaL_getmetatable(L, "LuaBook.mg_addr");
	lua_setmetatable(L, -2);
	if(!addr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}


mg_addr *checkaddr(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_addr");
	luaL_argcheck(L, ud != NULL, 1, "`mg_addr' expected");

	return (mg_addr*)ud;
}

static int _addr_ip(lua_State *L) {
	mg_addr *addr = checkaddr(L);
	if(addr)
		lua_pushnumber(L, *addr->ip);

	return 1;
}

static int _addr_port(lua_State *L) {
	mg_addr *addr = checkaddr(L);
	if(addr)
		lua_pushinteger(L, addr->port);

	return 1;
}

static int _addr_scope_id(lua_State *L) {
	mg_addr *addr = checkaddr(L);
	lua_pushinteger(L, addr->scope_id);

	return 1;
}

static int _addr_is_ip6(lua_State *L) {
	mg_addr *addr = checkaddr(L);
	lua_pushboolean(L, addr->is_ip6);

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

static const struct luaL_reg mg_addr_lib_f [] = {
	{"new", 	newaddr		},
	{NULL, NULL}
};

static const struct luaL_reg mg_addr_lib_m [] = {
	{"new", 	newaddr		},
	{"ip",		_addr_ip	},
	{"port",	_addr_port	},
	{"scope_id",	_addr_scope_id	},
	{"is_ip6",	_addr_is_ip6	},
	{NULL, NULL}
};

void mg_open_mg_addr(lua_State *L) {
	//printf("START MG.ADDR: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_addr_lib_m);
	lua_setfield(L, -2, "addr");
	// mg_addr
	luaL_newmetatable(L, "LuaBook.mg_addr");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_addr_lib_m, 0);
	luaL_openlib(L, "mg_addr", mg_addr_lib_f, 0);
	lua_pop(L, 2);
	//printf("END MG.DNS: \n"); dumpstack(L);
}