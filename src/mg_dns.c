#include "mg_dns.h"

int _mg_dns_new (lua_State *L) {
	mg_dns *dns;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		dns = (mg_dns*)lua_touserdata(L, 1);
		lua_pushlightuserdata(L, dns);
	}
	else
		dns = (mg_dns*)lua_newuserdata(L, sizeof(mg_dns));

	luaL_getmetatable(L, "LuaBook.mg_dns");
	lua_setmetatable(L, -2);
	if(!dns) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_dns *check_mg_dns(lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_dns");
	luaL_argcheck(L, ud != NULL, pos, "`mg_dns' expected");

	return (mg_dns*)ud;
};

static int _mg_dns_url(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_dns *dns = check_mg_dns(L, 1);
	if(nargs > 1)
		dns->url = luaL_checkstring(L, -1);

	lua_pushstring(L, dns->url);

	return 1;
};

static int _mg_dns_conn(lua_State *L) {
	mg_dns *dns = check_mg_dns(L, 1);
	lua_pushlightuserdata(L, dns->c);

	return 1;
};

static const struct luaL_reg mg_dns_lib_f [] = {
	{"new", 	_mg_dns_new	},
	{NULL, NULL}
};

static const struct luaL_reg mg_dns_lib_m [] = {
	{"new", 	_mg_dns_new	},
	{"url",		_mg_dns_url	},
	{"conn",	_mg_dns_conn	},
	{NULL, NULL}
};

void mg_open_mg_dns (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_dns_lib_m);
	lua_setfield(L, -2, "dns");
	// mg_dns
	luaL_newmetatable(L, "LuaBook.mg_dns");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_dns_lib_m, 0);
	luaL_openlib(L, "mg_dns", mg_dns_lib_f, 0);
	lua_pop(L, 2);
};
