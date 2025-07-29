#include "mg_queue.h"

int _mg_queue_new (lua_State *L) {
	mg_queue *q = (mg_queue*)lua_newuserdata(L, sizeof(mg_queue));
	luaL_getmetatable(L, "LuaBook.mg_queue");
	lua_setmetatable(L, -2);
	if(!q) lua_pushnil(L);

	return 1 ;  /* new userdatum is already on the stack */
};

mg_queue*check_mg_queue (lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_queue");
	luaL_argcheck(L, ud != NULL, pos, "`mg_queue' expected");

	return(mg_queue*)ud;
};

// void mg_queue_init(struct mg_queue *q, char *buf, size_t size);
static int _mg_queue_init(lua_State *L) {
	mg_queue *q = check_mg_queue(L, 1);
	char *buf = (char*) luaL_checkstring(L, 2);
	size_t size = (size_t)luaL_checknumber(L, 3);
	mg_queue_init(q, buf, size);

	return 0;
};

// size_t mg_queue_book(struct mg_queue *q, char **ptr, size_t len);
static int _mg_queue_book(lua_State *L) {
	mg_queue *q = check_mg_queue(L, 1);
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
};

// void mg_queue_add(struct mg_queue *q, size_t len);
static int _mg_queue_add(lua_State *L) {
	mg_queue *q = check_mg_queue(L, 1);
	size_t len = (size_t)luaL_checkinteger(L, 2);

	mg_queue_add(q, len);

	return 0;
};

// size_t mg_queue_next(struct mg_queue *q, char **ptr);
static int _mg_queue_next(lua_State *L) {
	mg_queue *q = check_mg_queue(L, 1);
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
};

// void mg_queue_del(struct mg_queue *q, size_t len);
static int _mg_queue_del(lua_State *L) {
	mg_queue *q = check_mg_queue(L, 1);
	size_t len = (size_t)luaL_checkinteger(L, 2);

	mg_queue_del(q, len);

	return 0;
};

// size_t mg_queue_vprintf(struct mg_queue *q, const char *fmt, va_list *);
static int _mg_queue_vprintf(lua_State *L) {
	if(L) return 0;

	return 0;
};

// size_t mg_queue_printf(struct mg_queue *q, const char *fmt, ...);
static int _mg_queue_printf(lua_State *L) {
	mg_queue *q = check_mg_queue(L, 1);
	const char *fmt = luaL_checkstring(L, 2);
	const char *argstr = luaL_checkstring(L, 3);

	size_t ret = mg_queue_printf(q, fmt, argstr);
	lua_pushinteger(L, ret);

	return 1;
};

static const struct luaL_reg mg_queue_lib_f [] = {
	{"new", 	_mg_queue_new		},
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
	{"new", 	_mg_queue_new		},
	{"init",	_mg_queue_init		},
	{"book",	_mg_queue_book		},
	{"add",		_mg_queue_add		},
	{"next", 	_mg_queue_next		},
	{"del",		_mg_queue_del		},
	{"printf",	_mg_queue_printf	},
	{"vprintf",	_mg_queue_vprintf	},
	{NULL, NULL}
};

void mg_open_mg_queue (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_queue_lib_m);
	lua_setfield(L, -2, "queue");
	// mg_queue
	luaL_newmetatable(L, "LuaBook.mg_queue");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_queue_lib_m, 0);
	luaL_openlib(L, "mg_queue", mg_queue_lib_f, 0);
	lua_pop(L, 2);
};
