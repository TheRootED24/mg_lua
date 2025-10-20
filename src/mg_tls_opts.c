#include "mg_tls_opts.h"

/*
struct mg_tls_opts {
  struct mg_str ca;    // CA certificate; for both listeners and clients. PEM or DER
  struct mg_str cert;  // Certificate. PEM or DER
  struct mg_str key;   // Private key. PEM or DER
  struct mg_str name;  // If not empty, enable server name verification
};
*/

int _mg_tls_opts_new (lua_State *L) {
	tls_opts *opts = (tls_opts*)lua_newuserdata(L, sizeof(tls_opts));
	memset(opts, 0, sizeof(tls_opts));

	luaL_getmetatable(L, "LuaBook.tls_opts");
	lua_setmetatable(L, -2);
	if(!opts) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

tls_opts *check_mg_tls_opts(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.tls_opts");
	luaL_argcheck(L, ud != NULL, pos, "`tls_opts' expected");

	return(tls_opts*)ud;
};

static int _mg_tls_opts_ca(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_mg_tls_opts(L, 1);
	if(nargs > 1) {
		opts->ca = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->ca.buf, opts->ca.len);

	return 1;
};

static int _mg_tls_opts_cert(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_mg_tls_opts(L, 1);
	if(nargs > 1) {
		opts->cert = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->cert.buf, opts->cert.len);

	return 1;
};

static int _mg_tls_opts_key(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_mg_tls_opts(L, 1);
	if(nargs > 1) {
		opts->key = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->key.buf, opts->key.len);

	return 1;
};

static int _mg_tls_opts_name(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_mg_tls_opts(L, 1);
	if(nargs > 1) {
		opts->name = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->name.buf, opts->name.len);

	return 1;
};

static int _mg_tls_opts_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "ca") == 0 ) {
				_mg_tls_opts_ca(L);
			}
			else if(key && strcmp(key, "cert") == 0 ) {
				_mg_tls_opts_cert(L);
			}
			else if(key && strcmp(key, "key") == 0 ) {
				_mg_tls_opts_key(L);
			}
			else if(key && strcmp(key, "name") == 0 ) {
				_mg_tls_opts_name(L);
			}
		}
	}

	return 0;
};

static int _mg_tls_opts_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "ca") == 0 ) {
			_mg_tls_opts_ca(L);
		}
		else if(key && strcmp(key, "cert") == 0 ) {
			_mg_tls_opts_cert(L);
		}
		else if(key && strcmp(key, "key") == 0 ) {
			_mg_tls_opts_key(L);
		}
		else if(key && strcmp(key, "name") == 0 ) {
			_mg_tls_opts_name(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_tls_opts_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_tls_opts_newt(lua_State * L) {
	_mg_tls_opts_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_tls_opts_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_tls_opts_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg tls_opts_lib_f [] = {
	{"new", 	_mg_tls_opts_newt	},
	{NULL, NULL}
};

static const struct luaL_reg tls_opts_lib_m [] = {
	{"new", 	_mg_tls_opts_newt	},
	{"ca",		_mg_tls_opts_ca		},
	{"cert",	_mg_tls_opts_cert	},
	{"key",		_mg_tls_opts_key	},
	{"name",	_mg_tls_opts_name	},
	{NULL, NULL}
};

void  mg_open_mg_tls_opts (lua_State *L) {
	lua_getfield(L, -1, "tls");
	lua_newtable(L);
	luaL_register(L, NULL, tls_opts_lib_m);
	lua_setfield(L, -2, "opts");
	// mg_tls_opts
	luaL_newmetatable(L, "LuaBook.tls_opts");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, tls_opts_lib_m, 0);
	luaL_openlib(L, "mg_tls_opts", tls_opts_lib_f, 0);
	lua_pop(L, 2);
};
