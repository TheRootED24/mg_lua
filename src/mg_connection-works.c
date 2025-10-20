#include "mg_connection.h"

static int _mg_connection_index(lua_State *L);
static int _mg_connection_new_index(lua_State *L);
// MG_CONNECTION USERDATUM
int _mg_connection_new (lua_State *L) {
	mg_connection *conn;
	int nargs = lua_gettop(L);
	
	if(nargs > 0) {
		int pos = nargs > 1 ? -1 : 1;
		if(lua_istable(L, 1)) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);

			return 1;
		}

		conn = (mg_connection*)lua_touserdata(L, pos);
	}
	else {
		conn = (mg_connection*)lua_newuserdata(L, sizeof(mg_connection));
		memset(conn, 0, sizeof(mg_connection));
	}

	luaL_getmetatable(L, "LuaBook.mg_connection");
	lua_setmetatable(L, -2);

	if(!conn) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_connection *check_mg_connection(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_connection");
	luaL_argcheck(L, ud != NULL, pos, "`mg_connection' expected");

	return (mg_connection*)ud;
};

static int _mg_connection_next(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	if(conn->next != NULL) {
		lua_settop(L, 0);
		lua_pushlightuserdata(L, conn->next);
	}
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_connection_mgr(lua_State *L) {
	mg_connection *conn = (mg_connection*)lua_topointer(L, 1);
	if(NULL != conn->mgr)
	{
		mg_mgr *m = conn->mgr;
		lua_pushlightuserdata(L, m);
	}
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_connection_mgr_conns(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	if(NULL != conn->mgr->conns)
	{
		mg_connection *conns = conn->mgr->conns;
		lua_pushlightuserdata(L, conns);
	}
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_connection_loc(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);

	if(nargs > 1) {
		mg_addr *addr = check_mg_addr(L, 2);
		*(conn->loc.ip) = *addr->ip;
		conn->loc.port = addr->port;
		conn->loc.scope_id = addr->scope_id;
		conn->loc.is_ip6 = addr->is_ip6;

		return 0;
	}
	lua_pushlightuserdata(L, &conn->loc);

	return 1;
};

static int _mg_connection_send(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	mg_iobuf io = conn->send;
	if(nargs > 1) {
		if(strcmp(luaL_checkstring(L, 2), "buf") == 0)
			lua_pushlstring(L, (char*)io.buf, io.len);
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
};

static int _mg_connection_recv(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	mg_iobuf io = conn->recv;
	if(nargs > 1) {
		if(strcmp(luaL_checkstring(L, 2), "buf") == 0)
			lua_pushlstring(L, (char*)io.buf, io.len);
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
};

static int _mg_connection_loc_ip(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[17] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip, &conn->loc);
	lua_pushlstring(L, buf, ret);
	//lua_pushlightuserdata(L, loc);

	return 1;
};

static int _mg_connection_loc_port(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[24] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip_port, &conn->loc);
	lua_pushlstring(L, buf, ret);
	//lua_pushlightuserdata(L, loc);

	return 1;
};

static int _mg_connection_rem_ip(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[17] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip, &conn->rem);
	lua_pushlstring(L, buf, ret);

	return 1;
};

static int _mg_connection_rem_port(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	char buf[24] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip_port, &conn->rem);
	lua_pushlstring(L, buf, ret);

	return 1;
};

static int _mg_connection_rem(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushlightuserdata(L, &conn->rem);

	return 1;
};

static int _mg_connection_fd(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushlightuserdata(L, conn->fd);

	return 1;
};

static int _mg_connection_id(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	lua_pushnumber(L, conn->id);

	return 1;
};

static int _mg_connection_data(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	size_t maxlen = sizeof(conn->data);

	if(nargs > 1){
		char *data = (char*)luaL_checkstring(L, 2);
		size_t len = (size_t)luaL_checkinteger(L, 3);

		if(data && len < maxlen -1 )
			strncpy(conn->data, data, len);

		return 0;
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
	if(nargs > 1) {
		conn->is_accepted = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_accepted);

	return 1;
};

static int _is_arplooking(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_arplooking = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_arplooking);

	return 1;
};

static int _is_client(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) { 
		conn->is_client = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_client);

	return 1;
};

static int _is_closing(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_closing = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_closing);

	return 1;
};

static int _is_draininig(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_draining = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_draining);

	return 1;
};

static int _is_full(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_full = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_full);

	return 1;
};

