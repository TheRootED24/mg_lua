#include "mg_mqtt_opts.h"

int new_mqtt_opts (lua_State *L) {
	mqtt_opts *opts = NULL;
	//const char *root_dir 		= luaL_optstring(L, 1, ".");
	//const char *ssi_pattern 	= NULL; //luaL_optstring(L, 2, NULL);
	//const char *extra_headers 	= NULL; //luaL_optstring(L, 3, NULL);
	//const char *mime_types 		= NULL; //luaL_optstring(L, 4, NULL);
	//const char *page404 		= NULL; //luaL_optstring(L, 5, NULL);

	opts = (mqtt_opts *)lua_newuserdata(L, sizeof(mqtt_opts));
	memset(opts, 0, sizeof(mqtt_opts));
	luaL_getmetatable(L, "LuaBook.mqtt_opts");
	lua_setmetatable(L, -2);

	if(!opts) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mqtt_opts *check_mqtt_opts(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mqtt_opts");
	luaL_argcheck(L, ud != NULL, 1, "`mg_mqtt_opts' expected");
	return(mqtt_opts *)ud;
};

static int _mqtt_opts_user(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->user = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, opts->user.buf, opts->user.len);
	return 1;
};

static int _mqtt_opts_pass(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->pass = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, opts->pass.buf, opts->pass.len);
	return 1;
};

static int _mqtt_opts_client_id(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->client_id = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, opts->client_id.buf, opts->client_id.len);
	return 1;
};

static int _mqtt_opts_topic(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->topic = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, opts->topic.buf, opts->topic.len);

	return 1;
};

static int _mqtt_opts_meassage(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->message = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, opts->message.buf, opts->message.len);
	return 1;
};

static int _mqtt_opts_qos(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->qos = (uint8_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, opts->qos);
	return 1;
};

static int _mqtt_opts_version(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->version = luaL_checkinteger(L, 2);

	lua_pushinteger(L, opts->version);
	return 1;
};

static int _mqtt_opts_keepalive(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->keepalive = luaL_checkinteger(L, 2);

	lua_pushinteger(L, opts->keepalive);
	return 1;
};

static int _mqtt_opts_retransmit_id(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->retransmit_id = luaL_checkinteger(L, 2);

	lua_pushinteger(L, opts->retransmit_id);
	return 1;
};

static int _mqtt_opts_retain(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->retain = lua_toboolean(L, 2);

	lua_pushboolean(L, opts->retain);
	return 1;
};

static int _mqtt_opts_clean(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->clean= (bool)luaL_checkinteger(L, 2);

	lua_pushboolean(L, opts->clean);
	return 1;
};

static int _mqtt_opts_num_props(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->num_props = (bool)luaL_checklong(L, 2);

	lua_pushnumber(L, opts->num_props);
	return 1;
};

static int _mqtt_opts_num_will_props(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_opts *opts = check_mqtt_opts(L);
	if(nargs > 1)
		opts->num_will_props = (bool)luaL_checklong(L, 2);

	lua_pushnumber(L, opts->num_will_props);
	return 1;
};

static const struct luaL_reg mqtt_opts_lib_f [] = {
	{"new", 	new_mqtt_opts	},
	{NULL, NULL}
};

static const struct luaL_reg mqtt_opts_lib_m [] = {
	{"new", 		new_mqtt_opts			},
	{"user", 		_mqtt_opts_user			},
	{"pass", 		_mqtt_opts_pass			},
	{"client_id", 		_mqtt_opts_client_id		},
	{"topic", 		_mqtt_opts_topic		},
	{"message", 		_mqtt_opts_meassage		},
	{"qos", 		_mqtt_opts_qos			},
	{"keepalive", 		_mqtt_opts_keepalive		},
	{"retransmit_id",	_mqtt_opts_retransmit_id	},
	{"retain", 		_mqtt_opts_retain		},
	{"clean", 		_mqtt_opts_clean		},
	{"version", 		_mqtt_opts_version		},
	{"num_props", 		_mqtt_opts_num_props		},
	{"num_will_props", 	_mqtt_opts_num_will_props	},
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
	luaL_openlib(L, "mg_mqtt_opts", mqtt_opts_lib_f, 0);
	lua_pop(L, 2);
};
