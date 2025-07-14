#include "mg_http_header.h"

//****  STRUCT MG_HTTP_HEADER USERDATUM ****//
/*
struct mg_http_header {
  struct mg_str name;   // Header name
  struct mg_str value;  // Header value
};
*/

typedef struct mg_http_header http_header;

int newheader (lua_State *L) {
	const char *name = NULL;
	const char *value = NULL;

	int nargs = lua_gettop(L);
	if(nargs == 2) {
		name = luaL_checkstring(L, 1);
		value = luaL_checkstring(L, 2);
		lua_settop(L, 0);
	}

	http_header *hdr = (http_header *)lua_newuserdata(L, sizeof(http_header));
	luaL_getmetatable(L, "LuaBook.http_header");
	lua_setmetatable(L, -2);

	if(name && value) {
		hdr->name = mg_str(name);
		hdr->value = mg_str(value);
	}
	if(!hdr) lua_pushnil(L);
	
	return 1;  /* new userdatum is already on the stack */
}

http_header *checkheader(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.http_header");
	luaL_argcheck(L, ud != NULL, 1, "`mg_http_header' expected");
	return (http_header*)ud;

}

static int _name(lua_State *L) {
	int index = lua_gettop(L);
	http_header *hdr = checkheader(L);
	if(index > 1)
		hdr->name = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hdr->name.buf, hdr->name.len);
	return 1;
}

static int _value(lua_State *L) {
	int index = lua_gettop(L);
	http_header *hdr = checkheader(L);
	if(index > 1)
		hdr->value = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hdr->value.buf, hdr->value.len);
	return 1;
}

static int _print_header(lua_State *L) {
	http_header *hdr = checkheader(L);

	lua_pushfstring(L, "name: %s\tvalue: %s",  hdr->name.buf, hdr->value.buf);
	return 1;
}

static int _header_json(lua_State *L) {
	http_header *hdr = checkheader(L);

	lua_pushfstring(L, "{\"name\": \"%s\", \"value\":\"%s\"}",  hdr->name.buf, hdr->value.buf);
	return 1;
}

static int _header_table(lua_State *L) {
	http_header *hdr = checkheader(L);
	lua_newtable(L);
	lua_pushlstring(L, hdr->name.buf, hdr->name.len);
	lua_setfield(L, -2, "name");
	lua_pushlstring(L, hdr->value.buf, hdr->value.len);
	lua_setfield(L, -2, "value");
	return 1;
}

static int _call_header(lua_State *L) {
	http_header *hdr = checkheader(L);

	lua_pushlstring(L, hdr->name.buf, hdr->name.len);
	lua_pushlstring(L, hdr->value.buf, hdr->value.len);
	return 2;
}

static void dumpstack (lua_State *L) {
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
}

static const struct luaL_reg http_header_lib_f [] = {
	{"new", newheader },
	{NULL, NULL}
};

static const struct luaL_reg http_header_lib_m [] = {
	{"__tostring",	_print_header	},
	{"__call",		_call_header	},
	{"table", 		_header_table	},
	{"json", 		_header_json	},
	{"name", 		_name		},
	{"value", 		_value		},
	{NULL, NULL}
};

void mg_open_mg_http_header(lua_State *L) {
	printf("START MG.HTTP.HEADER: \n");
	dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, http_header_lib_m);
	lua_setfield(L, -2, "header");
	// mg_mgr
	luaL_newmetatable(L, "LuaBook.http_header");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, http_header_lib_m, 0);
	luaL_openlib(L, "mg_http_header", http_header_lib_f, 0);
	lua_pop(L, 2);
	printf("END MG.HTTP.HEADER \n"); dumpstack(L);

}
