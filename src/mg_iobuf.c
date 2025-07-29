#include"mg_iobuf.h"

// MG_IOBUF USERDATUM
int new_mg_iobuf(lua_State *L) {
	mg_iobuf *io;
	int nargs = lua_gettop(L);

	if(nargs == 1) {
		io = (mg_iobuf*)lua_touserdata(L, 1);
		lua_pushlightuserdata(L, io);
	}
	else {

		unsigned char *buf = (unsigned char*)luaL_checkstring(L, 1);
		size_t len, size , align;
		len = (size_t)luaL_checkinteger(L, 2);
		size = (size_t)luaL_checkinteger(L, 3);
		align = (size_t)luaL_checkinteger(L, 4);
		lua_settop(L, 0);
		io = (mg_iobuf*)lua_newuserdata(L, sizeof(mg_iobuf));
		memset(io, 0 , sizeof(mg_iobuf));
		io->buf = buf;
		io->len = len;
		io->size = size;
		io->align = align;
	}

	luaL_getmetatable(L, "LuaBook.mg_iobuf");
	lua_setmetatable(L, -2);
	if(!io) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_iobuf *check_mg_iobuf(lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_iobuf");
	luaL_argcheck(L, ud != NULL, pos, "`mg_iobuf' expected");

	return (mg_iobuf*)ud;
};

static int _mg_iobuf_buf(lua_State *L) {
	mg_iobuf *io = check_mg_iobuf(L, 1);
	unsigned int ofs = luaL_optinteger(L, 2, 0);
	lua_pushlstring(L, (char*)io->buf, (io->len - ofs));

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

static const struct luaL_reg mg_iobuf_lib_f [] = {
	{"new", 	new_mg_iobuf		},
	{"init",	_mg_iobuf_init		},
	{NULL, NULL}
};

static const struct luaL_reg mg_iobuf_lib_m [] = {
	{"new", 	new_mg_iobuf		},
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
	luaL_openlib(L, NULL, mg_iobuf_lib_m, 0);
	luaL_openlib(L, MG_IOBUF, mg_iobuf_lib_f, 0);
	lua_pop(L, 2);
};

