#include "mg_fs_file.h"

//struct mg_str mg_file_read(struct mg_fs *fs, const char *path);
static int _mg_file_read(lua_State *L) {
	int type = luaL_checkinteger(L, 1);
	mg_fs *fs = type == 0 ? &mg_fs_posix : &mg_fs_packed;
	const char *path = luaL_checkstring(L, 2);

	if(fs) {
		struct mg_str file = mg_file_read(fs, path);
		if(file.len > 1)
			lua_pushlstring(L, file.buf, file.len);
	}
	else
		lua_pushnil(L);

	return 1;
};

// bool mg_file_write(struct mg_fs *fs, const char *path, const void *buf, size_t len);
static int _mg_file_write(lua_State *L) {
	int type = luaL_checkinteger(L, 1);
	mg_fs *fs = type == 0 ? &mg_fs_posix : &mg_fs_packed;
	const char *path = luaL_checkstring(L, 2);
	const void *buf = (const void*)luaL_checkstring(L, 3);
	size_t len = luaL_checkinteger(L, 4);

	if(fs)
		lua_pushboolean(L, mg_file_write(fs, path, buf, len));
	else
		lua_pushboolean(L, false);

	return 1;
};

// bool mg_file_printf(struct mg_fs *fs, const char *path, const char *fmt, ...);
static int _mg_file_printf(lua_State *L) {
	int type = luaL_checkinteger(L, 1);
	mg_fs *fs = type == 0 ? &mg_fs_posix : &mg_fs_packed;
	const char *path = luaL_checkstring(L, 2);
	const char *fmt = luaL_checkstring(L, 3);
	const char *str = luaL_checkstring(L, 4);

	if(fs)
		lua_pushboolean(L, mg_file_printf(fs, path, fmt, str));
	else
		lua_pushboolean(L, false);

	return 1;
};

static const struct luaL_reg fs_file_lib_f [] = {
	{"read"	,	_mg_file_read	},
	{"write", 	_mg_file_write	},
	{"printf",  	_mg_file_printf	},
	{NULL, NULL}
};

static const struct luaL_reg fs_file_lib_m [] = {
	{"read"	,	_mg_file_read	},
	{"write", 	_mg_file_write	},
	{"printf",  	_mg_file_printf },
	{NULL, NULL}
};

void  mg_open_mg_fs_file (lua_State *L) {
	lua_getfield(L, -1, "fs");
	lua_newtable(L);
	luaL_register(L, NULL, fs_file_lib_m);
	lua_setfield(L, -2, "file");
	// mg_fs_file
	luaL_newmetatable(L, "LuaBook.fs_file");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, fs_file_lib_m, 0);
	luaL_openlib(L, "mg_fs_file", fs_file_lib_f, 0);
	lua_pop(L, 2);
};
