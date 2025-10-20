#include "mg_http.h"

// struct mg_connection *mg_http_listen(struct mg_mgr *mgr, const char *url, mg_event_handler_t fn, void *fn_data);
static int _mg_http_listen(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	memset(GL, 0, sizeof(lua_State_t));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_http_listen(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	_mg_connection_newt(L); // push a new connection udata on stack
	check_mg_connection(L, 1); // check conn is ready

	return 1;
};

// struct mg_connection *mg_http_connect(struct mg_mgr *, const char *url, mg_event_handler_t fn, void *fn_data);
static int _mg_http_connect(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_http_connect(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	_mg_connection_newt(L); // push a new connection udata on stack
	check_mg_connection(L, 1); // check conn is ready

	return 1;
};

// int mg_http_status(const struct mg_http_message *hm);
static int _mg_http_status(lua_State *L) {
	const http_message *hm = check_mg_http_message(L, 1);

	lua_pushinteger(L, luaL_checkinteger(L, mg_http_status(hm)));

	return 1;
};

// int mg_http_get_request_len(const unsigned char *buf, size_t buf_len);
static int _mg_http_get_request_len(lua_State *L) {
	lua_pushinteger(L, luaL_checkinteger(L, mg_http_get_request_len((const unsigned char*)luaL_checkstring(L, 1), luaL_checklong(L, 2))));

	return 1;
};

// int mg_http_parse(const char *s, size_t len, struct mg_http_message *hm);
static int _mg_http_parse(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	size_t size = luaL_checklong(L, 2);
	lua_pop(L, 2);
	
	http_message *hm = check_mg_http_message(L, 1);

	lua_pushinteger(L, luaL_checkinteger(L, mg_http_parse(str, size, hm)));

	return 1;
};

// void mg_http_printf_chunk(struct mg_connection *c, const char *fmt, ...);
/* formating should be done pre-call via lua string.format to prevent the over head */
static int _mg_http_printf_chunk(lua_State *L) {
	
	mg_connection *conn = check_mg_connection(L, 1); //(mg_connection*)lua_topointer(L, 1);
	const char *chunk = lua_tostring(L, 2);
	mg_http_printf_chunk(conn, "%s", chunk);

	return 0;
};

// void mg_http_write_chunk(struct mg_connection *c, const char *buf, size_t len);
static int _mg_http_write_chunk(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1); //(mg_connection*)lua_topointer(L, 1);

	mg_http_write_chunk(conn, luaL_checkstring(L, 2), luaL_checklong(L, 3));

	return 0;
};

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

// void mg_http_serve_dir(struct mg_connection *c, struct mg_http_message *hm, const struct mg_http_serve_opts *opts);
static int _mg_http_serve_dir(lua_State *L) {
	mg_connection *conn = NULL;
	http_message *hm = NULL;
	http_serve_opts *opts = NULL;

	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		conn = (mg_connection*)lua_topointer(L, -1);
		lua_pop(L, 1);
	}
	else
		conn = (mg_connection*)lua_topointer(L, 1);
	
	if(lua_istable(L, 2)) {
		lua_getfield(L, 2, "ctx");
		hm = (http_message*)lua_topointer(L, -1);
		lua_pop(L, 1);
	}
	else
		hm = (http_message*)lua_topointer(L, 2);

	if(lua_istable(L, 3)) {
		lua_getfield(L, 3, "ctx");
		opts = check_mg_http_serve_opts(L, -1);
		lua_pop(L, 1);
	}
	else
		opts = (http_serve_opts *)lua_topointer(L, 3);

	mg_http_serve_dir(conn, hm, opts);

	return 0;
};

// void mg_http_serve_file(struct mg_connection *c, struct mg_http_message *hm, const char *path, struct mg_http_serve_opts *opts);
static int _mg_http_serve_file(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1); //(mg_connection*)lua_topointer(L, 1);
	http_message *hm = (http_message*)lua_topointer(L, 2);
	const char *path = luaL_checkstring(L, 3);

	http_serve_opts *opts = (http_serve_opts*)lua_topointer(L, 4);
	mg_http_serve_file(conn, hm, path, opts);

	return 0;
};

// void mg_http_reply(struct mg_connection *c, int status_code, const char *headers, const char *body_fmt, ...);
static int _mg_http_reply(lua_State *L) {
	//printf("REPLY: \n"); dumpstack(L);
	mg_connection *conn = check_mg_connection(L, 1); //(mg_connection*)lua_topointer(L, 1);
	int code = luaL_checkinteger(L, 2);
	const char *headers = luaL_optstring(L, 3, "");
	const char *fargs = luaL_checkstring(L, 4);
	//printf("REPLY: \n"); dumpstack(L);
	mg_http_reply(conn, code, headers, "%s", fargs);

	return 0;

};

// struct mg_str *mg_http_get_header(struct mg_http_message *hm, const char *name);
static int _mg_http_get_header(lua_State *L) {
	http_message *hm = (http_message*)lua_topointer(L, 1);
	struct mg_str *ret = mg_http_get_header(hm, luaL_checkstring(L, 2));
	lua_pushlightuserdata(L, ret);

	return 1;
};

// struct mg_str mg_http_get_header_var(struct mg_str s, struct mg_str v);
static int _mg_http_get_header_var(lua_State *L) {
	struct mg_str str = mg_http_get_header_var(mg_str(luaL_checkstring(L, 1)), mg_str(luaL_checkstring(L, 1)));
	lua_pushlstring(L, str.buf, str.len);

	return 1;
};


// size_t mg_http_next_multipart(struct mg_str body, size_t offset, struct mg_http_part *part);
static int _mg_http_next_multipart(lua_State *L) {
	mg_str *body = check_mg_str(L, 1);
	
	size_t ofs = (size_t)luaL_checkinteger(L, 2);
	http_part *part = check_mg_http_part(L, 3);
	size_t p = mg_http_next_multipart(*body, ofs, part);
	lua_pushinteger(L, p);

	return 1;
}

static const struct luaL_reg mg_http_lib_f [] = {
	{"listen",		_mg_http_listen		},
	{"connect",		_mg_http_connect	},
	{"status",		_mg_http_status		},
	{NULL, NULL}
};

static const struct luaL_reg mg_http_lib_m [] = {
	{"listen",		_mg_http_listen		},
	{"connect",		_mg_http_connect	},
	{"status",		_mg_http_status		},
	{"get_request_len", 	_mg_http_get_request_len},
	{"parse",		_mg_http_parse		},
	{"printf_chunk", 	_mg_http_printf_chunk	},
	{"write_chunk",		_mg_http_write_chunk	},
	{"serve_dir",		_mg_http_serve_dir	},
	{"serve_file", 		_mg_http_serve_file	},
	{"reply",		_mg_http_reply		},
	{"get_header",		_mg_http_get_header	},
	{"get_header_var",	_mg_http_get_header_var },
	{"next_multipart",	_mg_http_next_multipart	},
	{NULL, NULL}
};

void mg_open_mg_http(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_http_lib_m);
	lua_setfield(L, -2, "http");
	// mg_http
	luaL_newmetatable(L, "LuaBook.mg_http");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_http_lib_m, 0);
	luaL_openlib(L, "mg_http", mg_http_lib_f, 0);
	lua_pop(L, 2);
	// open sub-modules
	mg_open_mg_http_message(L);
	mg_open_mg_http_header(L);
	mg_open_mg_http_part(L);
	mg_open_mg_http_serve_opts(L);
	lua_pop(L, 1);
};
