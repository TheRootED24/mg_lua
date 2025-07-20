#include "mg_sntp.h"

// struct mg_connection *mg_sntp_connect(struct mg_mgr *mgr, const char *url, mg_event_handler_t fn, void *fn_data)
static int _mg_sntp_connect(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	mg_event_handler_t fn = (mg_event_handler_t)fn_serv;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_sntp_connect(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	//mg_connection *conn = 
	checkconn(L); // check conn is ready
	return 1; // return the udata on the stack
}

// void mg_sntp_request(struct mg_connection *c)
static int _mg_sntp_request(lua_State *L) {
	mg_connection *conn = checkconn(L);
	mg_sntp_request(conn);
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

static const struct luaL_reg mg_sntp_lib_m [] = {
	{"connect",	_mg_sntp_connect		},
	{"request",	_mg_sntp_request		},
	{NULL, NULL}
};

void mg_open_mg_sntp(lua_State *L) {
	//printf("START MG.SNTP: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_sntp_lib_m);
	lua_setfield(L, -2, "sntp");
	// mg_sntp
	luaL_newmetatable(L, "LuaBook.mg_sntp");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	//printf("END MG.SNTP: \n"); dumpstack(L);
}