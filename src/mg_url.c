#include "mg_url.h"

static int _mg_url_port(lua_State *L) {
	unsigned short url = mg_url_port(luaL_checkstring(L, 1));
	lua_pushinteger(L, url);

	return 1;
}

static int _mg_url_is_ssl(lua_State *L) {
	int url = mg_url_is_ssl(luaL_checkstring(L, 1));
	lua_pushinteger(L, url);

	return 1;
}

static int _mg_url_host(lua_State *L) {
	struct mg_str url = mg_url_host(luaL_checkstring(L, 1));
	if(url.len > 0)
		lua_pushlstring(L, url.buf, url.len);
	else
		lua_pushnil(L);

	return 1;
}

static int _mg_url_user(lua_State *L) {
	struct mg_str url = mg_url_user(luaL_checkstring(L, 1));
	if(url.len > 0)
		lua_pushlstring(L, url.buf, url.len);
	else
		lua_pushnil(L);

	return 1;
}

static int _mg_url_pass(lua_State *L) {
	struct mg_str url = mg_url_pass(luaL_checkstring(L, 1));
	if(url.len > 0)
		lua_pushlstring(L, url.buf, url.len);
	else
		lua_pushnil(L);

	return 1;
}

static int _mg_url_uri(lua_State *L) {
	lua_pushstring(L, mg_url_uri(luaL_checkstring(L, 1)));

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

static const struct luaL_reg mg_url_lib_m [] = {
	{"port",		_mg_url_port	},
	{"is_ssl",		_mg_url_is_ssl	},
	{"host",		_mg_url_host	},
	{"user",		_mg_url_user	},
	{"pass",		_mg_url_pass	},
	{"uri",			_mg_url_uri	},
	{NULL, NULL}
};

void mg_open_mg_url(lua_State *L) {
	//printf("START MG.URL: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_url_lib_m);
	lua_setfield(L, -2, "url");
	// mg_url
	luaL_newmetatable(L, "LuaBook.mg_url");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	//printf("END MG.URL: \n"); dumpstack(L);
}