static int _is_hexdumping(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_hexdumping = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_hexdumping);
	return 1;
};

static int _is_listening(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_listening = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_listening);

	return 1;
};

static int _is_mqtt5(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_mqtt5 = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}	
	lua_pushinteger(L, conn->is_mqtt5);

	return 1;
};

static int _is_readable(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_readable = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_readable);

	return 1;
};

static int _is_resolving(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_resolving = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_resolving);

	return 1;
};

static int _is_resp(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_resp = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_resp);

	return 1;
};

static int _is_tls(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_tls = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_tls);

	return 1;
};

static int _is_tls_hs(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_tls_hs = (unsigned int)luaL_checkinteger(L, 2);
	
		return 0;
	}
	lua_pushinteger(L, conn->is_tls_hs);

	return 1;
};

static int _is_tls_throttled(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_tls_throttled = (unsigned int)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, conn->is_tls_throttled);

	return 1;
};

static int _is_udp(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_udp = (unsigned int)luaL_checkinteger(L, 2);
		
		return 0;
	}
	lua_pushinteger(L, conn->is_udp);

	return 1;
};

static int _is_websocket(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_websocket = (unsigned int)luaL_checkinteger(L, 2);
	
		return 0;
	}
	lua_pushinteger(L, conn->is_websocket);

	return 1;
};

static int _is_writeable(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_connection *conn = check_mg_connection(L, 1);
	if(nargs > 1) {
		conn->is_writable = (unsigned int)luaL_checkinteger(L, 2);
	
		return 0;
	}
	lua_pushinteger(L, conn->is_writable);

	return 1;
};

static int _mg_connection_new_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		//printf("KEY: %s CANNOT BE UPDATED !\n", key);
		lua_remove(L, 2);

		if(lua_istable(L, 1) && (key && strcmp(key, "ctx") != 0)) {
		
			if(key && strcmp(key, "is_accepted") == 0 ) {
				_is_accepted(L);
			}
			else if(key && strcmp(key, "is_arplooking") == 0 ) {
				_is_arplooking(L);
			}
			else if(key && strcmp(key, "is_client") == 0 ) {
				_is_client(L);
			}
			else if(key && strcmp(key, "is_closing") == 0 ) {
				_is_closing(L);
			}
			else if(key && strcmp(key, "is_draininig") == 0 ) {
				_is_draininig(L);
			}
			else if(key && strcmp(key, "is_full") == 0 ) {
				_is_full(L);
			}
			else if(key && strcmp(key, "is_hexdumping") == 0 ) {
				_is_hexdumping(L);
			}
			else if(key && strcmp(key, "is_listening") == 0 ) {
				_is_listening(L);
			}
			else if(key && strcmp(key, "is_mqtt5") == 0 ) {
				_is_mqtt5(L);
			}
			else if(key && strcmp(key, "is_readable") == 0 ) {
				_is_readable(L);
			}
			else if(key && strcmp(key, "is_resolving") == 0 ) {
				_is_resolving(L);
			}
			else if(key && strcmp(key, "is_resp") == 0 ) {
				_is_resp(L);
			}
			else if(key && strcmp(key, "is_tls") == 0 ) {
				_mg_connection_fd(L);
			}
			else if(key && strcmp(key, "is_tls_hs") == 0 ) {
				_is_tls_hs(L);
			}
			else if(key && strcmp(key, "is_tls_throttled") == 0 ) {
				_is_tls_throttled(L);
			}
			else if(key && strcmp(key, "is_udp") == 0 ) {
				_is_udp(L);
			}
			else if(key && strcmp(key, "is_websocket") == 0 ) {
				_is_websocket(L);
			}
			else if(key && strcmp(key, "is_writeable") == 0 ) {
				_is_writeable(L);
			}
			else if(key && strcmp(key, "data") == 0 ) {
				lua_pushinteger(L, (int)strlen(lua_tostring(L, 2)));
				_mg_connection_data(L);
			}
			else
				if(key) printf("KEY: %s CANNOT BE UPDATED !\n", key);
		}
	}

	return 0;
};

