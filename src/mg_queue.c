#include "mg_queue.h"

int new_mg_queue (lua_State *L) {
	mg_queue *q = (mg_queue*)lua_newuserdata(L, sizeof(mg_queue));
	luaL_getmetatable(L, "LuaBook.mg_queue");
	lua_setmetatable(L, -2);
	if(!q) lua_pushnil(L);

	return 1 ;  /* new userdatum is already on the stack */
}

mg_queue*check_mg_queue (lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_queue");
	luaL_argcheck(L, ud != NULL, 1, "`mg_queue' expected");

	return(mg_queue*)ud;
}

// void mg_queue_init(struct mg_queue *q, char *buf, size_t size);
static int _mg_queue_init(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	char *buf = (char*) luaL_checkstring(L, 2);
	size_t size = (size_t)luaL_checknumber(L, 3);
	mg_queue_init(q, buf, size);

	return 0;
}

// size_t mg_queue_book(struct mg_queue *q, char **ptr, size_t len);
static int _mg_queue_book(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	size_t len = (size_t)luaL_checknumber(L, 2);

	char *ptr;
	size_t ret = mg_queue_book(q, &ptr, len);
	if(ret < len || !ptr) {
		lua_pushnil(L);
		lua_pushinteger(L, ret);
	}
	else {
		lua_pushlightuserdata(L, ptr);
		lua_pushinteger(L, ret);
	}

	return 2;
}

// void mg_queue_add(struct mg_queue *q, size_t len);
static int _mg_queue_add(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	size_t len = (size_t)luaL_checkinteger(L, 2);

	mg_queue_add(q, len);

	return 0;
}

// size_t mg_queue_next(struct mg_queue *q, char **ptr);
static int _mg_queue_next(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	char *ptr;
	size_t ret;
	if((ret = mg_queue_next(q, &ptr) > 0)) {
		lua_pushlightuserdata(L, ptr);
		lua_pushinteger(L, ret);
	}
	else {
		lua_pushnil(L);
		lua_pushinteger(L, ret);
	}

	return 2;
}

// void mg_queue_del(struct mg_queue *q, size_t len);
static int _mg_queue_del(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	size_t len = (size_t)luaL_checkinteger(L, 2);

	mg_queue_del(q, len);

	return 0;
}

// size_t mg_queue_vprintf(struct mg_queue *q, const char *fmt, va_list *);
static int _mg_queue_vprintf(lua_State *L) {
	if(L) return 0;

	return 0;
}

// size_t mg_queue_printf(struct mg_queue *q, const char *fmt, ...);
static int _mg_queue_printf(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	const char *fmt = luaL_checkstring(L, 2);
	const char *argstr = luaL_checkstring(L, 3);

	size_t ret = mg_queue_printf(q, fmt, argstr);
	lua_pushinteger(L, ret);

	return 1;
}

static const struct luaL_reg mg_queue_lib_f [] = {
	{"new", 	new_mg_queue	},
	{"new", 	new_mg_queue		},
	{"init",	_mg_queue_init		},
	{"book",	_mg_queue_book		},
	{"add",		_mg_queue_add		},
	{"next", 	_mg_queue_next		},
	{"del",		_mg_queue_del		},
	{"printf",	_mg_queue_printf	},
	{"vprintf",	_mg_queue_vprintf	},
	{NULL, NULL}
};

static const struct luaL_reg mg_queue_lib_m [] = {
	{"new", 	new_mg_queue		},
	{"init",	_mg_queue_init		},
	{"book",	_mg_queue_book		},
	{"add",		_mg_queue_add		},
	{"next", 	_mg_queue_next		},
	{"del",		_mg_queue_del		},
	{"printf",	_mg_queue_printf	},
	{"vprintf",	_mg_queue_vprintf	},
	{NULL, NULL}
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

void mg_open_mg_queue (lua_State *L) {
	//printf("START MG.QUEUE: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_queue_lib_m);
	//dumpstack(L);
	lua_setfield(L, -2, "queue");
	// mg_queue
	luaL_newmetatable(L, "LuaBook.mg_queue");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_queue_lib_m, 0);
	luaL_openlib(L, "mg_mgr", mg_queue_lib_f, 0);
	lua_pop(L, 2);
	//printf("END MG.QUEUE: \n"); dumpstack(L);
}