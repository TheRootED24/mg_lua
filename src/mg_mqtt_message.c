#include "mg_mqtt_message.h"

/*
struct mg_mqtt_message {
  struct mg_str topic;  // Parsed topic for PUBLISH
  struct mg_str data;   // Parsed message for PUBLISH
  struct mg_str dgram;  // Whole MQTT packet, including headers
  uint16_t id;          // For PUBACK, PUBREC, PUBREL, PUBCOMP, SUBACK, PUBLISH
  uint8_t cmd;          // MQTT command, one of MQTT_CMD_*
  uint8_t qos;          // Quality of service
  uint8_t ack;          // CONNACK return code, 0 = success
  size_t props_start;   // Offset to the start of the properties (MQTT5)
  size_t props_size;    // Length of the properties
};
*/

int _mg_mqtt_message_new (lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm;

	if(nargs > 0) 
		mm = (mqtt_message*)lua_touserdata(L, 1);
	else
		mm = (mqtt_message*)lua_newuserdata(L, sizeof(mqtt_message));

	luaL_getmetatable(L, "LuaBook.mqtt_message");
	lua_setmetatable(L, -2);
	if(!mm) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mqtt_message *check_mqtt_message(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mqtt_message");
	luaL_argcheck(L, ud != NULL, pos, "`mg_mqtt_message' expected");

	return(mqtt_message *)ud;
};

static int _mg_mqtt_message_topic(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->topic = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, mm->topic.buf, mm->topic.len);

	return 1;
};

static int _mg_mqtt_message_data(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->data = mg_str(luaL_checkstring(L, 2));

		return 0;
	}	
	lua_pushlstring(L, mm->data.buf, mm->data.len);

	return 1;
};

static int _mg_mqtt_message_dgram(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->dgram = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, mm->dgram.buf, mm->dgram.len);

	return 1;
};

static int _mg_mqtt_message_id(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->id = (uint16_t)luaL_checkinteger(L, 2);

		return 0;
	}	
	lua_pushinteger(L, mm->id);

	return 1;
};

static int _mg_mqtt_message_cmd(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->cmd = (uint8_t)luaL_checkinteger(L, 2);

		return 0;
	}	
	lua_pushinteger(L, mm->cmd);

	return 1;
};

static int _mg_mqtt_message_qos(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->qos = (uint8_t)luaL_checkinteger(L, 2);
	
		return 0;
	}
	lua_pushinteger(L, mm->qos);

	return 1;
};

static int _mg_mqtt_message_ack(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->ack = (uint8_t)luaL_checkinteger(L, 2);
	
		return 0;
	}
	lua_pushinteger(L, mm->ack);

	return 1;
};

static int _mg_mqtt_message_props_start(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->props_start = (size_t)luaL_checkinteger(L, 2);

		return 0;
	}
	lua_pushinteger(L, mm->props_start);

	return 1;
};

static int _mg_mqtt_message_props_size(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L, 1);
	if(nargs > 1) {
		mm->props_size = (size_t)luaL_checkinteger(L, 2);
	
		return 0;
	}
	lua_pushinteger(L, mm->props_size);

	return 1;
};

static int _mg_mqtt_message_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "id") == 0 ) {
				_mg_mqtt_message_id(L);
			}
			else if(key && strcmp(key, "data") == 0 ) {
				_mg_mqtt_message_data(L);
			}
			else if(key && strcmp(key, "dgram") == 0 ) {
				_mg_mqtt_message_dgram(L);
			}
			else if(key && strcmp(key, "topic") == 0 ) {
				_mg_mqtt_message_topic(L);
			}
			else if(key && strcmp(key, "cmd") == 0 ) {
				_mg_mqtt_message_cmd(L);
			}
			else if(key && strcmp(key, "qos") == 0 ) {
				_mg_mqtt_message_qos(L);
			}
			else if(key && strcmp(key, "ack") == 0 ) {
				_mg_mqtt_message_ack(L);
			}
			else if(key && strcmp(key, "props_start") == 0 ) {
				_mg_mqtt_message_props_start(L);
			}
			else if(key && strcmp(key, "props_size") == 0 ) {
				_mg_mqtt_message_props_size(L);
			}
		}
	}

	return 0;
};

static int _mg_mqtt_message_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "id") == 0 ) {
			_mg_mqtt_message_id(L);
		}
		else if(key && strcmp(key, "data") == 0 ) {
			_mg_mqtt_message_data(L);
		}
		else if(key && strcmp(key, "dgram") == 0 ) {
			_mg_mqtt_message_dgram(L);
		}
		else if(key && strcmp(key, "topic") == 0 ) {
			_mg_mqtt_message_topic(L);
		}
		else if(key && strcmp(key, "cmd") == 0 ) {
			_mg_mqtt_message_cmd(L);
		}
		else if(key && strcmp(key, "qos") == 0 ) {
			_mg_mqtt_message_qos(L);
		}
		else if(key && strcmp(key, "ack") == 0 ) {
			_mg_mqtt_message_ack(L);
		}
		else if(key && strcmp(key, "props_start") == 0 ) {
			_mg_mqtt_message_props_start(L);
		}
		else if(key && strcmp(key, "props_size") == 0 ) {
			_mg_mqtt_message_props_size(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_mqtt_message_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_mqtt_message_newt(lua_State * L) {
	_mg_mqtt_message_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_mqtt_message_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_mqtt_message_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg mqtt_message_lib_f [] = {
	//{"new", 	_mg_mqtt_message_new		},
	{"new", 	_mg_mqtt_message_newt		},
	{NULL, NULL}
};

static const struct luaL_reg mqtt_message_lib_m [] = {
	//{"new", 	_mg_mqtt_message_new		},
	{"new", 	_mg_mqtt_message_newt		},
	{"id", 		_mg_mqtt_message_id		},
	{"data", 	_mg_mqtt_message_data		},
	{"dgram", 	_mg_mqtt_message_dgram		},
	{"topic", 	_mg_mqtt_message_topic		},
	{"cmd", 	_mg_mqtt_message_cmd		},
	{"qos", 	_mg_mqtt_message_qos		},
	{"ack", 	_mg_mqtt_message_ack		},
	{"props_start", _mg_mqtt_message_props_start	},
	{"props_size", 	_mg_mqtt_message_props_size	},
	{NULL, NULL}
};

void  mg_open_mg_mqtt_message (lua_State *L) {
	lua_getfield(L, -1, "mqtt");
	lua_newtable(L);
	luaL_register(L, NULL, mqtt_message_lib_m);
	lua_setfield(L, -2, "message");
	// mg_mqtt_message
	luaL_newmetatable(L, "LuaBook.mqtt_message");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mqtt_message_lib_m, 0);
	luaL_openlib(L, "mg_mqtt_message", mqtt_message_lib_f, 0);
	lua_pop(L, 2);
};
