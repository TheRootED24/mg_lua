#include "mg_dns.h"

static int _mg_dns_index(lua_State *L);
static int _mg_dns_new_index(lua_State *L);

int _mg_dns_new (lua_State *L) {
	mg_dns *dns;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		dns = (mg_dns*)lua_touserdata(L, 1);
	}
	else
		dns = (mg_dns*)lua_newuserdata(L, sizeof(mg_dns));

	luaL_getmetatable(L, "LuaBook.mg_dns");
	lua_setmetatable(L, -2);
	if(!dns) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_dns *check_mg_dns(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_dns");
	luaL_argcheck(L, ud != NULL, pos, "`mg_dns' expected");

	return (mg_dns*)ud;
};

static int _mg_dns_newt(lua_State * L) {
	_mg_dns_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_dns_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_dns_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static int _mg_dns_url(lua_State *L) {
	int nargs = lua_gettop(L);
	printf("HERE WE ARE nargs: %d\n", nargs);
	mg_dns *dns = check_mg_dns(L, 1);  // stack { table, key, dns_obj}
	if(nargs > 1) {
		dns->url = luaL_checkstring(L, nargs);
		printf("DNS URL: %s\n", dns->url);
	}
	if(dns->url) printf("DNS URL: %s\n", dns->url);
	 else dns->url = "NO GO ON SITE BRO";
	lua_pushstring(L, dns->url);

	return 1;
};

static int _mg_dns_conn(lua_State *L) {
	mg_dns *dns = check_mg_dns(L, 1);
	lua_pushlightuserdata(L, dns->c);

	return 1;
};

static int _mg_dns_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		//printf("KEY: %s CANNOT BE UPDATED !\n", key);
		//lua_pop(L, 1); // stack: { table }

		if(strcmp(key, "ctx") != 0) {
                	if(key && strcmp(key, "url") == 0 ) {
				printf("HERE NOW\n");
				_mg_dns_url(L); // stack: { table, key }
			}
			else if(key && strcmp(key, "conn") == 0 ) {
				_mg_dns_conn(L);
			}
		}
	}

	return 0;
};

static int _mg_dns_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "url") == 0 ) {
			printf("HERE NOW\n");
			_mg_dns_url(L);
		}
		else if(key && strcmp(key, "conn") == 0 ) {
			_mg_dns_conn(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);
			_mg_dns_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

static const struct luaL_reg mg_dns_lib_f [] = {
	{"ptr", 	_mg_dns_new	},
	{"new", 	_mg_dns_newt	},
	{NULL, NULL}
};

static const struct luaL_reg mg_dns_lib_m [] = {
	{"ptr", 	_mg_dns_new	},
	{"new", 	_mg_dns_newt	},
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
