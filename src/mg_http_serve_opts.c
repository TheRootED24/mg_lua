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

int _mg_http_serve_opts_new (lua_State *L) {
	http_serve_opts *opts = NULL;
	const char *root_dir = luaL_optstring(L, 1, ".");

	opts = (http_serve_opts*)lua_newuserdata(L, sizeof(http_serve_opts));
	memset(opts, 0 , sizeof(http_serve_opts));
	
	luaL_getmetatable(L, "LuaBook.http_serve_opts");
	lua_setmetatable(L, -2);

	if(opts)
		opts->root_dir = root_dir;
	else
		lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

http_serve_opts *check_mg_http_serve_opts(lua_State *L, int pos) {
	if(lua_istable(L, pos)) {
		lua_getfield(L, pos, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.http_serve_opts");
	luaL_argcheck(L, ud != NULL, pos, "`mg_http_serve_opts' expected");

	return (http_serve_opts*)ud;
};

static int _opts_root_dir(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = check_mg_http_serve_opts(L, 1);

	if(nargs > 1) {
		opts->root_dir = luaL_checkstring(L, 2);

		return 0;
	}

	lua_pushstring(L, opts->root_dir);

	return 1;
};

static int _opts_ssi_pattern(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = check_mg_http_serve_opts(L, 1);

	if(nargs > 1) {
		opts->ssi_pattern = luaL_checkstring(L, 2);

		return 0;
	}

	lua_pushstring(L, opts->ssi_pattern);

	return 1;
};

static int _opts_extra_headers(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = check_mg_http_serve_opts(L, 1);

	if(nargs > 1) {
		opts->extra_headers = luaL_checkstring(L, 2);

		return 0;
	}

	lua_pushstring(L, opts->extra_headers);

	return 1;
};

static int _opts_mime_types(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = check_mg_http_serve_opts(L, 1);

	if(nargs > 1) {
		opts->mime_types= luaL_checkstring(L, 2);

		return 0;
	}

	lua_pushstring(L, opts->mime_types);

	return 1;
};

static int _opts_page404(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = check_mg_http_serve_opts(L, 1);

	if(nargs > 1) {
		opts->page404 = luaL_checkstring(L, 2);

		return 0;
	}

	lua_pushstring(L, opts->page404);

	return 1;
};

static int _opts_fs(lua_State *L) {
	int nargs = lua_gettop(L);
	http_serve_opts *opts = check_mg_http_serve_opts(L, 1);

	if(nargs > 1 && lua_islightuserdata(L, 2)) {
		opts->fs = (void*)lua_topointer(L, 2);

		return 0;
	}

	lua_pushlightuserdata(L, opts->fs);

	return 1;
};

static int _mg_http_serve_opts_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "root_dir") == 0 ) {
				_opts_root_dir(L);
			}
			else if(key && strcmp(key, "ssi_pattern") == 0 ) {
				_opts_ssi_pattern(L);
			}
			else if(key && strcmp(key, "extra_headers") == 0 ) {
				_opts_extra_headers(L);
			}
			else if(key && strcmp(key, "mime_types") == 0 ) {
				_opts_mime_types(L);
			}
			else if(key && strcmp(key, "page404") == 0 ) {
				_opts_page404(L);
			}
			else if(key && strcmp(key, "fs") == 0 ) {
				_opts_fs(L);
			}
		}
	}

	return 0;
};

static int _mg_http_serve_opts_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "root_dir") == 0 ) {
			_opts_root_dir(L);
		}
		else if(key && strcmp(key, "ssi_pattern") == 0 ) {
			_opts_ssi_pattern(L);
		}
		else if(key && strcmp(key, "extra_headers") == 0 ) {
			_opts_extra_headers(L);
		}
		else if(key && strcmp(key, "mime_types") == 0 ) {
			_opts_mime_types(L);
		}
		else if(key && strcmp(key, "page404") == 0 ) {
			_opts_page404(L);
		}
		else if(key && strcmp(key, "fs") == 0 ) {
			_opts_fs(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);
			_mg_http_serve_opts_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_http_serve_opts_newt(lua_State * L) {
	_mg_http_serve_opts_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_http_serve_opts_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_http_serve_opts_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg http_serve_opts_lib_f [] = {
	{"ptr",		_mg_http_serve_opts_new		},
	{"new", 	_mg_http_serve_opts_newt	},
	{NULL, NULL}
};

static const struct luaL_reg http_serve_opts_lib_m [] = {

	{"ptr", 		_mg_http_serve_opts_new },
	{"new", 		_mg_http_serve_opts_newt},
	{"root_dir", 		_opts_root_dir		},
	{"ssi_pattern", 	_opts_ssi_pattern	},
	{"extra_headers", 	_opts_extra_headers	},
	{"mime_types", 		_opts_mime_types	},
	{"page404", 		_opts_page404		},
	{"fs", 			_opts_fs		},
	{NULL, NULL}
};

void mg_open_mg_http_serve_opts(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, http_serve_opts_lib_m);
	lua_setfield(L, -2, "serve_opts");
	// mg_http_serve_opts
	luaL_newmetatable(L, "LuaBook.http_serve_opts");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, http_serve_opts_lib_m, 0);
	luaL_openlib(L, "http_serve_opts", http_serve_opts_lib_f, 0);
	lua_pop(L, 2);
};
