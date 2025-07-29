#include "mg_fs.h"

static int _mg_fs_open(lua_State *L) {
	int fsys = luaL_checkinteger(L, 1);
	const char *path = luaL_checkstring(L, 2);
	int flags = luaL_checkinteger(L, 3);
	lua_pop(L, 3);

	mg_fs *fs = fsys == 0 ? &mg_fs_posix : &mg_fs_packed;
	mg_fd *fd = mg_fs_open(fs, path, flags);

	if(fd)
		lua_pushlightuserdata(L, fd);
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_fs_close(lua_State *L) {
	mg_fd *fd = check_mg_fd(L, 1);
	if(fd)
		mg_fs_close(fd);

	return 0;
};

static const struct luaL_reg mg_fs_lib_m [] = {
	{"open"	,	_mg_fs_open	},
	{"close", 	_mg_fs_close	},
	{NULL, NULL}
};

void mg_open_mg_fs(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_fs_lib_m);
	lua_setfield(L, -2, "fs");
	// mg_fs
	luaL_newmetatable(L, "LuaBook.mg_fs");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);

	mg_open_mg_fs_file(L);
	lua_pop(L, 1);
};
