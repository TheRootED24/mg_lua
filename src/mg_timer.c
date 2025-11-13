#include "mg_timer.h"

/*
struct mg_timer {
  uint64_t period_ms;       // Timer period in milliseconds
  uint64_t expire;          // Expiration timestamp in milliseconds
  unsigned flags;           // Possible flags values below
#define MG_TIMER_ONCE 0     // Call function once
#define MG_TIMER_REPEAT 1   // Call function periodically
#define MG_TIMER_RUN_NOW 2  // Call immediately when timer is set
  void (*fn)(void *);       // Function to call
  void *arg;                // Function argument
  struct mg_timer *next;    // Linkage
};
*/

int _mg_timer_new(lua_State *L) {
	mg_timer *tmr = (mg_timer*)lua_newuserdata(L, sizeof(mg_timer));

	luaL_getmetatable(L, "LuaBook.mg_timer");
	lua_setmetatable(L, -2);
	if(!tmr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
};

mg_timer *check_mg_timer(lua_State *L, int pos) {
	void *ud = luaL_checkudata(L, pos, "LuaBook.mg_timer");
	luaL_argcheck(L, ud != NULL, pos, "`mg_timer' expected");

	return(mg_timer*)ud;
};

// struct mg_timer *mg_timer_add(struct mg_mgr *mgr, uint64_t period_ms, unsigned flags, void (*fn)(void *), void *fn_data);
static int _mg_timer_add(lua_State *L) {
	void *arg = NULL;
	int nargs = lua_gettop(L);
	mg_mgr *mgr = check_mg_mgr(L);
	uint64_t period_ms = luaL_checkinteger(L, 2);
	unsigned flags = 0;
	uint8_t flag = 0;
	// parse flags
	for( int i = 0; i < 3; i++) {
		if(lua_type(L, (3 + i)) == LUA_TNUMBER) {
			if(i > 0) lua_remove(L, 3);
			flag = (uint8_t)lua_tointeger(L, 3);

			if(i == 0)
				flags = flag;
			else
				flags = flags | flag;
		}
		else
			break;
	}

	const char *cb = luaL_checkstring(L, 4);

	if(nargs > 4)
		arg = (void*)lua_topointer(L, 5);

	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	GL->callback = cb;
	GL->fn_data = arg;

	mg_timer_add(mgr, period_ms, flags, fn_lua_timer, GL);

	return 0;
};

// void mg_timer_init(struct mg_timer **head, struct mg_timer *t, uint64_t period_ms, unsigned flags, void (*fn)(void *), void *fn_data);
static int _mg_timer_init(lua_State *L) {
	mg_timer *head = check_mg_timer(L, 1);
	lua_remove(L, 1);
	mg_timer *t = check_mg_timer(L, 1);
	uint64_t period_ms = luaL_checknumber(L, 2);
	unsigned flags = luaL_checkint(L, 3);
	const char *cb = luaL_checkstring(L, 4);

	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; 		// pass the lua_State pointer to fn_serv
	GL->callback = cb;	 // pass the lua callbaack
	GL->fn_data = NULL;
	mg_timer_init(&head, t, period_ms, flags, fn_lua_timer, GL );

	return 0;
};

// void mg_timer_free(struct mg_timer **head, struct mg_timer *t);
static int _mg_timer_free(lua_State *L) {
	mg_timer *head = check_mg_timer(L, 1);
	lua_remove(L, 1);
	mg_timer *t = check_mg_timer(L, 1);
	mg_timer_free(&head, t);

	return 0;
};

// void mg_timer_poll(struct mg_timer **head, uint64_t uptime_ms);
static int _mg_timer_poll(lua_State *L) {
	mg_timer *head = check_mg_timer(L, 1);
	uint64_t uptime_ms = luaL_checknumber(L, 2);
	mg_timer_poll(&head, uptime_ms);

	return 0;
};

static const struct luaL_reg mg_timer_lib_f [] = {
	{"new", 	_mg_timer_new	},
	{NULL, NULL}
};

static const struct luaL_reg mg_timer_lib_m [] = {
	{"new",		_mg_timer_new	},
	{"add",		_mg_timer_add	},
	{"init",	_mg_timer_init	},
	{"poll",	_mg_timer_poll	},
	{"free",	_mg_timer_free	},
	{NULL, NULL}
};

void mg_open_mg_timer(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mg_timer_lib_m);
	lua_setfield(L, -2, "timer");
	// mg_timer
	luaL_newmetatable(L, "LuaBook.mg_timer");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	luaL_openlib(L, NULL, mg_timer_lib_m, 0);
	luaL_openlib(L, MGTIMER, mg_timer_lib_f, 0);
	lua_pop(L, 2);
};
