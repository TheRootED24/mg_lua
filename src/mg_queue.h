#ifndef MG_QUEUE_H
#define MG_QUEUE_H

#define MG_QUEUE "queue"

typedef struct mg_queue mg_queue;

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

void mg_open_mg_queue(lua_State *L);

#endif
