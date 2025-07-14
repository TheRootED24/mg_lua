#include "mg_url.h"

static int _mg_url_port(lua_State *L) {
	unsigned short url = mg_url_port(luaL_checkstring(L, 1));
	lua_pushinteger(L, url);

	return 1;
};

static int _mg_url_is_ssl(lua_State *L) {
	int url = mg_url_is_ssl(luaL_checkstring(L, 1));
	lua_pushinteger(L, url);

	return 1;
};

static int _mg_url_host(lua_State *L) {
	struct mg_str url = mg_url_host(luaL_checkstring(L, 1));

	if(url.len > 0)
		lua_pushlstring(L, url.buf, url.len);
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_url_user(lua_State *L) {
	struct mg_str url = mg_url_user(luaL_checkstring(L, 1));

	if(url.len > 0)
		lua_pushlstring(L, url.buf, url.len);
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_url_pass(lua_State *L) {
	struct mg_str url = mg_url_pass(luaL_checkstring(L, 1));

	if(url.len > 0)
		lua_pushlstring(L, url.buf, url.len);
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_url_uri(lua_State *L) {
	lua_pushstring(L, mg_url_uri(luaL_checkstring(L, 1)));

	return 1;
};

static const struct luaL_reg mg_url_lib_m [] = {
	{"port",	_mg_url_port	},
	{"is_ssl",	_mg_url_is_ssl	},
	{"host",	_mg_url_host	},
	{"user",	_mg_url_user	},
	{"pass",	_mg_url_pass	},
	{"uri",		_mg_url_uri	},
	{NULL, NULL}
};

void mg_open_mg_url(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_url_lib_m);
	lua_setfield(L, -2, "url");
	// mg_url
	luaL_newmetatable(L, "LuaBook.mg_url");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
};
