#include"mg_iobuf.h"

// MG_IOBUF USERDATUM
int newiobuf (lua_State *L) {
	mg_iobuf *io = (mg_iobuf*)lua_newuserdata(L, sizeof(mg_iobuf));
	luaL_getmetatable(L, "LuaBook.mg_iobuf");
	lua_setmetatable(L, -2);
	if(!io) lua_pushnil(L);
	
	return 1;  /* new userdatum is already on the stack */
}

mg_iobuf *checkiobuf(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_iobuf");
	luaL_argcheck(L, ud != NULL, 1, "`mg_iobuf' expected");

	return (mg_iobuf*)ud;
}

// int mg_iobuf_init(struct mg_iobuf *io, size_t size, size_t align);
static int _mg_iobuf_init(lua_State *L) {
	mg_iobuf *io = checkiobuf(L);
	bool ok = mg_iobuf_init(io, luaL_checklong(L, 2), luaL_checklong(L, 3));
	if(ok)
		lua_pushboolean(L, ok);

	return ok;
}

// int mg_iobuf_resize(struct mg_iobuf *io, size_t size);
static int _mg_iobuf_resize(lua_State *L) {
	mg_iobuf *io = checkiobuf(L);
	bool ok = (bool) mg_iobuf_resize(io, luaL_checklong(L, 2));
	if(ok)
		lua_pushinteger(L, ok);

	return ok;
}

// size_t mg_iobuf_add(struct mg_iobuf *io, size_t offset, const void *buf, size_t len);
static int _mg_iobuf_add(lua_State *L) {
	mg_iobuf *io = checkiobuf(L);
	size_t wsize = mg_iobuf_add(io, luaL_checklong(L, 2), lua_topointer(L, 3), luaL_checklong(L, 4));
	lua_pushnumber(L, wsize);

	return 1;
}

// size_t mg_iobuf_del(struct mg_iobuf *io, size_t offset, size_t len);
static int _mg_iobuf_del(lua_State *L) {
	mg_iobuf *io = checkiobuf(L);
	size_t wsize = mg_iobuf_del(io, luaL_checklong(L, 2), luaL_checklong(L, 3));
	lua_pushnumber(L, wsize);

	return 1;
}

// void mg_iobuf_free(struct mg_iobuf *io);
static int _mg_iobuf_free(lua_State *L) {
	mg_iobuf *io = checkiobuf(L);
	mg_iobuf_free(io);

	return 0;
}

static void dumpstack (lua_State *L) {
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
}

static const struct luaL_reg mg_iobuf_lib_f [] = {
	{"new", 	newiobuf	},
	{NULL, NULL}
};

static const struct luaL_reg mg_iobuf_lib_m [] = {
	{"init",		_mg_iobuf_init	},
	{"resize",		_mg_iobuf_resize},
	{"add",			_mg_iobuf_add	},
	{"del",			_mg_iobuf_del	},
	{"free",		_mg_iobuf_free	},
	{NULL, NULL}
};

void mg_open_mg_iobuf(lua_State *L) {
	// mg_iobuf
	printf("START MG.IOBUF: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_iobuf_lib_m);
	lua_setfield(L, -2, "iobuf");

	luaL_newmetatable(L, "LuaBook.mg_iobuf");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_iobuf_lib_m, 0);
	luaL_openlib(L, MG_IOBUF, mg_iobuf_lib_f, 0);
	lua_pop(L, 2);
	printf("END MG.IOBUF: \n"); dumpstack(L);
}
