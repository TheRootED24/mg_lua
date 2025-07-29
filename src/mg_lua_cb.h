#ifndef MG_LUA_CB_H
#define MG_LUA_CB_H

#define MG_LUA_CB "mg_lua_cb"

// C++ GAURDS FOR C LIBS
#ifndef __cplusplus
// LUA LIBS FOR gcc
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif

#ifdef __cplusplus
// LUA LIBS FOR g++
#include <lua.hpp>
extern "C" {
#endif
// MONGOOSE C LIB
#include "../includes/libmongoose/src/mongoose.h"
#include "mg_mgr.h"
#include "mg_connection.h"
#include "mg_timer.h"
#include "mg_rpc.h"
#include "mg_rpc_req.h"

#ifdef __cplusplus
}
#endif

typedef struct {
	lua_State *L;
	const char *callback;
	struct mg_mgr *fn_data;
} lua_State_t;

void store_state(lua_State *L, const char *cb);
void fn_lua_cb(mg_connection *c, int ev, void *ev_data);
void fn_lua_timer(void *);
void fn_lua_signal(int signo);
void lua_rpc_cb(rpc_req* req);

#endif
