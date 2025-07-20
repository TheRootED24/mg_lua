#include "mg_tls_opts.h"

/*
struct mg_tls_opts {
  struct mg_str ca;    // CA certificate; for both listeners and clients. PEM or DER
  struct mg_str cert;  // Certificate. PEM or DER
  struct mg_str key;   // Private key. PEM or DER
  struct mg_str name;  // If not empty, enable server name verification
};
*/

int new_tls_opts (lua_State *L) {
	tls_opts *opts = (tls_opts*)lua_newuserdata(L, sizeof(tls_opts));

	luaL_getmetatable(L, "LuaBook.tls_opts");
	lua_setmetatable(L, -2);
	if(!opts) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

tls_opts *check_tls_opts(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.tls_opts");
	luaL_argcheck(L, ud != NULL, 1, "`tls_opts' expected");

	return(tls_opts*)ud;
}

static int _mg_tls_opts_ca(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_tls_opts(L);
	if(nargs > 1)
		opts->ca = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, opts->ca.buf, opts->ca.len);

	return 1;
}

static int _mg_tls_opts_cert(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_tls_opts(L);
	if(nargs > 1)
		opts->cert = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, opts->cert.buf, opts->cert.len);

	return 1;
}

static int _mg_tls_opts_key(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_tls_opts(L);
	if(nargs > 1)
		opts->key = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, opts->key.buf, opts->key.len);

	return 1;
}

static int _mg_tls_opts_name(lua_State *L) {
	int nargs = lua_gettop(L);
	tls_opts *opts = check_tls_opts(L);
	if(nargs > 1)
		opts->name = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, opts->name.buf, opts->name.len);

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

static const struct luaL_reg tls_opts_lib_f [] = {
	{"new", 	new_tls_opts	},
	{NULL, NULL}
};

static const struct luaL_reg tls_opts_lib_m [] = {
	{"new", 	new_tls_opts		},
	{"ca",		_mg_tls_opts_ca		},
	{"cert",	_mg_tls_opts_cert	},
	{"key",		_mg_tls_opts_key	},
	{"name",	_mg_tls_opts_name	},
	{NULL, NULL}
};

void  mg_open_mg_tls_opts (lua_State *L) {
	//printf("START MG.TLS.OPTS: \n");
	lua_getfield(L, -1, "tls");
	//dumpstack(L);
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
	//printf("END MG.TL.OPTS:\n"); dumpstack(L);
}