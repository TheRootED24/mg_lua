#ifndef MG_RPC_H
#define MG_RPC_H

#define MG_RPC "rpc"

typedef struct mg_rpc mg_rpc;

#include "mg_rpc_req.h"
#include "mg_connection.h"
#include "mg_mgr.h"
#include "mg_lua.h"

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

void mg_open_mg_rpc(lua_State *L);

#endif
