#include "mg_mqtt_message.h"

int new_mqtt_message (lua_State *L) {
	mqtt_message *mm;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		mm = (mqtt_message*)lua_touserdata(L, 1);
		//mm = (mqtt_message*)lua_topointer(L, 1);
		//lua_pop(L, 1);
		//lua_pushlightuserdata(L, mm);
	 }
	else
		mm = (mqtt_message*)lua_newuserdata(L, sizeof(mqtt_message));

	luaL_getmetatable(L, "LuaBook.mqtt_message");
	lua_setmetatable(L, -2);
	if(!mm) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

mqtt_message *check_mqtt_message(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mqtt_message");
	luaL_argcheck(L, ud != NULL, 1, "`mg_mqtt_message' expected");

	return(mqtt_message *)ud;
}

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

static int _mqtt_message_topic(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->topic = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, mm->topic.buf, mm->topic.len);

	return 1;
}

static int _mqtt_message_data(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->data = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, mm->data.buf, mm->data.len);

	return 1;
}

static int _mqtt_message_dgram(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->dgram = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, mm->dgram.buf, mm->dgram.len);

	return 1;
}

static int _mqtt_message_id(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->id = (uint16_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, mm->id);

	return 1;
}

static int _mqtt_message_cmd(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->cmd = (uint8_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, mm->cmd);

	return 1;
}

static int _mqtt_message_qos(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->qos = (uint8_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, mm->qos);

	return 1;
}

static int _mqtt_message_ack(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->ack = (uint8_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, mm->ack);

	return 1;
}

static int _mqtt_message_props_start(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->props_start = (size_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, mm->props_start);

	return 1;
}

static int _mqtt_message_props_size(lua_State *L) {
	int nargs = lua_gettop(L);
	mqtt_message *mm = check_mqtt_message(L);
	if(nargs > 1)
		mm->props_size = (size_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, mm->props_size);

	return 1;
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

static const struct luaL_reg mqtt_message_lib_f [] = {
	{"new", 	new_mqtt_message	},
	{NULL, NULL}
};

static const struct luaL_reg mqtt_message_lib_m [] = {
	{"new", 	new_mqtt_message		},
	{"id", 		_mqtt_message_id		},
	{"data", 	_mqtt_message_data		},
	{"dgram", 	_mqtt_message_dgram		},
	{"topic", 	_mqtt_message_topic		},
	{"cmd", 	_mqtt_message_cmd		},
	{"qos", 	_mqtt_message_qos		},
	{"ack", 	_mqtt_message_ack		},
	{"props_start", _mqtt_message_props_start	},
	{"props_size", 	_mqtt_message_props_size	},
	{NULL, NULL}
};

void  mg_open_mg_mqtt_message (lua_State *L) {
	//printf("START MG.MQTT.MESSAGE: \n");
	lua_getfield(L, -1, "mqtt");
	//dumpstack(L);
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
	//printf("END MG.MQTT.MESSAGE:\n"); dumpstack(L);
}