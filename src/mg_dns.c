#include "mg_dns.h"

static int newdns (lua_State *L) {
	mg_dns *dns = (mg_dns*)calloc(1, sizeof(mg_dns));
	lua_pushlightuserdata(L, dns);
	luaL_getmetatable(L, "LuaBook.mg_dns");
	lua_setmetatable(L, -2);
	if(!dns) lua_pushnil(L);
	return 1;  /* new userdatum is already on the stack */
}

static mg_dns *checkdns(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_dns");
	luaL_argcheck(L, ud != NULL, 1, "`mg_dns' expected");

	return (mg_dns*)ud;
}

static int _mg_dns_url(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_dns *dns = checkdns(L);
	if(nargs > 1)
		dns->url = luaL_checkstring(L, -1);

	lua_pushstring(L, dns->url);

	return 1;
};

static int _mg_dns_conn(lua_State *L) {
	mg_dns *dns = checkdns(L);
	lua_pushlightuserdata(L, dns->c);
	return 1;
}

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

static const struct luaL_reg mg_dns_lib_f [] = {
	{"new", 	newdns	},
	{NULL, NULL}
};

static const struct luaL_reg mg_dns_lib_m [] = {
	{"new", 		newdns			},
	{"url",			_mg_dns_url		},
	{"conn",		_mg_dns_conn	},
	{NULL, NULL}
};

void mg_open_mg_dns (lua_State *L) {
	// mg_dns
	printf("START MG.DNS: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_dns_lib_m);
	lua_setfield(L, -2, "dns");

	luaL_newmetatable(L, "LuaBook.mg_dns");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_dns_lib_m, 0);
	luaL_openlib(L, "mg_dns", mg_dns_lib_f, 0);
	lua_pop(L, 2);
	printf("END MG.DNS: \n"); dumpstack(L);

}