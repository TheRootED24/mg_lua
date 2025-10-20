#include "mg_fd.h"

static int _mg_fd_index(lua_State *L);
static int _mg_fd_new_index(lua_State *L);

int _mg_fd_new (lua_State *L) {
	mg_fd *fd;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		fd = (mg_fd*)lua_touserdata(L, 1);
	}
	else
		fd = (mg_fd*)lua_newuserdata(L, sizeof(mg_fd));

	luaL_getmetatable(L, "LuaBook.mg_fd");
	lua_setmetatable(L, -2);

	if(!fd) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

/*
int _mg_fd_new (lua_State *L) {
	mg_fd *fd = (mg_fd*)lua_newuserdata(L, sizeof(mg_fd));
	luaL_getmetatable(L, "LuaBook.mg_fd");
	lua_setmetatable(L, -2);
	if(!fd) lua_pushnil(L);

	return 1;  
};
*/


mg_fd *check_mg_fd(lua_State *L, int pos) {
	if(lua_istable(L, 1)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_fd");
	luaL_argcheck(L, ud != NULL, pos, "`mg_fd' expected");

	return (mg_fd*)ud;
};

static int _mg_fd_newt(lua_State * L) {
	_mg_fd_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_fd_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_fd_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static int _mg_fd_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		printf("KEY: %s CANNOT BE UPDATED !\n", key);
		//lua_pop(L, 1); // stack: { table }
		/*
		if(strcmp(key, "ctx") != 0) {
                	if(key && strcmp(key, "url") == 0 ) {
				printf("HERE NOW\n");
				_mg_dns_url(L); // stack: { table, key }
			}
			else if(key && strcmp(key, "conn") == 0 ) {
				_mg_dns_conn(L);
			}
		}*/ // currently has no keys
	}

	return 0;
};

static int _mg_fd_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		/*if(key && strcmp(key, "url") == 0 ) {
			printf("HERE NOW\n");
			_mg_dns_url(L);
		}
		else if(key && strcmp(key, "conn") == 0 ) {
			_mg_dns_conn(L);
		}
		else*/ // currently has no values
		if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);
			_mg_fd_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};



static const struct luaL_reg mg_fd_lib_f [] = {
	{"new", 	_mg_fd_new	},
	{"newt",	_mg_fd_newt	},
	{NULL, NULL}
};

static const struct luaL_reg mg_fd_lib_m [] = {
	{"new",		_mg_fd_new	},
	{"newt",	_mg_fd_newt	},
	{NULL, NULL}
};

void mg_open_mg_fd (lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_fd_lib_m);
	lua_setfield(L, -2, "fd");
	// mg_fd
	luaL_newmetatable(L, "LuaBook.mg_fd");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_fd_lib_m, 0);
	luaL_openlib(L, "mg_fd", mg_fd_lib_f, 0);
	lua_pop(L, 2);
};
