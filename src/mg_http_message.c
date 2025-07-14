#include "mg_http_message.h"

// **** STRUCT MG_HTTP_MESSAGE USERDATUM **** //
/*
struct mg_http_message {
  struct mg_str method, uri, query, proto;             // Request/response line
  struct mg_http_header headers[MG_MAX_HTTP_HEADERS];  // Headers
  struct mg_str body;                                  // Body
  struct mg_str head;                                  // Request + headers
  struct mg_str message;  // Request + headers + body
};
*/
typedef struct mg_http_message http_message;

int newmessage (lua_State *L) {
	http_message *hm;
	int nargs = lua_gettop(L);

	if(nargs > 0) {
		hm = (http_message*)lua_topointer(L, 1);
		lua_pop(L, 1);
		lua_pushlightuserdata(L, hm);
	 }
	else
		hm = (http_message*)lua_newuserdata(L, sizeof(http_message));

	luaL_getmetatable(L, "LuaBook.http_message");
	lua_setmetatable(L, -2);
	if(!hm) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

http_message *checkmessage(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.http_message");
	luaL_argcheck(L, ud != NULL, 1, "`mg_http_message' expected");
	return(http_message *)ud;
}

static int _method(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = checkmessage(L);
	if(index > 1)
		hm->method = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->method.buf, hm->method.len);
	return 1;
}

static int _uri(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = (http_message*)lua_topointer(L, 1);
	if(index > 1)
		hm->uri = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->uri.buf, hm->uri.len);
	return 1;
}

static int _query(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = checkmessage(L);
	if(index > 1)
		hm->query = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->query.buf, hm->query.len);
	return 1;
}

static int _proto(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = checkmessage(L);
	if(index > 1)
		hm->proto = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->proto.buf, hm->proto.len);
	return 1;
}

static int _message(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = checkmessage(L);
	if(index > 1)
		hm->message = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->message.buf, hm->message.len);
	return 1;
}

static int _head(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = checkmessage(L);
	if(index > 1)
		hm->head = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->head.buf, hm->head.len);
	return 1;
}

static int _headers(lua_State *L) {
	int sargs = lua_gettop(L);
	http_message *hm = checkmessage(L);
	//http_header *hdr;
	int index;

	if(sargs == 4) {
		index = luaL_checkinteger(L,2);
		hm->headers[index].name = mg_str(lua_tostring(L, 3));
		hm->headers[index].value = mg_str(lua_tostring(L, 4));
		lua_pushlstring(L, hm->headers[index].name.buf, hm->headers[index].name.len);
		lua_pushlstring(L, hm->headers[index].value.buf, hm->headers[index].value.len);
		return 2;
	}

	index = luaL_checkinteger(L, 2);
	lua_pushlstring(L, hm->headers[index].name.buf, hm->headers[index].name.len);
	lua_pushlstring(L, hm->headers[index].value.buf, hm->headers[index].value.len);
	return 2;
}

static int _body(lua_State *L) {
	int index = lua_gettop(L);
	http_message *hm = checkmessage(L);
	if(index > 1)
		hm->body = mg_str(luaL_checkstring(L, -1));

	lua_pushlstring(L, hm->body.buf, hm->body.len);
	return 1;
}

static int _print_message(lua_State *L) {
	http_message *hm = checkmessage(L);
	lua_pushfstring(L, "method: %s uri: %s query: %s proto: %s head: %s body: %s message: %s headers: %d}",
		hm->method.buf, hm->uri.buf, hm->proto.buf, hm->query.buf, hm->head.buf, hm->body.buf, hm->message.buf, (int)(sizeof(hm->headers)/sizeof(http_header)));
	return 1;
}

static int _message_json(lua_State *L) {
	http_message *hm = checkmessage(L);

	lua_pushfstring(L, "{\"method\": \"%s\", \"uri\":\"%s\"\"query\": \"%s\", \"proto\":\"%s\", \"head\": \"%s\", \"body\":\"%s\"\"message\": \"%s\", headers: %d}",
		hm->method.buf, hm->uri.buf, hm->proto.buf, hm->query.buf, hm->head.buf, hm->body.buf, hm->message.buf, (int)(sizeof(hm->headers)/sizeof(http_header)));
	return 1;
}

static int _message_table(lua_State *L) {
	http_message *hm = checkmessage(L);
	lua_newtable(L);
	lua_pushlstring(L, hm->method.buf, hm->method.len);
	lua_setfield(L, -2, "method");
	lua_pushlstring(L, hm->uri.buf, hm->uri.len);
	lua_setfield(L, -2, "uri");
	lua_pushlstring(L, hm->query.buf, hm->query.len);
	lua_setfield(L, -2, "query");
	lua_pushlstring(L, hm->proto.buf, hm->proto.len);
	lua_setfield(L, -2, "proto");
	lua_pushlstring(L, hm->head.buf, hm->head.len);
	lua_setfield(L, -2, "head");
	lua_pushlstring(L, hm->body.buf, hm->body.len);
	lua_setfield(L, -2, "body");
	lua_pushlstring(L, hm->message.buf, hm->message.len);
	lua_setfield(L, -2, "message");
	lua_pushinteger(L, (int)(sizeof(hm->headers)/sizeof(http_header)));
	lua_setfield(L, -2, "headers");
	return 1;
}

static int _call_message(lua_State *L) {
	http_message *hm = checkmessage(L);

	lua_pushlstring(L, hm->method.buf, hm->method.len);
	lua_pushlstring(L, hm->uri.buf, hm->uri.len);
	lua_pushlstring(L, hm->query.buf, hm->query.len);
	lua_pushlstring(L, hm->proto.buf, hm->proto.len);
	lua_pushlstring(L, hm->head.buf, hm->head.len);
	lua_pushlstring(L, hm->body.buf, hm->body.len);
	lua_pushlstring(L, hm->message.buf, hm->message.len);
	lua_pushinteger(L, sizeof(hm->headers));
	return 8;
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

static const struct luaL_reg messagelib_f [] = {
	{"new", 		newmessage	},
	{NULL, NULL}
};

static const struct luaL_reg messagelib_m [] = {
	{"__tostring",	_print_message	},
	{"__call",		_call_message	},
	{"table", 		_message_table	},
	{"json", 		_message_json	},
	{"new", 		newmessage		},
	{"method", 		_method			},
	{"uri", 		_uri			},
	{"query", 		_query			},
	{"proto", 		_proto			},
	{"head", 		_head			},
	{"headers", 	_headers		},
	{"body", 		_body			},
	{"message", 	_message		},
	{NULL, NULL}
};

void  mg_open_mg_http_message (lua_State *L) {
	printf("START MG.HTTP.MESSAGE: \n");
	lua_getfield(L, -1, "http");
	dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, messagelib_m);
	lua_setfield(L, -2, "message");

	// http_message
	luaL_newmetatable(L, "LuaBook.http_message");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, messagelib_m, 0);
	luaL_openlib(L, "mg_http_message", messagelib_f, 0);
	lua_pop(L, 2);
	printf("END MG.HTTP.MESSAGE:\n"); dumpstack(L);

}
// require "mg" for k,v in pairs(mg) do print(k,v) end