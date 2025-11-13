#include "mg_http_message.h"

// **** STRUCT MG_HTTP_MESSAGE USERDATUM **** //
/*
struct mg_http_message {
  struct mg_str method, uri, query, proto;             // Request/response line
  struct mg_http_header headers[MG_MAX_HTTP_HEADERS];  // Headers
  struct mg_str body;                                  // Body
  struct mg_str head;                                  // Request + headers
  struct mg_str message;                               // Request + headers + body
};
*/

int _mg_http_message_new (lua_State *L) {
	http_message *hm;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		int pos = nargs > 1 ? -1 : 1;
		if(lua_istable(L, 1)) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);

			return 1;
		}
		
		hm = (http_message*)lua_touserdata(L, pos);
	}
	else
		hm = (http_message*)lua_newuserdata(L, sizeof(http_message));

	luaL_getmetatable(L, "LuaBook.http_message");
	lua_setmetatable(L, -2);

	if(!hm) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

http_message *check_mg_http_message(lua_State *L, int pos) {
	if(lua_istable(L, pos)) {
		lua_getfield(L, pos, "ctx");
		pos = -1;
	}

	void *ud = luaL_checkudata(L, pos, "LuaBook.http_message");
	luaL_argcheck(L, ud != NULL, pos, "`mg_http_message' expected");

	return(http_message *)ud;
	
};

static int _method(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->method = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->method.buf, hm->method.len);

	return 1;
};

static int _uri(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->uri = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->uri.buf, hm->uri.len);

	return 1;
};

static int _query(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->query = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->query.buf, hm->query.len);

	return 1;
};

static int _proto(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->proto = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->proto.buf, hm->proto.len);

	return 1;
};

static int _message(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->message = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->message.buf, hm->message.len);

	return 1;
};

static int _head(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->head = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->head.buf, hm->head.len);

	return 1;
};

static int _headers(lua_State *L) {
	int nargs = lua_gettop(L);
	int index = lua_tointeger(L,2);
	http_message *hm = check_mg_http_message(L, 1);
	lua_remove(L, 2); lua_remove(L, 1);

	if(nargs > 2) {
		http_header *hdr = check_mg_http_header(L, 1);
		hm->headers[index].name = mg_str(hdr->name.buf);
		hm->headers[index].value = mg_str(hdr->value.buf);

		return 0;
	}

	lua_remove(L, 1);
	lua_pushlstring(L, hm->headers[index].name.buf, hm->headers[index].name.len);
	lua_pushlstring(L, hm->headers[index].value.buf, hm->headers[index].value.len);
	_mg_http_header_newt(L);

	return 1;
};

static int _body(lua_State *L) {
	int nargs = lua_gettop(L);
	http_message *hm = check_mg_http_message(L, 1);
	if(nargs > 1) {
		hm->body = mg_str(luaL_checkstring(L, 2));

		return 0;
	}
	lua_pushlstring(L, hm->body.buf, hm->body.len);

	return 1;
};

static int _mg_http_headers_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		_headers(L);
	}
		
	return 0;
};

static int _mg_http_headers_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		_headers(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

static int _mg_http_message_new_index(lua_State *L) {
	if(lua_istable(L, 1)) { // stack : {table, key}
		const char *key = luaL_checkstring(L, 2);

		if(strcmp(key, "ctx") != 0) {
			if(key && strcmp(key, "method") == 0 ) {
			_method(L);
			}
			else if(key && strcmp(key, "uri") == 0 ) {
				_uri(L);
			}
			else if(key && strcmp(key, "query") == 0 ) {
				_query(L);
			}
			else if(key && strcmp(key, "proto") == 0 ) {
				_proto(L);
			}
			else if(key && strcmp(key, "head") == 0 ) {
				_head(L);
			}
			else if(key && strcmp(key, "body") == 0 ) {
				_body(L);
			}
			else if(key && strcmp(key, "message") == 0 ) {
				_message(L);
			}
		}
	}

	return 0;
};

static int _mg_http_message_index(lua_State *L) {
	if(lua_istable(L, 1)) {
		const char *key = luaL_checkstring(L, 2);
		lua_pop(L, 1);

		if(key && strcmp(key, "method") == 0 ) {
			_method(L);
		}
		else if(key && strcmp(key, "uri") == 0 ) {
			_uri(L);
		}
		else if(key && strcmp(key, "query") == 0 ) {
			_query(L);
		}
		else if(key && strcmp(key, "proto") == 0 ) {
			_proto(L);
		}
		else if(key && strcmp(key, "head") == 0 ) {
			_head(L);
		}
		else if(key && strcmp(key, "body") == 0 ) {
			_body(L);
		}
		else if(key && strcmp(key, "message") == 0 ) {
			_message(L);
		}
		else if(key && strcmp(key, "ctx") == 0 ) {
			lua_getfield(L, 1, "ctx");
			lua_remove(L, 1);
			_mg_http_message_new(L);
		}
		else
			lua_pushnil(L);

		return 1;
	}
	lua_pushnil(L);

	return 1;
};

int _mg_http_message_newt(lua_State * L) {
	_mg_http_message_new(L);

	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_pushstring(L, "headers");
	lua_createtable(L, MG_MAX_HTTP_HEADERS, 1);
	lua_settable(L, -3);

	lua_getfield(L, 2, "headers");
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "ctx");

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_http_headers_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_http_headers_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);
	lua_pop(L, 1);

	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, _mg_http_message_index);
	lua_settable(L, -3); // set the __index in the metatable (-3)

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, _mg_http_message_new_index);
	lua_settable(L, -3); // set the __newindex in the metatable (-3)

	lua_setmetatable(L, -2);

	return 1;
};

static const struct luaL_reg messagelib_f [] = {
	{"ptr", 	_mg_http_message_new	},
	{"new", 	_mg_http_message_newt	},
	{NULL, NULL}
};

static const struct luaL_reg messagelib_m [] = {
	{"ptr", 	_mg_http_message_new	},
	{"new", 	_mg_http_message_newt	},
	{"method", 	_method			},
	{"uri", 	_uri			},
	{"query", 	_query			},
	{"proto", 	_proto			},
	{"head", 	_head			},
	{"getheaders", 	_headers		},
	{"body", 	_body			},
	{"message", 	_message		},
	{NULL, NULL}
};

void  mg_open_mg_http_message (lua_State *L) {
	lua_getfield(L, -1, "http");
	lua_newtable(L);
	luaL_register(L, NULL, messagelib_m);
	lua_setfield(L, -2, "message");
	// mg_http_message
	luaL_newmetatable(L, "LuaBook.http_message");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, messagelib_m, 0);
	luaL_openlib(L, "mg_http_message", messagelib_f, 0);
	lua_pop(L, 2);
};
