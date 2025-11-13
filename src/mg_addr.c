#include "mg_addr.h"

/*
struct mg_addr {
  uint8_t ip[16];    // Holds IPv4 or IPv6 address, in network byte order
  uint16_t port;     // TCP or UDP port in network byte order
  uint8_t scope_id;  // IPv6 scope ID
  bool is_ip6;       // True when address is IPv6 address
};
*/

// MG_ADDR USERDATUM
int _mg_addr_new (lua_State *L) {
	mg_addr *addr;
	int nargs = lua_gettop(L);

	if(nargs > 0)
		addr = (mg_addr*)lua_touserdata(L, 1);
	else {
		addr = (mg_addr*)lua_newuserdata(L, sizeof(mg_addr));
		memset(addr, 0 , sizeof(mg_addr));
	}

	luaL_getmetatable(L, "LuaBook.mg_addr");
	lua_setmetatable(L, -2);
	if(!addr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

mg_addr *check_mg_addr(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_addr");
	luaL_argcheck(L, ud != NULL, pos, "`mg_addr' expected");

	return (mg_addr*)ud;
}

static int _addr_ip(lua_State *L) {
	mg_addr *addr = check_mg_addr(L, 1);
	if(addr)
		lua_pushnumber(L, *addr->ip);

	return 1;
}

static int _addr_port(lua_State *L) {
	mg_addr *addr = check_mg_addr(L, 1);
	if(addr)
		lua_pushinteger(L, addr->port);

	return 1;
}

static int _addr_scope_id(lua_State *L) {
	mg_addr *addr = check_mg_addr(L, 1);
	lua_pushinteger(L, addr->scope_id);

	return 1;
}

static int _addr_is_ip6(lua_State *L) {
	mg_addr *addr = check_mg_addr(L, 1);
	lua_pushboolean(L, addr->is_ip6);

	return 1;
}

static int _mg_addr_new_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		printf("KEY: %s CANNOT BE UPDATED !\n", key);
		lua_pop(L, 1);
	}

	return 0;
};

static int _mg_addr_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "ip") == 0 ) {
			_addr_ip(L);
		}
		else if(key && strcmp(key, "port") == 0 ) {
			_addr_port(L);
		}
		else if(key && strcmp(key, "scope") == 0 ) {
			_addr_scope_id(L);
		}
		else if(key && strcmp(key, "is_ip6") == 0 ) {
			_addr_is_ip6(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_addr_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_addr_newt(lua_State * L) {
	_mg_addr_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_addr_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_addr_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg mg_addr_lib_f [] = {
	{"ptr", 	_mg_addr_new	},
	{"new", 	_mg_addr_newt	},
	{NULL, NULL}
};

static const struct luaL_reg mg_addr_lib_m [] = {
	{"ptr", 	_mg_addr_new	},
	{"new", 	_mg_addr_newt	},
	{"ip",		_addr_ip	},
	{"port",	_addr_port	},
	{"scope_id",	_addr_scope_id	},
	{"is_ip6",	_addr_is_ip6	},
	{NULL, NULL}
};

void mg_open_mg_addr(lua_State *L) {
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
}
