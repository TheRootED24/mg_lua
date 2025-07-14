#include "mg_fs.h"

static int _mg_fs_open(lua_State *L) {
	mg_fd *fd = NULL;
	mg_fs *fs = NULL;
	int fsys = luaL_checkinteger(L, 1);
	const char *path = luaL_checkstring(L, 2);
	int flags = luaL_checkinteger(L, 3);
	lua_pop(L, 3);

	fs = fsys == 0 ? &mg_fs_posix : &mg_fs_packed;

	if(fs)
		fd = mg_fs_open(fs, path, flags);

	if(fd)
		lua_pushlightuserdata(L, fd);
	else
		lua_pushnil(L);

	return 1;
};

// int mg_fs_posix.st(const char *path, size_t *size, time_t *mtime);
static int _mg_fs_stat(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	size_t size = 0;
	time_t mtime = 0;

	int flags = mg_fs_posix.st(path, &size, &mtime);

	lua_newtable(L);
	lua_pushstring(L, path);
	lua_setfield(L, -2, "path");
	lua_pushnumber(L, size);
	lua_setfield(L, -2, "size");
	lua_pushnumber(L, (time_t)mtime);
	lua_setfield(L, -2, "mtime");
	lua_pushinteger(L, flags);
	lua_setfield(L, -2, "flags");

	return 1;
}

static int _mg_fs_close(lua_State *L) {
	mg_fd *fd = check_mg_fd(L, 1);
	if(fd)
		mg_fs_close(fd);

	return 0;
};

static const struct luaL_reg mg_fs_lib_m [] = {
	{"open"	,	_mg_fs_open	},
	{"stat"	,	_mg_fs_stat	},
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
