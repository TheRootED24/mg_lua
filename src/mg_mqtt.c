#include "mg_mqtt.h"
// struct mg_connection *mg_mqtt_connect(struct mg_mgr *mgr, const char *url,
					//const struct mg_mqtt_opts *opts, mg_event_handler_t fn, void *fn_data);

static int _mg_mqtt_connect(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	lua_remove(L, 1);
	const char *s_url = luaL_checkstring(L, 1);
	lua_remove(L, 1);
	mqtt_opts *opts = check_mqtt_opts(L);
	mg_event_handler_t fn = (mg_event_handler_t)fn_serv;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_mqtt_connect(mgr, s_url, opts, fn, GL);
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	//mg_connection *conn = 
	checkconn(L); // check conn is ready

	return 1;
}

static int _mg_mqtt_listen(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	const char *s_url = luaL_checkstring(L, 1);
	mg_event_handler_t fn = (mg_event_handler_t)fn_serv;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_mqtt_listen(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	//mg_connection *conn = 
	checkconn(L); // check conn is ready

	return 1;
}

// void mg_mqtt_login(struct mg_connection *c, const struct mg_mqtt_opts *opts);
static int _mg_mqtt_login(lua_State *L) {
	mg_connection *conn = checkconn(L);
	lua_remove(L, 1);
	mqtt_opts *opts = check_mqtt_opts(L);
	mg_mqtt_login(conn, opts);
	return 0;
}

// uint16_t mg_mqtt_pub(struct mg_connection *c, const struct mg_mqtt_opts *opts);
static int _mg_mqtt_pub(lua_State *L) {
	mg_connection *conn = checkconn(L);
	lua_remove(L, 1);
	mqtt_opts *opts = check_mqtt_opts(L);
	lua_pushinteger(L, mg_mqtt_pub(conn, opts));
	return 1;
}

// void mg_mqtt_sub(struct mg_connection *, const struct mg_mqtt_opts *opts);
static int _mg_mqtt_sub(lua_State *L) {
	mg_connection *conn = checkconn(L);
	lua_remove(L, 1);
	mqtt_opts *opts = check_mqtt_opts(L);
	mg_mqtt_sub(conn, opts);
	return 0;
}

// void mg_mqtt_send_header(struct mg_connection *c, uint8_t cmd, uint8_t flags, uint32_t len);
static int _mg_mqtt_send_header(lua_State *L) {
	mg_connection *conn = checkconn(L);
	uint8_t cmd = luaL_checkinteger(L, 2);
	uint8_t flags = luaL_checkinteger(L, 2);
	uint32_t len = luaL_checkint32(L, 2);
	mg_mqtt_send_header(conn, cmd, flags, len);
	return 0;
}

// void mg_mqtt_ping(struct mg_connection *c);
static int _mg_mqtt_ping(lua_State *L) {
	mg_connection *conn = checkconn(L);
	mg_mqtt_ping(conn);
	return 0;
}

// int mg_mqtt_parse(const uint8_t *buf, size_t len, uint8_t version, struct mg_mqtt_message *m)
static int _mg_mqtt_parse(lua_State *L) {
	mqtt_message *msg = check_mqtt_message(L);
	const uint8_t *buf = (uint8_t*)lua_topointer(L, 2);
	size_t len = luaL_checklong(L, 3);
	uint8_t ver = luaL_checkint(L, 4);
	lua_pushinteger(L, mg_mqtt_parse(buf, len, ver, msg));
	return 1;
}

// void mg_mqtt_disconnect(struct mg_connection *c, const struct mg_mqtt_opts *opts);
static int _mg_mqtt_disconnect(lua_State *L) {
	mg_connection *conn = checkconn(L);
	lua_remove(L, 1);
	mqtt_opts *opts = check_mqtt_opts(L);
	mg_mqtt_disconnect(conn, opts);
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

static const struct luaL_reg mg_mqtt_lib_f [] = {
	{"connect",			_mg_mqtt_connect		},
	{"listen",			_mg_mqtt_listen			},
	{NULL, NULL}
};

static const struct luaL_reg mg_mqtt_lib_m [] = {
	{"listen",			_mg_mqtt_listen			},
	{"connect",			_mg_mqtt_connect		},
	{"login",			_mg_mqtt_login			},
	{"pub", 			_mg_mqtt_pub			},
	{"sub", 			_mg_mqtt_sub			},
	{"send_header",		_mg_mqtt_send_header	},
	{"ping",			_mg_mqtt_ping			},
	{"parse",			_mg_mqtt_parse			},
	{"disconnect", 		_mg_mqtt_disconnect		},
	{NULL, NULL}
};

void mg_open_mg_mqtt(lua_State *L) {
	//printf("START MG.MQTT: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_mqtt_lib_m);
	lua_setfield(L, -2, "mqtt");
	// mg_mqtt
	luaL_newmetatable(L, "LuaBook.mg_mqtt");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_mqtt_lib_m, 0);
	luaL_openlib(L, "mg_mqtt", mg_mqtt_lib_f, 0);
	lua_pop(L, 2);

	mg_open_mg_mqtt_message(L);
	mg_open_mg_mqtt_opts(L);
	lua_pop(L, 1);
	//printf("END MG.MQTT: \n"); dumpstack(L);
}