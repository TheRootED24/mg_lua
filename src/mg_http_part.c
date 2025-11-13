#include "mg_http_part.h"

//**** STRUCT MG_HTTP_PART USERDATUM */
/*
// Parameter for mg_http_next_multipart
struct mg_http_part {
  struct mg_str name;      // Form field name
  struct mg_str filename;  // Filename for file uploads
  struct mg_str body;      // Part contents
};
*/

int _mg_http_part_new (lua_State *L) {
	http_part *hp = NULL;
	int nargs = lua_gettop(L);
	
	if(nargs > 0) {
		int pos = nargs > 1 ? -1 : 1;
		if(lua_istable(L, 1)) {
			lua_getfield(L, 1, "ctx");
			pos = -1;
		}

		hp = (http_part*)lua_touserdata(L, pos);
	}
	else {
		hp = (http_part *)lua_newuserdata(L, sizeof(http_part));
		memset(hp, 0 , sizeof(http_part));
	}

	luaL_getmetatable(L, "LuaBook.http_part");
	lua_setmetatable(L, -2);
	if(!hp) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

http_part *check_mg_http_part(lua_State *L, int pos) {
	if(lua_istable(L, pos)) {
		lua_getfield(L, pos, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.http_part");
	luaL_argcheck(L, ud != NULL, pos, "`mg_http_part' expected");

	return(http_part*)ud;
};

static int _http_part_name(lua_State *L) {
	int nargs = lua_gettop(L);
	http_part *hp = check_mg_http_part(L, 1); lua_remove(L, 1);
	if(nargs > 1) {
		if(lua_isstring(L, 1)) {
			//_mg_str_new(L)
			char *str = (char*)lua_tostring(L, 1);
			size_t len = strlen(str);

			strcpy(hp->name.buf, str);
			hp->name.len = len;
		}
		else
			hp->name = *check_mg_str(L, 1);
	}
	lua_pushlstring(L, hp->name.buf, hp->name.len);

	return 1;
};

static int _http_part_filename(lua_State *L) {
	int nargs = lua_gettop(L);
	http_part *hp = check_mg_http_part(L, 1); lua_remove(L, 1);
	if(nargs > 1) {
		if(lua_isstring(L, 1)) {

			char *str = (char*)lua_tostring(L, 1);
			size_t len = strlen(str);

			strcpy(hp->filename.buf, str);
			hp->filename.len = len;
		}
		else
			hp->filename = *check_mg_str(L, 1);
	}
	lua_pushlstring(L, hp->filename.buf, hp->filename.len);

	return 1;
};

static int _http_part_body(lua_State *L) {
	int nargs = lua_gettop(L);
	http_part *hp = check_mg_http_part(L, 1); lua_remove(L, 1);
	if(nargs > 1) {
		if(lua_isstring(L, 1)) {

			char *str = (char*)lua_tostring(L, 1);
			size_t len = strlen(str);

			strcpy(hp->body.buf, str);
			hp->body.len = len;
		}
		else
			hp->body = *check_mg_str(L, 1);
	}
	lua_pushlstring(L, hp->body.buf, hp->body.len);

	return 1;
};

static int _mg_http_part_new_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);
		
		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "name") == 0 ) 
				_http_part_name(L);
			else if(key && strcmp(key, "filename") == 0 ) 
				_http_part_filename(L);
			else if(key && strcmp(key, "body") == 0 ) 
				_http_part_body(L);
		}
	}

	return 0;
};

static int _mg_http_part_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "name") == 0 ) {
			_http_part_name(L);
		}
		else if(key && strcmp(key, "filename") == 0 ) {
			_http_part_filename(L);
		}
		else if(key && strcmp(key, "body") == 0 ) {
			_http_part_body(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			_mg_http_part_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_http_part_newt(lua_State * L) {
	_mg_http_part_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_http_part_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_http_part_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg http_part_lib_f [] = {
	{"ptr", 	_mg_http_part_new	},
	{"new", 	_mg_http_part_newt	},
	{NULL, NULL}
};

static const struct luaL_reg http_part_lib_m [] = {
	{"ptr", 	_mg_http_part_new	},
	{"new", 	_mg_http_part_newt	},
	{"name", 	_http_part_name		},
	{"filename", 	_http_part_filename	},
	{"body", 	_http_part_body		},
	{NULL, NULL}
};

void mg_open_mg_http_part(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, http_part_lib_m);
	lua_setfield(L, -2, "part");
	// mg_http_part
	luaL_newmetatable(L, "LuaBook.http_part");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, http_part_lib_m, 0);
	luaL_openlib(L, "http_part", http_part_lib_f, 0);
	lua_pop(L, 2);
};
