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
}

static int _mg_fs_close(lua_State *L) {
	mg_fd *fd = check_mg_fd(L);
	if(fd)
		mg_fs_close(fd);

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

static const struct luaL_reg mg_fs_lib_m [] = {
	{"open"	,	_mg_fs_open	},
	{"close", 	_mg_fs_close	},
	{NULL, NULL}
};

void mg_open_mg_fs(lua_State *L) {
	//printf("START MG.FS: \n"); dumpstack(L);
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
	//printf("END MG.FS: \n"); dumpstack(L);
}
