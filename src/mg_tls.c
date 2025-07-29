#include "mg_tls.h"

// void mg_tls_init(struct mg_connection *c, const struct mg_tls_opts *);
static int _mg_tls_init(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_remove(L, 1);
	tls_opts *opts = check_tls_opts(L);
	mg_tls_init(conn, opts);

	return 0;
};

static const struct luaL_reg mg_tls_lib_m [] = {
	{"init",	_mg_tls_init	},
	{NULL, NULL}
};

void mg_open_mg_tls (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_tls_lib_m);
	lua_setfield(L, -2, "tls");
	// mg_tls
	luaL_newmetatable(L, "LuaBook.mg_tls");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	// open sub module
	mg_open_mg_tls_opts(L);
	lua_pop(L, 1);
};
