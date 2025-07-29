#ifndef MG_MGR_H
#define MG_MGR_H

#define MG_ADDR "mg_addr"

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

typedef struct mg_addr mg_addr;

void mg_open_mg_addr(lua_State *L);
int _mg_addr_new (lua_State *L);
mg_addr *check_mg_addr(lua_State *L, int pos);

#endif
