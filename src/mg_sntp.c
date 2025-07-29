#include "mg_sntp.h"

// struct mg_connection *mg_sntp_connect(struct mg_mgr *mgr, const char *url, mg_event_handler_t fn, void *fn_data)
static int _mg_sntp_connect(lua_State *L) {
	mg_mgr *mgr = check_mg_mgr(L);
	const char *s_url = luaL_checkstring(L, 2);
	const char * cb = luaL_checkstring(L, 3);

	mg_event_handler_t fn = (mg_event_handler_t)fn_lua_cb;
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;

	lua_settop(L, 0); // clear the stack
	mg_connection *c = (mg_connection*)mg_sntp_connect(mgr, s_url, fn, GL);
	lua_pushlightuserdata(L, c);
	new_mg_connection(L); // push a new connection udata on stack
	check_mg_connection(L, 1); // check conn is ready

	return 1; // return the udata on the stack
};

// void mg_sntp_request(struct mg_connection *c)
static int _mg_sntp_request(lua_State *L) {
	mg_connection *conn = check_mg_connection(L, 1);
	mg_sntp_request(conn);

	return 0;
};

static const struct luaL_reg mg_sntp_lib_m [] = {
	{"connect",	_mg_sntp_connect	},
	{"request",	_mg_sntp_request	},
	{NULL, NULL}
};

void mg_open_mg_sntp(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_sntp_lib_m);
	lua_setfield(L, -2, "sntp");
	// mg_sntp
	luaL_newmetatable(L, "LuaBook.mg_sntp");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
};

