#include "mg_tls.h"

// void mg_tls_init(struct mg_connection *c, const struct mg_tls_opts *);
static int _mg_tls_init(lua_State *L) {
	mg_connection *conn = checkconn(L);
	lua_remove(L, 1);
	tls_opts *opts = check_tls_opts(L);
	mg_tls_init(conn, opts);
  
	return 0;
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

static const struct luaL_reg mg_tls_lib_m [] = {
	{"init",	_mg_tls_init	},
	{NULL, NULL}
};

void mg_open_mg_tls (lua_State *L) {
	//printf("START MG.TLS: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_tls_lib_m);
	lua_setfield(L, -2, "tls");
	// mg_tls
	luaL_newmetatable(L, "LuaBook.mg_tls");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);

	mg_open_mg_tls_opts(L);
	lua_pop(L, 1);
	//printf("END MG.TLS: \n"); dumpstack(L);
}