#include "mg_lua_cb.h"

lua_State_t _GL;
static int s_signo;

void fn_lua_cb(mg_connection *c, int ev, void *ev_data) {
	lua_State_t *GL = NULL;

	if(c->fn_data)
		GL = (lua_State_t*)c->fn_data;

	if(NULL == GL) GL = &_GL;
	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, c);
	lua_pushinteger(L, ev);
	lua_pushlightuserdata(L, ev_data);

	lua_pcall(L, 3, 0, 0);
};

void fn_lua_timer(void *arg) {
	if(arg == NULL) return;
	lua_State_t *GL = (lua_State_t*)arg;

	if(NULL == GL) GL = &_GL;

	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, GL->fn_data);

	lua_pcall(L, 1, 0, 0);
	
};

void fn_lua_signal(int signo) {
	s_signo = signo;
	lua_State *L = _GL.L;
	lua_getglobal(L, _GL.callback);
	lua_pushinteger(L, signo);

	lua_pcall(L, 1, 0, 0);
};

void store_state(lua_State *L, const char *cb) {
	_GL.L = L;
	_GL.callback = cb;
};

void lua_rpc_cb(rpc_req* req) {
	int fn_args = 1;
	lua_State_t *GL = (lua_State_t*)req->rpc->fn_data;
	if(NULL == GL) GL = &_GL;
	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, req);
	if(GL->fn_data != NULL){
		lua_pushlightuserdata(L, GL->fn_data);
		fn_args++;
	}

	lua_pcall(L, fn_args, 0, 0);
};
