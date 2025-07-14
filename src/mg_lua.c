/*
** compile **
gcc -W -Wall -Werror -Wextra -O3 -o mg_lua.so mg_lua.c mg_mgr.c mg_connection.c mg_http.c mg_http_message.c mg_http_header.c \
mg_http_part.c mg_http_serve_opts.c mg_dns.c mg_addr.c mg_ws.c mg_ws_message.c mg_sntp.c mg_iobuf.c mg_mqtt.c  mg_mqtt_opts.c \
mg_mqtt_message.c mg_tls.c mg_tls_opts.c mg_timer.c mg_time.c mg_json.c -shared -fpic -llua -lmongoose
*/

#include "mg_lua.h"

static const char *s_web_root = ".";

int mg_log_level = MG_LL_DEBUG;


static int _mg_web_root(lua_State *L) {
	s_web_root = luaL_checkstring(L, -1);

	return 0;
}
/*
static int _log_level(lua_State *L) {
	mg_log_level = luaL_checkinteger(L, -1);
	return 0;
}


void fn_lua(mg_connection *c, int ev, void *ev_data) {
	lua_State_t *GL = (lua_State_t*)c->fn_data;
	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, c);
	lua_pushinteger(L, ev);
	lua_pushlightuserdata(L, ev_data);
	//dumpstack(L);
	lua_pcall(L, 3, 0, 0);
}
*/

static int _MG_U32(lua_State *L) {
	uint8_t a = (uint8_t)lua_tointeger(L, 1);
	uint8_t b = (uint8_t)lua_tointeger(L, 2);
	uint8_t c = (uint8_t)lua_tointeger(L, 3);
	uint8_t d = (uint8_t)lua_tointeger(L, 4);
	lua_pushinteger(L, MG_U32(a, b, c, d));

	return 1;
}

/*
// Linked list management macros
#define LIST_ADD_HEAD(type_, head_, elem_) \
  do {                                     \
    (elem_)->next = (*head_);              \
    *(head_) = (elem_);                    \
  } while (0)

#define LIST_ADD_TAIL(type_, head_, elem_) \
  do {                                     \
    type_ **h = head_;                     \
    while (*h != NULL) h = &(*h)->next;    \
    *h = (elem_);                          \
  } while (0)

#define LIST_DELETE(type_, head_, elem_)   \
  do {                                     \
    type_ **h = head_;                     \
    while (*h != (elem_)) h = &(*h)->next; \
    *h = (elem_)->next;                    \
  } while (0)

static enum _types{
	HTTP_MESSAGE,	
};


static int _LIST_ADD_HEAD(lua_State *L) {
	mg_connection *_type = (mg_connection*)lua_topointer(L, 1);
	mg_connection *_head = (mg_connection*)lua_topointer(L, 2);
	mg_connection *_elem = (mg_connection*)lua_topointer(L, 3);


	LIST_ADD_HEAD(_type, &_head, _elem);
}
*/
void fn_serv(mg_connection *c, int ev, void *ev_data) {
	if (ev == MG_EV_OPEN) {
		c->is_hexdumping = 1;
	} else if (ev == MG_EV_HTTP_MSG) {
		struct mg_http_message *hm = (struct mg_http_message *) ev_data;
		if (mg_match(hm->uri, mg_str("/websocket"), NULL)) {
			// Upgrade to websocket. From now on, a connection is a full-duplex
			// Websocket connection, which will receive MG_EV_WS_MSG events.
			mg_ws_upgrade(c, hm, NULL);
		} else if (mg_match(hm->uri, mg_str("/rest"), NULL)) {
			// Serve REST response
			mg_http_reply(c, 200, "", "{\"result\": %d}\n", 123);
		} else {
			// Serve static files
			struct mg_http_serve_opts opts = {.root_dir = s_web_root};
			mg_http_serve_dir(c, ev_data, &opts);
		}
	} else if (ev == MG_EV_WS_MSG) {
		// Got websocket frame. Received data is wm->data. Echo it back!
		struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
		//lua_State_t *GL = (lua_State_t*)c->fn_data;
		//lua_pushlstring(GL->L, wm->data.buf, (int)wm->data.len);
		//lua_pushnumber(GL->L, (int)wm->data.len);
		//do_callback(GL->L);
		mg_ws_send(c, wm->data.buf, wm->data.len, WEBSOCKET_OP_TEXT);
	}
}


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
	newconn(L); // push a new connection udata on stack
	checkconn(L); // check conn is ready

	return 1;
}

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
	newconn(L); // push a new connection udata on stack
	checkconn(L); // check conn is ready

	return 1; // return the udata on the stack
}

static int _mg_send(lua_State *L) {
	mg_connection *conn = checkconn(L);
	//void *data = (void*)lua_topointer(L, -2);
	//size_t len = luaL_checknumber(L, -1);
	bool ret = mg_send(conn, (void*)lua_topointer(L, -2), luaL_checknumber(L, -1));
	lua_pushboolean(L, ret);

	return 1;
}

static int _mg_wakeup(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	// long id = (long)lua_tonumber(L, -3);
	// void *data = (void*)lua_topointer(L, -2);
	//size_t len = luaL_checknumber(L, -1);
	bool ret = mg_wakeup(mgr, (long)lua_tonumber(L, -3), (void*)lua_topointer(L, -2), luaL_checknumber(L, -1));
	lua_pushboolean(L, ret);

	return 1;
}

static int _mg_wakeup_init(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	bool ret = mg_wakeup_init(mgr);
	lua_pushboolean(L, ret);

	return 1;
}

// struct mg_connection *mg_wrapfd(struct mg_mgr *mgr, int fd, mg_event_handler_t fn, void *fn_data);

static int _mg_wrapfd(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	int fd = luaL_checkinteger(L, -2);
	mg_event_handler_t fn = (mg_event_handler_t)fn_serv;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_wrapfd(mgr, fd, fn, GL);
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	checkconn(L); // check conn is ready

	return 1;
}

static int _mg_printf(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	const char *fargs = luaL_checkstring(L, 2);
	size_t ret  = mg_printf(conn, "%s", fargs);
	lua_pushnumber(L, ret);

	return 1;

}

static int _mg_handle_sig(lua_State *L) {
	int signo = luaL_checkinteger(L, 1);
	store_state(L, luaL_checkstring(L, 2)); // store the state and callback name
	signal(signo, fn_lua_signal);

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

// lua_require "mg" for k,v in pairs(mg) do print(k,v) end;
int luaopen_mg_lua(lua_State *L)
{
	luaL_newmetatable(L, "mg.mg");
	lua_pushvalue(L, -1);	// pushes the metatable
	lua_setfield(L, -2, "__index");	// metatable.__index = metatable
	luaL_register(L, MG, mg_methods);
	//printf("BEGING ***>>>>>\n"); dumpstack(L);

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
}
