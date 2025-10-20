#include "mg_ws_message.h"

int _mg_ws_message_new (lua_State *L) {
	ws_message *wm;
	int nargs = lua_gettop(L);

	if(nargs > 0)
		wm = (ws_message*)lua_touserdata(L, 1);
	else {
		wm = (ws_message*)lua_newuserdata(L, sizeof(ws_message));
		memset(wm, 0, sizeof(ws_message));
	}

	luaL_getmetatable(L, "LuaBook.ws_message");
	lua_setmetatable(L, -2);
	if(!wm) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

ws_message *check_ws_message (lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.ws_message");
	luaL_argcheck(L, ud != NULL, pos, "`mg_ws_message' expected");

	return(ws_message*)ud;
};

static int _message_data(lua_State *L) {
	int nargs = lua_gettop(L);
	ws_message *msg = check_ws_message(L, 1);
	if(nargs > 1)
		msg->data = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, msg->data.buf, msg->data.len);

	return 1;
};

static int _message_next(lua_State *L) {
	ws_message *msg = check_ws_message(L, 1);
	int len = luaL_checkinteger(L, 2);
	msg->data.buf += len;
	msg->data.len -= len;

	return 0;
};

static int _message_len(lua_State *L) {
	int nargs = lua_gettop(L);
	ws_message *msg = check_ws_message(L, 1);
	if(nargs > 1)
		msg->data.len = (size_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, msg->data.len);

	return 1;
};

static int _message_flags(lua_State *L) {
	int nargs = lua_gettop(L);
	ws_message *msg = check_ws_message(L, 1);
	if(nargs > 1)
		msg->flags = (uint8_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, msg->flags);
	return 1;
};

static int _mg_ws_message_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "data") == 0 ) {
				_message_data(L);
			}
			else if(key && strcmp(key, "len") == 0 ) {
				_message_len(L);
			}
			else if(key && strcmp(key, "flags") == 0 ) {
				_message_flags(L);
			}
		}
	}

	return 0;
};

static int _mg_ws_message_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "data") == 0 ) {
			_message_data(L);
		}
		else if(key && strcmp(key, "len") == 0 ) {
			_message_len(L);
		}
		else if(key && strcmp(key, "flags") == 0 ) {
			_message_flags(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_ws_message_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_ws_message_newt(lua_State * L) {
	_mg_ws_message_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_pushcfunction(L, _message_next);
	lua_setfield(L, -2, "next");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_ws_message_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_ws_message_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg ws_message_lib_f [] = {
	{"new", 	_mg_ws_message_new 	},
	{"newt", 	_mg_ws_message_newt	},
	{NULL, NULL}
};

static const struct luaL_reg ws_message_lib_m [] = {
	{"new",		_mg_ws_message_new 	},
	{"newt", 	_mg_ws_message_newt	},
	{"data",	_message_data		},
	{"next",	_message_next		},
	{"len",		_message_len		},
	{"flags",	_message_flags		},
	{NULL, NULL}
};

void  mg_open_mg_ws_message (lua_State *L) {
	lua_getfield(L, -1, "ws");
	lua_newtable(L);
	luaL_register(L, NULL, ws_message_lib_m);
	lua_setfield(L, -2, "message");
	// mg_ws_message
	luaL_newmetatable(L, "LuaBook.ws_message");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, ws_message_lib_m, 0);
	luaL_openlib(L, "ws_message", ws_message_lib_f, 0);
	lua_pop(L, 2);
};
