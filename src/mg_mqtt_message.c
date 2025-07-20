#include "mg_mqtt_message.h"

int new_mqtt_message (lua_State *L) {
	mqtt_message *msg = (mqtt_message *)lua_newuserdata(L, sizeof(mqtt_message));

	luaL_getmetatable(L, "LuaBook.mqtt_message");
	lua_setmetatable(L, -2);
	if(!msg) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

mqtt_message *check_mqtt_message(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mqtt_message");
	luaL_argcheck(L, ud != NULL, 1, "`mg_mqtt_message' expected");
	return(mqtt_message *)ud;
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
	{"new", 		new_mqtt_message	},
	{NULL, NULL}
};

static const struct luaL_reg mqtt_message_lib_m [] = {
	{"new", 		new_mqtt_message	},
	{NULL, NULL}
};

void  mg_open_mg_mqtt_message (lua_State *L) {
	//printf("START MG.MQTT.MESSAGE: \n");
	lua_getfield(L, -1, "mqtt");
	//dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mqtt_message_lib_m);
	lua_setfield(L, -2, "message");

	// http_message
	luaL_newmetatable(L, "LuaBook.mqtt_message");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mqtt_message_lib_m, 0);
	luaL_openlib(L, "mg_mqtt_message", mqtt_message_lib_f, 0);
	lua_pop(L, 2);

	//printf("END MG.MQTT.MESSAGE:\n"); dumpstack(L);

}