#ifndef MG_RPC_REQ_H
#define MG_RPC_REQ_H

#define RPC_REQ "mg_rpc_req"

typedef struct mg_rpc_req rpc_req;

#include "mg_lua.h"
#include "mg_rpc.h"

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
rpc_req *check_rpc_req (lua_State *L);
int new_rpc_req (lua_State *L);

#endif
