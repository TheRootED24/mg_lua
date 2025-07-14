#include "mg_fs_file.h"

//struct mg_str mg_file_read(struct mg_fs *fs, const char *path);
static int _mg_file_read(lua_State *L) {
	int type = luaL_checkinteger(L, 1);
	mg_fs *fs = type == 0 ? &mg_fs_posix : &mg_fs_packed;
	const char *path = luaL_checkstring(L, 2);

	if(fs) {
		mg_str s = mg_file_read(fs, path);
		lua_pushlstring(L, s.buf, s.len);

		if(s.buf)
			free(s.buf);
	}

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

// void mg_fs_close(struct mg_fd *fd);
static int _mg_file_close(lua_State *L) {
	mg_fd *fd = check_mg_fd(L, 1);
	mg_fs_close(fd->fd);

	return 0;
};

static int _mg_file_free(lua_State *L) {
	mg_str *str = check_mg_str(L, 1);

	if(str->buf != NULL)
		free(str->buf);

	return 0;
};

static const struct luaL_reg fs_file_lib_f [] = {
	{"read"	,	_mg_file_read	},
	{"write", 	_mg_file_write	},
	{"printf",  	_mg_file_printf	},
	{"close",  	_mg_file_close	},
	{"free",  	_mg_file_free	},
	{NULL, NULL}
};

static const struct luaL_reg fs_file_lib_m [] = {
	{"read"	,	_mg_file_read	},
	{"write", 	_mg_file_write	},
	{"printf",  	_mg_file_printf },
	{"close",  	_mg_file_close	},
	{"free",  	_mg_file_free	},
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
