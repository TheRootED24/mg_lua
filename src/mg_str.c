#include "mg_string.h"

int new_mg_str (lua_State *L) {
	int a = 0, nargs = lua_gettop(L);
	char *buf = NULL;

	if(nargs == 1) {
		buf = (char*)luaL_checkstring(L, 1);
		a =strlen(buf);
	} else if(nargs == 2) {
		buf = (char*)luaL_checkstring(L, 1);
		a = luaL_checkinteger(L, 2);
	}

	lua_settop(L, 0);
	struct mg_str *str = (struct mg_str *)lua_newuserdata(L, sizeof(struct mg_str));
	//lua_insert(L, 1);
	luaL_getmetatable(L, "LuaBook.mg_str");
	lua_setmetatable(L, -2);

	if(nargs) {
		str->buf = buf;
		str->len = a;
	}

	if(!str) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

struct mg_str *check_mg_str(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_str");
	luaL_argcheck(L, ud != NULL, 1, "`mg_str' expected");
	return(struct mg_str*)ud;
}

static int _mg_str_buf(lua_State *L) {
	int index = lua_gettop(L);
	struct mg_str *str = check_mg_str(L);
	if(index > 1) {
		str->buf = (char*)luaL_checkstring(L, -1);
		str->len = strlen(str->buf);
	}

	lua_pushlstring(L, str->buf, str->len);
	return 1;
}

static int _mg_str_len(lua_State *L) {
	struct mg_str *str = check_mg_str(L);
	lua_pushinteger(L, str->len);
	return 1;
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

static const struct luaL_reg mg_str_lib_f [] = {
	{"new", 	new_mg_str	},
	{NULL, NULL}
};

static const struct luaL_reg mg_str_lib_m [] = {
	{"new",			new_mg_str	},
	{"buf",			_mg_str_buf },
	{"len",			_mg_str_len	},
	{NULL, NULL}
};

void  mg_open_mg_str (lua_State *L) {
	//printf("START MG.STR: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_str_lib_m);
	lua_setfield(L, -2, "str");

	// mg_str
	luaL_newmetatable(L, "LuaBook.mg_str");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_str_lib_m, 0);
	luaL_openlib(L, "mg_str", mg_str_lib_f, 0);
	lua_pop(L, 2);
	//printf("END MG.STR\n"); dumpstack(L);
}