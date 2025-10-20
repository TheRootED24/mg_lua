#ifndef MG_CONNECTION_H
#define MG_CONNECTION_H

#define MG_CONN "connection"

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
#include "mg_iobuf.h"
#include "mg_addr.h"
#include "mg_mgr.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_connection mg_connection;

void mg_open_mg_connection(lua_State *L);
int _mg_connection_new (lua_State *L);
int _mg_connection_newt (lua_State *L);
mg_connection *check_mg_connection(lua_State *L, int pos);

#endif
