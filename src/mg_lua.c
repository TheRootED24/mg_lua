#include "mg_lua.h"

static const char *s_web_root = ".";

int mg_log_level = MG_LL_DEBUG;

static int _mg_web_root(lua_State *L) {
	s_web_root = luaL_checkstring(L, -1);

	return 0;
};

static int _MG_U32(lua_State *L) {
	uint8_t a = (uint8_t)lua_tointeger(L, 1);
	uint8_t b = (uint8_t)lua_tointeger(L, 2);
	uint8_t c = (uint8_t)lua_tointeger(L, 3);
	uint8_t d = (uint8_t)lua_tointeger(L, 4);
	lua_pushinteger(L, MG_U32(a, b, c, d));

	return 1;
};

/* MG CORE */
static int _mg_listen(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_listen(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	_mg_connection_new(L); // push a new connection udata on stack
	check_mg_connection(L, 1); // check conn is ready

	return 1;
};

static int _mg_connect(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	const char *s_url = luaL_checkstring(L, -1);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_connect(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	_mg_connection_new(L); // push a new connection udata on stack
	check_mg_connection(L, 1); // check conn is ready

	return 1; // return the udata on the stack
};

static int _mg_send(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1); //(mg_connection*)lua_topointer(L, 1);
	const char *data = lua_tostring(L, 2);
	size_t len = luaL_checkinteger(L, 3);
	bool ret = mg_send(conn, (const void *)data, len);
	lua_pushboolean(L, ret);

	return 1;
};

static int _mg_wakeup(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	// long id = (long)lua_tonumber(L, -3);
	// void *data = (void*)lua_topointer(L, -2);
	//size_t len = luaL_checknumber(L, -1);
	bool ret = mg_wakeup(mgr, (unsigned long)lua_tonumber(L, 2), lua_tostring(L, 3), (size_t)luaL_checklong(L, 4));
	lua_pushboolean(L, ret);

	return 1;
};

static int _mg_wakeup_init(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	bool ret = mg_wakeup_init(mgr);
	lua_pushboolean(L, ret);

	return 1;
};

// struct mg_connection *mg_wrapfd(struct mg_mgr *mgr, int fd, mg_event_handler_t fn, void *fn_data);
static int _mg_wrapfd(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	int fd = luaL_checkinteger(L, -2);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;

	//lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_wrapfd(mgr, fd, fn, GL);
	lua_pushlightuserdata(L, c);
	_mg_connection_new(L); // push a new connection udata on stack
	check_mg_connection(L, -1); // check conn is ready

	return 1;
};

static int _mg_printf(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);//(mg_connection*)lua_topointer(L, 1);
	const char *fargs = luaL_checkstring(L, 2);
	size_t ret  = mg_printf(conn, "%s", fargs);
	lua_pushnumber(L, ret);

	return 1;
};

static int _mg_handle_sig(lua_State *L) {
	int signo = luaL_checkinteger(L, 1);
	store_state(L, luaL_checkstring(L, 2)); // store the state and callback name
	signal(signo, fn_lua_signal);

	return 0;
};

static const luaL_reg mg_methods[] = {
	{ "set_web_root",	_mg_web_root	},
	{ "connect",		_mg_connect	},
	{ "send",		_mg_send	},
	{ "listen",		_mg_listen	},
	{ "wakeup",		_mg_wakeup	},
	{ "wakeup_init",	_mg_wakeup_init	},
	{ "wrapfd",		_mg_wrapfd	},
	{ "mg_u32",		_MG_U32		},
	{ "printf",		_mg_printf	},
	{ "handle_sig",		_mg_handle_sig	},
	{NULL, NULL}
};

