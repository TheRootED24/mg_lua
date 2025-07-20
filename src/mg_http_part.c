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

int newpart (lua_State *L) {
	http_part *hp = (http_part *)lua_newuserdata(L, sizeof(http_part));

	luaL_getmetatable(L, "LuaBook.http_part");
	lua_setmetatable(L, -2);
	if(!hp) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

http_part *checkpart(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.http_part");
	luaL_argcheck(L, ud != NULL, 1, "`mg_http_part' expected");
	return(http_part*)ud;
}

static int _http_part_name(lua_State *L) {
	int nargs = lua_gettop(L);
	http_part *hp = checkpart(L);
	if(nargs > 1)
		hp->name = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hp->name.buf, hp->name.len);
	return 1;
}

static int _http_part_filename(lua_State *L) {
	int nargs = lua_gettop(L);
	http_part *hp = checkpart(L);
	if(nargs > 1)
		hp->filename = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hp->filename.buf, hp->filename.len);
	return 1;
}

static int _http_part_body(lua_State *L) {
	int nargs = lua_gettop(L);
	http_part *hp = checkpart(L);
	if(nargs > 1)
		hp->body = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hp->body.buf, hp->body.len);
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


static const struct luaL_reg http_part_lib_f [] = {
	{"new", newpart },
	{NULL, NULL}
};

static const struct luaL_reg http_part_lib_m [] = {
	//{"__tostring",	_print_header	},
	//{"__call",		_call_header	},
	{"name", 			_http_part_name	},
	{"filename", 		_http_part_filename	},
	{"body", 			_http_part_body	},
	{NULL, NULL}
};

void mg_open_mg_http_part(lua_State *L) {
	//printf("START MG.HTTP.PART: \n");dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, http_part_lib_m);
	lua_setfield(L, -2, "part");
	// mg_http_part
	luaL_newmetatable(L, "LuaBook.http_part");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, http_part_lib_m, 0);
	luaL_openlib(L, "mg_http_part", http_part_lib_f, 0);
	lua_pop(L, 2);
	//printf("END MG.HTTP.PART: \n");dumpstack(L);
}
