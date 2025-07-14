#include "mg_lua_cb.h"

lua_State_t _GL;

void fn_lua_cb(mg_connection *c, int ev, void *ev_data) {
	lua_State_t *GL = (lua_State_t*)c->fn_data;
	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, c);
	lua_pushinteger(L, ev);
	lua_pushlightuserdata(L, ev_data);
	//dumpstack(L);
	lua_pcall(L, 3, 0, 0);
}

/*
static void timer_fn(void *arg) {
  struct mg_mgr *mgr = (struct mg_mgr *) arg;
  struct mg_mqtt_opts opts = {.clean = true,
                              .qos = s_qos,
                              .topic = mg_str(s_pub_topic),
                              .version = 4,
                              .message = mg_str("bye")};
  if (s_conn == NULL) s_conn = mg_mqtt_connect(mgr, s_url, &opts, fn, NULL);
}*/

void fn_lua_timer(void *arg) {
	lua_State_t *GL = (lua_State_t*)arg;
	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, GL->fn_data);
	lua_pcall(L, 1, 0, 0);
}

static int s_signo;
void fn_lua_signal(int signo) {
	s_signo = signo;
	lua_State *L = _GL.L;
	lua_getglobal(L, _GL.callback);
	lua_pushinteger(L, signo);
	lua_pcall(L, 1, 0, 0);
}

void store_state(lua_State *L, const char *cb) {
	_GL.L = L;
	_GL.callback = cb;
}



void lua_rpc_cb(rpc_req* req) {
	int fn_args = 1;
	lua_State_t *GL = (lua_State_t*)req->rpc->fn_data;
	lua_State *L = GL->L;
	lua_getglobal(L, GL->callback);
	lua_pushlightuserdata(L, req);
	if(GL->fn_data){
		lua_pushlightuserdata(L, GL->fn_data);
		fn_args++;
	}
	lua_pcall(L, fn_args, 0, 0);
}
