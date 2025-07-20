#include "mg_ws.h"

// struct mg_connection *mg_ws_connect(struct mg_mgr *mgr, const char *url, mg_event_handler_t fn, void *fn_data, const char *fmt, ...);
static int _mg_ws_connect(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	mg_event_handler_t fn = (mg_event_handler_t)fn_serv;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_ws_connect(mgr, s_url, fn, GL, NULL);
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	//mg_connection *conn =
	checkconn(L); // check conn is ready
	return 1; // return the udata on the stack
}

// void mg_ws_upgrade(struct mg_connection *c, struct mg_http_message *, const char *fmt, ...);
static int _mg_ws_upgrade(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1); // check conn is ready
	http_message *hm = (http_message*)lua_topointer(L, 2);
	mg_ws_upgrade(conn, hm, NULL);
	return 0;
}

// size_t mg_ws_send(struct mg_connection *c, const void *buf, size_t len, int op);
static int _mg_ws_send(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	const char *buf = lua_tostring(L, 2);
	size_t len = luaL_checkinteger(L, 3);
	int op = luaL_checkinteger(L, 4);
	//printf("buf: %s len: %ld op: %d\n", buf, len, op);
	size_t sent = mg_ws_send(conn, (const void*)buf, len, op);
	lua_pushnumber(L, sent);

	return 1;
}

// TODO VARIADIC ARG HANDLER
static int _mg_ws_printf(lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// TODO VARIADIC ARG HANDLER
static int _mg_ws_vprintf(lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// size_t mg_ws_wrap(struct mg_connection *c, size_t len, int op)
static int _mg_ws_wrap(lua_State *L) {
	mg_connection *conn = checkconn(L);
	size_t len = luaL_checklong(L, 2);
	int op = luaL_checkinteger(L, 3);
	size_t sent = mg_ws_wrap(conn, len, op);
	lua_pushnumber(L, sent);
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

static const struct luaL_reg mg_ws_lib_m [] = {
	{"connect",	_mg_ws_connect	},
	{"upgrade",	_mg_ws_upgrade	},
	{"send",	_mg_ws_send,	},
	{"printf", 	_mg_ws_printf	},
	{"vprintf",	_mg_ws_vprintf	},
	{"wrap",	_mg_ws_wrap	},
	{NULL, NULL}
};

void mg_open_mg_ws(lua_State *L) {
	//printf("START MG.WS: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_ws_lib_m);
	lua_setfield(L, -2, "ws");
	// mg_ws
	luaL_newmetatable(L, "LuaBook.mg_ws");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);

	mg_open_mg_ws_message(L);
	lua_pop(L, 1);
	//printf("END MG.WS: \n"); dumpstack(L);
}