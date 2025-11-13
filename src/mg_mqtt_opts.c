#include "mg_mqtt_opts.h"

int _mg_mqtt_opts_new (lua_State *L) {
	mqtt_opts *opts = NULL;

	opts = (mqtt_opts *)lua_newuserdata(L, sizeof(mqtt_opts));
	memset(opts, 0, sizeof(mqtt_opts));
	luaL_getmetatable(L, "LuaBook.mqtt_opts");
	lua_setmetatable(L, -2);

	if(!opts) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mqtt_opts *check_mqtt_opts(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mqtt_opts");
	luaL_argcheck(L, ud != NULL, pos, "`mg_mqtt_opts' expected");
	return(mqtt_opts *)ud;
};

static int _mg_mqtt_opts_user(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->user = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->user.buf, opts->user.len);
	return 1;
};

static int _mg_mqtt_opts_pass(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->pass = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->pass.buf, opts->pass.len);
	return 1;
};

static int _mg_mqtt_opts_client_id(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->client_id = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->client_id.buf, opts->client_id.len);

	return 1;
};

static int _mg_mqtt_opts_topic(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->topic = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->topic.buf, opts->topic.len);

	return 1;
};

static int _mg_mqtt_opts_meassage(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->message = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, opts->message.buf, opts->message.len);

	return 1;
};

static int _mg_mqtt_opts_qos(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->qos = (uint8_t)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, opts->qos);

	return 1;
};

static int _mg_mqtt_opts_version(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->version = luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, opts->version);
	return 1;
};

static int _mg_mqtt_opts_keepalive(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->keepalive = luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, opts->keepalive);

	return 1;
};

static int _mg_mqtt_opts_retransmit_id(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->retransmit_id = luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, opts->retransmit_id);

	return 1;
};

static int _mg_mqtt_opts_retain(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->retain = lua_toboolean(L, 2);

		return 0;
	}
	lua_pushboolean(L, opts->retain);

	return 1;
};

static int _mg_mqtt_opts_clean(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->clean= (bool)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushboolean(L, opts->clean);

	return 1;
};

static int _mg_mqtt_opts_num_props(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->num_props = (bool)luaL_checklong(L, 2);

		return 0;
	}
	lua_pushnumber(L, opts->num_props);

	return 1;
};

static int _mg_mqtt_opts_num_will_props(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L, 1);
	if(nargs > 1) {
		opts->num_will_props = (bool)luaL_checklong(L, 2);

		return 0;
	}
	lua_pushnumber(L, opts->num_will_props);

	return 1;
};

static int _mg_mqtt_opts_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "user") == 0 ) {
				_mg_mqtt_opts_user(L);
			}
			else if(key && strcmp(key, "pass") == 0 ) {
				_mg_mqtt_opts_pass(L);
			}
			else if(key && strcmp(key, "client_id") == 0 ) {
				_mg_mqtt_opts_client_id(L);
			}
			else if(key && strcmp(key, "topic") == 0 ) {
				_mg_mqtt_opts_topic(L);
			}
			else if(key && strcmp(key, "message") == 0 ) {
				_mg_mqtt_opts_meassage(L);
			}
			else if(key && strcmp(key, "qos") == 0 ) {
				_mg_mqtt_opts_qos(L);
			}
			else if(key && strcmp(key, "keepalive") == 0 ) {
				_mg_mqtt_opts_keepalive(L);
			}
			else if(key && strcmp(key, "retransmit_id") == 0 ) {
				_mg_mqtt_opts_retransmit_id(L);
			}
			else if(key && strcmp(key, "retain") == 0 ) {
				_mg_mqtt_opts_retain(L);
			}
			else if(key && strcmp(key, "clean") == 0 ) {
				_mg_mqtt_opts_clean(L);
			}
			else if(key && strcmp(key, "version") == 0 ) {
				_mg_mqtt_opts_version(L);
			}
			else if(key && strcmp(key, "num_props") == 0 ) {
				_mg_mqtt_opts_num_props(L);
			}
			else if(key && strcmp(key, "num_will_props") == 0 ) {
				_mg_mqtt_opts_num_will_props(L);
			}
		}
	}

	return 0;
};

static int _mg_mqtt_opts_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "user") == 0 ) {
			_mg_mqtt_opts_user(L);
		}
		else if(key && strcmp(key, "pass") == 0 ) {
			_mg_mqtt_opts_pass(L);
		}
		else if(key && strcmp(key, "client_id") == 0 ) {
			_mg_mqtt_opts_client_id(L);
		}
		else if(key && strcmp(key, "topic") == 0 ) {
			_mg_mqtt_opts_topic(L);
		}
		else if(key && strcmp(key, "message") == 0 ) {
			_mg_mqtt_opts_meassage(L);
		}
		else if(key && strcmp(key, "qos") == 0 ) {
			_mg_mqtt_opts_qos(L);
		}
		else if(key && strcmp(key, "keepalive") == 0 ) {
			_mg_mqtt_opts_keepalive(L);
		}
		else if(key && strcmp(key, "retransmit_id") == 0 ) {
			_mg_mqtt_opts_retransmit_id(L);
		}
		else if(key && strcmp(key, "retain") == 0 ) {
			_mg_mqtt_opts_retain(L);
		}
		else if(key && strcmp(key, "clean") == 0 ) {
			_mg_mqtt_opts_clean(L);
		}
		else if(key && strcmp(key, "version") == 0 ) {
			_mg_mqtt_opts_version(L);
		}
		else if(key && strcmp(key, "num_props") == 0 ) {
			_mg_mqtt_opts_num_props(L);
		}
		else if(key && strcmp(key, "num_will_props") == 0 ) {
			_mg_mqtt_opts_num_will_props(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_mqtt_opts_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_mqtt_opts_newt(lua_State * L) {
	_mg_mqtt_opts_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_mqtt_opts_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_mqtt_opts_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg mqtt_opts_lib_f [] = {
	{"ptr", 	_mg_mqtt_opts_new	},
	{"new", 	_mg_mqtt_opts_newt	},
	{NULL, NULL}
};

static const struct luaL_reg mqtt_opts_lib_m [] = {
	{"ptr", 		_mg_mqtt_opts_new		},
	{"new", 		_mg_mqtt_opts_newt		},
	{"user", 		_mg_mqtt_opts_user		},
	{"pass", 		_mg_mqtt_opts_pass		},
	{"client_id", 		_mg_mqtt_opts_client_id		},
	{"topic", 		_mg_mqtt_opts_topic		},
	{"message", 		_mg_mqtt_opts_meassage		},
	{"qos", 		_mg_mqtt_opts_qos		},
	{"keepalive", 		_mg_mqtt_opts_keepalive		},
	{"retransmit_id",	_mg_mqtt_opts_retransmit_id	},
	{"retain", 		_mg_mqtt_opts_retain		},
	{"clean", 		_mg_mqtt_opts_clean		},
	{"version", 		_mg_mqtt_opts_version		},
	{"num_props", 		_mg_mqtt_opts_num_props		},
	{"num_will_props", 	_mg_mqtt_opts_num_will_props	},
	{NULL, NULL}
};

// must be called after open mqtt message !!
void  mg_open_mg_mqtt_opts (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mqtt_opts_lib_m);
	lua_setfield(L, -2, "opts");
	// mg_mqtt_opts
	luaL_newmetatable(L, "LuaBook.mqtt_opts");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mqtt_opts_lib_m, 0);
	luaL_openlib(L, "mqtt_opts", mqtt_opts_lib_f, 0);
	lua_pop(L, 2);
};
