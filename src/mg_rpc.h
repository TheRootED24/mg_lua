#ifndef MG_RPC_H
#define MG_RPC_H

#define MG_RPC "rpc"

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
#include "mg_rpc_req.h"
#include "mg_connection.h"
#include "mg_mgr.h"
#include "mg_lua_cb.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_rpc mg_rpc;

void mg_open_mg_rpc(lua_State *L);
int _mg_rpc_new (lua_State *L);
mg_rpc *check_mg_rpc (lua_State *L, int pos);

#endif
