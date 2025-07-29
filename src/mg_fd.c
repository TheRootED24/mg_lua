#include "mg_fd.h"

int new_mg_fd (lua_State *L) {
	mg_fd *fd = (mg_fd*)lua_newuserdata(L, sizeof(mg_fd));
	luaL_getmetatable(L, "LuaBook.mg_fd");
	lua_setmetatable(L, -2);
	if(!fd) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_fd *check_mg_fd(lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_fd");
	luaL_argcheck(L, ud != NULL, pos, "`mg_fd' expected");

	return (mg_fd*)ud;
};

static const struct luaL_reg mg_fd_lib_f [] = {
	{"new", 	new_mg_fd	},
	{NULL, NULL}
};

static const struct luaL_reg mg_fd_lib_m [] = {
	{"new",		new_mg_fd	},
	{NULL, NULL}
};

void mg_open_mg_fd (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_fd_lib_m);
	lua_setfield(L, -2, "fd");
	// mg_fd
	luaL_newmetatable(L, "LuaBook.mg_fd");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_fd_lib_m, 0);
	luaL_openlib(L, "mg_fd", mg_fd_lib_f, 0);
	lua_pop(L, 2);
};
