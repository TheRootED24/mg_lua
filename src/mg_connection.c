#include "mg_connection.h"

// MG_CONNECTION USERDATUM
int new_mg_connection (lua_State *L) {
	mg_connection *conn;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		conn = (mg_connection*)lua_touserdata(L, 1);
		lua_pushlightuserdata(L, conn);
	}
	else
		conn = (mg_connection*)lua_newuserdata(L, sizeof(mg_connection));

	luaL_getmetatable(L, "LuaBook.mg_connection");
	lua_setmetatable(L, -2);
	if(!conn) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

mg_connection *check_mg_connection(lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_connection");
	luaL_argcheck(L, ud != NULL, pos, "`mg_connection' expected");

	return (mg_connection*)ud;
}

static int _mg_connection_next(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	if(conn->next != NULL)
		lua_pushlightuserdata(L, conn->next);
	else
		lua_pushnil(L);

	return 1;
}

static int _mg_connection_mgr(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	if(NULL != conn->mgr->conns)
	{
		//newconn(L);
		mg_connection *conns = conn->mgr->conns;
		lua_pushlightuserdata(L, conns);
	}
	else
		lua_pushnil(L);

	return 1;
}

static int _mg_connection_loc(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushlightuserdata(L, &conn->loc);

	return 1;
}

static int _mg_connection_send(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	mg_iobuf io = conn->send;
	if(nargs > 1) {
		if(strcmp(luaL_checkstring(L, 2), "len") == 0)
			lua_pushinteger(L, io.len);
		if(strcmp(luaL_checkstring(L, 2), "size") == 0)
			lua_pushinteger(L, io.size);
		if(strcmp(luaL_checkstring(L, 2), "align") == 0)
			lua_pushinteger(L, io.align);

		return 1;
	}
	else
	{
		lua_pushlstring(L, (const char*)io.buf, io.len);
		lua_pushinteger(L, io.len);
		lua_pushinteger(L, io.size);
		lua_pushinteger(L, io.align);
	}

	return 4;
}

static int _mg_connection_recv(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	mg_iobuf io = conn->recv;

	lua_pushlightuserdata(L, &io);
	new_mg_iobuf(L);
	check_mg_iobuf(L, 1);

	return 1;
}

static int _mg_connection_loc_ip(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[17] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip, &conn->loc);
	lua_pushlstring(L, buf, ret);
	//lua_pushlightuserdata(L, loc);

	return 1;
}

static int _mg_connection_loc_port(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[24] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip_port, &conn->loc);
	lua_pushlstring(L, buf, ret);
	//lua_pushlightuserdata(L, loc);

	return 1;
}

static int _mg_connection_rem_ip(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[17] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip, &conn->rem);
	lua_pushlstring(L, buf, ret);

	return 1;
}

static int _mg_connection_rem_port(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[24] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip_port, &conn->rem);
	lua_pushlstring(L, buf, ret);

	return 1;
}

static int _mg_connection_rem(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushlightuserdata(L, &conn->rem);

	return 1;
}

static int _mg_connection_fd(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushlightuserdata(L, conn->fd);

	return 1;
}

static int _mg_connection_id(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushnumber(L, conn->id);

	return 1;
}

static int _mg_connection_data(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	size_t maxlen = sizeof(conn->data);

	if(nargs > 1){
		char *data = (char*)luaL_checkstring(L, 2);
		size_t len = (size_t)luaL_checkinteger(L, 3);

		if(data && len < maxlen -1 )
			strncpy(conn->data, data, len);
	}

	if(conn)
		lua_pushstring(L, conn->data);
	else
		lua_pushnil(L);

	return 1;
};

static int _is_accepted(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_accepted = (uint8_t)luaL_checkinteger(L, -1);

	lua_pushinteger(L, (uint8_t)conn->is_accepted);

	return 1;
};

static int _is_arplooking(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_arplooking = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_arplooking);

	return 1;
};

static int _is_client(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_client = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_client);

	return 1;
};

static int _is_closing(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_closing = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_closing);

	return 1;
};

static int _is_draininig(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_draining = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_draining);

	return 1;
};

static int _is_full(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_full = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_full);

	return 1;
};

static int _is_hexdumping(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_hexdumping = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_hexdumping);
	return 1;
};

static int _is_listening(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_listening = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_listening);

	return 1;
};

static int _is_mqtt5(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_mqtt5 = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_mqtt5);

	return 1;
};

static int _is_readable(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_readable = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_readable);

	return 1;
};

static int _is_resolving(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_resolving = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_resolving);

	return 1;
};

static int _is_resp(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_resp = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_resp);

	return 1;
};

static int _is_tls(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_tls = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_tls);

	return 1;
};

static int _is_tls_hs(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_tls_hs = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_tls_hs);

	return 1;
};

static int _is_tls_throttled(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_tls_throttled = luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_tls_throttled);

	return 1;
};

static int _is_udp(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_udp= luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_udp);

	return 1;
};

static int _is_websocket(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_websocket= luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_websocket);

	return 1;
};

static int _is_writable(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1)
		conn->is_writable= luaL_checkinteger(L, -1);

	lua_pushinteger(L, conn->is_writable);

	return 1;
};

static const struct luaL_reg mg_connection_lib_f [] = {
	{"new", 		new_mg_connection	},
	{NULL, NULL}
};

static const struct luaL_reg mg_connection_lib_m [] = {
	{"new", 		new_mg_connection	},
	{"send", 		_mg_connection_send	},
	{"recv", 		_mg_connection_recv	},
	{"next",		_mg_connection_next	},
	{"mgr",			_mg_connection_mgr	},
	{"data",		_mg_connection_data	},
	{"loc",			_mg_connection_loc	},
	{"loc_ip",		_mg_connection_loc_ip	},
	{"loc_port",		_mg_connection_loc_port	},
	{"rem",			_mg_connection_rem	},
	{"rem_ip",		_mg_connection_rem_ip	},
	{"rem_port",		_mg_connection_rem_port	},
	{"fd",			_mg_connection_fd	},
	{"id",			_mg_connection_id	},
	{"is_accepted",		_is_accepted		},
	{"is_arplooking",	_is_arplooking		},
	{"is_client",		_is_client		},
	{"is_closing", 		_is_closing		},
	{"is_draining",		_is_draininig		},
	{"is_full",		_is_full		},
	{"is_hexdumping", 	_is_hexdumping		},
	{"is_listening",	_is_listening		},
	{"is_mqtt5",		_is_mqtt5		},
	{"is_readable",		_is_readable		},
	{"is_resolving",	_is_resolving		},
	{"is_resp",		_is_resp		},
	{"is_tls", 		_is_tls			},
	{"is_tls_hs",		_is_tls_hs		},
	{"is_tls_throttled",	_is_tls_throttled	},
	{"is_udp", 		_is_udp			},
	{"is_websocket",	_is_websocket		},
	{"is_writeable",	_is_writable		},
	{NULL, NULL}
};

void mg_open_mg_connection(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_connection_lib_m);
	lua_setfield(L, -2, "connection");
	// mg_connection
	luaL_newmetatable(L, "LuaBook.mg_connection");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_connection_lib_m, 0);
	luaL_openlib(L, "mg_connection",mg_connection_lib_f, 0);
	lua_pop(L, 2);
}
