#include "mg_http_header.h"

//****  STRUCT MG_HTTP_HEADER USERDATUM ****//
/*
struct mg_http_header {
  struct mg_str name;   // Header name
  struct mg_str value;  // Header value
};
*/

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

static int _mg_http_header_index(lua_State *L);
static int _mg_http_header_new_index(lua_State *L);

int _mg_http_header_new (lua_State *L) {
	http_header *hdr = NULL;

	int nargs = lua_gettop(L);
	if(nargs > 0) {
		//int pos = nargs > 1 ? -1 : 1;
		if(lua_istable(L, 1)) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);

			return 1;
		}
		else if(lua_isuserdata(L, 1))
			hdr = (http_header*)lua_touserdata(L, 1);
	
		else {
			hdr = (http_header *)lua_newuserdata(L, sizeof(http_header));
			memset(hdr, 0, sizeof(http_header));
		}
		//printf("HERE: \n"); dumpstack(L);

		if(nargs == 2) { // fix check mg_str --> str needs work aswell

			if(lua_isstring(L, 1)) {
				hdr->name = mg_str(luaL_checkstring(L, 1));
				lua_remove(L, 1);
			}

			if(lua_isstring(L, 1)) {
				hdr->value = mg_str(luaL_checkstring(L, 1));
				lua_remove(L, 1);
			}

			//printf("HERE2: \n"); dumpstack(L);
		}
		
	}
	else {
		hdr = (http_header *)lua_newuserdata(L, sizeof(http_header));
		memset(hdr, 0, sizeof(http_header));
	}

	luaL_getmetatable(L, "LuaBook.http_header");
	lua_setmetatable(L, -2);

	if(!hdr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

http_header *check_mg_http_header(lua_State *L, int pos) {
	//dumpstack(L);
	if(lua_istable(L, pos)) {
		lua_getfield(L, 1, "ctx");
		pos = -1;
		//lua_remove(L, 1);
	}
	
	void *ud = luaL_checkudata(L, pos, "LuaBook.http_header");
	luaL_argcheck(L, ud != NULL, pos, "`mg_http_header' expected");

	return (http_header*)ud;

};

int _mg_http_header_newt(lua_State * L) {
	_mg_http_header_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_http_header_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_http_header_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static int _name(lua_State *L) {
	int index = lua_gettop(L);
	http_header *hdr = check_mg_http_header(L, 1);
	if(index > 1)
		hdr->name = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, hdr->name.buf, hdr->name.len);

	return 1;
};

static int _value(lua_State *L) {
	int index = lua_gettop(L);
	http_header *hdr = check_mg_http_header(L, 1);
	if(index > 1)
		hdr->value = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, hdr->value.buf, hdr->value.len);

	return 1;
};

static int _print_header(lua_State *L) {
	http_header *hdr = check_mg_http_header(L, 1);

	lua_pushfstring(L, "name: %s\tvalue: %s",  hdr->name.buf, hdr->value.buf);
	return 1;
};

static int _header_json(lua_State *L) {
	http_header *hdr = check_mg_http_header(L, 1);

	lua_pushfstring(L, "{\"name\": \"%s\", \"value\":\"%s\"}",  hdr->name.buf, hdr->value.buf);

	return 1;
};

static int _header_table(lua_State *L) {
	http_header *hdr = check_mg_http_header(L, 1);
	lua_newtable(L);
	lua_pushlstring(L, hdr->name.buf, hdr->name.len);
	lua_setfield(L, -2, "name");
	lua_pushlstring(L, hdr->value.buf, hdr->value.len);
	lua_setfield(L, -2, "value");

	return 1;
};

static int _call_header(lua_State *L) {
	http_header *hdr = check_mg_http_header(L, 1);

	lua_pushlstring(L, hdr->name.buf, hdr->name.len);
	lua_pushlstring(L, hdr->value.buf, hdr->value.len);

	return 2;
};

static int _mg_http_header_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);
		lua_remove(L, 2);

		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "name") == 0 ) {
				//dumpstack(L);
				_name(L);
			}
			else if(key && strcmp(key, "value") == 0 ) {
				_value(L);
			}
		}
	}

	return 0;
};

static int _mg_http_header_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "name") == 0 ) {
			_name(L);
		}
		else if(key && strcmp(key, "value") == 0 ) {
			_value(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			//lua_remove(L, 1);
			_mg_http_header_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

static const struct luaL_reg http_header_lib_f [] = {
	{"new", 	_mg_http_header_new	},
	{"newt", 	_mg_http_header_newt	},
	{"name", 	_name			},
	{"value", 	_value			},
	{NULL, NULL}
};

static const struct luaL_reg http_header_lib_m [] = {
	{"newt", 	_mg_http_header_newt	},
	{"__tostring",	_print_header		},
	{"__call",	_call_header		},
	{"table", 	_header_table		},
	{"json", 	_header_json		},
	{"name", 	_name			},
	{"value", 	_value			},
	{NULL, NULL}
};

void mg_open_mg_http_header(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, http_header_lib_m);
	lua_setfield(L, -2, "header");
	// mg_http_header
	luaL_newmetatable(L, "LuaBook.http_header");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, http_header_lib_m, 0);
	luaL_openlib(L, "http_header", http_header_lib_f, 0);
	lua_pop(L, 2);
};
