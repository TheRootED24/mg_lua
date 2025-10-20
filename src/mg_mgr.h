#ifndef MGMGR_H
#define MGMGR_H

#define MGMGR "mgr"

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
#include "mg_connection.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_mgr mg_mgr;

void mg_open_mg_mgr (lua_State *L);
mg_mgr *check_mg_mgr(lua_State *L);
int _mg_mgr_new (lua_State *L);

#endif
