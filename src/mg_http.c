#include "mg_http.h"

static void dumpstack (lua_State *L);

void fn_servh(mg_connection *c, int ev, void *ev_data) {
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
			struct mg_http_serve_opts opts = {.root_dir = "."};
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

// struct mg_connection *mg_http_listen(struct mg_mgr *mgr, const char *url, mg_event_handler_t fn, void *fn_data);
static int _mg_http_listen(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;
	
	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_http_listen(mgr, s_url, fn, GL);
	printf("sshhh!! .. listen ... ??\n");
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	//mg_connection *conn = 
	checkconn(L); // check conn is ready

	return 1;
}

// struct mg_connection *mg_http_connect(struct mg_mgr *, const char *url, mg_event_handler_t fn, void *fn_data);
static int _mg_http_connect(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char * cb = luaL_checkstring(L, 3);
	
	mg_event_handler_t fn = (mg_event_handler_t)fn_lua;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;
	
	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_http_connect(mgr, s_url, fn, GL);
	printf("connecct dammit !!\n");
	lua_pushlightuserdata(L, c);
	newconn(L); // push a new connection udata on stack
	//mg_connection *conn = 
	checkconn(L); // check conn is ready

	return 1;
}

// int mg_http_status(const struct mg_http_message *hm);
static int _mg_http_status(lua_State *L) {
	const http_message *hm = checkmessage(L);
	lua_pushinteger(L, luaL_checkinteger(L, mg_http_status(hm)));
	return 1;
}

// int mg_http_get_request_len(const unsigned char *buf, size_t buf_len);
static int _mg_http_get_request_len(lua_State *L) {
	lua_pushinteger(L, luaL_checkinteger(L, mg_http_get_request_len((const unsigned char*)luaL_checkstring(L, 1), luaL_checklong(L, 2))));
	return 1;
}

// int mg_http_parse(const char *s, size_t len, struct mg_http_message *hm);
static int _mg_http_parse(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	size_t size = luaL_checklong(L, 2);
	lua_pushvalue(L, 3);
	http_message *hm = checkmessage(L);
	lua_pushinteger(L, luaL_checkinteger(L, mg_http_parse(str, size, hm)));
	return 1;
}

// void mg_http_printf_chunk(struct mg_connection *c, const char *fmt, ...);
static int _mg_http_printf_chunk(lua_State *L) {
	//mg_connection *conn = checkconn(L);
	//const char *fmt = luaL_checkstring(L, 2);
	//int nargs = lua_gettop(L);
	//for(int i = 2; i < nargs-2; i++) {
		// TODO FINISHED THIS FUNC
	//}
	if(L)
		return 0;

	return 0;
}

// void mg_http_write_chunk(struct mg_connection *c, const char *buf, size_t len);
static int _mg_http_write_chunk(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	mg_http_write_chunk(conn, luaL_checkstring(L, 2), luaL_checklong(L, 3));
	return 0;
}

// void mg_http_serve_dir(struct mg_connection *c, struct mg_http_message *hm, const struct mg_http_serve_opts *opts);
static int _mg_http_serve_dir(lua_State *L) {
	mg_connection *conn = NULL;
	conn = (mg_connection*)lua_topointer(L, 1);
	http_message *hm = NULL;
	hm = (http_message*)lua_topointer(L, 2);
	http_serve_opts *opts = NULL;
	opts = (http_serve_opts*)lua_topointer(L, 3);
	mg_http_serve_dir(conn, hm, opts);
	return 0;
}

// void mg_http_serve_file(struct mg_connection *c, struct mg_http_message *hm, const char *path, struct mg_http_serve_opts *opts);
static int _mg_http_serve_file(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	http_message *hm = (http_message*)lua_topointer(L, 2);
	const char *path = luaL_checkstring(L, 3);

	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 4);
	mg_http_serve_file(conn, hm, path, opts);
	return 0;
}

// void mg_http_reply(struct mg_connection *c, int status_code, const char *headers, const char *body_fmt, ...);
static int _mg_http_reply(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	int code = luaL_checkinteger(L, 2);
	const char *headers = luaL_optstring(L, 3, "");
	const char *fargs = luaL_checkstring(L, 4);
	mg_http_reply(conn, code, headers, "%s", fargs);
	
	return 0;

}

// struct mg_str *mg_http_get_header(struct mg_http_message *hm, const char *name);
static int _mg_http_get_header(lua_State *L) {
	http_message *hm = (http_message*)lua_topointer(L, 1);
	struct mg_str *ret = mg_http_get_header(hm, luaL_checkstring(L, 2));
	lua_pushlightuserdata(L, ret);
	return 1;
}

// struct mg_str mg_http_get_header_var(struct mg_str s, struct mg_str v);
static int _mg_http_get_header_var(lua_State *L) {
	struct mg_str str = mg_http_get_header_var(mg_str(luaL_checkstring(L, 1)), mg_str(luaL_checkstring(L, 1)));
	lua_pushlstring(L, str.buf, str.len);
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

static const struct luaL_reg mg_http_lib_f [] = {
	{"new", 	newconn	},
	{NULL, NULL}
};


static const struct luaL_reg mg_http_lib_m [] = {
	{"listen",			_mg_http_listen		}	,
	{"connect",			_mg_http_connect		},
	{"status",			_mg_http_status			},
	{"get_request_len", _mg_http_get_request_len},
	{"parse",			_mg_http_parse			},
	{"printf_chunk", 	_mg_http_printf_chunk	},
	{"write_chunk",		_mg_http_write_chunk	},
	{"serve_dir",		_mg_http_serve_dir		},
	{"serve_file", 		_mg_http_serve_file		},
	{"reply",			_mg_http_reply			},
	{"get_header",		_mg_http_get_header		},
	{"get_header_var",	_mg_http_get_header_var	},
	{NULL, NULL}
};

void mg_open_mg_http(lua_State *L) {
	printf("START MG.HTTP: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_http_lib_m);
	lua_setfield(L, -2, "http");
	// mg_mgr
	luaL_newmetatable(L, "LuaBook.mg_http");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_http_lib_m, 0);
	luaL_openlib(L, "mg_http", mg_http_lib_f, 0);
	lua_pop(L, 2);

	mg_open_mg_http_message(L);
	mg_open_mg_http_header(L);
	mg_open_mg_http_part(L);
	mg_open_mg_http_serve_opts(L);
	lua_pop(L, 1);
	printf("END MG.HTTP: \n"); dumpstack(L);
}