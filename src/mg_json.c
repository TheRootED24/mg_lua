#include "mg_json.h"

// int mg_json_get(struct mg_str json, const char *path, int *toklen);
static int _mg_json_get(lua_State *L) {
	struct mg_str *json = check_mg_str(L);
	const char *path = luaL_checkstring(L, 2);
	int toklen, offset = mg_json_get(*json, path, &toklen);
	lua_pushinteger(L, offset);
	lua_pushinteger(L, toklen);

	return 2;
}

// struct mg_str mg_json_get_tok(struct mg_str json, const char *path);
static int _mg_json_get_tok(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	struct mg_str str =  mg_json_get_tok(*json, path);
	lua_pushlstring(L, str.buf, str.len);

	return 1;
}

// bool mg_json_get_num(struct mg_str json, const char *path, double *v);
static int _mg_json_get_num(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	double b;
	bool found = mg_json_get_num(*json, path, &b);
	if(found)
		lua_pushnumber(L, b);

	return found;
}

// bool mg_json_get_bool(struct mg_str json, const char *path, bool *v);
static int _mg_json_get_bool(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	bool b = false;
	mg_json_get_bool(*json, path, &b);
	lua_pushboolean(L, b);

	return 1;
}

// long mg_json_get_long(struct mg_str json, const char *path, long default_val);
static int _mg_json_get_long(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	long l = luaL_optlong(L, 3, -1);
	l = mg_json_get_long(*json, path, l);
	lua_pushinteger(L, l);

	return 1;
}

// char *mg_json_get_str(struct mg_str json, const char *path);
static int _mg_json_get_str(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	char *res = mg_json_get_str(*json, path);
	if(res) {
		lua_pushstring(L, res);
		lua_pushinteger(L, (int)strlen(res));
		free(res);
		return 2;
	}
	else
		lua_pushnil(L);

	return 1;
}

// char *mg_json_get_hex(struct mg_str json, const char *path, int *len);
static int _mg_json_get_hex(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	int len = luaL_optinteger(L, 3, 0);
	char *res = mg_json_get_hex(*json, path, &len);
	if(res) {
		lua_pushstring(L, res);
		lua_pushinteger(L, len);
		free(res);
		return 2;
	}
	else
		lua_pushnil(L);

	return 1;
}

// char *mg_json_get_b64(struct mg_str json, const char *path, int *len);
static int _mg_json_get_b64(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	const char *path = luaL_checkstring(L, 2);
	int len = luaL_checkinteger(L, 3);
	char *res = mg_json_get_b64(*json, path, &len);
	if(res) {
		lua_pushstring(L, res);
		lua_pushinteger(L, len);
		free(res);
		return 2;
	}
	else
		lua_pushnil(L);

	return 1;
}

// bool mg_json_unescape(struct mg_str str, char *buf, size_t len);
static int _mg_json_unescape(lua_State *L) {
	struct mg_str *json = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	size_t len = luaL_checklong(L, 2);
	char buf[len];
	memset(buf, 0, len);
	bool found = mg_json_unescape( *json, buf, len);
	if(found){
		lua_pushboolean(L, found);
		lua_pushlstring(L, buf, len);
		return 2;
	}
	else {
		lua_pushboolean(L, found);
		lua_pushnil(L);
	}

	return 1;
}

// size_t mg_json_next(struct mg_str obj, size_t ofs, struct mg_str *key, struct mg_str *val);
static int _mg_json_next(lua_State *L) {
	struct mg_str *obj = check_mg_str(L); //mg_str(luaL_checkstring(L, 1));
	size_t ofs = (size_t)luaL_checkinteger(L, 2);
	struct mg_str *key = (struct mg_str *)lua_topointer(L, 3);
	struct mg_str *val = (struct mg_str *)lua_topointer(L, 4);

	size_t next = mg_json_next(*obj, ofs, key, val);
	lua_pushnumber(L, next);

	return 1;
}

static const struct luaL_reg mg_json_lib_m [] = {
	{"get",		_mg_json_get		},
	{"get_tok",	_mg_json_get_tok	},
	{"get_num",	_mg_json_get_num	},
	{"get_bool",	_mg_json_get_bool	},
	{"get_long",	_mg_json_get_long	},
	{"get_str",	_mg_json_get_str	},
	{"get_hex",	_mg_json_get_hex	},
	{"get_b64",	_mg_json_get_b64	},
	{"unescape",	_mg_json_unescape	},
	{"next",	_mg_json_next		},
	{NULL, NULL}
};

void mg_open_mg_json(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_json_lib_m);
	lua_setfield(L, -2, "json");
	// mg_json
	luaL_newmetatable(L, "LuaBook.mg_json");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
}