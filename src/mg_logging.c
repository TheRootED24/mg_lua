#include "mg_logging.h"

static int _error(lua_State *L) {
	const char *args = luaL_checkstring(L, 1);
	MG_ERROR((args));

	return 0;
}

static int _info(lua_State *L) {
	const char *args = luaL_checkstring(L, 1);
	MG_INFO((args));

	return 0;
}

static int _debug(lua_State *L) {
	const char *args = luaL_checkstring(L, 1);
	MG_DEBUG((args));

	return 0;
}

static int _verbose(lua_State *L) {
	const char *args = luaL_checkstring(L, 1);
	MG_VERBOSE((args));

	return 0;
}

static int _mg_log_set(lua_State *L) {
	int level = luaL_checkinteger(L, 1);
	mg_log_set(level);

	return 0;
}

static int _hexdump(lua_State *L) {
	const void *buf = (void*)lua_topointer(L, 1);
	int length = luaL_checkinteger(L, 2);
	mg_hexdump(buf, length);

	return 0;
}

static int _mg_log_set_fn(lua_State *L) {
	mg_pfn_t lg_fn = (mg_pfn_t)lua_topointer(L, 1);
	void *param = (void*)lua_topointer(L, 2);
	mg_log_set_fn(lg_fn, param);

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

static const struct luaL_reg mg_log_lib_m [] = {
	{"error",		_error		},
	{"info",		_info		},
	{"debug", 		_debug		},
	{"verbose",		_verbose	},
	{"log_set", 		_mg_log_set	},
	{"hexdump",		_hexdump	},
	{"log_set_fn",		_mg_log_set_fn	},
	{NULL, NULL}
};

void mg_open_mg_logging (lua_State *L) {
	//printf("START MG.LOGGING: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_log_lib_m);
	lua_setfield(L, -2, "log");
	// mg_log
	luaL_newmetatable(L, "LuaBook.mg_log");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	//printf("END MG.LOGGING \n"); dumpstack(L);
}