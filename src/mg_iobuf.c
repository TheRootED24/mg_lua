#include"mg_iobuf.h"

static int _mg_iobuf_index(lua_State *L);
static int _mg_iobuf_new_index(lua_State *L);
static int _mg_iobuf_gc(lua_State *L);

// MG_IOBUF USERDATUM
int _mg_iobuf_new(lua_State *L) {
	mg_iobuf *io = NULL;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		int pos = nargs > 1 ? -1 : 1;
		if(lua_istable(L, 1)) {
			lua_getfield(L, 1, "ctx");
			// pop the table and return the pointer
			lua_remove(L, 1);
			
			return 1;
		}
		
		io = (mg_iobuf*)lua_touserdata(L, pos);
	}
	else {
		io = (mg_iobuf*)lua_newuserdata(L, sizeof(mg_iobuf));
		memset(io, 0 , sizeof(mg_iobuf));
	}

	luaL_getmetatable(L, "LuaBook.mg_iobuf");
	lua_setmetatable(L, -2);
	if(!io) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_iobuf *check_mg_iobuf(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_iobuf");
	luaL_argcheck(L, ud != NULL, pos, "`mg_iobuf' expected");

	return (mg_iobuf*)ud;
};

static int _mg_iobuf_buf(lua_State *L) {
	int nargs = lua_gettop(L);
	unsigned int ofs = 0;

	if(nargs > 1) {
		ofs = luaL_checkinteger(L, 2);
		lua_pop(L, 1);
	}
		
	mg_iobuf *io = check_mg_iobuf(L, 1);
	if(io)
		lua_pushlstring(L, (char*)io->buf, (io->len - ofs));
	else
		lua_pushnil(L);

	return 1;
};

static int _mg_iobuf_len(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	lua_pushinteger(L, (size_t)io->len);

	return 1;
};

static int _mg_iobuf_size(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	lua_pushinteger(L, (size_t)io->size);

	return 1;
};

static int _mg_iobuf_align(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	lua_pushinteger(L, (size_t)io->align);

	return 1;
};

// int mg_iobuf_init(struct mg_iobuf *io, size_t size, size_t align);
static int _mg_iobuf_init(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	bool ok = mg_iobuf_init(io, (size_t)luaL_checkinteger(L, 2), (size_t)luaL_checkinteger(L, 3));
	lua_pushboolean(L, ok);

	return 1;
};

// int mg_iobuf_resize(struct mg_iobuf *io, size_t size);
static int _mg_iobuf_resize(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	bool ok = (bool) mg_iobuf_resize(io, (size_t)luaL_checkinteger(L, 2));
	lua_pushinteger(L, ok);

	return 1;
};

// size_t mg_iobuf_add(struct mg_iobuf *io, size_t offset, const void *buf, size_t len);
static int _mg_iobuf_add(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	size_t wsize = mg_iobuf_add(io, (size_t)luaL_checkinteger(L, 2), (const void*)lua_tostring(L, 3), (size_t)luaL_checkinteger(L, 4));
	lua_pushnumber(L, wsize);

	return 1;
};

// size_t mg_iobuf_del(struct mg_iobuf *io, size_t offset, size_t len);
static int _mg_iobuf_del(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	size_t wsize = mg_iobuf_del(io, (size_t)luaL_checkinteger(L, 2), (size_t)luaL_checkinteger(L, 3));
	lua_pushnumber(L, wsize);

	return 1;
};

// void mg_iobuf_free(struct mg_iobuf *io);
static int _mg_iobuf_free(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	mg_iobuf_free(io);

	return 0;
};

int _mg_iobuf_newt(lua_State * L) {
	_mg_iobuf_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_pushcfunction(L, _mg_iobuf_init);
	lua_setfield(L, -2, "init");

	lua_pushcfunction(L, _mg_iobuf_add);
	lua_setfield(L, -2, "add");

	lua_pushcfunction(L, _mg_iobuf_del);
	lua_setfield(L, -2, "del");

	lua_pushcfunction(L, _mg_iobuf_resize);
	lua_setfield(L, -2, "resize");

	lua_pushcfunction(L, _mg_iobuf_free);
	lua_setfield(L, -2, "free");

	lua_pushcfunction(L, _mg_iobuf_buf);
	lua_setfield(L, -2, "getbuf");

	lua_pushcfunction(L, _mg_iobuf_len);
	lua_setfield(L, -2, "getlen");

	lua_pushcfunction(L, _mg_iobuf_size);
	lua_setfield(L, -2, "getsize");

	lua_pushcfunction(L, _mg_iobuf_align);
	lua_setfield(L, -2, "getalign");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_iobuf_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_iobuf_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _mg_iobuf_gc);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static int _mg_iobuf_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "buf") == 0 ) 
				_mg_iobuf_buf(L);
			else if(key && strcmp(key, "getlen") == 0 ) 
				_mg_iobuf_len(L);
			else if(key && strcmp(key, "getsize") == 0 ) 
				_mg_iobuf_size(L);
			else if(key && strcmp(key, "getalign") == 0 ) 
				_mg_iobuf_align(L);
		}
	}

	return 0;
};

static int _mg_iobuf_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "buf") == 0 ) {
			_mg_iobuf_buf(L);
		}
		else if(key && strcmp(key, "len") == 0 ) {
			_mg_iobuf_len(L);
		}
		else if(key && strcmp(key, "size") == 0 ) {
			_mg_iobuf_size(L);
		}
		else if(key && strcmp(key, "align") == 0 ) {
			_mg_iobuf_align(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_iobuf_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

static int _mg_iobuf_gc(lua_State *L) {
	mg_iobuf *io = NULL;
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		io = (mg_iobuf*)lua_topointer(L, 1);
	}
	else if(lua_islightuserdata(L, 1))
		io = (mg_iobuf*)lua_topointer(L, 1);

	if(io && io->buf) {
		mg_iobuf_free(io);

	}

    	return 0;
};

static const struct luaL_reg mg_iobuf_lib_f [] = {
	{"ptr", 	_mg_iobuf_new		},
	{"new", 	_mg_iobuf_newt		},
	{"init",	_mg_iobuf_init		},
	{NULL, NULL}
};

static const struct luaL_reg mg_iobuf_lib_m [] = {
	{"ptr", 	_mg_iobuf_new		},
	{"new", 	_mg_iobuf_newt		},
	{"init",	_mg_iobuf_init		},
	{"add",		_mg_iobuf_add		},
	{"resize",	_mg_iobuf_resize	},
	{"del",		_mg_iobuf_del		},
	{"buf", 	_mg_iobuf_buf		},
	{"len",		_mg_iobuf_len		},
	{"size",	_mg_iobuf_size		},
	{"align",	_mg_iobuf_align		},
	{"free",	_mg_iobuf_free		},
	{NULL, NULL}
};

void mg_open_mg_iobuf(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_iobuf_lib_m);
	lua_setfield(L, -2, "iobuf");
	// mg_iobuf
	luaL_newmetatable(L, "LuaBook.mg_iobuf");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _mg_iobuf_gc);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_iobuf_lib_m, 0);
	luaL_openlib(L, MG_IOBUF, mg_iobuf_lib_f, 0);
	lua_pop(L, 2);
};

