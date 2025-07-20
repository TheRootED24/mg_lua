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

void dummy(void *data){
	printf("data address :%p\n", data);
}

int new_mg_timer(lua_State *L) {
	mg_timer *tmr = (mg_timer*)lua_newuserdata(L, sizeof(mg_timer));

	luaL_getmetatable(L, "LuaBook.mg_timer");
	lua_setmetatable(L, -2);
	if(!tmr) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}

mg_timer *check_mg_timer(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_timer");
	luaL_argcheck(L, ud != NULL, 1, "`mg_timer' expected");
	return(mg_timer*)ud;
}

// struct mg_timer *mg_timer_add(struct mg_mgr *mgr, uint64_t period_ms, unsigned flags, void (*fn)(void *), void *fn_data);
static int _mg_timer_add(lua_State *L) {
	mg_mgr *mgr = checkmgr(L);
	uint64_t period_ms = luaL_checknumber(L, 2);
	unsigned flags = luaL_checkint(L, 3);
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	new_mg_timer(L);
	mg_timer *tmr = mg_timer_add(mgr, period_ms, flags, dummy, GL);
	if(!tmr) lua_pushnil(L);

	return 1;
}

// void mg_timer_init(struct mg_timer **head, struct mg_timer *t, uint64_t period_ms, unsigned flags, void (*fn)(void *), void *fn_data);
static int _mg_timer_init(lua_State *L) {
	mg_timer *head = check_mg_timer(L);
	lua_remove(L, 1);
	mg_timer *t = check_mg_timer(L);
	uint64_t period_ms = luaL_checknumber(L, 2);
	unsigned flags = luaL_checkint(L, 3);
	lua_State_t *GL = ((lua_State_t*)lua_newuserdata(L, sizeof(lua_State_t)));
	GL->L = L; // pass the lua_State pointer to fn_serv
	mg_timer_init(&head, t, period_ms, flags, dummy, GL );
	return 0;
}

// void mg_timer_free(struct mg_timer **head, struct mg_timer *t);
static int _mg_timer_free(lua_State *L) {
	mg_timer *head = check_mg_timer(L);
	lua_remove(L, 1);
	mg_timer *t = check_mg_timer(L);
	mg_timer_free(&head, t);
	return 0;
}

// void mg_timer_poll(struct mg_timer **head, uint64_t uptime_ms);
static int _mg_timer_poll(lua_State *L) {
	mg_timer *head = check_mg_timer(L);
	uint64_t uptime_ms = luaL_checknumber(L, 2);
	mg_timer_poll(&head, uptime_ms);
	return 0;
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

static const struct luaL_reg mg_timer_lib_f [] = {
	{"new", 	new_mg_timer	},
	{NULL, NULL}
};

static const struct luaL_reg mg_timer_lib_m [] = {
	{"new",		new_mg_timer	},
	{"add",		_mg_timer_add	},
	{"init",	_mg_timer_init	},
	{"poll",	_mg_timer_poll	},
	{"free",	_mg_timer_free	},
	{NULL, NULL}
};

void mg_open_mg_timer(lua_State *L) {
	//printf("START MG.TIMER: \n"); dumpstack(L);
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
	//printf("END MG.TIMER: \n"); dumpstack(L);
}