static int _mg_connection_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "send") == 0 ) {
			_mg_connection_send(L);
		}
		else if(key && strcmp(key, "recv") == 0 ) {
			_mg_connection_recv(L);
		}
		else if(key && strcmp(key, "next") == 0 ) {
			_mg_connection_next(L);
		}
		else if(key && strcmp(key, "mgr") == 0 ) {
			_mg_connection_mgr(L);
		}
		else if(key && strcmp(key, "mgr_conns") == 0 ) {
			_mg_connection_mgr_conns(L);
		}
		else if(key && strcmp(key, "data") == 0 ) {
			_mg_connection_data(L);
		}
		else if(key && strcmp(key, "loc") == 0 ) {
			_mg_connection_loc(L);
		}
		else if(key && strcmp(key, "loc_ip") == 0 ) {
			_mg_connection_loc_ip(L);
		}
		else if(key && strcmp(key, "loc_port") == 0 ) {
			_mg_connection_loc_port(L);
		}
		else if(key && strcmp(key, "rem") == 0 ) {
			_mg_connection_rem(L);
		}
		else if(key && strcmp(key, "rem_ip") == 0 ) {
			_mg_connection_rem_ip(L);
		}
		else if(key && strcmp(key, "rem_port") == 0 ) {
			_mg_connection_rem_port(L);
		}
		else if(key && strcmp(key, "fd") == 0 ) {
			_mg_connection_fd(L);
		}
		else if(key && strcmp(key, "id") == 0 ) {
			_mg_connection_id(L);
		}
		else if(key && strcmp(key, "is_accepted") == 0 ) {
			_is_accepted(L);
		}
		else if(key && strcmp(key, "is_arplooking") == 0 ) {
			_is_arplooking(L);
		}
		else if(key && strcmp(key, "is_client") == 0 ) {
			_is_client(L);
		}
		else if(key && strcmp(key, "is_closing") == 0 ) {
			_is_closing(L);
		}
		else if(key && strcmp(key, "is_draininig") == 0 ) {
			_is_draininig(L);
		}
		else if(key && strcmp(key, "is_full") == 0 ) {
			_is_full(L);
		}
		else if(key && strcmp(key, "is_hexdumping") == 0 ) {
			_is_hexdumping(L);
		}
		else if(key && strcmp(key, "is_listening") == 0 ) {
			_is_listening(L);
		}
		else if(key && strcmp(key, "is_mqtt5") == 0 ) {
			_is_mqtt5(L);
		}
		else if(key && strcmp(key, "is_readable") == 0 ) {
			_is_readable(L);
		}
		else if(key && strcmp(key, "is_resolving") == 0 ) {
			_is_resolving(L);
		}
		else if(key && strcmp(key, "is_resp") == 0 ) {
			_is_resp(L);
		}
		else if(key && strcmp(key, "is_tls") == 0 ) {
			_mg_connection_fd(L);
		}
		else if(key && strcmp(key, "is_tls_hs") == 0 ) {
			_is_tls_hs(L);
		}
		else if(key && strcmp(key, "is_tls_throttled") == 0 ) {
			_is_tls_throttled(L);
		}
		else if(key && strcmp(key, "is_udp") == 0 ) {
			_is_udp(L);
		}
		else if(key && strcmp(key, "is_websocket") == 0 ) {
			_is_websocket(L);
		}
		else if(key && strcmp(key, "is_writeable") == 0 ) {
			_is_writeable(L);
		}
		else if(key && strcmp(key, "connection") == 0 ) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);
			_mg_connection_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_connection_newt(lua_State * L) {
	_mg_connection_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_pushcfunction(L, _mg_connection_mgr_conns);
	lua_setfield(L, -2, "mgr_conns");

	lua_pushcfunction(L, _mg_connection_next);
	lua_setfield(L, -2, "next");

	lua_pushcfunction(L, _mg_connection_mgr);
	lua_setfield(L, -2, "mgr");

	lua_pushcfunction(L, _mg_connection_send);
	lua_setfield(L, -2, "send");

	lua_pushcfunction(L, _mg_connection_recv);
	lua_setfield(L, -2, "recv");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_connection_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_connection_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg mg_connection_lib_f [] = {
	{"ptr", 		_mg_connection_new	},
	{NULL, NULL}
};

static const struct luaL_reg mg_connection_lib_m [] = {
	{"ptr", 		_mg_connection_new	},
	{"new", 		_mg_connection_newt	},
	{"send", 		_mg_connection_send	},
	{"recv", 		_mg_connection_recv	},
	{"next",		_mg_connection_next	},
	{"mgr",			_mg_connection_mgr	},
	{"mgr_conns",		_mg_connection_mgr_conns},
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
	{"is_writeable",	_is_writeable		},
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
};
