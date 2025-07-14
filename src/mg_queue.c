#include "mg_queue.h"

int new_mg_queue (lua_State *L) {
	mg_queue *q = (mg_queue*)lua_newuserdata(L, sizeof(mg_queue));
	luaL_getmetatable(L, "LuaBook.mg_queue");
	lua_setmetatable(L, -2);
	if(!q) lua_pushnil(L);

	return 1 ;  /* new userdatum is already on the stack */
}

mg_queue*check_mg_queue (lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_queue");
	luaL_argcheck(L, ud != NULL, 1, "`mg_queue' expected");
	return(mg_queue*)ud;
}

// void mg_queue_init(struct mg_queue *q, char *buf, size_t size);
static int _mg_queue_init(lua_State *L) {
	mg_queue *q = check_mg_queue(L);
	char *buf = (char*) luaL_checkstring(L, 2);
	size_t size = (size_t)luaL_checknumber(L, 3);
	mg_queue_init(q, buf, size);

	return 0;
}