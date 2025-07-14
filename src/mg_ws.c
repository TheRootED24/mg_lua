#include "mg_ws.h"

// struct mg_connection *mg_ws_connect(struct mg_mgr *mgr, const char *url, mg_event_handler_t fn, void *fn_data, const char *fmt, ...);
static int _mg_ws_connect(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_mgr *mgr = check_mg_mgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char *cb = luaL_checkstring(L, 3);
	const char *argstr;
	mg_connection *c;

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;
	if(nargs > 3) {
		//fmt = luaL_checkstring(L, 4);
		argstr = luaL_checkstring(L, 4);
		printf("ARGSTR: %s\n", argstr);
		c = mg_ws_connect(mgr, s_url, fn, GL, "%s", argstr);
	}
	else
		c = mg_ws_connect(mgr, s_url, fn, GL, NULL);

	lua_settop(L, 0); // clear the stack

	//c->is_websocket = 1;
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	checkconn(L); // check conn is ready

	return 1; // return the udata on the stack
}

// void mg_ws_upgrade(struct mg_connection *c, struct mg_http_message *, const char *fmt, ...);
static int _mg_ws_upgrade(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	http_message *hm = (http_message*)lua_topointer(L, 2);
	const char *fmt = NULL;
	if(nargs > 2){
		fmt = luaL_checkstring(L, 3);
		mg_ws_upgrade(conn, hm, fmt, luaL_checkstring(L, 4));
	}
	else
		mg_ws_upgrade(conn, hm, fmt);

	//conn->is_websocket = 1;

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
// size_t mg_ws_printf(struct mg_connection *, int op, const char *fmt, ...);

static int _mg_ws_printf(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	int op = luaL_checkinteger(L, 2);
	const char *fmt = luaL_checkstring(L, 3);
	const char *argstr = luaL_checkstring(L, 4);

	size_t ret = mg_ws_printf(conn, op, fmt, argstr);
	lua_pushinteger(L, ret);

	return 1;
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
	size_t len = (size_t)luaL_checkinteger(L, 2);
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