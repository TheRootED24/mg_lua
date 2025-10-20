#ifndef MG_RPC_REQ_H
#define MG_RPC_REQ_H

#define RPC_REQ "mg_rpc_req"

typedef struct mg_rpc_req rpc_req;

#include "mg_lua_cb.h"
#include "mg_rpc.h"
#include "mg_iobuf.h"

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

#ifdef __cplusplus
}
#endif

void mg_open_mg_rpc_req (lua_State *L);
rpc_req *check_rpc_req (lua_State *L, int pos);
int _mg_rpc_req_new (lua_State *L);

#endif
