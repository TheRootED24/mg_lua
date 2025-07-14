#include "mg_ws_message.h"

typedef struct mg_ws_message ws_message;

int new_ws_msg (lua_State *L) {
	ws_message *wm = NULL;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		wm = (ws_message*)lua_topointer(L, 1);
		lua_pop(L, 1);
		lua_pushlightuserdata(L, wm);
	 }
	else
		wm = (ws_message*)lua_newuserdata(L, sizeof(ws_message));

	luaL_getmetatable(L, "LuaBook.ws_message");
	lua_setmetatable(L, -2);
	if(!wm) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

ws_message *check_ws_message (lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.ws_message");
	luaL_argcheck(L, ud != NULL, 1, "`ws_message' expected");
	return(ws_message*)ud;
}


static int _message_data(lua_State *L) {
	int nargs = lua_gettop(L);
	ws_message *msg = check_ws_message(L);
	if(nargs > 1)
		msg->data = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, msg->data.buf, msg->data.len);
	return 1;
}

static int _message_len(lua_State *L) {
	int nargs = lua_gettop(L);
	ws_message *msg = check_ws_message(L);
	if(nargs > 1)
		msg->data.len = luaL_checkinteger(L, -1);

	lua_pushinteger(L, msg->data.len);

	return 1;
}

static int _message_flags(lua_State *L) {
	int nargs = lua_gettop(L);
	ws_message *msg = check_ws_message(L);
	if(nargs > 1)
		msg->flags = luaL_checkinteger(L, -1);

	lua_pushinteger(L, msg->flags);
	return 1;
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

static const struct luaL_reg ws_message_lib_f [] = {
	{"new", 	new_ws_msg	},
	{NULL, NULL}
};

static const struct luaL_reg ws_message_lib_m [] = {
	{"new",			new_ws_msg		},
	{"data",		_message_data	},
	{"len",			_message_len	},
	{"flags",		_message_flags	},
	{NULL, NULL}
};

void  mg_open_mg_ws_message (lua_State *L) {
	printf("START MG.WS.MESSAGE: \n");
	lua_getfield(L, -1, "ws");
	dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, ws_message_lib_m);
	lua_setfield(L, -2, "message");

	// http_message
	luaL_newmetatable(L, "LuaBook.ws_message");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, ws_message_lib_m, 0);
	luaL_openlib(L, "mg_ws_message", ws_message_lib_f, 0);
	lua_pop(L, 2);

	printf("END MG.WS.MESSAGE:\n"); dumpstack(L);

}