int luaopen_mg_lua(lua_State *L)
{
	luaL_newmetatable(L, "mg.mg");
	lua_pushvalue(L, -1);	// pushes the metatable
	lua_setfield(L, -2, "__index");	// metatable.__index = metatable
	luaL_register(L, MG, mg_methods);

	// MG LOG LEVEL ENUMS
	lua_pushinteger(L, 0);
	lua_setglobal(L, "MG_LL_NONE");
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MG_LL_ERROR");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MG_LL_INFO");
	lua_pushinteger(L, 3);
	lua_setglobal(L, "MG_LL_DEBUG");
	lua_pushinteger(L, 4);
	lua_setglobal(L, "MG_LL_VERBOSE");

	// MG MGR EVENT (EV) ENUMS
	lua_pushinteger(L, 0);
	lua_setglobal(L, "MG_EV_ERROR");
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MG_EV_OPEN");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MG_EV_POLL");
	lua_pushinteger(L, 3);
	lua_setglobal(L, "MG_EV_RESOLVE");
	lua_pushinteger(L, 4);
	lua_setglobal(L, "MG_EV_CONNECT");
	lua_pushinteger(L, 5);
	lua_setglobal(L, "MG_EV_ACCEPT");
	lua_pushinteger(L, 6);
	lua_setglobal(L, "MG_EV_TLS_HS");
	lua_pushinteger(L, 7);
	lua_setglobal(L, "MG_EV_READ");
	lua_pushinteger(L, 8);
	lua_setglobal(L, "MG_EV_WRITE");
	lua_pushinteger(L, 9);
	lua_setglobal(L, "MG_EV_CLOSE");
	lua_pushinteger(L, 10);
	lua_setglobal(L, "MG_EV_HTTP_HDRS");

	lua_pushinteger(L, 11);
	lua_setglobal(L, "MG_EV_HTTP_MSG");
	lua_pushinteger(L, 12);
	lua_setglobal(L, "MG_EV_WS_OPEN");
	lua_pushinteger(L, 13);
	lua_setglobal(L, "MG_EV_WS_MSG");
	lua_pushinteger(L, 14);
	lua_setglobal(L, "MG_EV_WS_CTL");
	lua_pushinteger(L, 15);
	lua_setglobal(L, "MG_EV_MQTT_CMD");
	lua_pushinteger(L, 16);
	lua_setglobal(L, "MG_EV_MQTT_MSG");
	lua_pushinteger(L, 17);
	lua_setglobal(L, "MG_EV_MQTT_OPEN");
	lua_pushinteger(L, 18);
	lua_setglobal(L, "MG_EV_SNTP_TIME");
	lua_pushinteger(L, 19);
	lua_setglobal(L, "MG_EV_WAKEUP");
	lua_pushinteger(L, 20);
	lua_setglobal(L, "MG_EV_USER");

	// MG WEBSOCKET OP CODE ENUMS
	lua_pushinteger(L, 0);
	lua_setglobal(L, "WEBSOCKET_OP_CONTINUE");
	lua_pushinteger(L, 1);
	lua_setglobal(L, "WEBSOCKET_OP_TEXT");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "WEBSOCKET_OP_BINARY");
	lua_pushinteger(L, 8);
	lua_setglobal(L, "WEBSOCKET_OP_CLOSE");
	lua_pushinteger(L, 9);
	lua_setglobal(L, "WEBSOCKET_OP_PING");
	lua_pushinteger(L, 10);
	lua_setglobal(L, "WEBSOCKET_OP_PONG");
	
	// MG FILESYTEM OPS ENUMS
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MG_FS_READ");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MG_FS_WRITE");
	lua_pushinteger(L, 4);
	lua_setglobal(L, "MG_FS_DIR");

	// FILESYTEM TYPE ENUMS
	lua_pushinteger(L, 0);
	lua_setglobal(L, "MG_FS_POSIX");
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MG_FS_FAT");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MG_FS_PACKED");

	// MG TIMER ENUMS
	lua_pushinteger(L, 0);
	lua_setglobal(L, "MG_TIMER_ONCE");
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MG_TIMER_REPEAT");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MG_TIMER_RUN_NOW");

	// MQTT CMD ENUMS
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MQTT_CMD_CONNECT");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MQTT_CMD_CONNACK");
	lua_pushinteger(L, 3);
	lua_setglobal(L, "MQTT_CMD_PUBLISH");
	lua_pushinteger(L, 4);
	lua_setglobal(L, "MQTT_CMD_PUBACK");
	lua_pushinteger(L, 5);
	lua_setglobal(L, "MQTT_CMD_PUBREC");
	lua_pushinteger(L, 6);
	lua_setglobal(L, "MQTT_CMD_PUBREL");
	lua_pushinteger(L, 7);
	lua_setglobal(L, "MQTT_CMD_PUBCOMP");
	lua_pushinteger(L, 8);
	lua_setglobal(L, "MQTT_CMD_SUBSCRIBE");
	lua_pushinteger(L, 9);
	lua_setglobal(L, "MQTT_CMD_SUBACK");
	lua_pushinteger(L, 10);
	lua_setglobal(L, "MQTT_CMD_UNSUBSCRIBE");
	lua_pushinteger(L, 11);
	lua_setglobal(L, "MQTT_CMD_UNSUBACK");
	lua_pushinteger(L, 12);
	lua_setglobal(L, "MQTT_CMD_PINGREQ");
	lua_pushinteger(L, 13);
	lua_setglobal(L, "MQTT_CMD_PINGRESP");
	lua_pushinteger(L, 14);
	lua_setglobal(L, "MQTT_CMD_DISCONNECT");

	// MG MQTT STATE ENUMS
	lua_pushinteger(L, 0);
	lua_setglobal(L, "MQTT_OK");
	lua_pushinteger(L, 1);
	lua_setglobal(L, "MQTT_INCOMPLETE");
	lua_pushinteger(L, 2);
	lua_setglobal(L, "MQTT_MALFORMED");

	// open modules
	mg_open_mg_addr(L);
	mg_open_mg_dns(L);
	mg_open_mg_mgr(L);
	mg_open_mg_connection(L);
	mg_open_mg_http(L);
	mg_open_mg_ws(L);
	mg_open_mg_sntp(L);
	mg_open_mg_iobuf(L);
	mg_open_mg_mqtt(L);
	mg_open_mg_tls(L);
	mg_open_mg_timer(L);
	mg_open_mg_time(L);
	mg_open_mg_json(L);
	mg_open_mg_rpc(L);
	mg_open_mg_util(L);
	mg_open_mg_string(L);
	mg_open_mg_str(L);
	mg_open_mg_url(L);
	mg_open_mg_logging(L);
	mg_open_mg_fs(L);
	mg_open_mg_fd(L);
	mg_open_mg_queue(L);

	return 1;
};
