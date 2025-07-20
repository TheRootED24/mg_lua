#ifndef MG_CONNECTION_H
#define MG_CONNECTION_H

#define MG_CONN "connection"

typedef struct mg_connection mg_connection;
#include "mg_addr.h"

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

void mg_open_mg_connection(lua_State *L);
int newconn (lua_State *L);
mg_connection *checkconn(lua_State *L);

#endif
