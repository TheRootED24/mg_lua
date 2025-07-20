#include "mg_http_serve_opts.h"

// **** MG_HTTP_SERVE_OPTS USERDATA ****//
/*
struct mg_http_serve_opts {
  const char *root_dir;       // Web root directory, must be non-NULL
  const char *ssi_pattern;    // SSI file name pattern, e.g. #.shtml
  const char *extra_headers;  // Extra HTTP headers to add in responses
  const char *mime_types;     // Extra mime types, ext1=type1,ext2=type2,..
  const char *page404;        // Path to the 404 page, or NULL by default
  struct mg_fs *fs;           // Filesystem implementation. Use NULL for POSIX
};
*/

int newopts (lua_State *L) {
	http_serve_opts *opts = NULL;
	const char *root_dir 		= luaL_optstring(L, 1, ".");
	//const char *ssi_pattern 	= NULL; //luaL_optstring(L, 2, NULL);
	//const char *extra_headers 	= NULL; //luaL_optstring(L, 3, NULL);
	//const char *mime_types 		= NULL; //luaL_optstring(L, 4, NULL);
	//const char *page404 		= NULL; //luaL_optstring(L, 5, NULL);

	opts = (http_serve_opts *)lua_newuserdata(L, sizeof(http_serve_opts));
	memset(opts, 0, sizeof(http_serve_opts));
	luaL_getmetatable(L, "LuaBook.http_serve_opts");
	lua_setmetatable(L, -2);

	opts->root_dir 		= root_dir;

	if(!opts) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

http_serve_opts *checkopts(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.http_serve_opts");
	luaL_argcheck(L, ud != NULL, 1, "`http_serve_opts' expected");
	return (http_serve_opts*)ud;

};

static int _opts_root_dir(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 1);
	if(nargs > 1)
		opts->root_dir = luaL_checkstring(L, -1);

	lua_pushstring(L, opts->root_dir);
	return 1;
};

static int _opts_ssi_pattern(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 1);
	if(nargs > 1)
		opts->ssi_pattern = luaL_checkstring(L, -1);

	lua_pushstring(L, opts->ssi_pattern);
	return 1;
};

static int _opts_extra_headers(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 1);
	if(nargs > 1)
		opts->extra_headers = luaL_checkstring(L, -1);

	lua_pushstring(L, opts->extra_headers);
	return 1;
};

static int _opts_mime_types(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 1);
	if(nargs > 1)
		opts->mime_types= luaL_checkstring(L, -1);

	lua_pushstring(L, opts->mime_types);
	return 1;
};

static int _opts_page404(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 1);
	if(nargs > 1)
		opts->page404 = luaL_checkstring(L, -1);

	lua_pushstring(L, opts->page404);
	return 1;
};

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

static int _opts_fs(lua_State *L) {
	//int nargs = lua_gettop(L);
	http_serve_opts *opts = checkopts(L);
	if(lua_islightuserdata(L, -1))
		opts->fs = (void*)lua_topointer(L, -1);

	lua_pushlightuserdata(L, opts->fs);
	return 1;
};

static const struct luaL_reg http_opts_lib_f [] = {
	{"new", newopts },
	{NULL, NULL}
};

static const struct luaL_reg http_opts_lib_m [] = {
	//{"__tostring",	_print_header	},
	//{"__call",		_call_header	},
	{"new", 			newopts 			},
	{"root_dir", 		_opts_root_dir		},
	{"ssi_pattern", 	_opts_ssi_pattern	},
	{"extra_headers", 	_opts_extra_headers	},
	{"mime_types", 		_opts_mime_types	},
	{"page404", 		_opts_page404		},
	{"fs", 				_opts_fs			},
	{NULL, NULL}
};

void mg_open_mg_http_serve_opts(lua_State *L) {
	//printf("START MG.HTTP.OPTS: \n");
	//dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, http_opts_lib_m);
	lua_setfield(L, -2, "serve_opts");
	// mg_http_serve_opts
	luaL_newmetatable(L, "LuaBook.http_opts");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, http_opts_lib_m, 0);
	luaL_openlib(L, "mg_http_opts", http_opts_lib_f, 0);
	lua_pop(L, 2);

	//printf("END MG.HTTP.OPTS: \n"); dumpstack(L);
}
