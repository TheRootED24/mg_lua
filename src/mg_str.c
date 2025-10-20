#include "mg_str.h"

static int _mg_str_index(lua_State *L);
static int _mg_str_new_index(lua_State *L);
static int _mg_str_gc(lua_State *L);

int _mg_str_new (lua_State *L) {
	int nargs = lua_gettop(L);
	mg_str *str = NULL;

	if(nargs > 0) {
		//case s = mg.str.new(ex-s)
		if(lua_istable(L, 1)) {
			lua_getfield(L, 1, "ctx");
			// pop the table and return the pointer
			lua_remove(L, 1);

			return 1;
		}
		// cast existing mg_str pointer to mg_str userdatum type
		else if(lua_isuserdata(L, 1) && nargs == 1) { 
			str = lua_touserdata(L, -1);
		}
		else {
			// create a new mg_str userdatum and initialize it
			str = (mg_str *)lua_newuserdata(L, sizeof(mg_str));
			memset(str, 0, sizeof(mg_str));
		}

		// new str with a null terminated string arg
		// case s = mg.str.new("string")
		if(lua_isstring(L, 1) && nargs == 1) {
			str->buf = (char*)luaL_checkstring(L, 1); 
			str->len = strlen(str->buf);
			lua_remove(L, 1);
		}
		// non null terminated string with user supplied length
		// case	s = mg.str.new(string, len)
		else if(lua_isstring(L, 1) && lua_isnumber(L, 2)) {
			str->buf = (char*)luaL_checkstring(L, 1); 
			str->len = (size_t)luaL_checklong(L, 2);
			lua_remove(L, 2);
			lua_remove(L, 1);
		}
	}
	else {
		// new initialized empty mg_str userdatum
		//case s = mg.str.new()
		str = (mg_str *)lua_newuserdata(L, sizeof(mg_str));
		memset(str, 0, sizeof(mg_str));
	}

	luaL_getmetatable(L, "LuaBook.mg_str");
	lua_setmetatable(L, -2);
	
	if(!str) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

int _mg_str_to_str(lua_State *L) {
	mg_str *data = (mg_str*)lua_topointer(L, 1);

	lua_pushlstring(L, data->buf, data->len);

	return 1;
}

mg_str *check_mg_str(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_str");
	luaL_argcheck(L, ud != NULL, pos, "`mg_str' expected");

	return(mg_str*)ud;
};

int _mg_str_newt(lua_State * L) {
	_mg_str_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_str_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_str_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _mg_str_gc);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static int _mg_str_buf(lua_State *L) {
	int nargs = lua_gettop(L);
	mg_str *str = check_mg_str(L , 1);
	if(nargs == 2) {
		str->buf = (char*)luaL_checkstring(L, 2);
		str->len = strlen(str->buf);
	}
	else if(nargs == 3){
		str->buf = (char*)luaL_checkstring(L, 2);
		str->len = (size_t)luaL_checklong(L, 3);
	}

	lua_pushlstring(L, str->buf, str->len);

	return 1;
};

static int _mg_str_len(lua_State *L) {
	mg_str *str = check_mg_str(L , 1);
	lua_pushinteger(L, (size_t)str->len);

	return 1;
};

static int _mg_str_new_index(lua_State *L) {
	const char *key = luaL_checkstring(L, 2); // stack : {table, key, value}
	lua_remove(L, 2);

	if(lua_istable(L, 1)) { // stack : {table, value}
	
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "buf") == 0 ) 
				_mg_str_buf(L);
			else if(key && strcmp(key, "len") == 0 ) 
				_mg_str_len(L);
		}
	}

	return 0;
};

static int _mg_str_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2); // stack : {table, key}
		lua_pop(L, 1);

		if(key && strcmp(key, "buf") == 0 ) { // stack : { table }
			_mg_str_buf(L);
		}
		else if(key && strcmp(key, "len") == 0 ) {
			_mg_str_len(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_str_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

static int _mg_str_gc(lua_State *L) {
	mg_str *str = NULL;
	printf("********************************* Destroy mg_str *********************************\n");

	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		str = (mg_str*)lua_topointer(L, 1);
	}
	else if(lua_isuserdata(L, 1))
		str = (mg_str*)lua_topointer(L, 1);

	if(str && str->buf) {
		free(str);
		//str->len = 0;
	}

    	return 0;
};

static const struct luaL_reg mg_str_lib_f [] = {
	{"new", 	_mg_str_new	},
	{"newt",	_mg_str_newt	},
	{NULL, NULL}
};

static const struct luaL_reg mg_str_lib_m [] = {
	{"new",		_mg_str_new	},
	{"newt",	_mg_str_newt	},
	{"buf",		_mg_str_buf	},
	{"length",	_mg_str_len	},
	{"to_str",	_mg_str_to_str	},
	{NULL, NULL}
};

void  mg_open_mg_str (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_str_lib_m);
	lua_setfield(L, -2, "str");
	// mg_str
	luaL_newmetatable(L, "LuaBook.mg_str");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	//lua_pushstring(L, "__gc");
	//lua_pushcfunction(L, _mg_str_gc);  /* pushes the metatable */
	//lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_str_lib_m, 0);
	luaL_openlib(L, "mg_str", mg_str_lib_f, 0);
	lua_pop(L, 2);
};
